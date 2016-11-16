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
