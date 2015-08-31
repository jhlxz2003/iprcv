#ifndef _U_MD5_H_
#define _U_MD5_H_

typedef unsigned UNSIGNED32;

/* Definitions of _ANSI_ARGS and EXTERN that will work in either
   C or C++ code:
 */
#undef _ANSI_ARGS_
#if ((defined(__STDC__) || defined(SABER)) && !defined(NO_PROTOTYPE)) || defined(__cplusplus) || defined(USE_PROTOTYPE)
#   define _ANSI_ARGS_(x)       x
#else
#   define _ANSI_ARGS_(x)       ()
#endif
#ifdef __cplusplus
#   define EXTERN extern "C"
#else
#   define EXTERN extern
#endif

/* MD5 context. */
typedef struct MD5Context {
  UNSIGNED32 state[4];	/* state (ABCD) */
  UNSIGNED32 count[2];	/* number of bits, modulo 2^64 (lsb first) */
  unsigned char buffer[64];	/* input buffer */
} MD5_CTX;

EXTERN void   u_md5_init (MD5_CTX *);
EXTERN void   u_md5_update (MD5_CTX *, const unsigned char *, unsigned int);
EXTERN void   u_md5_final (unsigned char [16], MD5_CTX *);
EXTERN char * u_md5_end(MD5_CTX *, char *);
EXTERN char * u_md5_file(const char *, char *);
EXTERN char * u_md5_data(const unsigned char *, unsigned int, char *);
#endif /* _SYS_MD5_H_ */
