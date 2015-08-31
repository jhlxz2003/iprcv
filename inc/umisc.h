#ifndef _U_MISC_H_
#define _U_MISC_H_

extern char *tmpnam_r(char *s);
extern char *u_trim(char *str);
/*
extern int     u_grp_strcmp(char **grp, char *s);
extern int     u_grp_strcmp_n(char **grp, char *s, int n);

extern int     u_token_get_list(char *str, char *del, char ***list);
extern void   u_token_free_list(char **list, int  num);

extern int     u_token_get_list_static(char *str, char *del, char ***list);
extern void   u_token_free_list_static(char **list, int  num);
*/

extern unsigned short  u_crc16(unsigned char *buf, int len);

#endif

