#ifndef _U_FLOCK_H_
#define _U_FLOCK_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "umacros.h"

U_BEGIN_DECLS

#define FLK_RW_FLAG             (O_RDWR|O_CREAT)
#define FLK_WO_FLAG             (O_WRONLY|O_CREAT)
#define MANDATORY_FLK_RW_FLAG   (O_RDWR|O_CREAT|O_TRUNC)
#define MANDATORY_FLK_WO_FLAG   (O_WRONLY|O_CREAT|O_TRUNC)

/* flock basic macros */
#define u_flock_rlock(fd, offset, whence, len) \
        u_flock_reg(fd, F_SETLK, F_RDLCK, offset, whence, len)

#define u_flock_rlockw(fd, offset, whence, len) \
        u_flock_reg(fd, F_SETLKW, F_RDLCK, offset, whence, len)

#define u_flock_wlock(fd, offset, whence, len) \
        u_flock_reg(fd, F_SETLK, F_WRLCK, offset, whence, len)

#define u_flock_wlockw(fd, offset, whence, len) \
        u_flock_reg(fd, F_SETLKW, F_WRLCK, offset, whence, len)

#define u_flock_unlock(fd, offset, whence, len) \
        u_flock_reg(fd, F_SETLK, F_UNLCK, offset, whence, len)

#define u_flock_is_rlock(fd, offset, whence, len) \
        u_flock_test(fd, F_RDLCK, offset, whence, len)

#define u_flock_is_wlock(fd, offset, whence, len) \
        u_flock_test(fd, F_WRLCK, offset, whence, len)

extern int u_flock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len);
extern pid_t u_flock_test(int fd, int type, off_t offset, int whence, off_t len);
extern int u_flock_enter_mandatory(int fd);
extern int u_flock_daemon_mutex(char *fname);

U_END_DECLS

#endif

