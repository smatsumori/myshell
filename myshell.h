/* defines */
#define DEBUG
#define MAXINPUT
#define MAXARG 80
#define BUFSIZE 1024

/* token status */
#define TKN_NORMAL 1
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

/* prototypes */
extern int parser(int *, char *[], char buf[]);
extern void set_pwd(char *, char *[], int *);
/* type struct */
