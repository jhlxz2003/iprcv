#ifndef DAEMON_INIT_H
#define DAEMON_INIT_H

#ifdef __cplusplus
extern "C" {
#endif

int u_daemon_init(const char *root, const char *pname, int facility);

#ifdef __cplusplus
}
#endif

#endif

