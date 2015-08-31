/* writen by LXZ in 2008.04.23 */
#ifndef _U_MEM_H
#define _U_MEM_H

#include <pthread.h>
#include "umacros.h"
#include "ulist.h"

U_BEGIN_DECLS

typedef struct _node {
	struct _node *next;
} Node;

typedef Node*  PNode;

#define  NodeSize  sizeof(Node)
typedef struct _UMemChunk {
	void*			pData;
	pthread_mutex_t mutex;
	unsigned long	BlockNumber;
	unsigned long	BlockSize; /* include NodeSize */
	unsigned long	FreeCnt;
	volatile int	UsedCnt;
	int             flag;
	PNode		    pFree;
} UMemChunk;
typedef UMemChunk*  PUMemChunk;

#define UNPAYLOAD   NodeSize

/* dynamic fixed size chunk */
typedef struct _UMap {
	void  *pData;  /* point to block memory */
} UMap;
typedef UMap*  PUMap;

/* Dynamic Memory Chunk */
#define	MAP_MAX       10
typedef struct _UDMemChunk {
	UMap		    map[MAP_MAX];
	int             pos;
	unsigned long	blknum;
	unsigned long	blksize;
	unsigned long	FreeCnt;
	volatile int	UsedCnt;
	int             flag;
	pthread_mutex_t mutex;
	UListHdr    	FreeList;
} UDMemChunk;

typedef UDMemChunk* PUDMemChunk;
#define DUNPAYLOAD  UListHdrSize

/* normal memory chunk function:fixed blksize and fixed blkcount */
/* pChunk:point to static structure */
PUMemChunk  u_mem_chunk_new(unsigned long BlkNum, unsigned long BlkSize);
int  u_mem_chunk_init(PUMemChunk pChunk, unsigned long BlkNum, unsigned long BlkSize);
void* u_mem_alloc(PUMemChunk pChunk, unsigned int size);
void  u_mem_free(PUMemChunk pChunk, void* pData);
void  u_mem_chunk_free(PUMemChunk pChunk);

/* Dynamic memory chunk function */
/* pChunk:point to static structure */
PUDMemChunk  u_dmem_chunk_new(unsigned long BlkNum, unsigned long BlkSize);
int  u_dmem_chunk_init(PUDMemChunk pChunk, unsigned long BlkNum, unsigned long BlkSize);
void* u_dmem_alloc(PUDMemChunk pChunk, unsigned int size);
void  u_dmem_free(PUDMemChunk pChunk, void* pData);
void  u_dmem_chunk_free(PUDMemChunk pChunk);

U_END_DECLS


#endif 
