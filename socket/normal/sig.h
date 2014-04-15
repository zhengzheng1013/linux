#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "util.h"

typedef void sigfunc(int);

sigfunc *signal(int signo, sigfunc *func);

void sig_child(int signo);
