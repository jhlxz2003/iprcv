#ifndef _U_PING_H
#define _U_PING_H

#ifdef __cplusplus
extern "C" {
#endif

extern unsigned short u_cksum(unsigned short *addr, int len);
extern int            u_ping_prepare(int fd, char *remoteip);
extern int            u_ping (int sockfd, int nsec);
extern int            u_ping_once(char *remoteip, int nsec);

#ifdef __cplusplus
}
#endif

#endif
