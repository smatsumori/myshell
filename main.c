/* created by Shoya Matsumori
 * SID: 61417276
 */
#include "myshell.h"



int main(int argc, char const* argv[])
{
	int pid, ac ,stat;
	int i;
	char *av[MAXARG], buf[BUFSIZE];

	while (1) {
		printf("$");
		if (fgets(buf, sizeof(buf), stdin) == NULL) {
			if (ferror(stdin)) {
				perror("fgets");
				exit(1);
			}
			printf("\nExiting.../n");
			exit(0);
		}
		buf[strlen(buf) - 1] = '\0'; // safe sentinel
		memset(av, 0, sizeof av);
		parser(&ac, av, buf);
	}
	return 0;
}
