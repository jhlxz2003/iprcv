#ifndef _U_IO_H_
#define _U_IO_H_
#include <sys/types.h>
#include "umacros.h"

U_BEGIN_DECLS

extern int      u_open(char *fname, int oflag, mode_t mode);
extern int      u_close(int fd);
extern int      u_set_nonblock(int fd);
extern int      u_set_block   (int fd);
extern ssize_t  u_read(int fd, void *buf, size_t size);
extern int      u_cp  (int fromfd, int tofd);
extern int      u_copy  (char *s, char *d);
extern int      u_cp1  (int fromfd, int tofd, void *cb, int sz);
extern int      u_copy1(char *s, char *d, void *cb, int sz);
extern ssize_t  u_readn(int fd, void *vptr, size_t n);
extern ssize_t  u_writen(int fd, const void *vptr, size_t n);
extern ssize_t  u_readline(int fd, void *vptr, size_t maxlen);
extern ssize_t  u_readline_buf(void **vptrptr);
extern ssize_t  u_rreadline(int fd, void *vptr, size_t maxlen);

extern int  u_readable_timeo(int fd, int sec);
extern int  u_readable_otimeval(int fd, struct timeval* timeout);
extern int  u_writable_timeo(int fd, int sec);
extern int  u_readable_mtimeo(int fd, int ms);
extern int  u_writable_mtimeo(int fd, int ms);
extern void u_flush_fd(int fd);

extern void     u_set_keypress(void);
extern void     u_reset_keypress(void);

U_END_DECLS

#endif
