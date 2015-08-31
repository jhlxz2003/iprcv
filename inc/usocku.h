
#ifndef _U_SOCKU_H_
#define _U_SOCKU_H_

#include "umacros.h"

U_BEGIN_DECLS

#define SOCKERR_IO          -1
#define SOCKERR_CLOSED      -2
#define SOCKERR_INVARG      -3
#define SOCKERR_OK          0

int u_socku_serv_listen(const char *name);
int u_socku_serv_accept(int listenfd);
int u_socku_cli_conn(const char *name);
int u_socku_write(int fd, const void* buff, int count);
int u_socku_read(int fd, void* buff, int count);
int u_socketpair(int family, int type, int protocol, int fd[2]);


U_END_DECLS

#endif

 
