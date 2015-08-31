#ifndef _U_MQ_H_
#define _U_MQ_H_

#include "umacros.h"
#include "ulist.h"
#include <semaphore.h>

#define MSG_HEAP_SIZE    1024
#define MSGQ_MAX_NUM     40
#define MSGSLOT_SIZE     512

typedef void (*CB_FUNC)(void *);

typedef struct _UMSG {
	void*         obj;
	unsigned int	message;
	unsigned int	wParam;
	unsigned long	lParam;
	void*	        pData;
} UMSG;
typedef UMSG* PUMSG;

typedef struct _UMsgSlot {
	UMSG   msg[MSGSLOT_SIZE];
	int    irptr;
	int    iwptr;
} UMsgSlot;

struct _UMsgQueue
{
	void*            obj;
	pthread_mutex_t  mutex;
	sem_t		         sem;
	UListHdr        *head[3];
	UListHdr         timer;
	UListHdr         sync;
	UListHdr         ntf;
	UMsgSlot	       slot;
	int              quit;
	CB_FUNC          exit_func;
	void            *data;
};

typedef struct _UMsgQueue  UMsgQueue;
typedef UMsgQueue* 	PUMsgQueue;

#define u_msg_value(msg)    ((msg)->message)
#define u_msg_wParam(msg)   ((msg)->wParam)
#define u_msg_lParam(msg)   ((msg)->lParam)
#define u_msg_target(msg)   ((msg)->obj)

U_BEGIN_DECLS

/* the following 2 functions must be 
   called before create message queue. */
int  u_mq_prepare();
void u_mq_finish ();
void u_mq_atexit (PUMsgQueue mq, CB_FUNC func, void *data);

/* the following functions must be called 
   after above functions being called. */
PUMsgQueue  u_mq_new();
void        u_mq_destroy (PUMsgQueue pMq);
int         u_mq_get_msg (PUMSG pMsg, PUMsgQueue msgqueue);
int         u_mq_post_msg(PUMsgQueue mq, PUMSG pMsg);
int         u_mq_ntf_msg (PUMsgQueue mq, PUMSG pMsg);
int         u_mq_ntf_sync_msg(PUMsgQueue mq, PUMSG pMsg);
/* called after handling the sync msg */
int         u_mq_sync_msg_return(PUMSG msg, int retval);
int         u_mq_snd_timer_msg(PUMsgQueue mq, PUMSG pMsg);
int         u_mq_ntf_quit(PUMsgQueue mq);

U_END_DECLS

#endif



