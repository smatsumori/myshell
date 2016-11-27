#include "myshell.h"

void cmd_redirect(char *filename,  size_t mode){
	int fd;
	switch (mode) {
		case CMD_REDIR_IN:
			Open(filename, O_RDONLY, 0644, fd);
			Close(STDIN_FILENO);
			dup(fd); close(fd);
			break;
		case CMD_REDIR_OUT:
			Open(filename, O_WRONLY|O_CREAT|O_TRUNC, 0644, fd);
			Close(STDOUT_FILENO);	// Close stdout
			dup(fd);	// dup() returns 1(stdout)
			Close(fd);
			break;
		default:
			report_error_and_exit("cmd_redirect", ERR_CODE);
	}
	return;
}

// newfd -> oldfd
void redirect(int oldfd, int newfd) {
	if (oldfd != newfd) {
		if (dup2(oldfd, newfd) != -1)
			Close(oldfd);
		else
			perror("ERROR: dup2");
	}
}

void exec_pipeline(char **cmds[], int *cmdid, size_t pos, int in_fd) {
	/* cmds: commands 
	 * pos: position
	 * ifd: input file descriptor */
	int stat;
	int pid;
	if (cmdid[pos + 1] == CMD_EOC) {	/* IF: last command */
		redirect(in_fd, STDIN_FILENO);
		execvp(cmds[pos][0], cmds[pos]);
		return;
	} else {
		// set pfd in current process
		int pfd[2];	// i0: read, i1: write
		if (pipe(pfd) == -1) perror("ERROR: pipe");
		switch ((pid = fork())) {
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
				if (waitpid(pid, &stat, 0) < 0 ) report_error_and_exit("wait", ERR_WAIT);
				exec_pipeline(cmds, cmdid, pos + 1, pfd[FD_READ]);
				break;
		}
	}
}

void exec_cmd(char **cmds[], int *cmdid, size_t pos, int in_fd) {
	
	if (pos == 0) {
		if (cmdid[pos] == CMD_PIPE) exec_pipeline(cmds, cmdid, pos, in_fd);
	} else {
		if (cmdid[pos - 1] == CMD_PIPE) exec_pipeline(cmds, cmdid, pos, in_fd);
	}
	return;
}
