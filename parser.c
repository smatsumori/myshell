#include "myshell.h"
const char *delim = " \t\n";	// Deliminator


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
		printf("Depth: %d\n", *depth);
		printf("tp %s\n", tp);

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
