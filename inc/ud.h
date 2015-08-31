#ifndef UD_H_
#define UD_H_

#define  UD_INFO     0
#define  UD_FSIZE    1
#define  UD_DLSZ     2
#define  UD_DLFIN    3
#define  UD_PRECPY   4
#define  UD_CPSZ     5
#define  UD_CPFIN    6
#define  UD_DLERR    7
#define  UD_CPERR    8

#define  ENOUD        4
#define  EUDFAILED    5
#define  EUPDATED     6
#define  EUINVAL      7
#define  EUDCANCELED  8
#define  UD_MAX_INFO  9

#define  FTP_BLKSZ   (32*1024)

typedef struct  _ud {
	int  type;
	char *serip;
	char *usr;
	char *pas;
	unsigned int ver;
} ud_t;

extern void  (*g_ud_cb)(int  x, int y);
extern ud_t   *ud_inf;

int   u_prepare_ud();
void  u_ud_on();
void  u_ud_off();
void  u_ud_canceled();
void  u_ud_now();
void  u_ud_itvl(int itvl);

void  u_udinf_set(ud_t *inf);
int   u_ud_lock();
void  u_ud_unlock();
int   u_ifud();
void  u_ud_set_cb(void (*fn)(int, int));
char *u_ud_get_text(int i);

void   u_ud_force_logo();
void   u_ud_force_index();

/* internal api, should be called by update loop when updating finished. */
void  _ud_finish();
/* internal api, should be called by update event loop. */
int   _start_ud_loop();
int   _run_ud();
void  _quit_ud_loop();
void  _cancel_ud();

void  _ftp_option(void *conn, int ms, long arg, long cbytes);
void  _ftp_clear_option(void *conn);

#endif

