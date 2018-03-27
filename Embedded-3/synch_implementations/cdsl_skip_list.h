#ifndef __CDSL_SKIP_LIST_H__
#define __CDSL_SKIP_LIST_H__

#include <assert.h>
#include "cdsl_lock_definitions.h"

#define CDSL_SKIP_LIST_MAX_LEVEL 5

/************************************ lock-based skip-list ****************************************/
typedef struct cdsl_skip_list_lock_based_node_struct {
	unsigned int key;
	void *value;
	struct cdsl_skip_list_lock_based_node_struct **next;
	unsigned int marked;
	unsigned int fully_linked;
	unsigned int top_level;
	
	cdsl_pthread_rc_lock lock;
} cdsl_skip_list_lock_based_node;

typedef struct cdsl_skip_list_lock_based_struct {
	cdsl_skip_list_lock_based_node *head;
	cdsl_skip_list_lock_based_node *tail;
	
	unsigned int (*insert)(unsigned int, struct cdsl_skip_list_lock_based_struct*, unsigned int, void*);
	unsigned int (*lookup)(unsigned int, struct cdsl_skip_list_lock_based_struct*, unsigned int);
	unsigned int (*remove)(unsigned int, struct cdsl_skip_list_lock_based_struct*, unsigned int);
	void* 		 (*remove_min)(unsigned int, struct cdsl_skip_list_lock_based_struct*);
	void* 		 (*lookup_get)(unsigned int, struct cdsl_skip_list_lock_based_struct*, unsigned int);
	unsigned int (*get_min_key)(unsigned int, struct cdsl_skip_list_lock_based_struct*);
	
} cdsl_skip_list_lock_based;

void cdsl_skip_list_lock_based_init_sentinel(cdsl_skip_list_lock_based_node *node, unsigned int key);
void cdsl_skip_list_lock_based_init_node(cdsl_skip_list_lock_based_node *node, unsigned int height, unsigned int key, void *value);
int cdsl_skip_list_lock_based_find(cdsl_skip_list_lock_based* list, unsigned int key, cdsl_skip_list_lock_based_node **preds, cdsl_skip_list_lock_based_node **succs);

cdsl_skip_list_lock_based* cdsl_skip_list_lock_based_init(void);
unsigned int cdsl_skip_list_lock_based_insert(unsigned int cpu_id, cdsl_skip_list_lock_based *list, unsigned int key, void *value);
unsigned int cdsl_skip_list_lock_based_lookup(unsigned int cpu_id, cdsl_skip_list_lock_based *list, unsigned int key);
unsigned int cdsl_skip_list_lock_based_remove(unsigned int cpu_id, cdsl_skip_list_lock_based *list, unsigned int key);
void* cdsl_skip_list_lock_based_remove_min(unsigned int cpu_id, cdsl_skip_list_lock_based *list);
void* cdsl_skip_list_lock_based_lookup_get(unsigned int cpu_id, cdsl_skip_list_lock_based *list, unsigned int key);
unsigned int cdsl_skip_list_lock_based_get_min_key(unsigned int cpu_id, cdsl_skip_list_lock_based *list);

/************************************ lockless skip-list ****************************************/
typedef struct cdsl_skip_list_lockless_node_struct {
	unsigned int key;
	void *value;
	struct cdsl_skip_list_lockless_node_struct **next;
	int top_level;	
} cdsl_skip_list_lockless_node;

typedef struct cdsl_skip_list_lockless_struct {
	cdsl_skip_list_lockless_node *head;
	cdsl_skip_list_lockless_node *tail;
	
	unsigned int (*insert)(unsigned int, struct cdsl_skip_list_lockless_struct*, unsigned int, void*);
	unsigned int (*lookup)(unsigned int, struct cdsl_skip_list_lockless_struct*, unsigned int);
	unsigned int (*remove)(unsigned int, struct cdsl_skip_list_lockless_struct*, unsigned int);
	void* 		 (*remove_min)(unsigned int, struct cdsl_skip_list_lockless_struct*);
	void* 		 (*lookup_get)(unsigned int, struct cdsl_skip_list_lockless_struct*, unsigned int);
	unsigned int (*get_min_key)(unsigned int, struct cdsl_skip_list_lockless_struct*);
	
} cdsl_skip_list_lockless;

void cdsl_skip_list_lockless_init_sentinel(cdsl_skip_list_lockless_node *node, unsigned int key);
void cdsl_skip_list_lockless_init_node(cdsl_skip_list_lockless_node *node, unsigned int height, unsigned int key, void *value);
int cdsl_skip_list_lockless_find(cdsl_skip_list_lockless *list, unsigned int key, cdsl_skip_list_lockless_node **preds, cdsl_skip_list_lockless_node **succs);

cdsl_skip_list_lockless* cdsl_skip_list_lockless_init(void);
unsigned int cdsl_skip_list_lockless_insert(unsigned int cpu_id, cdsl_skip_list_lockless *list, unsigned int key, void *value);
unsigned int cdsl_skip_list_lockless_lookup(unsigned int cpu_id, cdsl_skip_list_lockless *list, unsigned int key);
unsigned int cdsl_skip_list_lockless_remove(unsigned int cpu_id, cdsl_skip_list_lockless* list, unsigned int key);
void* cdsl_skip_list_lockless_remove_min(unsigned int cpu_id, cdsl_skip_list_lockless *list);
void* cdsl_skip_list_lockless_lookup_get(unsigned int cpu_id, cdsl_skip_list_lockless *list, unsigned int key);
unsigned int cdsl_skip_list_lockless_get_min_key(unsigned int cpu_id, cdsl_skip_list_lockless *list);
/********************************************************************************************/
#endif
