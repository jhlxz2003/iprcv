#ifndef FQ_H_
#define FQ_H_

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FQ_NODE_SZ   32

struct fq_node_s {
    char buf[FQ_NODE_SZ];
    int  size;
};

struct fq_s {
    int  pfd[2];
	pthread_mutex_t fmtx;
	struct fq_node_s *node;
	int  size;
	int  ridx;
	int  widx;
};

typedef struct fq_node_s  fq_node_t;
typedef struct fq_s       fq_t;

#define FQ_NODE_DATA(node)    ((node)->buf)
#define FQ_NODE_SIZE(node)    ((node)->size)

#define fq_fdin(fq)     ((fq)->pfd[0])
#define fq_fdout(fq)    ((fq)->pfd[1])

fq_t         *fq_new   (unsigned int sz);
void          fq_free  (fq_t *fq);

#if 0
#ifdef DEBUG
fq_node_t    *fq_get   (fq_t *fq, int *n);
#else
fq_node_t    *fq_get   (fq_t *fq);
#endif

#ifdef DEBUG
fq_node_t    *fq_get_ai(fq_t *fq, int *n);
#else
fq_node_t    *fq_get_ai(fq_t *fq);
#endif
#endif

int           fq_get   (fq_t *fq, char *buf, int size);
int           fq_get_ai(fq_t *fq, char *buf, int size);

void          fq_put   (fq_t *fq, char *buf, int size);
void          fq_drain (fq_t *fq);
void          fq_putcmd(fq_t *fq, unsigned char c);
unsigned char fq_getcmd(fq_t *fq);

#ifdef __cplusplus
}
#endif

#endif

