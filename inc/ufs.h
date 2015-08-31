#ifndef _UFS_H_
#define _UFS_H_

#ifdef __cplusplus
extern "C" {
#endif

unsigned long u_fs_space(char *path, int flg);
unsigned long u_fs_free_space(char *path, int flg);
unsigned long u_fs_avail_space(char *path, int flg);

#ifdef __cplusplus
}
#endif

#endif
