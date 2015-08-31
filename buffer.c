#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>

#include "ulib.h"
#include "buffer.h"


extern void *memrchr(const void *s, int c, size_t n);

void
buffer_reset(buffer_t *buf)
{
	buf->rptr = buf->bufbase;
	buf->misalign = 0;
	buf->off = 0;
}

void
buffer_init(buffer_t *buf, int  sz)
{
	if (sz > 0)
	{
		buf->bufbase = calloc(sz, 1);
		buf->totallen = sz;
	}
	buffer_reset(buf);
}

void
buffer_base_free(buffer_t *buf)
{
	if (buf->bufbase != NULL)
		free(buf->bufbase);
}

static void
buffer_align(buffer_t *buf)
{
	memmove(buf->bufbase, buf->rptr, buf->off);
	buf->rptr = buf->bufbase;
	buf->misalign = 0;
}

static int
buffer_expand(buffer_t *buf, size_t datlen)
{
	size_t need = buf->misalign + buf->off + datlen;

	if (buf->totallen >= need)
		return (0);

	if (buf->misalign >= datlen)
	{
		TRACE("--- the misalignment fulfills our data needs ---\n");
		buffer_align(buf);
	}
	else
	{
		void *newbuf;
		size_t length = buf->totallen;

		if (length < 256)
			length = 256;
		while (length < need)
			length <<= 1;

		if (buf->bufbase != buf->rptr)
			buffer_align(buf);
		if ((newbuf = realloc(buf->rptr, length)) == NULL)
		{
		    TRACE("--- realloc failed ---\n");
			return (-1);
		}
		TRACE("--- realloc to %d ---\n", length);
		buf->bufbase = buf->rptr = newbuf;
		buf->totallen = length;

	    return 0;
	}

	return (0);
}

#ifdef DEBUG
static void
buffer_dump(char *p, int n)
{
	int i;
    printf("--- received: ");
    for (i=0; i<n; ++i)
    {
    	printf("%x ", p[i]);
    }
    printf(" ---\n");
}
#endif

/*
 * Reads data from a file descriptor into a buffer.
 */
int
buffer_read(buffer_t *buf, int fd)
{
	char *p;
	int   n;


	if (ioctl(fd, FIONREAD, &n) < 0)
	{
	    TRACE("--- ioctl:FIONREAD error ---\n");
	    buffer_reset(buf);
	    return -1;
	}
	if (n <= 0) return 0;

	TRACE("------ RECVBUF HAS %d BYTES --------\n", n);
	if (buffer_expand(buf, n) == -1)
	{
		return (0);
	}

	p = buf->rptr + buf->off;
	n = read(fd, p, n);
	if (n > 0)
	{
		TRACE("------ RECEIVED %d BYTES --------\n", n);
	    buf->off += n;
	#ifdef DEBUG
	    buffer_dump(p, n);
	#endif
	}
    return n;
}

/* DONE */
/* called when read data from buffer */
void
buffer_drain(buffer_t *buf, size_t len)
{
	if (len >= buf->off)
	{
		buf->off = 0;
		buf->rptr = buf->bufbase;
		buf->misalign = 0;
	}
    else
    {
	    buf->rptr += len;
	    buf->misalign += len;
	    buf->off -= len;
	}
}

void
buffer_drain_ptr(buffer_t *buf, char *ptr)
{
    int len;
    len = ptr - buf->rptr;
    if (ptr - buf->rptr >= buf->off)
	{
		buf->off = 0;
		buf->rptr = buf->bufbase;
		buf->misalign = 0;
	}
    else
    {
	    buf->rptr = ptr;
	    buf->misalign += len;
	    buf->off -= len;
	}
}

char *
buffer_find_chr(buffer_t *buf, char c)
{
	return memchr(buf->rptr, (int)c, buf->off);
}

char *
buffer_find_rchr(buffer_t *buf, char c)
{
	return memrchr(buf->rptr, (int)c, buf->off);
}

char *
buffer_find(buffer_t *buf, const char *what, size_t len)
{
	char *search = buf->rptr, *end = search + buf->off;
	char *p;

	while (search < end &&
	    (p = memchr(search, *what, end - search)) != NULL) {
		if (p + len > end)
			break;
		if (memcmp(p, what, len) == 0)
			return (p);
		search = p + 1;
	}

	return (NULL);
}
