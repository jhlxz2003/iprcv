#ifndef UTIL_H_
#define UTIL_H_

#ifdef WIN32
#define _SOCKET_ERROR() WSAGetLastError()
#define _SET_SOCKET_ERROR(errcode)		\
	do { WSASetLastError(errcode); } while (0)
#else
#define _SOCKET_ERROR() (errno)
#define _SET_SOCKET_ERROR(errcode)		\
		do { errno = (errcode); } while (0)
#endif

#ifdef WIN32
#define U_CLOSESOCKET(s) closesocket(s)
#else
#define U_CLOSESOCKET(s) close(s)
#endif

#ifdef HAVE_SETFD
#define FD_CLOSEONEXEC(x) do { \
	if (fcntl(x, F_SETFD, 1) == -1) \
		{TRACE("fcntl(%d, F_SETFD)\n", x);}  \
} while (0)
#else
#define FD_CLOSEONEXEC(x)
#endif


#endif


