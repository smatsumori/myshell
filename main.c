/* created by Shoya Matsumori
 * SID: 61417276
 */
#include "myshell.h"

int main(int argc, char const* argv[])
{
	int pid, ac ,stat;
	int i;
	char *av[MAXARG], buf[BUFSIZE];
	const char *homepath = getenv("HOME");

	while (1) {
		printf("$");
		if (fgets(buf, sizeof(buf), stdin) == NULL) {
			if (ferror(stdin)) {
				perror("fgets");
				exit(1);
			}
			printf("\nExiting...\n");
			exit(0);
		}
		buf[strlen(buf) - 1] = '\0'; // safe sentinel
		memset(av, 0, sizeof av);

		if (parser(&ac, av, buf) == 1) {
			// If inputs nothing
			continue;
		}

		if (strcmp(av[0], "cd") == 0) {
			int stat;
			if (ac < 1) {
				stat = chdir(homepath);
			} else {
				stat = chdir(av[1]);
			}
			if (stat == -1) {
				perror("Error_cd");
			}
			continue;
		}

	}
	return 0;
}
