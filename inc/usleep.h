#ifndef _U_SLEEP_H
#define _U_SLEEP_H

#ifdef __cplusplus
extern "C" {
#endif

extern void  u_msleep(int ms);
extern void  u_mdelay(int ms);
extern void  u_sleep(int s);
extern void  u_delay(int s);

#ifdef __cplusplus
}
#endif

#endif
