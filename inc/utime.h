#ifndef U_TIME_H_
#define U_TIME_H_

#include <time.h>

#define SECS_PER_DAY     (24L * 60L * 60L)
#define LEAP_YEAR(year)   (((!((year) % 4)) && ((year) % 100)) || !((year) % 400))
#define YEAR_SIZE(year)   (LEAP_YEAR(year) ? 366 : 365)

void    u_localtime_r(unsigned long v, struct tm *tm);

#endif

