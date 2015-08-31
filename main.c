#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "ulib.h"
#include "fq.h"
#include "buffer.h"

#if 0
#define TCP_KEEP_ALIVE_IDLE   60
#define TCP_PROBE_ITVL        10
#define TCP_PROBE_CNT         3
#endif

#define MAX_CLIENT       4096
#define HB_TMEO          300

#define UART_DEV     "/dev/ttyS2"
#define NET_CONF     "/davinci/net.conf"

#define ALARM_CACHE_MAX    4096
#define CMD_CACHE_MAX      256

#define DEF_PRT           8888

#define UART_ACK_TMEO    (1.0)
#define UART_MAX_TRY      1

#define MAX_USER_ID       10000

#define FQ_CMD_DATAIN     ((unsigned char)1)
#define FQ_CMD_QUIT       ((unsigned char)0)

struct _ev_ntmeo;

typedef struct _ev_nio {
    UListHdr  list;
    ev_io  watcher;
    struct _ev_ntmeo *tev;
    int    fd;
    int    id;
    buffer_t buff;
    char   *rptr;
    char   *tail;
} ev_nio_t;

typedef struct _ev_ntmeo {
	UListHdr  list;
	ev_timer  watcher;
	ev_nio_t *nio;
} ev_ntmeo_t;

typedef int (*FQ_GET_FUNC)(fq_t *fq, char *buf, int size);

static FQ_GET_FUNC  fq_get_ptr[2] = {fq_get, fq_get_ai};

/////////////////////////////////////////
//            about uart io            //
/////////////////////////////////////////
static struct ev_loop *g_loop;
static ev_io      inq_watcher;
static ev_io      ruart_watcher;
static ev_timer   uart_tmeo_watcher;

static int  repeat_counter = 0;

static int       uart_fd = -1;
static char     *uart_rptr = NULL;
static char     *uart_tail = NULL;
static buffer_t  uart_buf;

static char uart_obuf[FQ_NODE_SZ] = {0};
static int  uart_obuf_sz;

#define uart_buf_reset()  \
    do {   \
    uart_rptr = NULL; \
    uart_tail = NULL; \
    buffer_reset(&uart_buf); \
    } while(0)

/////////////////////////////////////////
//             about net io            //
/////////////////////////////////////////
static unsigned short  g_prt = DEF_PRT;

static char  hw_ipaddr[16] = {0};
static char  hw_msk[16] = {0};
static char  hw_mac[6] = {0};
static char  hw_gw[16] = {0};

static char  g_ipaddr[16] = {0};
static char  g_msk[16] = {0};
static char  g_mac[6] = {0};
static char  g_gw[16] = {0};
static int   hb_tmeo = 3;

static int   g_nl = 0;
static pthread_mutex_t  nl_mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t   nl_cnd = PTHREAD_COND_INITIALIZER;

#define nl_lock()     pthread_mutex_lock(&nl_mtx)
#define nl_unlock()   pthread_mutex_unlock(&nl_mtx)

#define nl_p()        pthread_cond_wait(&nl_cnd, &nl_mtx)
#define nl_v()        pthread_cond_signal(&nl_cnd)

static int    listenFd = -1;
static struct ev_loop *nl_loop = NULL;
static ev_io  nio_watcher;
static ev_io  outq_watcher;

/* ev_nio list */
static UListHdr  free_list = {&free_list, &free_list};
static UListHdr  g_watcher_list = {&g_watcher_list, &g_watcher_list};
static ev_nio_t *ev_arr[MAX_USER_ID] = {0};
static int       g_iocnt = 0;

/* ev_ntmeo list */
static UListHdr  tmeo_free_list = {&tmeo_free_list, &tmeo_free_list};
static UListHdr  g_tmeo_list = {&g_tmeo_list, &g_tmeo_list};
static int       g_tmeocnt = 0;

#define ev_nio_buf_reset(ev)  \
    do {   \
    (ev)->rptr = NULL; \
    (ev)->tail = NULL; \
    buffer_reset(&(ev)->buff); \
    } while(0)

static fq_t  *g_inq = NULL;
static fq_t  *g_outq = NULL;

static void  listen_cb(struct ev_loop *loop, ev_io *w, int revents);

/*
static inline void
reset_ether(void)
{
	u_if_down("eth0");
	u_msleep(10);
	u_if_up("eth0");
}
*/

/* DONE */
static int
is_valid_unip(char *val)
{
	struct in_addr ia;
	if (inet_aton(val, &ia) == 0) return 0;
	return (!u_bad_addr(ia)&&!u_mcast_addr(ia));
}

/* DONE */
static void
get_def_netinfo(void)
{
	char tmp[16];
    
	memset(tmp, 0, 16);
	if (u_if_get_ip(NULL, tmp, 16, NULL) == 0)
	{
		memcpy(hw_ipaddr, tmp, 16);
		TRACE("******** hardware ip = %s *******\n", hw_ipaddr);
	}

	memset(tmp, 0, 16);
	if (u_if_get_mask(NULL, tmp, 16) == 0)
	{
		memcpy(hw_msk, tmp, 16);
		TRACE("******** hardware netmask = %s *******\n", hw_msk);
	}
	
	memset(tmp, 0, 16);
	if (u_if_get_mac(NULL, tmp) == 0)
	{
#ifdef DEBUG
		int i;
		printf("------- hardware macaddr = %02x:", tmp[0]);
		for (i=1; i<6; ++i)
		{
			printf("%02x:", tmp[i]);
		}
		printf(" -------\n");
#endif
		memcpy(hw_mac, tmp, 6);
	}

	memset(tmp, 0, 16);
	if (u_if_def_gw(tmp) == 0)
	{
		memcpy(hw_gw, tmp, 16);
		TRACE("------ default gateway = %s ------\n", hw_gw);
	}
}

static void
config_netif(void)
{
	u_if_set_ip(NULL, g_ipaddr);
	u_if_set_mask(NULL, g_msk);
	u_if_down("eth0");
	u_if_set_mac("eth0", g_mac);
	u_if_up("eth0");
	u_route_add(NULL, g_gw, "0.0.0.0", "0.0.0.0", 1);
}

static void
load_net_conf(void)
{
	FILE *fp;
	char  buf[256];
	char  key[32];
	char  value[128];
	char *addr = NULL;
	char *ptr;
	int   gw = 0;

	if ((fp = fopen(NET_CONF, "r")) == NULL) return;				
	while(!feof(fp))
	{
		memset(buf, '\0', 256);
		memset(key, '\0', 32);
		memset(value, '\0', 128);	
		fgets(buf, 256, fp);
		
		ptr = buf;
		while (*ptr!='\0'&&isspace(*ptr))
			++ptr;
		if (!isalpha(*ptr))continue;
		
		addr = strchr(ptr, '=');
		if (addr == NULL) continue;
		*addr = ' ';
		if (sscanf(ptr, "%s %s", key, value) != 2) continue;

		TRACE("key = %s, value = %s\n", key, value);
		if (strncasecmp("ipaddr", key, 6) == 0)
		{
			if (strlen(value) > 0&&is_valid_unip(value))
			{
				strncpy(g_ipaddr, value, 15);
				if (strcmp(g_ipaddr, hw_ipaddr))
				{
				    u_if_set_ip(NULL, g_ipaddr);
				}
			}
		}
		else if (strncasecmp("port", key, 4) == 0)
		{
		    if (strlen(value) > 0)
			{
			    g_prt = atoi(value);
			}
		}
		else if(strncasecmp("netmask", key, 7) == 0)
		{
			if (strlen(value) > 0)
			{
				strncpy(g_msk, value, 15);
				if (strcmp(g_msk, hw_msk))
				{
				    u_if_set_mask(NULL, g_msk);
				}
			}
		}
		else if(strncasecmp("gateway", key, 7) == 0)
		{
			if (strlen(value) > 0)
			{
				strncpy(g_gw, value, 15);
				if (strcmp(g_gw, hw_gw))
				{
				   gw = 1;
				}
			}
		}
		else if(strncasecmp("macaddr", key, 7) == 0)
		{
			if (strlen(value) > 0)
			{
				
			}
		}
		else if(strncasecmp("hbtmeo", key, 6) == 0)
		{
			if (strlen(value) > 0)
			{
				hb_tmeo = atoi(value);
				TRACE("--- heartbeat = %d ---\n", hb_tmeo);
			}
		}
	}
	fclose(fp);
	if (gw == 1)
	{
		u_route_add(NULL, g_gw, "0.0.0.0", "0.0.0.0", 1);
	}
}

/* DONE */
static void
store_net_conf(void)
{	
	FILE *fp = NULL;

	if ((fp = fopen(NET_CONF, "w")) != NULL)
	{
		fprintf(fp, "ipaddr = %s\n", g_ipaddr);
		fprintf(fp, "port = %d\n", g_prt);
		fprintf(fp, "netmask = %s\n", g_msk);
		fprintf(fp, "gateway = %s\n", g_gw);
		fprintf(fp, "macaddr = %x:%x:%x:%x:%x:%x\n", g_mac[0], g_mac[1], g_mac[2], g_mac[3], g_mac[4], g_mac[5]);
		fprintf(fp, "hbtmeo = %d\n", hb_tmeo);
		fflush(fp);
		fclose(fp);
	}
}

//////////////////////////////////////////////
//                net_io thread             //
//////////////////////////////////////////////
#if 0
{
#endif

/* DONE */
static ev_ntmeo_t*
ev_ntmeo_new(void)
{
	ev_ntmeo_t* ev = NULL;
    
	if (u_list_empty(&tmeo_free_list))
	{
		if (g_tmeocnt >= MAX_CLIENT) return NULL;
		ev = calloc(1, sizeof(ev_ntmeo_t));
		if (ev != NULL)
		{
			U_INIT_LIST_HEAD(&ev->list);
		}
		++g_tmeocnt;
		TRACE("--------- timer node count = %d -----------\n", g_tmeocnt);
	}
	else
	{
		ev = (ev_ntmeo_t*)(tmeo_free_list.next);
		u_list_del(&ev->list);
		U_INIT_LIST_HEAD(&ev->list);
	}
    return ev;
}

/* DONE */
static void
ev_ntmeo_free(ev_ntmeo_t *ev)
{
    if (!u_list_empty(&ev->list))
    {
        u_list_del(&ev->list);
    }
    U_INIT_LIST_HEAD(&ev->list);
    u_list_append(&ev->list, &tmeo_free_list);
    TRACE("--- ev_ntmeo_free ---\n");
}

#define ev_ntmeo_add(ev)      u_list_append(&(ev)->list, &g_tmeo_list)
#define ev_ntmeo_remove(ev)   \
    do { \
        ev_timer_stop(nl_loop, &((ev)->watcher)); \
        ev_ntmeo_free(ev); \
    } while(0)

/* DONE */
static void
ev_ntmeo_cleanup(void)
{
	ev_ntmeo_t *ev;

	while (!u_list_empty(&g_tmeo_list))
	{
		ev = (ev_ntmeo_t*)(g_tmeo_list.next);
		ev_ntmeo_remove(ev);
	}
}

static void
ev_nio_remove_timer(ev_nio_t *ev)
{
	if (ev->tev != NULL)
	{
		ev_ntmeo_remove(ev->tev);
		ev->tev = NULL;
	}
}

/* DONE */
static ev_nio_t*
ev_nio_new(void)
{
	ev_nio_t* ev = NULL;
    
	if (u_list_empty(&free_list))
	{
		if (g_iocnt >= MAX_CLIENT) return NULL;
		ev = calloc(1, sizeof(ev_nio_t));
		if (ev != NULL)
		{
			U_INIT_LIST_HEAD(&ev->list);
			buffer_init(&ev->buff, 256);
			ev->id = -1;
		}
		++g_iocnt;
		TRACE("--------- nio node count = %d -----------\n", g_iocnt);
	}
	else
	{
		ev = (ev_nio_t*)(free_list.next);
		u_list_del(&ev->list);
		ev_nio_buf_reset(ev);
		U_INIT_LIST_HEAD(&ev->list);
		ev->id = -1;
	}
	return ev;
}

/* DONE */
static void
ev_nio_free(ev_nio_t *ev)
{
	if (!u_list_empty(&ev->list))
	{
		u_list_del(&ev->list);
	}
	ev_nio_buf_reset(ev);
	if (ev->id >= 0)
	{
		ev_arr[ev->id] = NULL;
	}
	ev->id = -1;
	U_INIT_LIST_HEAD(&ev->list);
	u_list_append(&ev->list, &free_list);
}

#define ev_nio_add(ev)      u_list_append(&(ev)->list, &g_watcher_list)
/*
#define ev_nio_remove(ev)   \
    do { \
        ev_io_stop(nl_loop, &((ev)->watcher)); \
//        u_tcp_nowait_close((ev)->fd); \
        close((ev)->fd); \
        ev_nio_free(ev); \
    } while(0)
*/

static void
ev_nio_remove(ev_nio_t *ev)
{
	ev_io_stop(nl_loop, &(ev->watcher));
        close((ev)->fd);
        TRACE("---- kick off id = %d ----\n", ev->id);
        ev_nio_free(ev);
}

/* DONE */
static void
ev_nio_cleanup(void)
{
	ev_nio_t *ev;

	while (!u_list_empty(&g_watcher_list))
	{
		ev = (ev_nio_t*)(g_watcher_list.next);
		ev_nio_remove(ev);
	}
	memset(ev_arr, 0, sizeof(ev_arr));
}

static void
ntmeo_cb(struct ev_loop *loop, ev_timer *w, int revents)
{
	ev_ntmeo_t *tev;
	ev_nio_t  *ev;

	tev = U_CONTAINER_OF(w, struct _ev_ntmeo, watcher);
	TRACE("--- NOT RECEIVED HEARTBEAT and close the socket ---\n");
	ev = tev->nio;
	ev_nio_remove(ev);
	ev_timer_stop(loop, w);
	ev_ntmeo_free(tev);
}

static void
ev_nio_timer_add(ev_nio_t *ev)
{
	ev_ntmeo_t *tev;

	tev = ev_ntmeo_new();
	if (tev == NULL)
	{
		TRACE("--- ev_ntmeo_new error ---\n");
		ev_nio_remove(ev);
		return;
	}

	ev->tev = tev;
	tev->nio = ev;
	ev_timer_init(&tev->watcher, ntmeo_cb, hb_tmeo*60+10, 0);
	ev_timer_start(nl_loop, &tev->watcher);
	ev_ntmeo_add(tev);
	TRACE("--- add tcp timeout timer ---\n");
}

/* DONE */
static int
msg_id(char *buf, char *id)
{
    char *ptr = buf;

    ++ptr;
    if (id != NULL)
    {
        memcpy(id, ptr, 4);
    }
    return (ptr[0]*1000+ptr[1]*100+ptr[2]*10+ptr[3]);
}

static int
handle_netmsg(ev_nio_t *ev)
{
	int id;
	char ack[] = {0xee, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0xdd};

	id = msg_id(ev->rptr, &ack[1]);
	if (id >= 0&&id < MAX_USER_ID)
	{
		if (ev->id == -1)
		{
			if (ev_arr[id] != NULL)
			{
				TRACE("--- the ID EXISTED ---\n");
				ev_nio_remove_timer(ev_arr[id]);
				ev_nio_remove(ev_arr[id]);
			}
			ev->id = id;
			ev_arr[id] = ev;
		}
		else if (ev->id != id)
		{
			TRACE("--- ID MISMATCHED ---\n");
			ev_nio_remove(ev);
			return (-1);
		}

		if (u_writen(ev->fd, ack, sizeof(ack)) < 0)
		{
			TRACE("--- SOCKET WRITE ERROR ---\n");
			ev_nio_remove(ev);
			return (-1);
		}

		fq_put(g_inq, ev->rptr, (ev->tail - ev->rptr + 1));
		fq_putcmd(g_inq, FQ_CMD_DATAIN);

		buffer_drain_ptr(&ev->buff, ev->tail+1);
		ev->rptr = NULL;
		ev->tail = NULL;

		return (0);
	}
	else
	{
		TRACE("--- ID INVALID ---\n");
		ev_nio_remove(ev);
		return (-1);
	}
}

/* DONE */
/* return value:
   >0:receiving package completed.
   =0:receiving package uncompleted, it means that received part.
   <0:-1:no data or data error, -2:io error.
*/
static void
handle_nio(ev_nio_t *ev)
{
	if (buffer_read(&ev->buff, ev->fd) <= 0)
	{
		TRACE("--- SOCKET READ ERROR, errno = %d, %s ---\n", errno, strerror(errno));
		ev_nio_remove(ev);
		return;
	}

	while (BUFFER_LENGTH(ev->buff) > 0)
	{
		if (!ev->rptr)
		{
			ev->rptr = buffer_find_chr(&ev->buff, 0xee);
			if (!ev->rptr)
			{
				TRACE("----- invalid data, aborted -----\n");
				ev_nio_buf_reset(ev);
				break;
			}
		}

		ev->tail = buffer_find_chr(&ev->buff, 0xdd);
		if (ev->tail)
		{    
			if (handle_netmsg(ev) < 0)
			{
				return;
			}
		}
		else
		{
			break;
		}
	}
	ev_nio_timer_add(ev);
}

/* DONE */
static void
conn_cb(struct ev_loop *loop, ev_io *w, int revents)
{
	ev_nio_t *ev;

	ev = U_CONTAINER_OF(w, struct _ev_nio, watcher);

	ev_nio_remove_timer(ev);
	if (revents & EV_ERROR)
	{
		TRACE("--- SOCKET ERROR, errno = %d, %s ---\n", errno, strerror(errno));
		ev_nio_remove(ev);
		return;
	}

	if (revents & EV_READ)
	{
		handle_nio(ev);
	}
}

static int
add_listen_fd(void)
{
	listenFd = u_tcp_serv(NULL, g_prt);
	if (listenFd < 0) {
		TRACE("--- create tcp server failed ---\n");
		return -1;
	}
	u_set_nonblock(listenFd);

	/* monitor the connection. */
        ev_io_init(&nio_watcher, listen_cb, listenFd, EV_READ);
        ev_io_start(nl_loop, &nio_watcher);

        return 0;
}

/* DONE */
static void
listen_cb(struct ev_loop *loop, ev_io *w, int revents)
{
	int   conFd = -1;
	ev_nio_t *ev = NULL;

#ifdef DEBUG
	struct sockaddr_in addr;
	socklen_t  addrsize;
	char peerip[16];

	addrsize = sizeof(addr);
	conFd = accept(listenFd, (struct sockaddr *)&addr, &addrsize);
#else
	conFd = accept(listenFd, NULL, NULL);
#endif
	if (conFd >= 0)
	{
#ifdef DEBUG
		memset(peerip, 0, 16);
		inet_ntop(AF_INET, &addr.sin_addr, peerip, 16);
		printf("--- connection accepted: peerip = %s ---\n", peerip);
#endif	    
		u_set_nonblock(conFd);
		u_tcp_set_nodelay(conFd);
		ev = ev_nio_new();
		if (ev)
		{
			ev->fd = conFd;
			ev_io_init(&ev->watcher, conn_cb, conFd, EV_READ);
			ev_io_start(nl_loop, &ev->watcher);
			ev_nio_add(ev);
			ev_nio_timer_add(ev);
		}
		else
		{
			TRACE("--- ev_nio_new error ---\n");
			close(conFd);
		}
	} 
	else {
		TRACE("--- accept ERROR, errno = %d, %s ---\n", errno, strerror(errno));
		ev_io_stop(nl_loop, &nio_watcher);
		close(listenFd);
		add_listen_fd();
	}
}

static void
write_nio(void)
{
	char  buf[FQ_NODE_SZ] = {0};
	int   n = 0;
	int   id;

	n = fq_get(g_outq, buf, FQ_NODE_SZ);
	if (n > 0)
	{
		id = msg_id(buf, NULL);
		if (id >= 0&&id < MAX_USER_ID) {
			if (ev_arr[id] != NULL)
			{
				if (u_writen(ev_arr[id]->fd, buf, n) < 0)
				{
					TRACE("--- SOCKET WRITE ERROR, errno = %d ---\n", errno);
					ev_nio_remove_timer(ev_arr[id]);
					ev_nio_remove(ev_arr[id]);
				}
			}
		} else {
			TRACE("--- ID ERROR: id = %d ---\n", id);
		}
		fq_drain(g_outq);
	}
}

/* DONE */
static void
outq_cb(struct ev_loop *loop, ev_io *w, int revents)
{
	unsigned char ch;

	if (revents & EV_READ)
	{
		ch = fq_getcmd(g_outq);
		if (ch == FQ_CMD_DATAIN)
		{
			TRACE("--- has reverse message ---\n");
			write_nio();
		}
		else if (ch == FQ_CMD_QUIT)
		{
			TRACE("--- received quit cmd ---\n");
			ev_break(loop, EVBREAK_ALL);
		}
	}
}

static void*
net_loop(void* arg)
{
	pthread_detach(pthread_self());
	nl_loop = ev_loop_new(EVFLAG_AUTO);
	if (nl_loop == NULL) exit(-1);
	TRACE("--- going to enter net loop ---\n");
	while (1)
	{
		nl_lock();
		while (g_nl == 0)
			nl_p();
		nl_unlock();

		TRACE("--- net module started ---\n");
		add_listen_fd();

        	/* monitor the output queue. */
        	ev_io_init(&outq_watcher, outq_cb, fq_fdin(g_outq), EV_READ);
        	ev_io_start(nl_loop, &outq_watcher);

        	ev_run(nl_loop, 0);

        	ev_io_stop(nl_loop, &nio_watcher);
        	ev_io_stop(nl_loop, &outq_watcher);

		ev_nio_cleanup();
		ev_ntmeo_cleanup();
		close(listenFd);
        	TRACE("--- net module stopped ---\n");

		nl_lock();
		g_nl = 0;
		nl_v();
		nl_unlock();
	}
	ev_loop_destroy(nl_loop);

	pthread_exit(NULL);
}

/* called by uart/main thread, DONE */
static void
stop_net_module(void)
{
    fq_putcmd(g_outq, FQ_CMD_QUIT);
    nl_lock();
    while (g_nl != 0)
        nl_p();
    nl_unlock();
}

/* called by uart/main thread */
static void
start_net_module(void)
{
    nl_lock();
    if (g_nl == 0)
    {
        g_nl = 1;
        nl_v();
    }
    nl_unlock();
}

#if 0
}
#endif


///////////////////////////////////////////////
//            uart_io main thread            //
///////////////////////////////////////////////
#if 0
{
#endif

static void  ruart_cb(struct ev_loop *loop, ev_io *w, int revents);

/* DONE */
static void
dbg_uart(void)
{
#ifdef DEBUG
    char *ptr;
    if (!uart_rptr||!uart_tail) return;
    ptr = uart_rptr;
    printf("--- uart received: ");
    while (ptr <= uart_tail)
    {
        printf("0x%02x ", *ptr);
        ++ptr;
    }
    printf("---\n");
#endif
}

/* DONE */
static void
handle_netconf(char *buf)
{
    char  p[16];
    char *ptr = buf;
    unsigned short prt;
    int   tmeo;
    int   need_reset = 0;

    ++ptr;
    memset(p, 0, 16);
    sprintf(p, "%d.%d.%d.%d", ptr[0], ptr[1], ptr[2], ptr[3]);
//    TRACE("--- uart: received ipaddr: %s ---\n", p);

    if (ptr[0] == 0&&ptr[1] == 0&&ptr[2] == 0&&ptr[3] == 0)goto inext;
    if (is_valid_unip(p))
    {
        if (strcmp(g_ipaddr, p))
        {
            memset(g_ipaddr, 0, 16);
            strcpy(g_ipaddr, p);
            ++need_reset;
        }
    }
    else
    {
//    	TRACE("--- invalid ip address ---\n");
    }

inext:
    ptr += 4;
    memset(p, 0, 16);
    sprintf(p, "%d.%d.%d.%d", ptr[0], ptr[1], ptr[2], ptr[3]);
//    TRACE("--- uart: received netmask: %s ---\n", p);
    if (ptr[0] == 0&&ptr[1] == 0&&ptr[2] == 0&&ptr[3] == 0)goto next;
    if (strcmp(g_msk, p))
    {
        memset(g_msk, 0, 16);
        strcpy(g_msk, p);
        ++need_reset;
    }

next:

    ptr += 4;
    memset(p, 0, 16);
    sprintf(p, "%d.%d.%d.%d", ptr[0], ptr[1], ptr[2], ptr[3]);
//    TRACE("--- uart: received gateway: %s ---\n", p);
    if (ptr[0] == 0&&ptr[1] == 0&&ptr[2] == 0&&ptr[3] == 0)goto next1;
    if (strcmp(g_gw, p))
    {
        memset(g_gw, 0, 16);
        strcpy(g_gw, p);
        ++need_reset;
    }

next1:
    ptr += 4;
    if (ptr[0] == 0&&ptr[1] == 0&&ptr[2] == 0&&ptr[3] == 0&&ptr[4] == 0&&ptr[5] == 0)goto next2;
    if (memcmp(g_mac, ptr, 6))
    {
        memcpy(g_mac, ptr, 6);
    }

next2:
    ptr += 6;
    prt = (ptr[0]<<8)|ptr[1];
//    TRACE("--- uart: received port: %d ---\n", prt);
    if (prt != 0&&g_prt != prt)
    {
        g_prt = prt;
        need_reset++;
    }

    ptr += 2;
    tmeo = *ptr;
    if (tmeo != 0&&tmeo != hb_tmeo)
    {
    	hb_tmeo = tmeo;
 //   	TRACE("--- uart: received heartbeat timeout: %d ---\n", tmeo);
    	++need_reset;
    }

    if (need_reset)
    {
//        TRACE("--- need reconfigure net interface ---\n");
        ev_io_stop(g_loop, &ruart_watcher);
        stop_net_module();
        store_net_conf();
        config_netif();
        start_net_module();
        ev_io_init(&ruart_watcher, ruart_cb, uart_fd, EV_READ);
        ev_io_start(g_loop, &ruart_watcher);
        TRACE("--- reconfigure finished ---\n");
    }
}

static void uart_tmeo_cb(struct ev_loop *loop, ev_timer *w, int revents);

#define uart_timer_start(loop, w)  \
    do {  \
    	ev_timer_init((w), uart_tmeo_cb, UART_ACK_TMEO, 0.); \
        ev_timer_start((loop), (w));  \
    } while (0)

static void
send_uart(int t)
{
	uart_obuf_sz = (*fq_get_ptr[t])(g_inq, uart_obuf, FQ_NODE_SZ);
	if (uart_obuf_sz > 0)
	{
//    	TRACE("--- send uart data ---\n");
		repeat_counter = UART_MAX_TRY;
		u_writen(uart_fd, uart_obuf, uart_obuf_sz);
		uart_timer_start(g_loop, &uart_tmeo_watcher);
	}
}

/* DONE */
static void
uart_tmeo_cb(struct ev_loop *loop, ev_timer *w, int revents)
{
	ev_timer_stop(loop, w);
	if (repeat_counter > 0) {
		--repeat_counter;
//        TRACE("--- UART ACK timeout and retry sending message ---\n");
		if (uart_obuf_sz > 0) {
			u_writen(uart_fd, uart_obuf, uart_obuf_sz);        
			uart_timer_start(loop, w);
		}
	} else {
//        TRACE("--- UART sending message failed and discard it, try to send next message ---\n");
		send_uart(1);
	}
}

/* DONE */
static void
_handle_uart(void)
{
    char *ptr = uart_rptr;

    dbg_uart();
    if (*ptr == 0xbb) //alarm ack from arm7, DONE
    {
//        TRACE("--- alarm ack from uart ---\n");
        ev_timer_stop(g_loop, &uart_tmeo_watcher);
        repeat_counter = 0;
        send_uart(1);
    }
    else if (*ptr == 0xee)
    {
        ++ptr;
        if (*ptr == 0xf5) //handle shake hands:0XEE 0XF5 0XDD, DONE
        {
 //           TRACE("--- shake hands from uart ---\n");
            if (ptr+1 == uart_tail)
            {
                *ptr = 0xf6;
                u_writen(uart_fd, uart_rptr, 3);
//                TRACE("--- send shake hands ack: %02x,%02x,%02x ---\n", *(ptr-1), *ptr, *(ptr+1));
            }
        }
        else if (*ptr == 0xf3) //handle net configuration, DONE
        {
            char nack[] = {0xee, 0xf4, 0x15, 0xdd};
            
//            TRACE("--- net info from uart ---\n");
            u_writen(uart_fd, nack, sizeof(nack));
            handle_netconf(ptr);
        }
        else //handle reverse control, DONE
        {
            char rack[] = {0xcc, 0x00, 0x00, 0x00, 0x00, 0xdd};
            memcpy(&rack[1], ptr, 4);
            u_writen(uart_fd, rack, sizeof(rack));
//            TRACE("--- reverse control from uart ---\n");
            fq_put(g_outq, uart_rptr, uart_tail-uart_rptr+1);
            fq_putcmd(g_outq, FQ_CMD_DATAIN);
        }
    }
}

/*
static void
uart_dump(char *p, int n)
{
	int i;
    printf("--- received: ");
    for (i=0; i<n; ++i)
    {
    	printf("%x ", p[i]);
    }
    printf(" ---\n");
}
*/

static void
handle_uart(void)
{
	char *p0 = NULL;
	char *p1 = NULL;
//	char  buf[256];
//	int   n;
	buffer_t *rbuf = &uart_buf;

	if (buffer_read(rbuf, uart_fd) <= 0)
	{
		TRACE("--- UART READ ERROR ---\n");
		uart_buf_reset();
		return;
	}

	while (BUFPTR_LENGTH(rbuf) > 0)
	{
	    if (!uart_rptr)
	    {
	    	p0 = buffer_find_chr(rbuf, 0xee);
	        p1 = buffer_find_chr(rbuf, 0xbb);
	        if ((!p0 && !p1)||(p0 && p1))
	        {
	            TRACE("----- invalid data, aborted -----\n");
	            uart_buf_reset();
	            return;
	        }
		    uart_rptr = ((p0 == NULL) ? p1 : p0);
	    }
	    uart_tail = buffer_find_chr(rbuf, 0xdd);
	    if (uart_tail) //received whole completely packet.
	    {
	        _handle_uart();
	        buffer_drain_ptr(rbuf, uart_tail+1);
	    }
	    else //no end flag
	    {
	        return;
	    }
	}
/*
    memset(buf, 0, 256);
    n = read(uart_fd, buf, 356);
    if (n > 0)
    {
    	uart_dump(buf, n);
    	u_writen(uart_fd, buf, n);
    }
*/
}

/* DONE */
static void
ruart_cb(struct ev_loop *loop, ev_io *w, int revents)
{
    if (revents & EV_READ)
    {
        handle_uart();
    }
}

/* DONE */
static void
inq_cb(struct ev_loop *loop, ev_io *w, int revents)
{
    if (revents & EV_READ)
    {
        if (fq_getcmd(g_inq) == FQ_CMD_DATAIN)
        {
            if (ev_is_active(&uart_tmeo_watcher) == 0)
            {
//                TRACE("--- data in input queue arrived and will be transfered to uart ---\n");
                send_uart(0);
            }
        }
    }
}

#if 0
/* DONE */
static int
set_rs232(int fd, int speed, int data, int stop, int parity, int vtime, int vmin)
{
	if (u_tty_set_baud(fd, speed) < 0)
	{
		TRACE("set baudrate error\n");
		return -1;
	}

	switch (data)
	{
	case 8:
		if (u_tty_set_fomat(fd, CS8, stop, parity) < 0)
		{
			TRACE("set data8 format error\n");
			return -1;
		}
		break;
	case 7:
		if (u_tty_set_fomat(fd, CS7, stop, parity) < 0)
		{
			TRACE("set data7 format error\n");
			return -1;
		}
		break;
	case 6:
		if (u_tty_set_fomat(fd, CS6, stop, parity) < 0)
		{
			TRACE("set data6 format error\n");
			return -1;
		}
		break;
	case 5:
		if (u_tty_set_fomat(fd, CS5, stop, parity) < 0)
		{
			TRACE("set data5 format error\n");
			return -1;
		}
		break;
	default:
		TRACE("not support this format\n");
		return -1;
	}

	if (u_tty_set_fc(fd, 0, 0) < 0)
	{
		TRACE("set flow control error\n");
		return -1;
	}
	
	if (u_tty_raw(fd, vmin, vtime) < 0)
	{
		TRACE("set raw mode error\n");
		return -1;
	}

	return 0;
}
#endif

#define  OPEN_MODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)

/* DONE */
static int
uart_init()
{
	int fd;
	fd = open(UART_DEV, O_RDWR|O_NOCTTY|O_NDELAY, OPEN_MODE);
	if (fd < 0) return -1;
	u_set_nonblock(fd);
//	set_rs232(fd, 115200, 8, 1, 0, 0, 0);
	u_tty_set(fd, 115200, 8, 1, 0, 0, 0);
	return fd;
}

static void
sigterm_hanle(int signo)
{
	if (signo == SIGTERM)
	{
		TRACE("--- received SIGTERN and exit ---\n");
		exit(0);
	}
}

static void
setup_signals(void)
{
	struct sigaction act;

	sigfillset(&act.sa_mask);
	act.sa_flags = 0;

	act.sa_handler = SIG_IGN;
	sigaction(SIGHUP, &act, NULL);

	act.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &act, NULL);

	act.sa_handler = SIG_IGN;
	sigaction(SIGINT , &act, NULL);

	act.sa_handler = sigterm_hanle;
	sigaction(SIGTERM, &act, NULL);
}

/* DONE */
int
main(int argc, char *argv[])
{
	pthread_t tid;

	g_loop = EV_DEFAULT;

	setup_signals();
	uart_fd = uart_init();
	if (uart_fd < 0)
	{
		TRACE("--- UART INIT ERROR ---\n");
		return (-1);
	}

	g_inq = fq_new(ALARM_CACHE_MAX);
	g_outq = fq_new(CMD_CACHE_MAX);
	if (!g_inq || !g_outq)
	{
		close(uart_fd);
		return (-1);
	}

	buffer_init(&uart_buf, 16*1024);

	get_def_netinfo();
	load_net_conf();

	if (pthread_create(&tid, NULL, net_loop, NULL) < 0)
	{
		buffer_base_free(&uart_buf);
		close(uart_fd);
		fq_free(g_inq);
		fq_free(g_outq);
		return (-1);
	}

	/* monitor the uart input. */
	ev_io_init(&ruart_watcher, ruart_cb, uart_fd, EV_READ);
	ev_io_start(g_loop, &ruart_watcher);

	/* monitor the input queue. */
	ev_io_init(&inq_watcher, inq_cb, fq_fdin(g_inq), EV_READ);
	ev_io_start(g_loop, &inq_watcher);

	ev_timer_init(&uart_tmeo_watcher, uart_tmeo_cb, UART_ACK_TMEO, 0.);

	start_net_module();

	TRACE("--- ENTER UART LOOP ---\n");
	ev_run(g_loop, 0);

	buffer_base_free(&uart_buf);
	ev_loop_destroy(g_loop);
	g_loop = NULL;

	close(uart_fd);
	fq_free(g_inq);
	fq_free(g_outq);

	return (0);
}

#if 0
}
#endif

