/* created by Shoya Matsumori
 * SID: 61417276
 */
#include "myshell.h"

void printinfo();

int main(int argc, char const* argv[])
{
	int tkno, ac;
	char raw_input[BUFSIZE];
	char **cmds[MAXARG];
	int tkid[MAXARG];
	char *tkseq[MAXARG];
	int cmdid[MAXARG];

	printinfo();	

	while (1) {
		printf("\n$");
		memset(raw_input, TKN_EOF, sizeof(raw_input));

		if (fgets(raw_input, sizeof(raw_input), stdin) == NULL) {
			if (ferror(stdin)) report_error_and_exit("fgets", ERR_FGETS);
			printf("\nExiting...\n");
			exit(0);
		}
		// test tokenizer
		tkno = tokenize(raw_input, tkseq, tkid);
		for (int i = 0; i < tkno; i++) {
			printf("[%d: %s]", tkid[i], tkseq[i]);
		}
		ac = parse(tkseq, tkid, (char ***)cmds, cmdid);
		printf("\n------CMD-------\n");
		for (int i = 0; i < ac - 1; i++) {
			printf("[%d: %s]", cmdid[i], cmds[i][0]);
		}
		printf("\n");
		exec_cmd((char ***)cmds, cmdid, 0, STDIN_FILENO);
	}
	return 0;
}

void printinfo(){
	printf("msh ver %s\n", VER);
	printf("Executing on [PID: %d][PPID: %d]\n\n", getpid(), getppid());
}
