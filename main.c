/* created by Shoya Matsumori
 * SID: 61417276
 */
#include "myshell.h"


#ifdef DEBUG_LEX
int main(int argc, char const* argv[])
{
	int tkno;
	int pid, ac, stat, fd;
//	int tkn[MAXARG];
	char *av[MAXARG], raw_input[BUFSIZE];
	// char **cmds[MAXARG];	// {cmd0, cmd1, cmd3}
	/* echo hogehoge | grep hoge | wc */
	char *cmd1[] = {"echo", "hogehoge", NULL};
	char *cmd2[] = {"grep", "hoge", NULL};
	char *cmd3[] = {"wc", NULL};
	char **cmds[] = {cmd1, cmd2, cmd3, NULL};
	int tkid[MAXARG];

	while (1) {
		printf("\n$");
//		exec_pipeline((char ***)cmds, 0, STDIN_FILENO);
		
		if (fgets(raw_input, sizeof(raw_input), stdin) == NULL) {
			if (ferror(stdin)) {
				perror("fgets");
				exit(1);
			}
			printf("\nExiting...\n");
			exit(0);
		}
		// test tokenizer
		char *tkseq[MAXARG];
		int cmdid[MAXARG];
		tkno = tokenize(raw_input, tkseq, tkid);
		for (int i = 0; i < tkno; i++) {
			printf("[%d: %s]\n", tkid[i], tkseq[i]);
		}
		ac = parse(tkseq, tkid, (char ***)cmds, cmdid);
		for (int i = 0; i < ac - 1; i++) {
			printf("[%d: %s]\n", cmdid[i], cmds[i][0]);
		}
	}
	return 0;
}
#endif



#ifdef INTERP
int main(int argc, char const* argv[])
{
	int pid, ac ,stat, fd;
	int pfd[2];
	int redi = 0;	// Redirection Index
	int pipc = 0;	// Pipe Counter
	int pipi[10];	// Pipe Index
	int i;
	char *av[MAXARG], buf[BUFSIZE];
	const char *homepath = getenv("HOME");
	const char *user = getenv("USER");
	char *currentpath = getenv("PWD");
	char *pwd[MAXARG];
	int depth;
	printf("msh ver %s\n", VER);
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
		redi = 0;	// initialize
		
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

		/* Check Redirection and Pipe */
		/* redi:  index of redirect token */
		/* pipc:  pipe counter */
		/* pipi:  pipe index */
		for (int i = 0; i < ac; i++) {
			if ((strcmp(av[i], ">")) == 0) {
				av[i] = NULL;
				redi = i;
				break;
			} else if ((strcmp(av[i], "<")) == 0) {
				av[i] = NULL;
				redi = -i;
				break;
			} else if ((strcmp(av[i], "|")) == 0) {
				av[i] = NULL;
				pipi[pipc++] = i;
				continue;
			}
		}


		if (pipc > 0) {
			pipe(pfd);
			exec_pipeline(av, pfd, pipc, pipi);
			continue;	// Skip the following
		}

		/* NORMAL PROCESS EXECUTION */
		/* Stage 3 */
		switch (pid) {
			case -1:
				/* Cannot fork */
				perror("ERROR: fork");
				break;
				// ERROR
			case 0:
				/* Child process */
				/* File Open for Redirection */
				if (redi > 0) {
					// OUTPUT REDIRECTION
					Open(av[redi + 1], O_WRONLY|O_CREAT|O_TRUNC, 0644, fd);
					Close(STDOUT_FILENO);	// Close stdout
					dup(fd);	// dup() returns 1(stdout)
					Close(fd);
				}
				if (redi < 0) {
					// INPUT REDIRECTION
					Open(av[(-redi) + 1], O_RDONLY, 0644, fd);
					Close(STDIN_FILENO);
					dup(fd);
					close(fd);
			}
				#ifdef DEBUG
					fprintf(stderr, "Executing CHIDLD PID: %d\n", getpid());
				#endif
				/* EXECUTION */
				if ((execvp(av[0], av)) == -1) {
					perror("Error_execvp");	
					exit(1);
				}
				continue;
				break;
			
			default:
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

#endif
