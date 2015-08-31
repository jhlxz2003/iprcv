#ifndef _U_PNG_H_
#define _U_PNG_H_

#include <stdio.h>
#include "utypes.h"

#ifdef __cplusplus
extern "C" {
#endif

extern int  u_png_check(char *file_name);
extern int  u_png_inf(char *file_name, PUImgInf inf);
extern int  u_png_dec_hi(char *file_name, PUImage image, int strip);
extern int  u_png_dec_hi1(char *file_name, PUImage image, int strip);
extern int  u_png_dec_hi2(char *file_name, PUImage image, int strip);

#ifdef __cplusplus
}
#endif

#endif

