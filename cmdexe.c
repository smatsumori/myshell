#include "myshell.h"

static void cmd_redirect(int oldfd, int newfd, char *filename){
	return;
}

// newfd -> oldfd
static void redirect(int oldfd, int newfd) {
	if (oldfd != newfd) {
		if (dup2(oldfd, newfd) != -1)
			Close(oldfd);
		else
			perror("ERROR: dup2");
	}
}

static void exec_pipeline(char **cmds[], size_t pos, int in_fd) {
	/* cmds: commands
	 * pos: position
	 * ifd: input file descriptor */
	if (cmds[pos + 1] == NULL) {	/* IF: last command */
		redirect(in_fd, STDIN_FILENO);
		execvp(cmds[pos][0], cmds[pos]);
		return;
	} else {
		// set pfd in current process
		int pfd[2];	// i0: read, i1: write
		if (pipe(pfd) == -1) perror("ERROR: pipe");
		switch (fork()) {
			case -1:
				perror("ERROR: fork");
				break;
			case 0:		/* CHILD */
				Close(pfd[FD_READ]);	// close read
				redirect(in_fd, STDIN_FILENO);	// STDIN -> in_fd
				redirect(pfd[FD_WRITE], STDOUT_FILENO);	// STDOUT -> fd[1](write)
				execvp(cmds[pos][0], cmds[pos]);
				break;
			default:	/* PARENT */
				Close(pfd[FD_WRITE]);
				Close(in_fd);
				exec_pipeline(cmds, pos + 1, pfd[FD_READ]);
				break;
		}
	}
}

static void exec_cmd(char **cmds[]) {
	return;
}
