
#ifndef _U_SHMEM_H_
#define _U_SHMEM_H_

#include "umacros.h"

U_BEGIN_DECLS
#define  DFT_SHM_SEM_ID   999
int   u_shm_init    (int key, int size);
void  u_shm_uninit  ();
int   u_shm_detach  (void *shmaddr);
int   u_shm_remove  (int shmid, void *shmaddr);
int   u_shm_read    (char* pDest, int iOffset, int iLen);
int   u_shm_write   (char* pSource, int iOffset, int iLen);
char* u_shm_get_addr();

U_END_DECLS

#endif 



 
