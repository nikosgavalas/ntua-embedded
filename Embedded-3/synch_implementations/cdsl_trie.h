#ifndef __CDSL_TRIE_H__
#define __CDSL_TRIE_H__

#include <assert.h>
#include "cdsl_lock_definitions.h"

/********************** Radix Tree struct  ***************************************/
typedef struct cdsl_radix_tree_struct {
	unsigned int key;						// Node key
	char b;									// Bit to check
	struct cdsl_radix_tree_struct *left;			// Pointer to left node
	struct cdsl_radix_tree_struct *right;			// Pointer to right node

	unsigned int (*insert)(unsigned int, struct cdsl_radix_tree_struct*, unsigned int, unsigned int);
	unsigned int (*lookup)(unsigned int, struct cdsl_radix_tree_struct*, unsigned int);
	unsigned int (*remove)(unsigned int, struct cdsl_radix_tree_struct*, unsigned int);

	unsigned int data;
	cdsl_rw_pthread_lock lock;
} cdsl_radix_tree;
/*********************************************************************************/

/***************************** Radix Tree functions ******************************/
// Return whether or not bit 'i' is set in 'key'.
unsigned int cdsl_radix_tree_bit(int i, unsigned int key);
// Insert a tree node recursively :
cdsl_radix_tree* cdsl_radix_tree_insert_recur(cdsl_radix_tree *head, cdsl_radix_tree *h, cdsl_radix_tree *n, int d);
// Insert a new node:
unsigned int cdsl_radix_tree_insert(unsigned int cpu_id, cdsl_radix_tree *root, unsigned int key, unsigned int value);
// Remove an entry given a key in the Radix Tree:
unsigned int cdsl_radix_tree_remove(unsigned int cpu_id, cdsl_radix_tree *root, unsigned int key);
// Find an entry given a key in a Patricia trie.
unsigned int cdsl_radix_tree_lookup(unsigned int cpu_id, cdsl_radix_tree *root, unsigned int key);
// Radix Tree initialization function:
cdsl_radix_tree* cdsl_radix_tree_init(void);
/*********************************************************************************/

/*********************** ctrie structs and definitions ***************************/
#define CDSL_CTRIE_GET_NODE_TYPE(options) (options & 7)
#define CDSL_CTRIE_SET_NODE_TYPE(options, type) {options &= 248; options |= type;}
#define CDSL_CTRIE_EMPTY_KVNODE 0
#define CDSL_CTRIE_KVNODE_ID 	1
#define CDSL_CTRIE_LNODE_ID 	3
#define CDSL_CTRIE_CNODE_ID 	4

#define CDSL_CTRIE_OK 0
#define CDSL_CTRIE_RESTART 1
#define CDSL_CTRIE_NOT_FOUND 2


#define CDSL_CTRIE_HASH_INDEX_LVL_BIT_CNT 6
#define CDSL_CTRIE_HASH_INDEX_MAX_BIT_CNT 6

#define CDSL_CTRIE_HASH_INDEX(hash, bitcount) ((hash >> bitcount) & 63)
#define CDSL_CTRIE_CAS2(ptr, expected, desired)                        \
    __sync_bool_compare_and_swap_8((volatile void *) ptr,  \
    		(uint64_t) expected,    \
                                   (uint64_t) desired)
#define CDSL_CTRIE_SET_BRANCH_TYPES(a, n) (memcpy(a, (unsigned int [64])           \
    {n,n,n,n,n,n,n,n,n,n,n,n,n,n,n,n,n,n,n,n,n,n,n,n,n,n,n,n,n,n,n,n,  \
     n,n,n,n,n,n,n,n,n,n,n,n,n,n,n,n,n,n,n,n,n,n,n,n,n,n,n,n,n,n,n,n}, \
     64*sizeof(unsigned int)))

#define CDSL_CTRIE_CNODE_ARRAY_LEN 64
typedef unsigned long long cdsl_ctrie_uint64;

typedef struct cdsl_ctrie_struct {
	cdsl_ctrie_uint64 options;
    void **root;

	unsigned int (*insert)(unsigned int, struct cdsl_ctrie_struct*, unsigned int, unsigned int);
	unsigned int (*lookup)(unsigned int, struct cdsl_ctrie_struct*, unsigned int);
	unsigned int (*remove)(unsigned int, struct cdsl_ctrie_struct*, unsigned int);

} cdsl_ctrie;

typedef struct cdsl_ctrie_kvnode_struct {
	cdsl_ctrie_uint64 key;
	unsigned int value;
} cdsl_ctrie_kvnode;

typedef struct cnode {
    // TODO: maybe put inode gen in some bits in branch_type?
	cdsl_ctrie_uint64 branch_types[CDSL_CTRIE_CNODE_ARRAY_LEN];
    cdsl_ctrie_kvnode branches[CDSL_CTRIE_CNODE_ARRAY_LEN];
} cdsl_ctrie_cnode;

typedef struct cdsl_ctrie_lnode_struct {
    int kvs_len;
    cdsl_ctrie_kvnode *kvs;
} cdsl_ctrie_lnode;
/*********************************************************************************/

/********************************* ctrie functions *******************************/
unsigned int cdsl_ctrie_internal_delete_cn(struct cnode *cn);
cdsl_ctrie_uint64 cdsl_ctrie_internal_insert(cdsl_ctrie *ct, cdsl_ctrie_uint64 key, cdsl_ctrie_uint64 value);
cdsl_ctrie_uint64 cdsl_ctrie_internal_lookup(cdsl_ctrie *ct, cdsl_ctrie_kvnode *lookup_kv);
cdsl_ctrie_uint64 cdsl_ctrie_internal_delete(cdsl_ctrie *ct, cdsl_ctrie_kvnode *delete_kv);

cdsl_ctrie *cdsl_ctrie_init(void);
unsigned int cdsl_ctrie_insert(unsigned int cpu_id, cdsl_ctrie *ct, unsigned int key, unsigned int value);
unsigned int cdsl_ctrie_lookup(unsigned int cpu_id, cdsl_ctrie *ct, unsigned int key);
unsigned int cdsl_ctrie_remove(unsigned int cpu_id, cdsl_ctrie  *ct, unsigned int key);
/*********************************************************************************/
#endif
