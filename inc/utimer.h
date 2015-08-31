#ifndef __U_TIMER_H__
#define __U_TIMER_H__

#include "umacros.h"

U_BEGIN_DECLS

//#define THREAD_SUPPORTED     1

/* microseconds per second */
typedef struct timeval      UTimeVal;
typedef struct timespec     UTimeSpec;

#ifndef INT_MAX
#define INT_MAX	0x7FFFFFFF
#endif

#define U_USEC_PER_SEC 1000000

#define GETTIMEVAL(timeval)  gettimeofday (&(timeval), NULL)
#define GETTIMESPEC(timespec)  do { \
	UTimeVal   now;                           \
	gettimeofday(&now, NULL);                 \
	(timespec).tv_sec = now.tv_sec;           \
	(timespec).tv_nsec = now.tv_usec*1000;    \
} while(0)

/* Operations on timevals. */
#define	u_timval_isset(tvp)	((tvp)->tv_sec || (tvp)->tv_usec)
#define	u_timval_clear(tvp)	(tvp)->tv_sec = (tvp)->tv_usec = 0

#define	u_timval_cmp(tvp, uvp, cmp)		\
	(((tvp)->tv_sec == (uvp)->tv_sec) ?	      \
	((tvp)->tv_usec cmp (uvp)->tv_usec) :    \
	((tvp)->tv_sec cmp (uvp)->tv_sec))

#define	TIMEVAL_TO_TIMESPEC(tv, ts) {					\
	(ts)->tv_sec = (tv)->tv_sec;					\
	(ts)->tv_nsec = (tv)->tv_usec * 1000;				\
}
#define	TIMESPEC_TO_TIMEVAL(tv, ts) {					\
	(tv)->tv_sec = (ts)->tv_sec;					\
	(tv)->tv_usec = (ts)->tv_nsec / 1000;				\
}

#define	u_timeradd(tvp, uvp, vvp)						\
	do {								\
		(vvp)->tv_sec = (tvp)->tv_sec + (uvp)->tv_sec;		\
		(vvp)->tv_usec = (tvp)->tv_usec + (uvp)->tv_usec;	\
		if ((vvp)->tv_usec >= 1000000) {			\
			(vvp)->tv_sec++;				\
			(vvp)->tv_usec -= 1000000;			\
		}							\
	} while (0)
#define	u_timersub(tvp, uvp, vvp)						\
	do {								\
		(vvp)->tv_sec = (tvp)->tv_sec - (uvp)->tv_sec;		\
		(vvp)->tv_usec = (tvp)->tv_usec - (uvp)->tv_usec;	\
		if ((vvp)->tv_usec < 0) {				\
			(vvp)->tv_sec--;				\
			(vvp)->tv_usec += 1000000;			\
		}							\
	} while (0)

/* Operations on timespecs. */
#define	u_timespec_clear(tsp)		(tsp)->tv_sec = (tsp)->tv_nsec = 0
#define	u_timespec_isset(tsp)		((tsp)->tv_sec || (tsp)->tv_nsec)
#define	u_timespec_cmp(tsp, usp, cmp)					\
	(((tsp)->tv_sec == (usp)->tv_sec) ?				\
	    ((tsp)->tv_nsec cmp (usp)->tv_nsec) :			\
	    ((tsp)->tv_sec cmp (usp)->tv_sec))

#define	u_timespec_add(tsp, usp, vsp)					\
	do {								\
		(vsp)->tv_sec = (tsp)->tv_sec + (usp)->tv_sec;		\
		(vsp)->tv_nsec = (tsp)->tv_nsec + (usp)->tv_nsec;	\
		if ((vsp)->tv_nsec >= 1000000000L) {			\
			(vsp)->tv_sec++;				\
			(vsp)->tv_nsec -= 1000000000L;			\
		}							\
	} while (0)
#define	u_timespec_sub(tsp, usp, vsp)					\
	do {								\
		(vsp)->tv_sec = (tsp)->tv_sec - (usp)->tv_sec;		\
		(vsp)->tv_nsec = (tsp)->tv_nsec - (usp)->tv_nsec;	\
		if ((vsp)->tv_nsec < 0) {				\
			(vsp)->tv_sec--;				\
			(vsp)->tv_nsec += 1000000000L;			\
		}							\
	} while (0)

void    u_usleep(unsigned long  microseconds);
void    u_time_val_add(UTimeVal *time_, long  microseconds);
void    u_time_spec_add (UTimeSpec *time_, long microseconds);


UTimeVal u_timeval_diff(struct timeval tm1, struct timeval tm2);
UTimeVal u_timeval_add(struct timeval tm1, struct timeval tm2);
UTimeVal u_timeval_add1(UTimeVal *time_, long microseconds);
UTimeVal u_timeval_untill(struct timeval futureEvent);
int      u_timeval_gt(struct timeval tm1, struct timeval tm2);

U_END_DECLS

#endif

