#ifndef	_USOCK_H
#define	_USOCK_H

#include	<sys/types.h>	/* basic system data types */
#include	<sys/socket.h>	/* basic socket definitions */
#include	<sys/time.h>	/* timeval{} for select() */
#include	<time.h>		/* timespec{} for pselect() */
#include	<errno.h>
#include	<fcntl.h>		/* for nonblocking */
#include	<netdb.h>
#include	<signal.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<sys/stat.h>	/* for S_xxx file mode constants */
#include	<sys/uio.h>		/* for iovec{} and readv/writev */
#include	<unistd.h>
#include	<sys/wait.h>
#include	<sys/un.h>		/* for Unix domain sockets */
#include  <netinet/if_ether.h>  /* ARPHRD_ETHER and struct arpreq */
#include  <netinet/tcp.h>  /* TCP_KEEPIDLE, TCP_KEEPINTVL, TCP_KEEPCNT */
#include	<netinet/in.h>	/* sockaddr_in{} and other Internet defns */
#include	<arpa/inet.h>	/* inet(3) functions */
#include  <linux/route.h> 
#include	<linux/netlink.h>
#include	<sys/select.h>

#include	<sys/param.h>
//#include	<sys/sysctl.h>

#include	<poll.h>

//#include	<sys/event.h>

#include	<sys/ioctl.h>
//#include	<sys/filio.h>
//#include	<sys/sockio.h>

#include	<pthread.h>

//#include	<net/if_dl.h>
//#include	<netinet/sctp.h>

#ifndef	INADDR_NONE
#define	INADDR_NONE	0xffffffff	/* should have been in <netinet/in.h> */
#endif

#ifndef	SHUT_RD				/* these three POSIX names are new */
#define	SHUT_RD		0	/* shutdown for reading */
#define	SHUT_WR		1	/* shutdown for writing */
#define	SHUT_RDWR	2	/* shutdown for reading and writing */
#endif

/* *INDENT-OFF* */
#ifndef INET_ADDRSTRLEN
#define	INET_ADDRSTRLEN		16	/* "ddd.ddd.ddd.ddd\0" 1234567890123456 */
#endif

/* Define following even if IPv6 not supported, so we can always allocate
   an adequately sized buffer without #ifdefs in the code. */
#ifndef INET6_ADDRSTRLEN
/* $$.Ic INET6_ADDRSTRLEN$$ */
#define	INET6_ADDRSTRLEN	46	/* max size of IPv6 address string:
				   "xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx" or
				   "xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:ddd.ddd.ddd.ddd\0"
				    1234567890123456789012345678901234567890123456 */
#endif
/* *INDENT-ON* */

/* We need the newer CMSG_LEN() and CMSG_SPACE() macros, but few
   implementations support them today.  These two macros really need
    an ALIGN() macro, but each implementation does this differently. */
#ifndef	CMSG_LEN
#define	CMSG_LEN(size)		(sizeof(struct cmsghdr) + (size))
#endif
#ifndef	CMSG_SPACE
#define	CMSG_SPACE(size)	(sizeof(struct cmsghdr) + (size))
#endif

/* POSIX requires the SUN_LEN() macro, but not all implementations DefinE
   it (yet).  Note that this 4.4BSD macro works regardless whether there is
   a length field or not. */
#ifndef	SUN_LEN
/* $$.Im SUN_LEN$$ */
#define	SUN_LEN(su) (sizeof(*(su)) - sizeof((su)->sun_path) + strlen((su)->sun_path))
#endif

/* POSIX renames "Unix domain" as "local IPC."
   Not all systems DefinE AF_LOCAL and PF_LOCAL (yet). */
#ifndef	AF_LOCAL
#define AF_LOCAL	AF_UNIX
#endif
#ifndef	PF_LOCAL
#define PF_LOCAL	PF_UNIX
#endif

/* POSIX requires that an #include of <poll.h> DefinE INFTIM, but many
   systems still DefinE it in <sys/stropts.h>.  We don't want to include
   all the STREAMS stuff if it's not needed, so we just DefinE INFTIM here.
   This is the standard value, but there's no guarantee it is -1. */
#ifndef INFTIM
#define INFTIM          (-1)    /* infinite poll timeout */
/* $$.Ic INFTIM$$ */
#ifdef	HAVE_POLL_H
#define	INFTIM_UNPH				/* tell unpxti.h we defined it */
#endif
#endif

/* Following could be derived from SOMAXCONN in <sys/socket.h>, but many
   kernels still #define it as 5, while actually supporting many more */
#define	LISTENQ		1024	/* 2nd argument to listen() */
#define NETWORK_TMEOUT  5

/* Following shortens all the typecasts of pointer arguments: */
#define	SA	struct sockaddr

#define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
					/* default file access permissions for new files */
#define	DIR_MODE	(FILE_MODE | S_IXUSR | S_IXGRP | S_IXOTH)
					/* default permissions for new directories */

#define IFI_NAME     16            /* same as IFNAMSIZ in <net/if.h> */
#define IFI_HADDR     8            /* allow for 64-bit EUI-64 in future */

struct  ifi_info {
	char     ifi_name[IFI_NAME];   /* interface name, null-terminated */
	short    ifi_index;            /* interface index */
	short    ifi_mtu;              /* interface MTU */
	unsigned char   ifi_haddr[IFI_HADDR];     /* hardware address */
	unsigned short  ifi_hlen;             /* # bytes in hardware address: 0, 6, 8 */
	short    ifi_flags;            /* IFF_xxx constants from <net/if.h> */
	short    ifi_myflags;          /* our own IFI_xxx flags */
	struct  sockaddr *ifi_addr;    /* primary address */
	struct  sockaddr *ifi_brdaddr;     /* broadcast address */
	struct  sockaddr *ifi_dstaddr;     /* destination address */
	struct  ifi_info *ifi_next;    /* next of these structures */
};

#define IFI_ALIAS    1           /* ifi_addr is an alias */

#define PROCNET_ROUTE		"/proc/net/route"
#define PROCNET_RTCACHE	"/proc/net/rt_cache"

#define RT_INF          "/mnt/rtinfo"


unsigned short  u_in_cksum(unsigned short *addr, int len);

/* defined in mcast.c */
int  u_mcast_get_loop(int sockfd);
int  u_mcast_get_ttl(int sockfd);
int  u_mcast_set_if(int sockfd, struct in_addr *addr);
int  u_mcast_set_loop(int sockfd, int onoff);
int  u_mcast_set_ttl(int sockfd, int val);
int  u_mcast_join(int sockfd, struct in_addr grp, struct in_addr ifaddr);
/*
int  u_mcast_join_source_group(int sockfd, struct sockaddr_in *grp,
                          struct sockaddr_in *ifaddr, struct sockaddr_in *src);
int  u_mcast_block_source(int sockfd, struct sockaddr_in *grp, struct sockaddr_in *src);
int  u_mcast_unblock_source(int sockfd, struct sockaddr_in *grp, struct sockaddr_in *src);
*/
int  u_mcast_leave(int sockfd, struct in_addr grp);
//int  u_mcast_leave_source_group(int sockfd, struct sockaddr_in *grp, struct sockaddr_in *src);
int  u_mcast_addr(struct in_addr addr);
int  u_mcast_llocal_addr(struct in_addr addr);
int  u_mcast_routable_addr(struct in_addr addr);
int  u_mcast_set_sendfd(int sockfd, int  ttl, int loop);
int  u_mcast_set_recvfd(int sockfd, unsigned short prt, struct in_addr ia);
/* only for receiving */
int  u_mcast_rcvsock(struct in_addr addr, unsigned short port);
/* only for sending */
int  u_mcast_sndsock();
int  u_mcast_sock(char *localip, unsigned short port);

int    u_mcast_recv_sock(struct in_addr grp, unsigned short port);
void  u_mcast_quit(int fd, struct in_addr grp);

int   u_bad_addr(struct in_addr addr);
int   u_inet_aton(const char *cp, struct in_addr *ap);
char *u_sock_ntop(const struct sockaddr *sa, socklen_t salen);
char *u_sock_ntop_host(const struct sockaddr *sa, socklen_t salen);
char **u_addrs(int *addrtype);
void  u_sock_set_addr(struct sockaddr *sa, socklen_t salen, const void *addr);
void  u_sock_set_port(struct sockaddr *sa, socklen_t salen, int port);
void  u_sock_set_wild(struct sockaddr *sa, socklen_t salen);
int   u_sock_get_port(const struct sockaddr *sa, socklen_t salen);
int   u_sock_cmp_addr(const struct sockaddr *sa1, const struct sockaddr *sa2, socklen_t salen);
int   u_sock_cmp_port(const struct sockaddr *sa1, const struct sockaddr *sa2, socklen_t salen);
int   u_sock_bind_wild(int sockfd, int family);
int   u_sock_family(int sockfd);
int   u_sock_level(int family);
struct addrinfo * u_host_serv(const char *host, const char *serv, int family, int socktype);

int   u_sock_atmark(int fd);

unsigned  u_sock_sndbuf_sz(int socket);
unsigned  u_sock_rcvbuf_sz(int socket);
unsigned  u_sock_set_sndbuf_sz(int socket, unsigned requestedSize);
unsigned  u_sock_set_rcvbuf_sz(int socket, unsigned requestedSize);
unsigned  u_sock_inc_sndbuf_sz(int socket, unsigned requestedSize);
unsigned  u_sock_inc_rcvbuf_sz(int socket, unsigned requestedSize);

int       u_if_set_ip(char *ifname, char *szIp);
int       u_if_get_ip(char* ifname, char *szIp, int len, struct in_addr *ia);
int       u_if_get_mac(char *ifname, char *mac);
int       u_ifarp_get_mac(char *ifname, char *mac);
int       u_if_set_mac(char *ifname, char *mac);
int       u_if_up(char *ifname);
int       u_if_down(char *ifname);
int       u_if_get_mask(char* ifname, char *netmask, int len);
int       u_if_set_mask(char* ifname, char *netmask);
int       u_ifaddr_is_host(char *addr, char *mask);
int       u_ifaddr_is_net(char *addr, char *mask);
int       u_if_def_gw(char *gw);
int       u_route_add(char* ifname, char *gw, char *dst, char *mask, int net);
int       u_route_del(char* ifname, char *gw, char *dst, char *mask, int net);

int       u_if_flags(char *ifname);
int       u_if_is_up(char *ifname);
int       u_if_mcast(char *ifname);

int       u_if_ethtool_is_up(const char *if_name);
int       u_if_mii_is_up(const char *if_name);

void      u_listen(int fd, int backlog);
int       u_tcp_serv(char *ip, short port);
int       u_accept(int listenfd, char *pIp);
int       u_accept_mutex(int listenfd, char *pIp);
int       u_connect(const char *hostip, short port);
int       u_connect_byaddr(struct in_addr ia, short port);
int       connect_nonb(int sockfd, struct sockaddr *saptr, socklen_t salen, int nsec);
int       u_connect_nonb(char *srvip, unsigned short port, int nsec);
int       u_connect_nonb_byaddr(struct in_addr sa, unsigned short port, int nsec);
int       u_tcp_quit(int sockfd);
int       u_tcp_timedwait_close(int connfd, int secs);
int       u_tcp_nowait_close(int connfd);
int       u_tcp_set_nodelay(int connfd);
int       u_udp_sock(char *servip, unsigned short serport);
int       u_tcp_send_blk(int netfd, void *vptr, size_t nSize, int blksz);
int       u_tcp_keepalive(int fd, int idle, int intvl, int cnt);
int       u_sock_get_error(int fd, int *err);
int       u_sock_icmp();

int      u_nl_sock(int proto, int grp);
int      u_nl_recv(int sfd, void *from, char *buf, int  n);

int      u_ntp_sock(char *serip);
int      u_ntp_req(int sockfd);
long     u_ntp_ack(int sockfd, struct timeval *tv);

#endif

