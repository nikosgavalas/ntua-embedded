#ifndef __CDSL_MEMORY_POOL_H__
#define __CDSL_MEMORY_POOL_H__

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define TYPE		unsigned int
#define DATA_SIZE	128 		// Bytes

typedef struct cdsl_freelist_node_struct {
	struct cdsl_freelist_node_struct *next;
	struct cdsl_freelist_node_struct *prev;
	char data[DATA_SIZE];
} cdsl_freelist_node;

typedef struct cdsl_freelist_struct {
	cdsl_freelist_node sentinel;
	cdsl_freelist_node *top;
} cdsl_freelist;

cdsl_freelist* cdsl_freelist_init(unsigned int num_elements);
cdsl_freelist_node* cdsl_freelist_new_node(cdsl_freelist *list);
void cdsl_freelist_push(cdsl_freelist *list, cdsl_freelist_node *node);
cdsl_freelist_node* cdsl_freelist_pop(cdsl_freelist *list);
void* cdsl_malloc(cdsl_freelist *list);
void cdsl_free(cdsl_freelist *list, void *data);

#endif
