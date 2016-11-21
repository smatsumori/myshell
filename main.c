/* created by Shoya Matsumori
 * SID: 61417276
 */
#include "myshell.h"

int main(int argc, char const* argv[])
{
	int pid, ac ,stat, fd;
	int pfd[2];
	int redi = 0;	// Redirection Index
	int pipi = 0;	// Pipe Idex
	int i;
	char *av[MAXARG], buf[BUFSIZE];
	const char *homepath = getenv("HOME");
	const char *user = getenv("USER");
	char *currentpath = getenv("PWD");
	char *pwd[MAXARG];
	int depth;
	printf("msh ver 2.1\n");
	printf("Executing on PID: %d\n\n", getpid());
	/* Initialization */
	set_pwd(currentpath, pwd, &depth);

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
		
		/* PARSING */
		if (parser(&ac, av, buf) == 1) {
			// If inputs nothing
			continue;
		}

		/* CHANGE DIR */
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
			printf("\n");
			continue;
		} else if (strcmp(av[0], "exit") == 0) {
			fprintf(stderr, "Exitting...\n");
			exit(0);
		}

		/* Stage 2 */
		/* folk process 
		   if child tries to folk process
		   fork returns 0 */
		if ((pid = fork()) < 0) {
			/* Error handling */
			perror("Error: fork");
		} 
		#ifdef DEBUG
			fprintf(stderr, "process forked PID: %d\n", pid);
		#endif 

		/* REDIRECTION */
		/* redi:  index of redirect token */
		if(pid == 0) {
			for (int i = 0; i < ac; i++) {
				if ((strcmp(av[i], ">")) == 0) {
					av[i] = NULL;
					redi = i;
					break;
				} else if ((strcmp(av[i], "<")) == 0) {
					av[i] = NULL;
					redi = -i;
					break;
				}	
			}
			if (redi > 0) {
				// OUTPUT REDIRECTION
				if ((fd = open(av[redi + 1],
							   	O_WRONLY|O_CREAT|O_TRUNC, 0644))
						< 0) {
					perror("Error: file open");
				}
				close(1);	// Close stdout
				dup(fd);	// dup() returns 1(stdout)
				close(fd);
			}
			if (redi < 0) {
				// INPUT REDIRECTION
				if ((fd = open(av[(-redi) + 1],
							O_RDONLY, 0644))
					   < 0)	{
					perror("Error: file open");
				}
				close(0);
				dup(fd);
				close(fd);
			}
		}

		/* PIPELINE */
		/* pfd[0] for stdin, pfd[1] for stdout */
		if (pid == 0) {
			if (pipi > 0) {
				close(1);
				dup(pfd[1]);
				close(pfd[0]);
				close(pdf[1]);
			}
		}

		/* PROCESS EXECUTION */
		/* Stage 3 */
		if (pid == 0) {
			/* IF child process */
			#ifdef DEBUG
				fprintf(stderr, "Executing CHIDLD PID: %d\n", getpid());
			#endif
			if ((execvp(av[0], av)) == -1) {
				perror("Error_execvp");	
				exit(1);
			}
			continue;
		} else {
			/* Parent process */
			/* Waiting for child process */
			if (waitpid(pid, &stat, 0) < 0) {
				perror("Error_waitpid");
				exit(3);
			}
			printf("\n");
			continue;
		}
	}
	return 0;
}
