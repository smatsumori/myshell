#include "myshell.h"

/* PROTOTYPE */
static void exec_norm(char ***, int *, size_t, int);
static void exec_pipeline(char ***, int *, size_t, int);
static void redirect(int, int);
static void cmd_redirect(char *, size_t);

static void cmd_redirect(char *filename,  size_t mode){
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
static void redirect(int oldfd, int newfd) {
	if (oldfd != newfd) {
		if (dup2(oldfd, newfd) != -1)
			Close(oldfd);
		else
			perror("ERROR: dup2");
	}
}

static void exec_pipeline(char **cmds[], int *cmdid, size_t pos, int in_fd) {
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

static void exec_norm(char **cmds[], int *cmdid, size_t pos, int in_fd) {
	int pid, stat;
	switch ((pid = fork())) {
	case -1:	// Cannot fork
		report_error_and_exit("fork", ERR_FORK);
	case 0:	/* CHILD */
		#ifdef DEBUG
			fprintf(stderr, "Executing CHIDLD PID: %d\n", getpid());
		#endif
		/* EXECUTION */
		if (execvp(cmds[pos][0], cmds[pos]) == -1) report_error_and_exit("execvp", ERR_EXECVP);
		break;
	default: /* PARENT */
		if (waitpid(pid, &stat, 0) < 0) report_error_and_exit("wait", ERR_WAIT);
	}
	return;
}

void exec_cmd(char **cmds[], int *cmdid, size_t pos, int in_fd) {
	int id;
	if (pos == 0) {
		switch (cmdid[0]) {
			case CMD_NORM:
				exec_norm(cmds, cmdid, pos, in_fd);
				break;
			case CMD_PIPE:
				exec_pipeline(cmds, cmdid, pos, in_fd);
				break;
			case CMD_REDIR_IN:
				cmd_redirect(cmds[pos + 1][0], CMD_REDIR_IN);
				break;
			case CMD_REDIR_OUT:
				cmd_redirect(cmds[pos + 1][0], CMD_REDIR_OUT);
				break;
		}
	} else {
		if (cmdid[pos - 1] == CMD_PIPE) exec_pipeline(cmds, cmdid, pos, in_fd);
	}
	return;
}
