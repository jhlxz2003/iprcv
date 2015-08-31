#ifndef U_SIGNAL_H
#define U_SIGNAL_H

typedef	void	Sigfunc(int);	/* for signal handlers */

#ifdef __cplusplus
extern "C" {
#endif

Sigfunc *u_signal(int signo, Sigfunc *func);
Sigfunc *u_signal_intr(int signo, Sigfunc *func);

#ifdef __cplusplus
}
#endif

#endif
