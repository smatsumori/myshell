#include "myshell.h"
const char *delim = " \t\n&><|";	// Deliminator

static void redirect(int oldfd, int newfd) {
	/* newfd will point at oldfd */
	if (oldfd != newfd) {
		if (dup2(oldfd, newfd)) 
			Close(oldfd);
		else
			perror("dup2");
	}
	return;
}

int iscop(char c) {
	return ((isalnum(c) || ispunct(c)) 
		&& (c != '&') && (c != '>') && (c != '<') && (c != '|'));
}

void tokenizer(char *rawinput, char **cmds[MAXARG], int tkn[MAXARG]) {
	/* ls -al | grep "something" | more */
	char *ptr = rawinput;
	int cmdno = 0;
}

/*
void show_tkno(int tkno) {
	switch (tkno) {
		case TKN_COP:
			fprintf(stderr, "[COP]");
			break;
		case TKN_REDIR_IN:
			fprintf(stderr, "[REDIR_IN]");
			break;
		case TKN_REDIR_OUT:
			fprintf(stderr, "[REDIR_OUT]");
			break;
		case TKN_PIPE:
			fprintf(stderr, "[PIPE]");
			break;
		case TKN_EOL:
			fprintf(stderr, "[EOL]\n");
			break;
	}
	return;
}
*/

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
