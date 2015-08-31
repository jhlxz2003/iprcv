#ifndef __U_RAND_H__
#define __U_RAND_H__

#include "umacros.h"

U_BEGIN_DECLS

typedef struct _URand    URand;

void  u_non_thr_srand();
int   u_non_thr_rand_int();
int   u_non_thr_rand_int_range(int begin, int end);
/* URand - a good and fast random number generator: Mersenne Twister
 * see http://www.math.keio.ac.jp/~matumoto/emt.html for more info.
 * The range functions return a value in the intervall [begin, end).
 * int          -> [0..2^32-1]
 * int_range    -> [begin..end-1]
 * double       -> [0..1)
 * double_range -> [begin..end)
 */
void    u_rand_thread_init(void);

URand*  u_rand_new_with_seed  (unsigned int  seed);
URand*  u_rand_new_with_seed_array (const unsigned int *seed,unsigned int seed_length);
URand*  u_rand_new            (void);
void    u_rand_free           (URand   *rand_);
URand*  u_rand_copy           (URand   *rand_);
void    u_rand_set_seed       (URand   *rand_,unsigned int  seed);
void	u_rand_set_seed_array (URand   *rand_,const unsigned int *seed,unsigned int seed_length);

#define u_rand_boolean(rand_) ((u_rand_int(rand_)&(1 << 15)) != 0)

unsigned int  u_rand_int(URand   *rand_);
int    u_rand_int_range(URand   *rand_,int   begin,int   end);
double u_rand_double(URand   *rand_);
double u_rand_double_range(URand   *rand_, double  begin, double  end);
void    u_random_set_seed(unsigned int  seed);

#define u_random_boolean() ((u_random_int () & (1 << 15)) != 0)

unsigned int  u_random_int(void);
int    u_random_int_range(int   begin, int   end);
double u_random_double(void);
double u_random_double_range (double  begin, double  end);

/* my random api */
char             *u_my_initstate(unsigned int seed, char *arg_state, int n);
char             *u_my_setstate(char *arg_state);
void               u_my_srandom(unsigned int x);
long               u_my_random();
unsigned int    u_my_random32();


U_END_DECLS

#endif /* __U_RAND_H__ */






