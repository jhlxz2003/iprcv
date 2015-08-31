
#ifndef __U_MACROS_H__
#define __U_MACROS_H__

#include <pthread.h>

/* Guard C code in headers, while including them from C++ */
#ifdef  __cplusplus
#define U_BEGIN_DECLS  extern "C" {
#define U_END_DECLS    }
#else
#define U_BEGIN_DECLS
#define U_END_DECLS
#endif

U_BEGIN_DECLS
/* Provide definitions for some commonly used macros.
 *  Some of them are only provided if they haven't already
 *  been defined. It is assumed that if they are already
 *  defined then the current definition is correct.
 */
#ifndef NULL
#ifdef __cplusplus
#define NULL        (0L)
#else /* !__cplusplus */
#define NULL        ((void*)0)
#endif /* !__cplusplus */
#endif

#ifndef	false
#define	false	(0)
#endif

#ifndef	true
#define	true	(!false)
#endif

typedef int  (*UCompareFunc)(const void*, const void*);
typedef void (*UFunc)(void*, void*);
typedef void (*UDestroyFunc)(void*);

/* Count the number of elements in an array. The array must be defined
 * as such; using this with a dynamically allocated array will give
 * incorrect results.
 */
#define U_N_ELEMENTS(arr)		(sizeof (arr) / sizeof ((arr)[0]))

#define U_STMT_START	do
#define U_STMT_END	    while (0)

/* Provide convenience macros for handling structure
 * fields through their offsets.
 */
#define U_STRUCT_OFFSET(struct_type, member)	\
    ((long) ((unsigned char*) &((struct_type*)0)->member))
#define U_STRUCT_MEMBER_P(struct_p, struct_offset)   \
    ((void*) ((unsigned char*)(struct_p) + (long)(struct_offset)))
#define U_STRUCT_MEMBER(member_type, struct_p, struct_offset)   \
    (*(member_type*)U_STRUCT_MEMBER_P((struct_p), (struct_offset)))
#define U_CONTAINER_OF(ptr, struct_type, member)  \
	(struct_type*)((unsigned char*)(ptr) - U_STRUCT_OFFSET(struct_type, member))

typedef pthread_mutex_t     ULOCK;
typedef pthread_cond_t      UCond;

#define U_STATIC_MUTEX_INIT  PTHREAD_MUTEX_INITIALIZER
#define U_STATIC_COND_INIT   PTHREAD_COND_INITIALIZER

#define U_LOCK_NAME(name)   u_ ## name ## _lock

#define U_LOCK_DEFINE_STATIC(name)    static U_LOCK_DEFINE(name)
#define U_LOCK_DEFINE(name)           \
        pthread_mutex_t U_LOCK_NAME(name) = PTHREAD_MUTEX_INITIALIZER
#define U_LOCK_DEFINE_UNINIT(name)   pthread_mutex_t U_LOCK_NAME(name)
#define U_LOCK_EXTERN(name)   extern pthread_mutex_t U_LOCK_NAME(name)

#define U_LOCK_INIT_NAME(name) pthread_mutex_init(&U_LOCK_NAME(name), NULL)
#define U_LOCK_INIT(name) pthread_mutex_init(&(name), NULL)

#define U_LOCK(name)    pthread_mutex_lock(&U_LOCK_NAME(name))
#define U_UNLOCK(name)  pthread_mutex_unlock(&U_LOCK_NAME(name))

#define U_TRYLOCK(name) pthread_mutex_trylock(&U_LOCK_NAME(name))
#define U_MUTEX_LOCK(pMutex)    pthread_mutex_lock(pMutex)
#define U_MUTEX_UNLOCK(pMutex)  pthread_mutex_unlock(pMutex)
#define U_MUTEX_TRYLOCK(pMutex) pthread_mutex_trylock(pMutex)

#define U_COND_NAME(name)   u__ ## name ## _cond
#define U_COND_DEFINE_STATIC(name)    static U_COND_DEFINE(name)
#define U_COND_DEFINE(name)           \
        pthread_cond_t U_COND_NAME(name) = U_STATIC_COND_INIT
#define U_COND_DEFINE_UNINIT(name)   pthread_cond_t U_COND_NAME(name)
#define U_COND_EXTERN(name)   extern pthread_cond_t U_COND_NAME(name)

#define U_COND_WAIT(name)      pthread_cond_wait(&U_COND_NAME(name), &U_LOCK_NAME(name))
#define U_COND_SIGNAL(name)               pthread_cond_signal(&U_COND_NAME(name))

#define u_cond_new()         \
({ UCond* cond; do {         \
	cond = malloc(sizeof(UCond)); \
	if (cond != NULL) {           \
		pthread_cond_init(cond, NULL); \
	}}while(0);cond;})

#define u_cond_free(cond)  U_STMT_START {  \
	if (cond == NULL) {                    \
		pthread_cond_destroy(cond);        \
		free(cond);                        \
	}                                      \
} U_STMT_END

#define u_cond_timed_wait(cond, mutex, abstime)  \
			pthread_cond_timedwait(cond, mutex, abstime)

#ifdef   DEBUG
#define  TRACE(fmt, ...)   \
         U_STMT_START {               \
            printf("File:%s,Line:%d,Func:%s-", __FILE__, __LINE__, __FUNCTION__); \
            printf(fmt, ##__VA_ARGS__); \
         } U_STMT_END
#define  DEBUGEXPR(expr)  expr
#else
#define  TRACE(fmt, ...)
#define  DEBUGEXPR(expr)
#endif

#define u_return_if_fail(expr) if (!(expr)) return
#define u_return_if_true(expr) if (expr) return
#define u_return_val_if_fail(expr, val)  if (!(expr)) return (val)
#define u_return_val_if_true(expr, val)  if (expr) return (val)
	
#undef	U_MAX
#define U_MAX(a, b)  (((a)>(b))?(a):(b))

#undef	U_MIN
#define U_MIN(a, b)  (((a)<(b))?(a):(b))

#undef	U_ABS
#define U_ABS(a)	   (((a)<0)?-(a):(a))

#undef	U_CLAMP
#define U_CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

#undef	U_SWAP
//#define U_SWAP(x,y)	{(x)^=(y);(y)^=(x);(x)^=(y);}
#define U_SWAP(x,y)	do {(x)^=(y);(y)^=(x);(x)^=(y);} while(0)

#undef	U_FLOOR
#define U_FLOOR(x, P)	((x)-((x)%(P)))

/*
 * Round up 'a' to next multiple of 'size', which must be a power of 2
 */
#define U_ROUNDUP(a, size) (((a) & ((size)-1)) ? (1 + ((a) | ((size)-1))) : (a))
#define U_ROUNDDN(a, size) (((a) & ((size)-1)) ? (a - ((a) & ((size)-1))) : (a))
#undef	U_CEILING
#define U_CEILING(x, P)	(U_FLOOR(x, P)+(P))

#undef	U_FLOOR16
#define U_FLOOR16(x)    ((x)-((x)&0x0f))

#undef	U_CEILING16
#define U_CEILING16(x)	(U_FLOOR16(x)+0x10)

#define U_SWAP_16(a) a = ((unsigned short)(a)>>8)|((unsigned short)(a)<<8)
#define U_SWAP_32(a) a = ((unsigned)(a)>>24)|(((unsigned)(a)>>8)&0xFF00)|(((unsigned)(a)<<8)&0xFF0000)|((unsigned)(a)<<24)

#ifndef ignore_result
#define ignore_result(x) ({ typeof(x) z = x; (void)sizeof z; })
#endif

U_END_DECLS

#endif /* __U_MACROS_H__ */

