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
	const char *user = getenv("USER");
	char *currentpath = getenv("PWD");
	char *pwd[MAXARG];
	int depth;
	printf("msh\n");
	/* Initialization */
	set_pwd(currentpath, pwd, &depth);
	printf("Depth: %d\n", depth);

	while (1) {
		printf("%s @ msh ~ ", user);
		for (int i = 0; i <= depth; i++) {
			printf("/%s", pwd[i]);
		}
		printf("\n$");
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
				chdir(homepath);
			} else {
				if(chdir(av[1]) != -1) {
					set_pwd(av[1], pwd, &depth);
				} else {
					perror("Error_cd");
				}
			}
			continue;
		}

	}
	return 0;
}
