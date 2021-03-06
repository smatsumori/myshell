/* created by Shoya Matsumori
 * SID: 61417276
 */
#include "myshell.h"

void report_syntax_error(int tkno, char *tkseq[], int *token_id);
int chdirw(char *);
void printinfo();
pid_t S_ppid;

int main(int argc, char const* argv[])
{
	int tkno, ac;
	char raw_input[BUFSIZE];
	char **cmds[MAXARG];
	int tkid[MAXARG];
	char *tkseq[MAXARG];
	int cmdid[MAXARG];
	const char *user = getenv("USER");
	const char *homepath = getenv("HOME");
	char pwd[PATH_MAX];

	S_ppid = getppid();
	printinfo();	

	while (1) {
		strcpy(pwd, getenv("PWD"));
		printf("%s @ msh ~ %s\n$ ", user, pwd);
		if (fgets(raw_input, sizeof(raw_input), stdin) == NULL) {
			if (ferror(stdin)) continue;
				//report_error_and_exit("fgets", ERR_FGETS);
			printf("\nExiting...\n");
			exit(EXIT_SUCCESS);
		}
		tkno = tokenize(raw_input, tkseq, tkid);
		for (int i = 0; i < tkno; i++) {
			printf("[%d: %s]", tkid[i], tkseq[i]);
		}
		if (tkid[0] == TKN_EOL) {
			printf("\n");
			continue;
		}
		ac = parse(tkseq, tkid, (char ***)cmds, cmdid);
		if (ac < 0) {
			report_syntax_error(tkno, tkseq, tkid);
			printf("\n\n");
			continue;
		}
		
		printf("\n------CMD-------\n");
		for (int i = 0; i < ac - 1; i++) {
			printf("[%d: %s]", cmdid[i], cmds[i][0]);
		}
		printf("\n");

		/* CHANGE DIR */
		if (strcmp(cmds[0][0], "cd") == 0) {
			if (cmds[0][1] == NULL) {
				chdirw((char *)homepath);
			} else {
				if(chdirw(cmds[0][1]) == -1) report_error_and_exit("chdir", ERR_CHDIR);
			continue;
			}
		} else if (strcmp(cmds[0][0], "exit") == 0) {
			fprintf(stderr, "Exitting...\n");
			exit(EXIT_SUCCESS);
		}

		/* EXECUTION */
		exec_cmd((char ***)cmds, cmdid, 0, STDIN_FILENO);
//		Default_INT();
		printf("\n");
	}
	return EXIT_SUCCESS;
}

void printinfo(){
	printf("msh ver %s\n", VER);
	printf("Executing on [PID: %d][PPID: %d][PGID: %d]\n\n",
		 	getpid(), getppid(), getpgrp());
	return;
}

int chdirw(char *dir) {
	char pwd[PATH_MAX];
	int stat = chdir(dir);
	char *newPWD = getcwd(pwd, sizeof(pwd));
	setenv("PWD", newPWD, 1);
	return stat;
}

void report_syntax_error(int tkno, char *tkseq[], int *token_id) {
	fprintf(stderr, "SYNTAX_ERROR:");
	for (int i = 0; i < tkno - 1; i++) {
		if (token_id[i] == TKN_SYN_ERROR) {
			fprintf(stderr, " [error: %s] ", tkseq[i]);
			continue;
		}
		fprintf(stderr, " %s ", tkseq[i]);
	}
	fprintf(stderr, "\n");
	return;
}
