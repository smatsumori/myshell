#include "myshell.h"

size_t iscop(char c) {
	// todo: add more token later
	return (isalnum(c) || (c == '.') || (c == '-'));
}

size_t isrpt(int tkn) { 
	// RV: if tkn is redirection or pipe
	return ((tkn == TKN_REDIR_IN) || (tkn == TKN_REDIR_OUT) 
			|| (tkn == TKN_PIPE));
}

int parse(char **tokenseq, int *token_id, char ***cmds, int *cmd_id) {
	/* returns numbers of commands */
	int cmdi = 0;
	int tk;
	// Check syntax error
	if (isrpt(token_id[0])) {
		token_id[0] = TKN_SYN_ERROR;
		return -1;
	}

	for (int tki = 0;;) {
		/* IF: end of command */
		if (token_id[tki] == TKN_EOL) {
			cmds[cmdi] = NULL;
			cmd_id[cmdi] = CMD_EOC;	
			break;
		}
		
		/* IF: cmd or param */
		if (token_id[tki] == TKN_COP) {
			cmd_id[cmdi] = CMD_NORM;
			cmds[cmdi++] = &tokenseq[tki++];
			while (token_id[tki] == TKN_COP) tki++;
			continue;
		}
		
		/* IF: redir or pipe */
		if (isrpt(token_id[tki])) {
					tokenseq[tki] = NULL;
			switch (token_id[tki++]) {
				case TKN_REDIR_OUT:
					cmd_id[cmdi - 1] = CMD_REDIR_OUT;
					break;
				case TKN_REDIR_IN:
					cmd_id[cmdi - 1] = CMD_REDIR_IN;
					break;
				case TKN_PIPE:
					cmd_id[cmdi - 1] = CMD_PIPE;
					break;
			}
			// Check syntax error
			// IF: redir and pipe continues twice in a row
			if (isrpt(token_id[tki])) {
				token_id[tki] = TKN_SYN_ERROR;
				return -1;
			}
			continue;
		}
	}
	return cmdi + 1;
}

int tokenize(char *rawinput, char **tokenseq, int token_id[MAXARG]) {
	/* sets: tokenseq, token_id from rawinput */
	/* rawinput: "ls -al | grep "something" | wc > hoge.txt" */
	/* RV: number of tokens */
	char *sp = rawinput;	// search pointer
	char tki = 0;
	while (*sp != '\n') {
		while (isblank(*sp)) {
			*sp= '\0';
			sp++;
		}
		if (iscop(*sp)) {	// COMMAND OR PARAM
			tokenseq[tki] = sp++;
			token_id[tki] = TKN_COP;
			tki++;
			while (iscop(*sp)) sp++;
			continue;
		}
		switch (*sp) {
			case '>':
					fprintf(stderr, "redirec\n");
					*sp = '\0';
					tokenseq[tki] = ">";
					token_id[tki] = TKN_REDIR_OUT;
					tki++;
					sp++;
					continue;
			case  '<':
					*sp = '\0';
					tokenseq[tki] = "<";
					token_id[tki] = TKN_REDIR_IN;
					tki++;
					sp++;
					continue;
			case '|':
					*sp = '\0';
					tokenseq[tki] = "|";
					token_id[tki] = TKN_PIPE;
					tki++;
					sp++;
					continue;
		}
	}
	tokenseq[tki] = NULL;
	token_id[tki] = TKN_EOL;
	*sp = '\0';
	return tki + 1;
}





#ifdef MERGE
void tokenizer(char *rawinput, char **cmds[MAXARG], int tkn[MAXARG]) {
	/* ls -al | grep "something" | more */
	const char *delim_p = "|";	// Deliminator
	char *ptr = rawinput;
	int cmdno = 0;

	if ((tp = strtok(buf, delim_p)) == NULL) {
		tkn[0] = setcmds(tp, (char ***)cmds);
		return 1;
	}
	tkn[0] = setcmds(tp, (char ***)cmds);
	while ((tp = strtok(NULL, delim)) != NULL) {
		tkn[cmno] = setcmds(tp, (char ***)cmds, cmdno);
	}
	return 0;
}

int parser(int *ac, char *av[MAXARG], char buf[BUFSIZE]) {
	/* sets ac av by parsing raw input buf */
	*ac = 0;	// Initialize conter to Zero
	char *tp;
	if ((tp = strtok(buf, delim)) == NULL) {
		return 1;
	}
	av[(*ac)++] = tp;
	while ((tp = strtok(NULL, delim)) != NULL) {
		av[(*ac)++] = tp;
	}
	return 0;
}

void set_pwd(char *dir_path, char *pwd[], int *depth){
	/* sets pwd to *pwd */	
	/* dir_path: directory path -- av[1] */
	/* pwd[]: holds pwd */
	/* depth: how deep from your inital pwd */
	char *tp;	// pt: prev
	char c;
	// if absolute path
	if (dir_path[0] == '/') {
		#ifdef DEBUG_SET_PWD
			fprintf(stderr, "absolute path mode\n");
		#endif
		*depth = -1;
	} else {
		#ifdef DEBUG_SET_PWD
			fprintf(stderr, "relative path mode\n");
		#endif
	}

	tp = strtok(dir_path, "/");
	do {
		#ifdef DEBUG_SET_PWD
		printf("Depth: %d\n", *depth);
		printf("tp %s\n", tp);
		#endif

		if (*tp == '.') {
			char *tmp = tp;
			while (*(tmp++) != '\0') {
				if (*tmp == '.') {
					(*depth)--;
					break;
				} else {
					/* Case .sth.swp */
					pwd[++(*depth)] = tp;
				}
			}
			continue;
		}
		pwd[++(*depth)] = tp;
	} while ((tp = strtok(NULL, "/")) != NULL);

	return;
}

#endif
