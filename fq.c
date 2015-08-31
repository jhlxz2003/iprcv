#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>
#include "ulib.h"
#include "fq.h"

#define  fq_lock(fio)     pthread_mutex_lock(&fio->fmtx)
#define  fq_unlock(fio)   pthread_mutex_unlock(&fio->fmtx)

/* DONE */
static unsigned int
power2_align(unsigned int n)
{
    if (n > 2048)
        return 4096;
    else if (n > 1024)
        return 2048;
    else if (n > 512)
        return 1024;
    else if (n > 256)
        return 512;
    else if (n > 128)
        return 256;
    else if (n > 64)
        return 128;
    else if (n > 32)
        return 64;
    else if (n > 16)
        return 32;
    else if (n > 8)
        return 16;
    else if (n > 4)
        return 8;
    else
        return 4;
}

/* DONE */
struct fq_s*
fq_new(unsigned int sz)
{
    struct fq_s *fq;

    fq = calloc(1, sizeof(struct fq_s));
    if (fq)
    {
        if (pipe(fq->pfd) < 0)
        {
            free(fq);
            return NULL;
        }
        sz = power2_align(sz);
        fq->node = calloc(sz, sizeof(struct fq_node_s));
        if (!fq->node)
        {
            close(fq->pfd[0]);
            close(fq->pfd[1]);
            free(fq);
            return NULL;
        }
        pthread_mutex_init(&fq->fmtx, NULL);
        u_set_nonblock(fq->pfd[0]);
        u_set_nonblock(fq->pfd[1]);
        fq->size = sz;
    }
    return fq;
}

/* DONE */
void
fq_free(struct fq_s *fq)
{
    if (fq)
    {
        close(fq->pfd[0]);
        close(fq->pfd[1]);
        pthread_mutex_destroy(&fq->fmtx);
        free(fq->node);
        free(fq);
    }
}

int
fq_get(struct fq_s *fq, char *buf, int size)
{
	int sz = 0;
    struct fq_node_s *node = NULL;
    fq_lock(fq);
    if (fq->ridx != fq->widx)
    {
        node = &(fq->node[fq->ridx]);
        sz = ((node->size > size) ? size : node->size);
        memcpy(buf, node->buf, sz);
    }
    fq_unlock(fq);
    return sz;
}

/* discard current node and get next node */
int
fq_get_ai(struct fq_s *fq, char *buf, int size)
{
    struct fq_node_s *node = NULL;
    int sz = 0;

    fq_lock(fq);
    if (fq->ridx != fq->widx)
    {
    	fq->ridx = (fq->ridx + 1) & (fq->size - 1);
    	if (fq->ridx != fq->widx)
        {
            node = &fq->node[fq->ridx];
            sz = ((node->size > size) ? size : node->size);
            memcpy(buf, node->buf, sz);
        }
    }
    fq_unlock(fq);
    return sz;
}

#if 0
/* DONE */
#ifdef DEBUG
struct fq_node_s*
fq_get(struct fq_s *fq, int *n)
#else
struct fq_node_s*
fq_get(struct fq_s *fq)
#endif
{
    struct fq_node_s *node = NULL;
    fq_lock(fq);
    if (fq->ridx != fq->widx)
    {
        node = &fq->node[fq->ridx];
    }
#ifdef DEBUG
    *n = ((fq->widx + fq->size - fq->ridx)&(fq->size - 1));
#endif    
    fq_unlock(fq);
    return node;
}

/* DONE */
#ifdef DEBUG
struct fq_node_s*
fq_get_ai(struct fq_s *fq, int *n)
#else
struct fq_node_s*
fq_get_ai(struct fq_s *fq)
#endif
{
    struct fq_node_s *node = NULL;

    fq_lock(fq);
    if (fq->ridx != fq->widx)
    {
    	fq->ridx = (fq->ridx + 1) & (fq->size - 1);
    	if (fq->ridx != fq->widx)
        {
            node = &fq->node[fq->ridx];
        }
    }
#ifdef DEBUG
    *n = ((fq->widx + fq->size - fq->ridx)&(fq->size - 1));
#endif
    fq_unlock(fq);
    return node;
}
#endif

/* DONE */
void
fq_drain(struct fq_s *fq)
{
	fq_lock(fq);
    if (fq->ridx != fq->widx)
    {
        fq->ridx = (fq->ridx + 1) & (fq->size - 1);
    }
    fq_unlock(fq);
}

/* DONE */
void 
fq_put(struct fq_s *fq, char *buf, int size)
{
    int w, len;
    struct fq_node_s *node;
    
    fq_lock(fq);
    w = (fq->widx + 1) & (fq->size - 1);
    if (w == fq->ridx) //buffer full
    {
        fq->ridx = (fq->ridx + 1) & (fq->size - 1);
    }
    node = &fq->node[fq->widx];
    len = (size > 32) ? 32 : size;
    memset(node->buf, 0, 32);
    memcpy(node->buf, buf, len);
    node->size = len;
    fq->widx = w;
    fq_unlock(fq);
}

/* DONE */
void
fq_putcmd(struct fq_s *fq, unsigned char c)
{
    write(fq->pfd[1], &c, 1);
}

unsigned char
fq_getcmd(struct fq_s *fq)
{
    unsigned char c = 0xff;
    if (read(fq->pfd[0], &c, 1) == 1)
        return c;
    return 0xff;
}

