#ifndef _HAL_INIT_H_
#define _HAL_INIT_H_

#define WDT_TIMER    30

#ifdef __cplusplus
extern "C" {
#endif

int   system_init  (void);
void  system_uninit(void);

#ifdef __cplusplus
}
#endif

#endif

