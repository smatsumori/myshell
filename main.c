/* created by Shoya Matsumori
 * SID: 61417276
 */
#include "myshell.h"


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
		printf("------CMD-------\n");
		for (int i = 0; i < ac - 1; i++) {
			printf("[%d: %s]\n", cmdid[i], cmds[i][0]);
		}
		exec_cmd((char ***)cmds, cmdid, 0, STDIN_FILENO);
	}
	return 0;
}

