#ifndef __CDSL_BP_TREE_H__
#define __CDSL_BP_TREE_H__

#include <assert.h>
#include "cdsl_lock_definitions.h"

/********************** B+ Tree struct *******************************************/
/* The order determines the maximum and minimum number of entries (keys and pointers) in any
 * node.  Every node has at most order - 1 keys and at least (roughly speaking) half that number.
 * Every leaf has as many pointers to data as keys, and every internal node has one more pointer
 * to a subtree than the number of keys. This global variable is initialized to the default value. */
#define CDSL_BP_TREE_ORDER	4

/* Type representing the record to which a given key refers. In a real B+ tree system, the
 * record would hold data (in a database) or a file (in an operating system)
 * or some other information. Users can rewrite this part of the code to change the type and content
 * of the value field. */
typedef struct cdsl_bp_tree_record_struct {
	void *value;
} cdsl_bp_tree_record;

/* Type representing a node in the B+ tree. This type is general enough to serve for both
 * the leaf and the internal node. The heart of the node is the array
 * of keys and the array of corresponding pointers.  The relation between keys
 * and pointers differs between leaves and internal nodes.  In a leaf, the index
 * of each key equals the index of its corresponding pointer, with a maximum of order - 1 key-pointer
 * pairs.  The last pointer points to the leaf to the right (or NULL in the case of the rightmost leaf).
 * In an internal node, the first pointer refers to lower nodes with keys less than
 * the smallest key in the keys array.  Then, with indices i starting at 0, the pointer
 * at i + 1 points to the subtree with keys greater than or equal to the key in this node at index i.
 * The num_keys field is used to keep track of the number of valid keys. In an internal node, the number of valid
 * pointers is always num_keys + 1. In a leaf, the number of valid pointers to data is always num_keys.  The
 * last leaf pointer points to the next leaf. */
typedef struct cdsl_bp_tree_node_struct {
	void ** pointers;
	int * keys;
	struct cdsl_bp_tree_node_struct * parent;
	unsigned int is_leaf;
	int num_keys;
	struct cdsl_bp_tree_node_struct * next; // Used for queue.
} cdsl_bp_tree_node;

typedef struct cdsl_bp_tree_struct {
	cdsl_bp_tree_node *root;

	unsigned int (*insert)(unsigned int, struct cdsl_bp_tree_struct*, unsigned int, void*);
	unsigned int (*lookup)(unsigned int, struct cdsl_bp_tree_struct*, unsigned int);
	unsigned int (*remove)(unsigned int, struct cdsl_bp_tree_struct*, unsigned int);
	void* 		 (*remove_min)(unsigned int, struct cdsl_bp_tree_struct*);
	void* 		 (*lookup_get)(unsigned int, struct cdsl_bp_tree_struct*, unsigned int);
	unsigned int (*get_min_key)(unsigned int, struct cdsl_bp_tree_struct*);

	cdsl_rw_pthread_lock lock;
} cdsl_bp_tree;

// Output and utility.
cdsl_bp_tree_node* cdsl_bp_tree_find_leaf(cdsl_bp_tree_node *root, int key);
cdsl_bp_tree_record* cdsl_bp_tree_find(cdsl_bp_tree_node *root, int key);
int cdsl_bp_tree_cut(int length);

// Insertion.
cdsl_bp_tree_record* cdsl_bp_tree_make_record(void *value);
cdsl_bp_tree_node* cdsl_bp_tree_make_node(void);
cdsl_bp_tree_node* cdsl_bp_tree_make_leaf(void);
int cdsl_bp_tree_get_left_index(cdsl_bp_tree_node *parent, cdsl_bp_tree_node *left);
cdsl_bp_tree_node* cdsl_bp_tree_insert_into_leaf( cdsl_bp_tree_node *leaf, int key, cdsl_bp_tree_record *pointer );
cdsl_bp_tree_node* cdsl_bp_tree_insert_into_leaf_after_splitting(cdsl_bp_tree_node *root, cdsl_bp_tree_node *leaf, int key, cdsl_bp_tree_record *pointer);
cdsl_bp_tree_node* cdsl_bp_tree_insert_into_node(cdsl_bp_tree_node * root, cdsl_bp_tree_node * parent, int left_index, int key, cdsl_bp_tree_node *right);
cdsl_bp_tree_node* cdsl_bp_tree_insert_into_node_after_splitting(cdsl_bp_tree_node *root, cdsl_bp_tree_node *parent, int left_index, int key, cdsl_bp_tree_node *right);
cdsl_bp_tree_node* cdsl_bp_tree_insert_into_parent(cdsl_bp_tree_node * root, cdsl_bp_tree_node *left, int key, cdsl_bp_tree_node *right);
cdsl_bp_tree_node* cdsl_bp_tree_insert_into_new_root(cdsl_bp_tree_node *left, int key, cdsl_bp_tree_node *right);
cdsl_bp_tree_node* cdsl_bp_tree_start_new_tree(int key, cdsl_bp_tree_record *pointer);

// Deletion.
int cdsl_bp_tree_get_neighbor_index(cdsl_bp_tree_node *n);
cdsl_bp_tree_node* cdsl_bp_tree_adjust_root(cdsl_bp_tree_node *root);
cdsl_bp_tree_node* cdsl_bp_tree_coalesce_nodes(cdsl_bp_tree_node *root, cdsl_bp_tree_node *n, cdsl_bp_tree_node *neighbor, int neighbor_index, int k_prime);
cdsl_bp_tree_node* cdsl_bp_tree_redistribute_nodes(cdsl_bp_tree_node * root, cdsl_bp_tree_node *n, cdsl_bp_tree_node *neighbor, int neighbor_index, int k_prime_index, int k_prime);
cdsl_bp_tree_node* cdsl_bp_tree_delete_entry(cdsl_bp_tree_node *root, cdsl_bp_tree_node *n, int key, void *pointer );

cdsl_bp_tree* cdsl_bp_tree_init(void);
unsigned int cdsl_bp_tree_insert(unsigned int cpu_id, cdsl_bp_tree *tree, unsigned int key, void *value);
unsigned int cdsl_bp_tree_lookup(unsigned int cpu_id, cdsl_bp_tree *tree, unsigned int key);
unsigned int cdsl_bp_tree_remove(unsigned int cpu_id, cdsl_bp_tree *tree, unsigned int key);
void* cdsl_bp_tree_remove_min(unsigned int cpu_id, cdsl_bp_tree *tree);
void* cdsl_bp_tree_lookup_get(unsigned int cpu_id, cdsl_bp_tree *tree, unsigned int key);
unsigned int cdsl_bp_tree_get_min_key(unsigned int cpu_id, cdsl_bp_tree *tree);
/*********************************************************************************/
#endif
