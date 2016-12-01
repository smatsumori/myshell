#ifndef MYSHELL_H
#define MYSHELL_H

/* defines */
#define DEBUG
#define DEBUG_LEX
#define MAXINPUT 100
#define MAXARG 80
#define BUFSIZE 1024
#define VER "4.2"
#define PATH_MAX 100

/* FD status */
#define FD_READ 0
#define FD_WRITE 1

/* token status */
#define TKN_COP 1
#define TKN_REDIR_IN 2
#define TKN_REDIR_OUT 3
#define TKN_PIPE 4
#define TKN_COM_BG 5
#define TKN_EOL -1
#define TKN_EOF -2
#define TKN_SYN_ERROR -3

/* cmd status */
#define CMD_NORM 11
#define CMD_REDIR_IN 12
#define CMD_REDIR_OUT 13
#define CMD_PIPE 14
#define CMD_BG 15
#define CMD_EOC -2

/* error status */
#define ERR_CODE -1
#define ERR_EXECVEW 2
#define ERR_CHDIR 3
#define ERR_MALLOC 4
#define ERR_WAIT 5
#define ERR_FORK 6
#define ERR_PIPE 7
#define ERR_SIG 8
#define ERR_EXECVP 10
#define ERR_FGETS 11

/* includes */
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>

/* Global */
extern char **environ;	// envp
extern pid_t S_ppid;

/* prototypes */
/* cmdexe.c */
extern void exec_cmd(char ***, int *, size_t, int);

/* parser.c */
extern int lexer(char buf[]);
extern int parser(int *, char *[], char buf[]);
extern void set_pwd(char *, char *[], int *);
extern void show_tkno(int);
extern int tokenize(char *, char **, int *);
extern int parse(char **, int *, char ***, int *);

/* macros */
#define Close(FD) do{	\
	int Close_fd = (FD);	\
	if (close(Close_fd) == -1) {	\
		perror("ERROR: file close");	\
		fprintf(stderr, "%s:%d: close("#FD") %d\n",	\
				__FILE__, __LINE__, Close_fd);	\
	} \
} while(0)	\

#define Open(FILENAME, STAT, PMI, FD) do{	\
	if ((FD = open(FILENAME, STAT, PMI)) == -1) {	\
		perror("ERROR: file open");	\
		fprintf(stderr, "%s:%d: open("#FILENAME") %s\n",	\
				__FILE__, __LINE__, FILENAME);	\
	} \
} while(0)	\

#define Malloc(PTR, TYPE, SIZE, ERRORNO) do{	\
	if ((PTR = (TYPE *) malloc(sizeof(TYPE) * SIZE)) == NULL) {	\
		fprintf(stderr, "Cannot allocate %d byte memory space.\n", \
				(int)sizeof(TYPE) * (int)(SIZE)); \
		exit(ERRORNO); \
	} \
} while (0) \

#define Show_pinfo() do{	\
	fprintf(stderr, "\nINFO:[PID: %d][PPID: %d][PRGP: %d]\n",	\
			getpid(), getppid(), getpgrp());	\
} while(0)\

#define Ignore_INT() do{	\
	if (signal(SIGINT, SIG_IGN) == SIG_ERR) report_error_and_exit("signal", ERR_SIG); \
} while(0) \

#define Default_INT() do{	\
	if (signal(SIGINT, SIG_DFL) == SIG_ERR) report_error_and_exit("signal", ERR_SIG); \
} while(0) \

#define Set_child_SIG() do{	\
	if (signal(SIGINT, SIG_DFL) == SIG_ERR) report_error_and_exit("signal", ERR_SIG); \
	if (signal(SIGTSTP, SIG_DFL) == SIG_ERR) report_error_and_exit("signal", ERR_SIG); \
} while(0) \

#define Set_parent_SIG() do{	\
	if (signal(SIGINT, SIG_IGN) == SIG_ERR) report_error_and_exit("signal", ERR_SIG); \
	if (signal(SIGTSTP, SIG_IGN) == SIG_ERR) report_error_and_exit("signal", ERR_SIG); \
} while(0) \

/* utils */
static void report_error_and_exit(const char* msg, int rpid) {
	perror(msg);
	exit(rpid);
	return;
}

#endif
