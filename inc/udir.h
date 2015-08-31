#ifndef _U_DIR_H_
#define _U_DIR_H_

#include <dirent.h>
#include "umacros.h"

U_BEGIN_DECLS

int  u_dir_get_list(char* lpszPath, struct dirent ***pFileList);
int  u_dir_get_list_pattern(char* lpszPath, struct dirent ***pFileList, char *pattern);
void u_dir_free_list(struct dirent **pFileList, int iFileNumber);
int  u_remove1(char *path);

U_END_DECLS

#endif

