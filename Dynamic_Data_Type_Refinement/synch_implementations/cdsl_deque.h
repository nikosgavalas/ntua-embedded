#ifndef __CDSL_DEQUE_H__
#define __CDSL_DEQUE_H__

#include "cdsl_config.h"
#include "cdsl_memory_pool.h"
#include "cdsl_lock_definitions.h"
#include "cdsl_rcl_definitions.h"
#include "cdsl_lockless_definitions.h"

/***************************** Common deque definitions ********************************/
typedef struct cdsl_deque_node_struct {
	struct cdsl_deque_node_struct* prev;
	struct cdsl_deque_node_struct* next;
	void *data;
} cdsl_deque_node;
typedef cdsl_deque_node* iterator_cdsl_deque_list_lock_based_pthread;

/*****************************************************************************************/

/*****************************************************************************************
*****************************************	List   ***************************************
*****************************************************************************************/

/****************************** List - Lock-based ***************************************/
/* pthread mutex */
typedef struct cdsl_deque_list_lock_based_pthread_struct {
	cdsl_deque_node *head;
	cdsl_deque_node *tail;

	void  (*push_tail)(unsigned int, struct cdsl_deque_list_lock_based_pthread_struct*, void *);
	void* (*pop_tail)(unsigned int, struct cdsl_deque_list_lock_based_pthread_struct*);
	void* (*pop_head)(unsigned int, struct cdsl_deque_list_lock_based_pthread_struct*);
	// remove(random number, pointer to cdsl_dll, pointer to data to be removed). Returns pointer to data removed or NULL if cdsl_dll was empty or data was not found: 
	void* (*remove)(unsigned int, struct cdsl_deque_list_lock_based_pthread_struct*, void*);
	// get_head(random number, pointer to cdsl_dll). Returns pointer to data at head, or NULL if cdsl_dll is empty:
	void* (*get_head)(unsigned int, struct cdsl_deque_list_lock_based_pthread_struct*);
	// enqueue(random number, pointer to cdsl_dll, pointer to data):
	void  (*enqueue)(unsigned int, struct cdsl_deque_list_lock_based_pthread_struct*, void*);
	// dequeue(random number, pointer to cdsl_dll). Returns pointer to data dequeued or NULL if the cdsl_dll was empty.
	void* (*dequeue)(unsigned int, struct cdsl_deque_list_lock_based_pthread_struct*);
	// iter_begin(pointer to cdsl_dll). Returns cdsl_dll iterator:
	iterator_cdsl_deque_list_lock_based_pthread (*iter_begin)(struct cdsl_deque_list_lock_based_pthread_struct*);
	// iter_end(pointer to cdsl_dll). Returns cdsl_dll iterator (actually returns NULL):
	iterator_cdsl_deque_list_lock_based_pthread (*iter_end)(struct cdsl_deque_list_lock_based_pthread_struct*);
	// iter_next(iterator). Returns cdsl_dll iterator:
	iterator_cdsl_deque_list_lock_based_pthread (*iter_next)(iterator_cdsl_deque_list_lock_based_pthread);
	// iter_deref(pointer to cdsl_dll, iterator). Returns pointer to data:
	void* (*iter_deref)(struct cdsl_deque_list_lock_based_pthread_struct*, iterator_cdsl_deque_list_lock_based_pthread);

	cdsl_pthread_lock h_lock;
	cdsl_pthread_lock t_lock;

#ifdef FREELISTS_ENABLED
	cdsl_freelist *freelist[NR_CORES];
#endif
} cdsl_deque_list_lock_based_pthread;

typedef cdsl_deque_list_lock_based_pthread cdsl_dll;

cdsl_deque_list_lock_based_pthread* cdsl_deque_list_lock_based_pthread_init(void);
void cdsl_deque_list_lock_based_pthread_push_tail(unsigned int cpu_id, cdsl_deque_list_lock_based_pthread* q, void* data);
void* cdsl_deque_list_lock_based_pthread_pop_tail(unsigned int cpu_id, cdsl_deque_list_lock_based_pthread* q);
void* cdsl_deque_list_lock_based_pthread_pop_head(unsigned int cpu_id, cdsl_deque_list_lock_based_pthread* q);
void* cdsl_deque_list_lock_based_pthread_remove(unsigned int cpu_id, cdsl_deque_list_lock_based_pthread* list, void *data);
void* cdsl_deque_list_lock_based_pthread_get_head(unsigned int cpu_id, cdsl_deque_list_lock_based_pthread* list);
void cdsl_deque_list_lock_based_pthread_enqueue(unsigned int cpu_id, cdsl_deque_list_lock_based_pthread *q, void *data);
void* cdsl_deque_list_lock_based_pthread_dequeue(unsigned int cpu_id, cdsl_deque_list_lock_based_pthread *q);
iterator_cdsl_deque_list_lock_based_pthread iter_begin_cdsl_deque_list_lock_based_pthread(cdsl_deque_list_lock_based_pthread *list);
iterator_cdsl_deque_list_lock_based_pthread iter_end_cdsl_deque_list_lock_based_pthread(cdsl_deque_list_lock_based_pthread *list);
iterator_cdsl_deque_list_lock_based_pthread iter_next_cdsl_deque_list_lock_based_pthread(iterator_cdsl_deque_list_lock_based_pthread i);
void* iter_deref_cdsl_deque_list_lock_based_pthread(cdsl_deque_list_lock_based_pthread *list, iterator_cdsl_deque_list_lock_based_pthread i);
cdsl_deque_list_lock_based_pthread* cdsl_dll_init(void);


typedef iterator_cdsl_deque_list_lock_based_pthread iterator_cdsl_dll;

/* pthread spinlock */
typedef struct cdsl_deque_list_lock_based_pthread_spinlock_struct {
	cdsl_deque_node *head;
	cdsl_deque_node *tail;

	void  (*push_tail)(unsigned int, struct cdsl_deque_list_lock_based_pthread_spinlock_struct*, void *);
	void* (*pop_tail)(unsigned int, struct cdsl_deque_list_lock_based_pthread_spinlock_struct*);
	void* (*pop_head)(unsigned int, struct cdsl_deque_list_lock_based_pthread_spinlock_struct*);

	cdsl_pthread_spinlock h_lock;
	cdsl_pthread_spinlock t_lock;

#ifdef FREELISTS_ENABLED
	cdsl_freelist *freelist[NR_CORES];
#endif
} cdsl_deque_list_lock_based_pthread_spinlock;

cdsl_deque_list_lock_based_pthread_spinlock* cdsl_deque_list_lock_based_pthread_spinlock_init(void);
void cdsl_deque_list_lock_based_pthread_spinlock_push_tail(unsigned int cpu_id, cdsl_deque_list_lock_based_pthread_spinlock* q, void* data);
void* cdsl_deque_list_lock_based_pthread_spinlock_pop_tail(unsigned int cpu_id, cdsl_deque_list_lock_based_pthread_spinlock* q);
void* cdsl_deque_list_lock_based_pthread_spinlock_pop_head(unsigned int cpu_id, cdsl_deque_list_lock_based_pthread_spinlock* q);
void* cdsl_deque_list_lock_based_pthread_get_head(unsigned int cpu_id, cdsl_deque_list_lock_based_pthread* list);

/* tas lock */
typedef struct cdsl_deque_list_lock_based_tas_struct {
	cdsl_deque_node *head;
	cdsl_deque_node *tail;

	void  (*push_tail)(unsigned int, struct cdsl_deque_list_lock_based_tas_struct*, void *);
	void* (*pop_tail)(unsigned int, struct cdsl_deque_list_lock_based_tas_struct*);
	void* (*pop_head)(unsigned int, struct cdsl_deque_list_lock_based_tas_struct*);

	cdsl_tas_lock h_lock;
	cdsl_tas_lock t_lock;

#ifdef FREELISTS_ENABLED
	cdsl_freelist *freelist[NR_CORES];
#endif
} cdsl_deque_list_lock_based_tas;

cdsl_deque_list_lock_based_tas* cdsl_deque_list_lock_based_tas_init(void);
void cdsl_deque_list_lock_based_tas_push_tail(unsigned int cpu_id, cdsl_deque_list_lock_based_tas* q, void* data);
void* cdsl_deque_list_lock_based_tas_pop_tail(unsigned int cpu_id, cdsl_deque_list_lock_based_tas* q);
void* cdsl_deque_list_lock_based_tas_pop_head(unsigned int cpu_id, cdsl_deque_list_lock_based_tas* q);

/* ttas lock */
typedef struct cdsl_deque_list_lock_based_ttas_struct {
	cdsl_deque_node *head;
	cdsl_deque_node *tail;

	void  (*push_tail)(unsigned int, struct cdsl_deque_list_lock_based_ttas_struct*, void *);
	void* (*pop_tail)(unsigned int, struct cdsl_deque_list_lock_based_ttas_struct*);
	void* (*pop_head)(unsigned int, struct cdsl_deque_list_lock_based_ttas_struct*);

	cdsl_ttas_lock h_lock;
	cdsl_ttas_lock t_lock;

#ifdef FREELISTS_ENABLED
	cdsl_freelist *freelist[NR_CORES];
#endif
} cdsl_deque_list_lock_based_ttas;

cdsl_deque_list_lock_based_ttas* cdsl_deque_list_lock_based_ttas_init(void);
void cdsl_deque_list_lock_based_ttas_push_tail(unsigned int cpu_id, cdsl_deque_list_lock_based_ttas* q, void* data);
void* cdsl_deque_list_lock_based_ttas_pop_tail(unsigned int cpu_id, cdsl_deque_list_lock_based_ttas* q);
void* cdsl_deque_list_lock_based_ttas_pop_head(unsigned int cpu_id, cdsl_deque_list_lock_based_ttas* q);

/* ttas lock with linear backoff */
typedef struct cdsl_deque_list_lock_based_ttas_backoff_linear_struct {
	cdsl_deque_node *head;
	cdsl_deque_node *tail;

	void  (*push_tail)(unsigned int, struct cdsl_deque_list_lock_based_ttas_backoff_linear_struct*, void *);
	void* (*pop_tail)(unsigned int, struct cdsl_deque_list_lock_based_ttas_backoff_linear_struct*);
	void* (*pop_head)(unsigned int, struct cdsl_deque_list_lock_based_ttas_backoff_linear_struct*);

	cdsl_ttas_backoff_linear_lock h_lock;
	cdsl_ttas_backoff_linear_lock t_lock;

#ifdef FREELISTS_ENABLED
	cdsl_freelist *freelist[NR_CORES];
#endif
} cdsl_deque_list_lock_based_ttas_backoff_linear;

cdsl_deque_list_lock_based_ttas_backoff_linear* cdsl_deque_list_lock_based_ttas_backoff_linear_init(unsigned int sleep_time);
void cdsl_deque_list_lock_based_ttas_backoff_linear_push_tail(unsigned int cpu_id, cdsl_deque_list_lock_based_ttas_backoff_linear* q, void* data);
void* cdsl_deque_list_lock_based_ttas_backoff_linear_pop_tail(unsigned int cpu_id, cdsl_deque_list_lock_based_ttas_backoff_linear* q);
void* cdsl_deque_list_lock_based_ttas_backoff_linear_pop_head(unsigned int cpu_id, cdsl_deque_list_lock_based_ttas_backoff_linear* q);

/* ttas lock with exp backoff */
typedef struct cdsl_deque_list_lock_based_ttas_backoff_exp_struct {
	cdsl_deque_node *head;
	cdsl_deque_node *tail;

	void  (*push_tail)(unsigned int, struct cdsl_deque_list_lock_based_ttas_backoff_exp_struct*, void *);
	void* (*pop_tail)(unsigned int, struct cdsl_deque_list_lock_based_ttas_backoff_exp_struct*);
	void* (*pop_head)(unsigned int, struct cdsl_deque_list_lock_based_ttas_backoff_exp_struct*);

	cdsl_ttas_backoff_exp_lock h_lock;
	cdsl_ttas_backoff_exp_lock t_lock;

#ifdef FREELISTS_ENABLED
	cdsl_freelist *freelist[NR_CORES];
#endif
} cdsl_deque_list_lock_based_ttas_backoff_exp;

cdsl_deque_list_lock_based_ttas_backoff_exp* cdsl_deque_list_lock_based_ttas_backoff_exp_init(void);
void cdsl_deque_list_lock_based_ttas_backoff_exp_push_tail(unsigned int cpu_id, cdsl_deque_list_lock_based_ttas_backoff_exp* q, void* data);
void* cdsl_deque_list_lock_based_ttas_backoff_exp_pop_tail(unsigned int cpu_id, cdsl_deque_list_lock_based_ttas_backoff_exp* q);
void* cdsl_deque_list_lock_based_ttas_backoff_exp_pop_head(unsigned int cpu_id, cdsl_deque_list_lock_based_ttas_backoff_exp* q);
/******************************************************************************************/

/**************************************** List - RCL **************************************/
/*
typedef struct cdsl_deque_list_rcl_struct {
	cdsl_deque_node *head;
	cdsl_deque_node *tail;

	void  (*push_tail)(struct cdsl_deque_list_rcl_struct*, void *);
	volatile void* (*pop_tail)(struct cdsl_deque_list_rcl_struct*);
	volatile void* (*pop_head)(struct cdsl_deque_list_rcl_struct*);

} cdsl_deque_list_rcl;

cdsl_deque_list_rcl* cdsl_deque_list_rcl_init(cdsl_deque_list_rcl* q);
void* cdsl_deque_list_rcl_server_start(cdsl_deque_list_rcl* q);
void cdsl_deque_list_rcl_server_push_tail(cdsl_deque_list_rcl* q, void* data);
void* cdsl_deque_list_rcl_server_pop_tail(cdsl_deque_list_rcl* q);
void* cdsl_deque_list_rcl_server_pop_head(cdsl_deque_list_rcl* q);
void cdsl_deque_list_rcl_client_push_tail(cdsl_deque_list_rcl* q, void* data);
volatile void* cdsl_deque_list_rcl_client_pop_tail(cdsl_deque_list_rcl* q);
volatile void* cdsl_deque_list_rcl_client_pop_head(cdsl_deque_list_rcl* q);
void cdsl_deque_rcl_list_client_exit(void);
*/
/******************************************************************************************/

/**********************************  List - Lockless **************************************/
/* no deque list lockless implementation */

/*****************************************************************************************
************************************   CYCLIC ARRAY   ************************************
*****************************************************************************************/
// Cyclic array initial size:
#define CDSL_DEQUE_CYCLIC_ARRAY_INIT_SIZE		1048576
#define CDSL_DEQUE_CYCLIC_ARRAY_MAX_SIZE		(1 << 20)

/****************************** Cyclic Array - Lock-based **********************************/
/* pthread mutex */
typedef struct cdsl_deque_cyclic_array_lock_based_pthread_struct {
	volatile size_t head;
	volatile size_t tail;
	unsigned long long size;
	void **ptr_array;

	void  (*push_tail)(unsigned int, struct cdsl_deque_cyclic_array_lock_based_pthread_struct*, void *);
	void* (*pop_tail)(unsigned int, struct cdsl_deque_cyclic_array_lock_based_pthread_struct*);
	void* (*pop_head)(unsigned int, struct cdsl_deque_cyclic_array_lock_based_pthread_struct*);

	cdsl_pthread_lock h_lock;
	cdsl_pthread_lock t_lock;
} cdsl_deque_cyclic_array_lock_based_pthread;

cdsl_deque_cyclic_array_lock_based_pthread* cdsl_deque_cyclic_array_lock_based_pthread_init(void);
void cdsl_deque_cyclic_array_lock_based_pthread_push_tail(unsigned int cpu_id, cdsl_deque_cyclic_array_lock_based_pthread* q, void* data);
void* cdsl_deque_cyclic_array_lock_based_pthread_pop_tail(unsigned int cpu_id, cdsl_deque_cyclic_array_lock_based_pthread* q);
void* cdsl_deque_cyclic_array_lock_based_pthread_pop_head(unsigned int cpu_id, cdsl_deque_cyclic_array_lock_based_pthread* q);

/* pthread spinlock */
typedef struct cdsl_deque_cyclic_array_lock_based_pthread_spinlock_struct {
	volatile size_t head;
	volatile size_t tail;
	unsigned long long size;
	void **ptr_array;

	void  (*push_tail)(unsigned int, struct cdsl_deque_cyclic_array_lock_based_pthread_spinlock_struct*, void *);
	void* (*pop_tail)(unsigned int, struct cdsl_deque_cyclic_array_lock_based_pthread_spinlock_struct*);
	void* (*pop_head)(unsigned int, struct cdsl_deque_cyclic_array_lock_based_pthread_spinlock_struct*);

	cdsl_pthread_spinlock h_lock;
	cdsl_pthread_spinlock t_lock;
} cdsl_deque_cyclic_array_lock_based_pthread_spinlock;

cdsl_deque_cyclic_array_lock_based_pthread_spinlock* cdsl_deque_cyclic_array_lock_based_pthread_spinlock_init(void);
void cdsl_deque_cyclic_array_lock_based_pthread_spinlock_push_tail(unsigned int cpu_id, cdsl_deque_cyclic_array_lock_based_pthread_spinlock* q, void* data);
void* cdsl_deque_cyclic_array_lock_based_pthread_spinlock_pop_tail(unsigned int cpu_id, cdsl_deque_cyclic_array_lock_based_pthread_spinlock* q);
void* cdsl_deque_cyclic_array_lock_based_pthread_spinlock_pop_head(unsigned int cpu_id, cdsl_deque_cyclic_array_lock_based_pthread_spinlock* q);

/* tas lock */
typedef struct cdsl_deque_cyclic_array_lock_based_tas_struct {
	volatile size_t head;
	volatile size_t tail;
	unsigned long long size;
	void **ptr_array;

	void  (*push_tail)(unsigned int, struct cdsl_deque_cyclic_array_lock_based_tas_struct*, void *);
	void* (*pop_tail)(unsigned int, struct cdsl_deque_cyclic_array_lock_based_tas_struct*);
	void* (*pop_head)(unsigned int, struct cdsl_deque_cyclic_array_lock_based_tas_struct*);

	cdsl_tas_lock h_lock;
	cdsl_tas_lock t_lock;
} cdsl_deque_cyclic_array_lock_based_tas;

cdsl_deque_cyclic_array_lock_based_tas* cdsl_deque_cyclic_array_lock_based_tas_init(void);
void cdsl_deque_cyclic_array_lock_based_tas_push_tail(unsigned int cpu_id, cdsl_deque_cyclic_array_lock_based_tas* q, void* data);
void* cdsl_deque_cyclic_array_lock_based_tas_pop_tail(unsigned int cpu_id, cdsl_deque_cyclic_array_lock_based_tas* q);
void* cdsl_deque_cyclic_array_lock_based_tas_pop_head(unsigned int cpu_id, cdsl_deque_cyclic_array_lock_based_tas* q);

/* ttas lock */
typedef struct cdsl_deque_cyclic_array_lock_based_ttas_struct {
	volatile size_t head;
	volatile size_t tail;
	unsigned long long size;
	void **ptr_array;

	void  (*push_tail)(unsigned int, struct cdsl_deque_cyclic_array_lock_based_ttas_struct*, void *);
	void* (*pop_tail)(unsigned int, struct cdsl_deque_cyclic_array_lock_based_ttas_struct*);
	void* (*pop_head)(unsigned int, struct cdsl_deque_cyclic_array_lock_based_ttas_struct*);

	cdsl_ttas_lock h_lock;
	cdsl_ttas_lock t_lock;
} cdsl_deque_cyclic_array_lock_based_ttas;

cdsl_deque_cyclic_array_lock_based_ttas* cdsl_deque_cyclic_array_lock_based_ttas_init(void);
void cdsl_deque_cyclic_array_lock_based_ttas_push_tail(unsigned int cpu_id, cdsl_deque_cyclic_array_lock_based_ttas* q, void* data);
void* cdsl_deque_cyclic_array_lock_based_ttas_pop_tail(unsigned int cpu_id, cdsl_deque_cyclic_array_lock_based_ttas* q);
void* cdsl_deque_cyclic_array_lock_based_ttas_pop_head(unsigned int cpu_id, cdsl_deque_cyclic_array_lock_based_ttas* q);

/* ttas backoff linear */
typedef struct cdsl_deque_cyclic_array_lock_based_ttas_backoff_linear_struct {
	volatile size_t head;
	volatile size_t tail;
	unsigned long long size;
	void **ptr_array;

	void  (*push_tail)(unsigned int, struct cdsl_deque_cyclic_array_lock_based_ttas_backoff_linear_struct*, void *);
	void* (*pop_tail)(unsigned int, struct cdsl_deque_cyclic_array_lock_based_ttas_backoff_linear_struct*);
	void* (*pop_head)(unsigned int, struct cdsl_deque_cyclic_array_lock_based_ttas_backoff_linear_struct*);

	cdsl_ttas_backoff_linear_lock h_lock;
	cdsl_ttas_backoff_linear_lock t_lock;
} cdsl_deque_cyclic_array_lock_based_ttas_backoff_linear;

cdsl_deque_cyclic_array_lock_based_ttas_backoff_linear* cdsl_deque_cyclic_array_lock_based_ttas_backoff_linear_init(unsigned int sleep_time);
void cdsl_deque_cyclic_array_lock_based_ttas_backoff_linear_push_tail(unsigned int cpu_id, cdsl_deque_cyclic_array_lock_based_ttas_backoff_linear* q, void* data);
void* cdsl_deque_cyclic_array_lock_based_ttas_backoff_linear_pop_tail(unsigned int cpu_id, cdsl_deque_cyclic_array_lock_based_ttas_backoff_linear* q);
void* cdsl_deque_cyclic_array_lock_based_ttas_backoff_linear_pop_head(unsigned int cpu_id, cdsl_deque_cyclic_array_lock_based_ttas_backoff_linear* q);

/* ttas backoff exp */
typedef struct cdsl_deque_cyclic_array_lock_based_ttas_backoff_exp_struct {
	volatile size_t head;
	volatile size_t tail;
	unsigned long long size;
	void **ptr_array;

	void  (*push_tail)(unsigned int, struct cdsl_deque_cyclic_array_lock_based_ttas_backoff_exp_struct*, void *);
	void* (*pop_tail)(unsigned int, struct cdsl_deque_cyclic_array_lock_based_ttas_backoff_exp_struct*);
	void* (*pop_head)(unsigned int, struct cdsl_deque_cyclic_array_lock_based_ttas_backoff_exp_struct*);

	cdsl_ttas_backoff_exp_lock h_lock;
	cdsl_ttas_backoff_exp_lock t_lock;
} cdsl_deque_cyclic_array_lock_based_ttas_backoff_exp;

cdsl_deque_cyclic_array_lock_based_ttas_backoff_exp* cdsl_deque_cyclic_array_lock_based_ttas_backoff_exp_init(void);
void cdsl_deque_cyclic_array_lock_based_ttas_backoff_exp_push_tail(unsigned int cpu_id, cdsl_deque_cyclic_array_lock_based_ttas_backoff_exp* q, void* data);
void* cdsl_deque_cyclic_array_lock_based_ttas_backoff_exp_pop_tail(unsigned int cpu_id, cdsl_deque_cyclic_array_lock_based_ttas_backoff_exp* q);
void* cdsl_deque_cyclic_array_lock_based_ttas_backoff_exp_pop_head(unsigned int cpu_id, cdsl_deque_cyclic_array_lock_based_ttas_backoff_exp* q);
/******************************************************************************************/

/************************************ Cyclic Array - RCL ***********************************/
/*
volatile void* cdsl_deque_cyclic_array_rcl_buffer[CDSL_DEQUE_RCL_NR_CLIENTS];
pthread_mutex_t cdsl_deque_cyclic_array_rcl_mtx[CDSL_DEQUE_RCL_NR_CLIENTS];
pthread_cond_t cdsl_deque_cyclic_array_rcl_cond_var[CDSL_DEQUE_RCL_NR_CLIENTS];

typedef struct cdsl_deque_cyclic_array_rcl_struct {
	volatile size_t head;
	volatile size_t tail;
	unsigned long long size;
	void **ptr_array;

	void  (*push_tail)(struct cdsl_deque_cyclic_array_rcl_struct*, void *);
	volatile void* (*pop_tail)(struct cdsl_deque_cyclic_array_rcl_struct*);
	volatile void* (*pop_head)(struct cdsl_deque_cyclic_array_rcl_struct*);

} cdsl_deque_cyclic_array_rcl;

cdsl_deque_cyclic_array_rcl* cdsl_deque_cyclic_array_rcl_init(cdsl_deque_cyclic_array_rcl* q);
void* cdsl_deque_cyclic_array_rcl_server_start(cdsl_deque_cyclic_array_rcl* q);
void cdsl_deque_cyclic_array_rcl_server_push_tail(cdsl_deque_cyclic_array_rcl* q, void* data);
void* cdsl_deque_cyclic_array_rcl_server_pop_tail(cdsl_deque_cyclic_array_rcl* q);
void* cdsl_deque_cyclic_array_rcl_server_pop_head(cdsl_deque_cyclic_array_rcl* q);
void cdsl_deque_cyclic_array_rcl_client_push_tail(cdsl_deque_cyclic_array_rcl* q, void* data);
volatile void* cdsl_deque_cyclic_array_rcl_client_pop_tail(cdsl_deque_cyclic_array_rcl* q);
volatile void* cdsl_deque_cyclic_array_rcl_client_pop_head(cdsl_deque_cyclic_array_rcl* q);
void cdsl_deque_rcl_cyclic_array_client_exit(void);
*/
/******************************************************************************************/

/****************************** Cyclic Array - Lockless **********************************/
typedef struct cdsl_deque_cyclic_array_lockless_struct {
	volatile size_t head;
	volatile size_t tail;
	unsigned long long size;
    void **ptr_array;

	void  (*push_tail)(unsigned int, struct cdsl_deque_cyclic_array_lockless_struct*, void *);
	void* (*pop_tail)(unsigned int, struct cdsl_deque_cyclic_array_lockless_struct*);
	void* (*pop_head)(unsigned int, struct cdsl_deque_cyclic_array_lockless_struct*);

} cdsl_deque_cyclic_array_lockless;

cdsl_deque_cyclic_array_lockless* cdsl_deque_cyclic_array_lockless_init(void);
void cdsl_deque_cyclic_array_lockless_push_tail(unsigned int cpu_id, cdsl_deque_cyclic_array_lockless* q, void* data);
void* cdsl_deque_cyclic_array_lockless_pop_tail(unsigned int cpu_id, cdsl_deque_cyclic_array_lockless* q);
void* cdsl_deque_cyclic_array_lockless_pop_head(unsigned int cpu_id, cdsl_deque_cyclic_array_lockless* q);
/******************************************************************************************/

#endif
