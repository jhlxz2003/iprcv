#ifndef BDL_H_
#define BDL_H_

extern int    u_start_batch_dl();
extern void   u_run_bdl(char **pp, int n);
extern void   u_bdl_set_cb(void *xcb, void *scb);
extern void   u_bdl_path(char *path);
extern int    u_ifbdl();

#endif

