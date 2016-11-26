#include "myshell.h"

#ifdef MERGE
int setcmds(char *tp, char **cmds[MAXARG], int cmdno) {
	/* RETURN: TOKENSTATUS */
	/* *tp: ls -al > someghing.txt */
	int tkno;
	char *ptr = tp;
	char **cmdptr;
	Malloc(cmdptr, char **, MAXARG, ERR_MALLOC);
	cmds[cmdno] = cmdptr;
	return tkno;
}
#endif

int iscop(char c) {
	// todo: add more token later
	return (isalnum(c) || (c == '.'));
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
		if (isalnum(*sp)) {	// COMMAND OR PARAM
			tokenseq[tki] = sp++;
			token_id[tki] = TKN_COP;
			tki++;
			while (isalnum(*sp)) sp++;
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
	*sp = '\0';
	// remove BL
	return tki;
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
