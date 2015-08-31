#ifndef __U_SYNQ_H__
#define __U_SYNQ_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _u_synq u_synq_t;

u_synq_t*   u_synq_new(void);
void   u_synq_lock(u_synq_t *q);
void   u_synq_unlock(u_synq_t *q);

void   u_synq_push(u_synq_t* q, void *data);
void   u_synq_push_unlocked(u_synq_t* q, void *data);

void   *u_synq_pop(u_synq_t* q);
void   *u_synq_pop_unlocked (u_synq_t* q);
void   *u_synq_try_pop(u_synq_t* q);
void   *u_synq_try_pop_unlocked(u_synq_t* q);

void   *u_synq_timed_pop(u_synq_t* q, struct timespec *end_time);
void   *u_synq_timed_pop_unlocked(u_synq_t* q, struct timespec *end_time);
int     u_synq_nwaitings(u_synq_t* q);

#ifdef __cplusplus
}
#endif

#endif

