#ifndef UBASE64_H
#define UBASE64_H

#ifdef __cplusplus
extern "C" {
#endif

unsigned char*  u_base64_decode(char* in, unsigned *resultSize, int trimTrailingZeros);
char*  u_base64_encode(char* origSigned, unsigned origLength);

#ifdef __cplusplus
}
#endif

#endif
