/* writen by LXZ, 2008.04.25 */

#ifndef __U_LIST_H__
#define __U_LIST_H__

#include "umacros.h"

U_BEGIN_DECLS

typedef struct list_hdr {
	struct list_hdr *next;
	struct list_hdr *prev;
} UListHdr;

typedef UListHdr* PUListHdr;
#define UListHdrSize	sizeof(UListHdr)

#define U_LIST_HEAD_INIT(name) { &(name), &(name) }

#define U_LIST_HEAD(name) \
	struct list_hdr name = U_LIST_HEAD_INIT(name)

#define U_INIT_LIST_HEAD(ptr) do { \
	(ptr)->next = (ptr); (ptr)->prev = (ptr); \
} while (0)

/*list_hdr member is the first member in structure */
#define u_list_entry(ptr, type)  ((type *)(ptr))
#define u_list_next_entry(ptr, type)  ((type *)((ptr)->next))
#define u_list_prev_entry(ptr, type)  ((type *)((ptr)->prev))

#define u_list_next(ptr)  ((ptr)->next)
#define u_list_prev(ptr)  ((ptr)->prev)

#define u_list_each_entry(ent, head)  \
   for (ent=(head)->next; ent!=NULL&&ent!=(head); ent=ent->next)

void u_list_del(struct list_hdr *entry);
void u_list_add(struct list_hdr *new, struct list_hdr *head);
void u_list_append(struct list_hdr *new, struct list_hdr *head);
void u_list_move(struct list_hdr *list, struct list_hdr *head);
int  u_list_empty(const struct list_hdr *head);

void u_list_append_list(struct list_hdr *s, struct list_hdr *d);

U_END_DECLS

#endif

