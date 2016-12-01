#include "myshell.h"

size_t iscop(char c) {
	// todo: add more token later
	return (isalnum(c) || (c == '.') || (c == '-') ||
			(c == '/') || (c == '[') || (c == ']') ||
			(c == '=') || (c == '+') || (c == '_') ||
			(c == '/') || (c == ':'));
}

size_t isrpbt(int tkn) { 
	// RV: if tkn is redirection or pipe
	return ((tkn == TKN_REDIR_IN) || (tkn == TKN_REDIR_OUT) 
			|| (tkn == TKN_PIPE) || (tkn == TKN_COM_BG));
}

int parse(char **tokenseq, int *token_id, char ***cmds, int *cmd_id) {
	/* returns numbers of commands */
	// ls | more > something &
	int cmdi = 0;
	int tk;
	// Check syntax error

	if (isrpbt(token_id[0])) {
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
		if (isrpbt(token_id[tki])) {
					tokenseq[tki] = NULL;
			switch (token_id[tki++]) {
				// think of ls | more > something &
				// com pipe com redir com bg
				// todo: handle both redirection and background
				case TKN_COM_BG:
					cmd_id[cmdi - 1] = CMD_BG;
					for (int i = cmdi - 2; 0 <= i; i--) {
						if (cmd_id[i] == CMD_PIPE) break;
						cmd_id[i] = CMD_BG;
					}
					break;
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
			if (isrpbt(token_id[tki])) {
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
			case '&':
					*sp = '\0';
					tokenseq[tki] = "&";
					token_id[tki] = TKN_COM_BG;
					tki++;
					sp++;
					continue;
			case '>':
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

