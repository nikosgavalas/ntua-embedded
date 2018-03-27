#ifndef __CDSL_HASH_H__
#define __CDSL_HASH_H__

#include <assert.h>
#include "cdsl_lock_definitions.h"

/************************************ lock-based hash ****************************************/
#define CDSL_HASH_LOAD_FACTOR	4						// number of entries / number of buckets

typedef struct cdsl_hash_lock_based_list_node_struct {
	unsigned int key;
	unsigned int value;

	struct cdsl_hash_lock_based_list_node_struct *next;
} cdsl_hash_lock_based_list_node;

typedef struct cdsl_hash_lock_based_list_struct {
	cdsl_hash_lock_based_list_node *head;
} cdsl_hash_lock_based_list;

typedef struct cdsl_hash_lock_based_struct {
	unsigned long int setSize;

	cdsl_hash_lock_based_list *bucket;
	unsigned int table_size; 							// number of buckets
	cdsl_hash_lock_based_list *old_bucket;
	unsigned int old_table_size;

	unsigned int (*insert)(unsigned int, struct cdsl_hash_lock_based_struct*, unsigned int, unsigned int);
	unsigned int (*lookup)(unsigned int, struct cdsl_hash_lock_based_struct*, unsigned int);
	unsigned int (*remove)(unsigned int, struct cdsl_hash_lock_based_struct*, unsigned int);

	unsigned int lock_size;
	cdsl_pthread_lock *lock_array;
} cdsl_hash_lock_based;

unsigned int cdsl_hash_lock_based_hashCode(unsigned int key, cdsl_hash_lock_based * ht);
unsigned int cdsl_hash_lock_based_policy(cdsl_hash_lock_based * ht);
unsigned int cdsl_hash_lock_based_list_init(cdsl_hash_lock_based_list * l);
cdsl_hash_lock_based_list_node *cdsl_hash_lock_create_node(unsigned int key, unsigned int val);
unsigned int cdsl_hash_lock_based_add_node(cdsl_hash_lock_based_list * l, unsigned int key, unsigned int val);
unsigned int cdsl_hash_lock_based_add_node_op(cdsl_hash_lock_based_list * l, cdsl_hash_lock_based_list_node * newNode);
unsigned int cdsl_hash_lock_based_init_bucket(cdsl_hash_lock_based * ht, unsigned int table_size);
unsigned int cdsl_hash_lock_based_lookup_node(cdsl_hash_lock_based_list * l, unsigned int key);
unsigned int cdsl_hash_lock_based_remove_node(cdsl_hash_lock_based_list * l, unsigned int key);
void cdsl_hash_lock_based_free_bucket(cdsl_hash_lock_based_list * bucket, unsigned int table_size);
void cdsl_hash_lock_based_free_node(cdsl_hash_lock_based_list_node * node);
void cdsl_hash_lock_based_free_hashtable(cdsl_hash_lock_based * ht);

cdsl_hash_lock_based *cdsl_hash_lock_based_init(unsigned int table_size);
unsigned int cdsl_hash_lock_based_insert(unsigned int cpu_id, cdsl_hash_lock_based * ht, const unsigned int key, const unsigned int val);
unsigned int cdsl_hash_lock_based_lookup(unsigned int cpu_id, cdsl_hash_lock_based * ht, unsigned int key);
unsigned int cdsl_hash_lock_based_remove(unsigned int cpu_id, cdsl_hash_lock_based * ht, unsigned int key);
/********************************************************************************************/

/************************************ lockless hash ****************************************/
/* These are GCC builtins; other compilers may require inline assembly */
#define CDSL_HASH_LOCKLESS_CAS(ADDR, OLDV, NEWV) __sync_val_compare_and_swap((ADDR), (OLDV), (NEWV))
#define CDSL_HASH_LOCKLESS_INCR(ADDR, INCVAL) __sync_fetch_and_add((ADDR), (INCVAL))

typedef struct cdsl_hash_lockless_list_node_struct {
    unsigned long int key;
    unsigned int value;
    unsigned long int next;
} cdsl_hash_lockless_list_node;

typedef struct cdsl_hash_lockless_struct {
    unsigned long int *B;  						// Buckets
    volatile unsigned int nr_elements;			// number of elements in hash
    volatile unsigned int table_size;			// number of buckets

	unsigned int (*insert)(unsigned int, struct cdsl_hash_lockless_struct*, unsigned int, unsigned int);
	unsigned int (*lookup)(unsigned int, struct cdsl_hash_lockless_struct*, unsigned int);
	unsigned int (*remove)(unsigned int, struct cdsl_hash_lockless_struct*, unsigned int);

} cdsl_hash_lockless;

#define CDSL_HASH_LOCKLESS_MARK_OF(x)           	((x) & 1)
#define CDSL_HASH_LOCKLESS_PTR_MASK(x)          	((x) & ~(unsigned long int)1)
#define CDSL_HASH_LOCKLESS_PTR_OF(x)            	((cdsl_hash_lockless_list_node *)CDSL_HASH_LOCKLESS_PTR_MASK(x))
#define CDSL_HASH_LOCKLESS_CONSTRUCT(mark, ptr)		(CDSL_HASH_LOCKLESS_PTR_MASK((unsigned long int)ptr) | (mark))
#define CDSL_HASH_LOCKLESS_UNINITIALIZED 			((unsigned long int)0)
#define CDSL_HASH_LOCKLESS_MSB 						(((unsigned long int)1) << 63)
#define CDSL_HASH_LOCKLESS_REVERSE_BYTE(x) 			((unsigned long int)((((((unsigned int)(x)) * 0x0802LU & 0x22110LU) | (((unsigned int)(x)) * 0x8020LU & 0x88440LU)) * 0x10101LU >> 16) & 0xff))
# define REVERSE(x)									((CDSL_HASH_LOCKLESS_REVERSE_BYTE((((unsigned long int)(x))) & 0xff) << 56) |       \
													 (CDSL_HASH_LOCKLESS_REVERSE_BYTE((((unsigned long int)(x)) >> 8) & 0xff) << 48) |  \
													 (CDSL_HASH_LOCKLESS_REVERSE_BYTE((((unsigned long int)(x)) >> 16) & 0xff) << 40) | \
													 (CDSL_HASH_LOCKLESS_REVERSE_BYTE((((unsigned long int)(x)) >> 24) & 0xff) << 32) | \
													 (CDSL_HASH_LOCKLESS_REVERSE_BYTE((((unsigned long int)(x)) >> 32) & 0xff) << 24) | \
													 (CDSL_HASH_LOCKLESS_REVERSE_BYTE((((unsigned long int)(x)) >> 40) & 0xff) << 16) | \
													 (CDSL_HASH_LOCKLESS_REVERSE_BYTE((((unsigned long int)(x)) >> 48) & 0xff) << 8) |  \
													 (CDSL_HASH_LOCKLESS_REVERSE_BYTE((((unsigned long int)(x)) >> 56) & 0xff) << 0))

unsigned long int cdsl_hash_lockless_so_regular_key(unsigned long int key);
unsigned long int cdsl_hash_lockless_so_dummy_key(unsigned long int key);
unsigned int cdsl_hash_lockless_get_parent(unsigned long int bucket);
void cdsl_hash_lockless_initialize_bucket(cdsl_hash_lockless *h, unsigned int  bucket);
unsigned int cdsl_hash_lockless_list_insert(unsigned long int *head, cdsl_hash_lockless_list_node *node, unsigned long int *ocur);
void* cdsl_hash_lockless_list_find(unsigned long int *head, unsigned long int key, unsigned long int **oprev, unsigned long int *ocur, unsigned long int  *onext);
unsigned int cdsl_hash_lockless_delete(unsigned long int *head, unsigned long int key);

cdsl_hash_lockless* cdsl_hash_lockless_init(unsigned int table_size);
unsigned int cdsl_hash_lockless_insert(unsigned int cpu_id, cdsl_hash_lockless *ht, unsigned int key, unsigned int val);
unsigned int cdsl_hash_lockless_lookup(unsigned int cpu_id, cdsl_hash_lockless *ht, unsigned int key);
unsigned int cdsl_hash_lockless_remove(unsigned int cpu_id, cdsl_hash_lockless *h, unsigned int key);
/********************************************************************************************/

/************************************ cuckoo hash *******************************************/
#define CDSL_HASH_CUCKOO_MAX_ATTEMPTS 2  // max number of random walk trials (original: 10000)
#define cdsl_hash_cuckoo_lockKey(ht, hashkey)   (hashkey % ht->lock_size)

typedef struct cdsl_hash_cuckoo_struct {
	unsigned int nr_probes;						// nr of probes
	unsigned int total_nr_elements;				// total number of elements
	unsigned int nr_elements_per_probe;			// size of each probe (total_nr_elements / nr_probes + 0.5)
	unsigned int **key_tables;					// key tables
	int **value_tables; 						// value tables

	unsigned int (*insert)(unsigned int, struct cdsl_hash_cuckoo_struct*, unsigned int, unsigned int);
	unsigned int (*lookup)(unsigned int, struct cdsl_hash_cuckoo_struct*, unsigned int);
	unsigned int (*remove)(unsigned int, struct cdsl_hash_cuckoo_struct*, unsigned int);

	unsigned int lock_size;
	cdsl_pthread_lock *lock_array;
} cdsl_hash_cuckoo;

unsigned int cdsl_hash_cuckoo_hash_function(unsigned int max);
void cdsl_hash_cuckoo_rd_lock_request(cdsl_hash_cuckoo * ht, unsigned int hashkey);
void cdsl_hash_cuckoo_wr_lock_request(cdsl_hash_cuckoo * ht, unsigned int hashkey);
void cdsl_hash_cuckoo_lock_release(cdsl_hash_cuckoo * ht, unsigned int hashkey);
int cdsl_hash_cuckoo_put(cdsl_hash_cuckoo *ht, unsigned int cuckoo_key, unsigned int val);
int cdsl_hash_cuckoo_find(cdsl_hash_cuckoo * ht, unsigned int key);
int cdsl_hash_cuckoo_delete(cdsl_hash_cuckoo * ht, unsigned int key);

cdsl_hash_cuckoo* cdsl_hash_cuckoo_init(unsigned int nr_probes, unsigned int total_nr_elements);
unsigned int cdsl_hash_cuckoo_insert(unsigned int cpu_id, cdsl_hash_cuckoo *ht, const unsigned int key, const unsigned int val);
unsigned int cdsl_hash_cuckoo_lookup(unsigned int cpu_id, cdsl_hash_cuckoo * ht, unsigned int key);
unsigned int cdsl_hash_cuckoo_remove(unsigned int cpu_id, cdsl_hash_cuckoo * ht, unsigned int key);
/********************************************************************************************/
#endif
