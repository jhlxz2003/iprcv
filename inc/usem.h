
#ifndef _USEM_H_
#define _USEM_H_

#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <semaphore.h>

#define u_sem_post(sem)        sem_post(sem)
#define u_sem_destroy(sem)     sem_destroy(sem)

#ifdef __cplusplus
extern "C" {
#endif

union semun{
	int val;
	struct   semid_ds  *buf;
	unsigned short int *array;
};

/* POSIX:XSI/System V Function Wrapper */
int  u_sem_create       (key_t sem_id, sig_atomic_t *readyp);
int  u_sem_create_locked(key_t sem_id, sig_atomic_t *readyp);
int  u_sem_lock         (int sem_id);
int  u_sem_unlock       (int sem_id);
void u_sem_remove       (int sem_id);

/* POSIX:SEM Function Wrapper */
int  u_sem_init       (sem_t *sem);
int  u_sem_init_val   (sem_t *sem, unsigned int val);
int  u_sem_init_locked(sem_t *sem);
int  u_sem_wait       (sem_t *sem);
int  u_sem_trywait    (sem_t *sem);
int  u_sem_get_value  (sem_t *sem);
/*
int  u_sem_post       (sem_t *sem);
int  u_sem_destroy    (sem_t *sem);
*/

/* POSIX:SEM Named Semaphores Function Wrapper */
int  u_sem_get         (char  *name, sem_t **sem, int val);
int  u_sem_close       (sem_t *sem);
int  u_sem_unlink      (char  *name);
int  u_sem_named_remove(char  *name, sem_t *sem);
 
#ifdef __cplusplus
}
#endif

#endif 



