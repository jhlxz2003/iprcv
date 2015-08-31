#ifndef  NTP_CLI_H_
#define  NTP_CLI_H_

typedef struct  _ntp_opt {
	void (*ntp_before) (void);
	void (*ntp_after) (void);
	void (*ntp_act) (void*);
} ntp_opt_t;

int   u_ntp_client();
void  u_ntp_run();
void  u_ntp_stop();
void  u_ntp_opt(void *b, void *a, void *act);
void  u_ntp_serip(char *serip);

#endif


