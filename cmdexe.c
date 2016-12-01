#include "myshell.h"

/* PROTOTYPE */
static void sig_handler(int);
static void exec_norm(char ***, int *, size_t, int);
static void exec_bg(char ***, int *, size_t, int);
static void exec_pipeline(char ***, int *, size_t, int);
static void redirect(int, int);
static void cmd_redirect(char *, size_t);
static void execvew(const char *, char *const []);


static void sig_handler(int signo) {
	fprintf(stderr, "Background process has executed: %d\n\n", signo);
	int status;
	waitpid(-1, &status, WNOHANG);
	signal(SIGCHLD, SIG_DFL);
	signal(SIGTSTP, SIG_DFL);
	signal(SIGINT, SIG_DFL);
	return;
}

static void execvew(const char *file, char *const argv[]) {
	/* wrapper function for execve */
	fprintf(stderr, "Searching: %s\n", argv[0]);
	char *envpath =  getenv("PATH");
	char *tp = envpath;
	char *path;
	Malloc(path, char , sizeof(envpath) + sizeof(argv[0]), ERR_MALLOC);
	if ((tp = strtok(envpath, ":")) == NULL) {
		strcpy(path, tp);
		strcat(path, "/");
		strcat(path, argv[0]);
		if (execve(path, argv, environ) < 0) report_error_and_exit("execvew", ERR_EXECVEW);
	} else {
		while ((tp = strtok(NULL, ":")) != NULL) {
			strcpy(path, tp);
			strcat(path, "/");
			strcat(path, argv[0]);
			printf("%s\n", path);
			if (execve(path, argv, environ) < 0) continue;
			return;
		}
		report_error_and_exit("execvew", ERR_EXECVEW);
	}
	return;
}

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
	pid_t pid;
	if (cmdid[pos + 1] == CMD_EOC) {	/* IF: last command */
		switch ((pid = fork())) {
			case -1:
				report_error_and_exit("fork", ERR_FORK);
			case 0:
				Show_pinfo();
				redirect(in_fd, STDIN_FILENO);
				execvew(cmds[pos][0], cmds[pos]);
				break;
			default:
				Close(in_fd);
				if (waitpid(pid, &stat, 0) < 0 ) report_error_and_exit("wait", ERR_WAIT);
				Default_INT();
		}
		return;
	} else {
		// set pfd in current process
		int pfd[2];	// i0: read, i1: write
		if (pipe(pfd) == -1) report_error_and_exit("pipe", ERR_PIPE);
		switch ((pid = fork())) {
			case -1:
				report_error_and_exit("fork", ERR_FORK);
				break;
			case 0:		/* CHILD */
				setpgid(getpid(), getpid());
				Show_pinfo();
				Close(pfd[FD_READ]);	// close read
				redirect(in_fd, STDIN_FILENO);	// STDIN -> in_fd
				redirect(pfd[FD_WRITE], STDOUT_FILENO);	// STDOUT -> fd[1](write)
				execvew(cmds[pos][0], cmds[pos]);
				break;
			default:	/* PARENT */
				Close(pfd[FD_WRITE]);
				if(pos != 0) Close(in_fd);	// Prevent closing myshell STDIN
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
		//setpgid(getpid(), getpid());
		Set_child_SIG();
		Show_pinfo();
		execvew(cmds[pos][0], cmds[pos]);
		break;
	default: /* PARENT */
		Set_parent_SIG();
		if (waitpid(pid, &stat, 0) < 0) report_error_and_exit("wait", ERR_WAIT);
		Default_INT();
	}
	return;
}

static void exec_bg(char **cmds[], int *cmdid, size_t pos, int in_fd) {
	int pid, stat, fd;
	switch ((pid = fork())) {
	case -1:	// Cannot fork
		report_error_and_exit("fork", ERR_FORK);
	case	0:
		Show_pinfo();
		setpgid(getpid(), getpid());
		//fd 	= open("/dev/tty", O_RDWR);
		//tcsetpgrp(fd, S_ppid); removed
		Set_child_SIG();
		execvew(cmds[pos][0], cmds[pos]);
		break;
	default: /* PARENT */
		Set_parent_SIG();
		signal(SIGCHLD, sig_handler);
		fprintf(stderr, "Executing on BG\n");
		// ignore sigint
		break;
	}
	return;
}

static void exec_redir(char **cmds[], int *cmdid, size_t pos, int in_fd) {
	int pid, stat;
	switch ((pid = fork())) {
	case -1:	// Cannot fork
		report_error_and_exit("fork", ERR_FORK);
	case 0:	/* CHILD */
		setpgid(getpid(), getpid());
		Set_child_SIG();
		Show_pinfo();
		cmd_redirect(cmds[pos + 1][0], cmdid[pos]);
		/* EXECUTION */
		execvew(cmds[pos][0], cmds[pos]);
		break;
	default: /* PARENT */
		Set_parent_SIG();
		if (waitpid(pid, &stat, 0) < 0) report_error_and_exit("wait", ERR_WAIT);
		Default_INT();
	}
	return;
}


void exec_cmd(char **cmds[], int *cmdid, size_t pos, int in_fd) {
	// Error check
	int id;
	if (pos == 0) {
		switch (cmdid[0]) {
			case CMD_BG:
				exec_bg(cmds, cmdid, pos, in_fd);
				break;
			case CMD_NORM:
				exec_norm(cmds, cmdid, pos, in_fd);
				break;
			case CMD_PIPE:
				exec_pipeline(cmds, cmdid, pos, in_fd);
				break;
			case CMD_REDIR_IN:
				exec_redir(cmds, cmdid, pos, in_fd);
				break;
			case CMD_REDIR_OUT:
				exec_redir(cmds, cmdid, pos, in_fd);
				break;
		}
	} else {
		if (cmdid[pos - 1] == CMD_PIPE) exec_pipeline(cmds, cmdid, pos, in_fd);
	}
	return;
}
