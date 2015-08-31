#ifndef U_TTY_H
#define U_TTY_H

#include "umacros.h"
U_BEGIN_DECLS

int     u_tty_cbreak(int fd, int  bytes, int tm);
int     u_tty_raw(int fd, int  bytes, int tm);
int     u_tty_reset(int fd);

void   u_tty_atexit(void);
int     u_tty_set_baud(int fd, int rate);
int     u_tty_set_fomat (int fd, int databits, int stopbits, int parity);
int     u_tty_set_fc(int fd, int control, int enable);

U_END_DECLS

#endif

