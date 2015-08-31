#ifndef __U_RWLOCK_H__
#define __U_RWLOCK_H__

#include "umacros.h"

U_BEGIN_DECLS

typedef struct _rwLock {
	ULOCK	rlock;
	ULOCK	wlock;
	unsigned int rcnt;
} URWLOCK;

#define u_rwlock_init(name)  do {      \
	pthread_mutex_init(&((name).rlock), NULL);   \
	pthread_mutex_init(&((name).wlock), NULL);   \
	(name).rcnt = 0;           \
} while (0)

URWLOCK* u_rwlock_new();
void	 u_rwlock_destroy(URWLOCK* rwlock);
void	 u_rwlock_rlock(URWLOCK *rwlock);
void	 u_rwlock_wlock(URWLOCK *rwlock);
void	 u_rwlock_unrlock(URWLOCK *rwlock);
void	 u_rwlock_unwlock(URWLOCK *rwlock);

#define URWLOCK_INITIALIZER   {PTHREAD_MUTEXT_INITIALIZER,PTHREAD_MUTEXT_INITIALIZER,0}

#define U_RWLOCK_NAME(name)   u__ ## name ## _rwlock

#define U_RWLOCK_DEFINE_STATIC(name)  \
        static URWLOCK U_RWLOCK_NAME(name) = URWLOCK_INITIALIZER

#define U_RWLOCK_DEFINE(name) URWLOCK U_RWLOCK_NAME(name)
#define U_RWLOCK_INIT(name)   u_rwlock_init(U_RWLOCK_NAME(name))

#define U_RWLOCK_RD(name)      u_rwlock_rlock(&U_RWLOCK_NAME(name))
#define U_UNRWLOCK_RD(name)    u_rwlock_unrlock(&U_RWLOCK_NAME(name))
#define U_RWLOCK_WR(name)      u_rwlock_wlock(&U_RWLOCK_NAME(name))
#define U_UNRWLOCK_WR(name)    u_rwlock_unwlock(&U_RWLOCK_NAME(name))

U_END_DECLS

#endif

