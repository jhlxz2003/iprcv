#ifndef  U_CRYPT_H
#define  U_CRYPT_H

#ifdef __cplusplus
extern "C" {
#endif

char*  u_encrypt(char *passwd);
int    u_decrypt(char *passwd, char *pass);

#ifdef __cplusplus
}
#endif
#endif

