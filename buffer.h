#ifndef BUFFER_H_
#define BUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _buffer {
	char  *rptr;
	char  *bufbase;

	size_t misalign;
	size_t totallen;
	size_t off;
} buffer_t;

#define BUFFER_LENGTH(x)   ((x).off)
#define BUFFER_DATA(x)	   ((x).rptr)

#define BUFPTR_LENGTH(x)   ((x)->off)
#define BUFPTR_DATA(x)	   ((x)->rptr)


void   buffer_reset     (buffer_t *buf);
void   buffer_init      (buffer_t *buf, int  sz);
void   buffer_base_free (buffer_t *rptr);
int    buffer_read      (buffer_t *buf, int fd);
void   buffer_drain     (buffer_t *buf, size_t len);
void   buffer_drain_ptr (buffer_t *buf, char *ptr);
char  *buffer_find_chr  (buffer_t *buf, char c);
char  *buffer_find_rchr (buffer_t *buf, char c);
char  *buffer_find      (buffer_t *buf, const char *what, size_t len);

#ifdef __cplusplus
}
#endif

#endif

