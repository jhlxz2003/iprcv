#ifndef _U_NOTIFIER_H
#define _U_NOTIFIER_H
#include <pthread.h>

struct u_notifier_node {
	int (*notifier_call)(struct u_notifier_node*, unsigned long, void *);
	struct u_notifier_node *next;
	int priority;
	void *data;
};

struct u_notifier_head {
	pthread_mutex_t  mtx;
	struct u_notifier_node *head;
};


#define NOTIFY_DONE		0x0000		/* Don't care */
#define NOTIFY_OK		0x0001		/* Suits me */
#define NOTIFY_STOP_MASK	0x8000	       /* Don't call further */
#define NOTIFY_BAD		(NOTIFY_STOP_MASK|0x0002)
						/* Bad/Veto action */
/*
 * Clean way to return from the notifier and stop further calls.
 */
#define NOTIFY_STOP		(NOTIFY_OK|NOTIFY_STOP_MASK)

extern void  u_notifier_head_init(struct u_notifier_head *nh);
extern void  u_notifier_node_init(struct u_notifier_node *n, void *fn, int pri, void *data);
extern int   u_notifier_chain_register(struct u_notifier_head *nh, struct u_notifier_node *n);
extern int   u_notifier_chain_unregister(struct u_notifier_head *nh, struct u_notifier_node *n);
extern int   u_notifier_call_chain(struct u_notifier_head *nh, unsigned long val, void *v);


#endif /* _U_NOTIFIER_H */

