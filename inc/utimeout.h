#ifndef _U_TIMEOUT_H_
#define _U_TIMEOUT_H_

#include "umacros.h"

U_BEGIN_DECLS

#define u_time_after(a,b)	((long)(b) - (long)(a) < 0)
#define u_time_before(a,b)	 time_after(b,a)

#define u_time_after_eq(a,b)	((long)(a) - (long)(b) >= 0)
#define u_time_before_eq(a,b)	time_after_eq(b,a)

#define CLK_TICKS  (sysconf(_SC_CLK_TCK))

/* the function must be called before any timer function being called. */
int  u_init_timerl(void);
unsigned int  u_timeout_add(unsigned int interval, void (*function)(unsigned long), unsigned long data);
unsigned int  u_timeout_add_once(unsigned int interval, void (*function)(unsigned long), unsigned long data);

/* shouldn't be called in timer routine */
void u_timeout_remove(unsigned int tid);
void u_timerl_destroy(void);

U_END_DECLS

#endif

