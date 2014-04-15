#include "sig.h"

sigfunc *signal(int signo, sigfunc *func)
{
	struct sigaction act, oact;
	
	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	
	if(signo == SIGALRM)
	{
#ifdef SA_INTERRUPT
		act.sa_flags |= SA_INTERRUPT;
#endif		
	}
	else
	{
#ifdef SA_RESTART
		act.sa_flags |= SA_RESTART;
#endif	
	}
	
	if(sigaction(signo, &act, &oact) < 0)
	{
		return SIG_ERR;
	}
	
	return oact.sa_handler;
}

void sig_child(int signo)
{
	pid_t pid;
	int stat;
	
	/*pid = wait(&stat);
	_printf("processing signal %d : child process[pid=%d] terminated.\n", signo, pid);*/
	while((pid = waitpid(-1, &stat, WNOHANG)) > 0)
	{
		_printf("processing signal %d : child process[pid=%d] terminated.\n", signo, pid);
	}
}
