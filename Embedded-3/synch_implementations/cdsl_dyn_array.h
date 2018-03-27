#ifndef __CDSL_DYN_ARRAY_H__
#define __CDSL_DYN_ARRAY_H__

#include "cdsl_config.h"
#include "cdsl_memory_pool.h"

#define INIT_SIZE	(4 * sizeof(void*))

/***************************** Common queue definitions **********************************/
typedef unsigned int iterator_cdsl_dyn_array;

typedef struct cdsl_dyn_array_struct {
	void **data_array;
	unsigned int size;
	unsigned int index;
	unsigned int max_index;

	// enqueue(random number, pointer to cdsl_dyn_array_struct, pointer to data):
	void  (*enqueue)(unsigned int, struct cdsl_dyn_array_struct*, void*);
	// dequeue(random number, pointer to cdsl_dyn_array_struct). Returns pointer to data dequeued or NULL if the cdsl_dyn_array_struct was empty.
	void* (*dequeue)(unsigned int, struct cdsl_dyn_array_struct*);
	// remove(random number, pointer to cdsl_dyn_array_struct, pointer to data to be removed). Returns pointer to data removed or NULL if cdsl_dyn_array_struct was empty or data was not found: 
	void* (*remove)(unsigned int, struct cdsl_dyn_array_struct*, void*);
	// get_head(random number, pointer to cdsl_dyn_array_struct). Returns pointer to data at head, or NULL if cdsl_dyn_array_struct is empty:
	void* (*get_head)(unsigned int, struct cdsl_dyn_array_struct*);
	
	// iter_begin(pointer to cdsl_dyn_array_struct). Returns cdsl_dyn_array_struct iterator:
	iterator_cdsl_dyn_array (*iter_begin)(struct cdsl_dyn_array_struct*);
	// iter_end(pointer to cdsl_dyn_array_struct). Returns cdsl_dyn_array_struct iterator (actually returns NULL):
	iterator_cdsl_dyn_array (*iter_end)(struct cdsl_dyn_array_struct*);
	// iter_next(iterator). Returns cdsl_dyn_array_struct iterator:
	iterator_cdsl_dyn_array (*iter_next)(iterator_cdsl_dyn_array);
	// iter_deref(pointer to cdsl_dyn_array_struct, iterator). Returns pointer to data:
	void* (*iter_deref)(struct cdsl_dyn_array_struct*, iterator_cdsl_dyn_array);

} cdsl_dyn_array;
/*****************************************************************************************/

cdsl_dyn_array* cdsl_dyn_array_init(void);
void cdsl_dyn_array_enqueue(unsigned int cpu_id, cdsl_dyn_array *dyn_array, void *data);
void* cdsl_dyn_array_dequeue(unsigned int cpu_id, cdsl_dyn_array *dyn_array);
void* cdsl_dyn_array_remove(unsigned int cpu_id, cdsl_dyn_array* dyn_array, void *data);
void* cdsl_dyn_array_get_head(unsigned int cpu_id, cdsl_dyn_array* dyn_array);
iterator_cdsl_dyn_array iter_begin_cdsl_dyn_array(cdsl_dyn_array *dyn_array);
iterator_cdsl_dyn_array iter_end_cdsl_dyn_array(cdsl_dyn_array *dyn_array);
iterator_cdsl_dyn_array iter_next_cdsl_dyn_array(iterator_cdsl_dyn_array i);
void* iter_deref_cdsl_dyn_array(cdsl_dyn_array *dyn_array, iterator_cdsl_dyn_array i);

#endif
