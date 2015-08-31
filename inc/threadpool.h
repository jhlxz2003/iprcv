#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*GFunc) (void*);

typedef struct _thread_pool     thread_pool_t;
typedef struct _task  task_t;

task_t *u_task_new(GFunc func, void *data);
void    u_task_free(task_t *tsk);

void   u_thread_pool_init(thread_pool_t* tp, int min_threads, int max_threads);
thread_pool_t* u_thread_pool_new(int min_threads, int max_threads);
int     u_thread_pool_run_task(thread_pool_t *pool, void *data);
void  u_thread_pool_free(thread_pool_t *tp);



#ifdef __cplusplus
}
#endif


#endif

