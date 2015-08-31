#ifndef _U_JPEG_H_
#define _U_JPEG_H_

#include "utypes.h"

#ifdef __cplusplus
extern "C" {
#endif

extern int  u_jpeg_dec_file(char *filename, PUImage img);
extern int  u_jpeg_dec_file1(char *filename, PUImage img);
extern int  u_jpeg_dec_file2(char *filename, PUImage img);

#ifdef __cplusplus
}
#endif

#endif

