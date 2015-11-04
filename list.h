#ifndef LIST_H
#define LIST_H

#include <pthread.h>
#include <stddef.h>

#define container_of(ptr, type, member) ({			\
			const typeof(((type *)0)->member) *__mptr = (ptr);	\
			(type *)((char *)__mptr - offsetof(type, member)); })

#ifndef LIST_H__
struct list_head {
	struct list_head *next, *prev;
};

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name) \
	struct list_head name = LIST_HEAD_INIT(name)

static inline void INIT_LIST_HEAD(struct list_head *list)
{
	list->next = list;
	list->prev = list;
}

/*
 * Insert a new entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __list_add(struct list_head *new,
			      struct list_head *prev,
			      struct list_head *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

static inline void list_add(struct list_head *new, struct list_head *head)
{
	__list_add(new, head, head->next);
}


/*
 * list_add_tail - add a new entry
 * @new: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
static inline void list_add_tail(struct list_head *new, struct list_head *head)
{
	__list_add(new, head->prev, head);
}

/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __list_del(struct list_head * prev, struct list_head * next)
{
	next->prev = prev;
	prev->next = next;
}

/*
 * list_del - deletes entry from list.
 * @entry: the element to delete from the list.
 * Note: list_empty() on entry does not return true after this, the entry is
 * in an undefined state.
 */
static inline void list_del(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
	entry->next = NULL;
	entry->prev = NULL;
}

/*
 * list_move - delete from one list and add as another's head
 * @list: the entry to move
 * @head: the head that will precede our entry
 */
static inline void list_move(struct list_head *list, struct list_head *head)
{
	__list_del(list->prev, list->next);
	list_add(list, head);
}

/*
 * list_move_tail - delete from one list and add as another's tail
 * @list: the entry to move
 * @head: the head that will follow our entry
 */
static inline void list_move_tail(struct list_head *list,
				  struct list_head *head)
{
	__list_del(list->prev, list->next);
	list_add_tail(list, head);
}

/*
 * list_is_last - tests whether @list is the last entry in list @head
 * @list: the entry to test
 * @head: the head of the list
 */
static inline int list_is_last(const struct list_head *list,
		const struct list_head *head)
{
	return list->next == head;
}

/*
 * list_empty - tests whether a list is empty
 * @head: the list to test.
 */
static inline int list_empty(const struct list_head *head)
{
	return head->next == head;
}


#define list_first_entry(ptr, type, member) \
		list_entry((ptr)->next, type, member)

/*
 * list_entry - get the struct for this entry
 * @ptr:	the &struct list_head pointer.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_struct within the struct.
 */
#define list_entry(ptr, type, member) \
	(type *)((char *)ptr - (unsigned long)&(((type *)0)->member))


/*
 * list_for_each	-	iterate over a list
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 */
#define list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); \
        	pos = pos->next)

/*
 * list_for_each_entry	-	iterate over list of given type
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
#define list_for_each_entry(pos, head, member)				\
	if ((head)->next != (head))	\
	for (pos = list_entry((head)->next, typeof(*pos), member);	\
	     pos && (&pos->member != (head)); 	\
	     pos = (pos->member.next == head)?\
			NULL:list_entry(pos->member.next, typeof(*pos), member))

/*
 * list_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */

#define list_for_each_entry_safe(pos, n, head, member)			\
		for (pos = list_entry((head)->next, typeof(*pos), member),	\
			n = list_entry(pos->member.next, typeof(*pos), member);	\
		    &pos->member != (head);					\
		    pos = n, n = list_entry(n->member.next, typeof(*n), member))

/*
 * list_for_each_entry_reverse - iterate backwards over list of given type.
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member6	the name of the list_struct within the struct.
 */
#define list_for_each_entry_reverse(pos, head, member)			\
	for (pos = list_entry((head)->prev, typeof(*pos), member);	\
	     prefetch(pos->member.prev), &pos->member != (head); 	\
	     pos = list_entry(pos->member.prev, typeof(*pos), member))

/*
 * list_for_each_entry_from - iterate over list of given type from the current point
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 *
 * Iterate over list of given type, continuing from current position.
 */
#define list_for_each_entry_from(pos, head, member) 			\
	for (; prefetch(pos->member.next), &pos->member != (head);	\
	     pos = list_entry(pos->member.next, typeof(*pos), member))

/*
 * @head      header of the list
 * @lock     spinlock of the list
 */

static inline void __list_splice(const struct list_head *list,
		struct list_head *prev,
		struct list_head *next)
{
	struct list_head *first = list->next;
	struct list_head *last = list->prev;

	first->prev = prev;
	prev->next = first;

	last->next = next;
	next->prev = last;
}

/**
 * list_splice_init - join two lists and reinitialise the emptied list.
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 *
 * The list at @list is reinitialised
 */
static inline void list_splice_init(struct list_head *list,
		struct list_head *head)
{
	if (!list_empty(list)) {
		__list_splice(list, head, head->next);
		INIT_LIST_HEAD(list);
	}
}
#endif

static inline struct list_head *del_queue_head(struct list_head *head)
{
	struct list_head    *pnode = NULL;

	if (head && head != head->next)
	{
		pnode = head->next;
		list_del(pnode);
	}

	return pnode;
}

static inline struct list_head *del_queue_head_lock(struct list_head *head, pthread_mutex_t *lock)
{
	struct list_head    *pnode = NULL;

	pthread_mutex_lock(lock);
	if (head && head != head->next)
	{
		pnode = head->next;
		list_del(pnode);
	}
	pthread_mutex_unlock(lock);

	return pnode;
}

static inline struct list_head *get_next_node(struct list_head *head, struct list_head *list)
{
	struct list_head	*pnode = NULL;

	if (list && list->next != head)
	{
		pnode = list->next;
	}

	return pnode;
}

static inline struct list_head *get_next_node_mutex(struct list_head *head, struct list_head *list, pthread_mutex_t *mutex)
{
	struct list_head	*pnode = NULL;

	pthread_mutex_lock(mutex);
	if (list && list->next != head)
	{
		pnode = list->next;
	}
	pthread_mutex_unlock(mutex);

	return pnode;
}

static inline void list_del_lock(struct list_head *list, pthread_mutex_t *mutex)
{
	pthread_mutex_lock(mutex);
	list_del(list);
	pthread_mutex_unlock(mutex);
}

static inline void list_add_tail_lock(struct list_head *list, struct list_head *head, pthread_mutex_t *mutex)
{
	pthread_mutex_lock(mutex);
	list_add_tail(list, head);
	pthread_mutex_unlock(mutex);
}

#endif
