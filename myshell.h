/* defines */
#define DEBUG
#define DEBUG_LEX
#define MAXINPUT
#define MAXARG 80
#define BUFSIZE 1024
#define VER "2.2"

/* token status */
#define TKN_COP 1	// Command or Parameter
#define TKN_REDIR_IN 2
#define TKN_REDIR_OUT 3
#define TKN_PIPE 4
#define TKN_BG 5
#define TKN_EOL -1
#define TKN_EOF -2

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>

/* prototypes */
extern int lexer();
extern int parser(int *, char *[], char buf[]);
extern void set_pwd(char *, char *[], int *);
extern void show_tkno(int);

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
