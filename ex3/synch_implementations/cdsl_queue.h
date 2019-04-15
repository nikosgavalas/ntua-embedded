#ifndef __CDSL_QUEUE_H__
#define __CDSL_QUEUE_H__

#include "cdsl_config.h"
#include "cdsl_memory_pool.h"
#include "cdsl_lock_definitions.h"
#include "cdsl_rcl_definitions.h"
#include "cdsl_lockless_definitions.h"

/***************************** Common queue definitions **********************************/
typedef struct cdsl_queue_node_struct {
	struct cdsl_queue_node_struct* next;
	void *data;
} cdsl_queue_node;

typedef cdsl_queue_node* iterator_cdsl_queue_list_lock_based_pthread;

/*****************************************************************************************/

/*****************************************************************************************
*****************************************	List   ***************************************
*****************************************************************************************/

/****************************** List - Lock-based ***************************************/
/* pthread mutex */
typedef struct cdsl_queue_list_lock_based_pthread_struct {
	cdsl_queue_node *head;
	cdsl_queue_node *tail;

	// enqueue(random number, pointer to cdsl_sll, pointer to data):
	void  (*enqueue)(unsigned int, struct cdsl_queue_list_lock_based_pthread_struct*, void*);
	// dequeue(random number, pointer to cdsl_sll). Returns pointer to data dequeued or NULL if the cdsl_sll was empty.
	void* (*dequeue)(unsigned int, struct cdsl_queue_list_lock_based_pthread_struct*);
	// remove(random number, pointer to cdsl_sll, pointer to data to be removed). Returns pointer to data removed or NULL if cdsl_sll was empty or data was not found: 
	void* (*remove)(unsigned int, struct cdsl_queue_list_lock_based_pthread_struct*, void*);
	// get_head(random number, pointer to cdsl_sll). Returns pointer to data at head, or NULL if cdsl_sll is empty:
	void* (*get_head)(unsigned int, struct cdsl_queue_list_lock_based_pthread_struct*); 
	// iter_begin(pointer to cdsl_sll). Returns cdsl_sll iterator:
	iterator_cdsl_queue_list_lock_based_pthread (*iter_begin)(struct cdsl_queue_list_lock_based_pthread_struct*);
	// iter_end(pointer to cdsl_sll). Returns cdsl_sll iterator (actually returns NULL):
	iterator_cdsl_queue_list_lock_based_pthread (*iter_end)(struct cdsl_queue_list_lock_based_pthread_struct*);
	// iter_next(iterator). Returns cdsl_sll iterator:
	iterator_cdsl_queue_list_lock_based_pthread (*iter_next)(iterator_cdsl_queue_list_lock_based_pthread);
	// iter_deref(pointer to cdsl_sll, iterator). Returns pointer to data:
	void* (*iter_deref)(struct cdsl_queue_list_lock_based_pthread_struct*, iterator_cdsl_queue_list_lock_based_pthread);

	cdsl_pthread_lock h_lock;
	cdsl_pthread_lock t_lock;

#ifdef FREELISTS_ENABLED
	cdsl_freelist *freelist[NR_CORES];
#endif
} cdsl_queue_list_lock_based_pthread;

typedef cdsl_queue_list_lock_based_pthread cdsl_sll;

cdsl_queue_list_lock_based_pthread* cdsl_queue_list_lock_based_pthread_init(void);
void cdsl_queue_list_lock_based_pthread_enqueue(unsigned int cpu_id, cdsl_queue_list_lock_based_pthread *q, void *data);
void* cdsl_queue_list_lock_based_pthread_dequeue(unsigned int cpu_id, cdsl_queue_list_lock_based_pthread *q);
void* cdsl_queue_list_lock_based_pthread_remove(unsigned int cpu_id, cdsl_queue_list_lock_based_pthread* list, void *data);
void* cdsl_queue_list_lock_based_pthread_get_head(unsigned int cpu_id, cdsl_queue_list_lock_based_pthread* list);
iterator_cdsl_queue_list_lock_based_pthread iter_begin_cdsl_queue_list_lock_based_pthread(cdsl_queue_list_lock_based_pthread *list);
iterator_cdsl_queue_list_lock_based_pthread iter_end_cdsl_queue_list_lock_based_pthread(cdsl_queue_list_lock_based_pthread *list);
iterator_cdsl_queue_list_lock_based_pthread iter_next_cdsl_queue_list_lock_based_pthread(iterator_cdsl_queue_list_lock_based_pthread i);
void* iter_deref_cdsl_queue_list_lock_based_pthread(cdsl_queue_list_lock_based_pthread *list, iterator_cdsl_queue_list_lock_based_pthread i);
cdsl_queue_list_lock_based_pthread* cdsl_sll_init(void);


typedef iterator_cdsl_queue_list_lock_based_pthread iterator_cdsl_sll;

/* pthread spinlock */
typedef struct cdsl_queue_list_lock_based_pthread_spinlock_struct {
	cdsl_queue_node *head;
	cdsl_queue_node *tail;

	void  (*enqueue)(unsigned int, struct cdsl_queue_list_lock_based_pthread_spinlock_struct*, void*);
	void* (*dequeue)(unsigned int, struct cdsl_queue_list_lock_based_pthread_spinlock_struct*);

	cdsl_pthread_spinlock h_lock;
	cdsl_pthread_spinlock t_lock;

#ifdef FREELISTS_ENABLED
	cdsl_freelist *freelist[NR_CORES];
#endif
} cdsl_queue_list_lock_based_pthread_spinlock;

cdsl_queue_list_lock_based_pthread_spinlock* cdsl_queue_list_lock_based_pthread_spinlock_init(void);
void cdsl_queue_list_lock_based_pthread_spinlock_enqueue(unsigned int cpu_id, cdsl_queue_list_lock_based_pthread_spinlock *q, void *data);
void* cdsl_queue_list_lock_based_pthread_spinlock_dequeue(unsigned int cpu_id, cdsl_queue_list_lock_based_pthread_spinlock *q);

/* tas lock */
typedef struct cdsl_queue_list_lock_based_tas_struct {
	cdsl_queue_node *head;
	cdsl_queue_node *tail;

	void  (*enqueue)(unsigned int, struct cdsl_queue_list_lock_based_tas_struct*, void*);
	void* (*dequeue)(unsigned int, struct cdsl_queue_list_lock_based_tas_struct*);

	cdsl_tas_lock h_lock;
	cdsl_tas_lock t_lock;

#ifdef FREELISTS_ENABLED
	cdsl_freelist *freelist[NR_CORES];
#endif
} cdsl_queue_list_lock_based_tas;

cdsl_queue_list_lock_based_tas* cdsl_queue_list_lock_based_tas_init(void);
void cdsl_queue_list_lock_based_tas_enqueue(unsigned int cpu_id, cdsl_queue_list_lock_based_tas *q, void *data);
void* cdsl_queue_list_lock_based_tas_dequeue(unsigned int cpu_id, cdsl_queue_list_lock_based_tas *q);

/* ttas lock */
typedef struct cdsl_queue_list_lock_based_ttas_struct {
	cdsl_queue_node *head;
	cdsl_queue_node *tail;

	void  (*enqueue)(unsigned int, struct cdsl_queue_list_lock_based_ttas_struct*, void*);
	void* (*dequeue)(unsigned int, struct cdsl_queue_list_lock_based_ttas_struct*);

	cdsl_ttas_lock h_lock;
	cdsl_ttas_lock t_lock;

#ifdef FREELISTS_ENABLED
	cdsl_freelist *freelist[NR_CORES];
#endif
} cdsl_queue_list_lock_based_ttas;

cdsl_queue_list_lock_based_ttas* cdsl_queue_list_lock_based_ttas_init(void);
void cdsl_queue_list_lock_based_ttas_enqueue(unsigned int cpu_id, cdsl_queue_list_lock_based_ttas *q, void *data);
void* cdsl_queue_list_lock_based_ttas_dequeue(unsigned int cpu_id, cdsl_queue_list_lock_based_ttas *q);

/* ttas lock with linear backoff */
typedef struct cdsl_queue_list_lock_based_ttas_backoff_linear_struct {
	cdsl_queue_node *head;
	cdsl_queue_node *tail;

	void  (*enqueue)(unsigned int, struct cdsl_queue_list_lock_based_ttas_backoff_linear_struct*, void*);
	void* (*dequeue)(unsigned int, struct cdsl_queue_list_lock_based_ttas_backoff_linear_struct*);

	cdsl_ttas_backoff_linear_lock h_lock;
	cdsl_ttas_backoff_linear_lock t_lock;

#ifdef FREELISTS_ENABLED
	cdsl_freelist *freelist[NR_CORES];
#endif
} cdsl_queue_list_lock_based_ttas_backoff_linear;

cdsl_queue_list_lock_based_ttas_backoff_linear* cdsl_queue_list_lock_based_ttas_backoff_linear_init(unsigned int sleep_time);
void cdsl_queue_list_lock_based_ttas_backoff_linear_enqueue(unsigned int cpu_id, cdsl_queue_list_lock_based_ttas_backoff_linear *q, void *data);
void* cdsl_queue_list_lock_based_ttas_backoff_linear_dequeue(unsigned int cpu_id, cdsl_queue_list_lock_based_ttas_backoff_linear *q);

/* ttas lock with exp backoff */
typedef struct cdsl_queue_list_lock_based_ttas_backoff_exp_struct {
	cdsl_queue_node *head;
	cdsl_queue_node *tail;

	void  (*enqueue)(unsigned int, struct cdsl_queue_list_lock_based_ttas_backoff_exp_struct*, void*);
	void* (*dequeue)(unsigned int, struct cdsl_queue_list_lock_based_ttas_backoff_exp_struct*);

	cdsl_ttas_backoff_exp_lock h_lock;
	cdsl_ttas_backoff_exp_lock t_lock;

#ifdef FREELISTS_ENABLED
	cdsl_freelist *freelist[NR_CORES];
#endif
} cdsl_queue_list_lock_based_ttas_backoff_exp;

cdsl_queue_list_lock_based_ttas_backoff_exp* cdsl_queue_list_lock_based_ttas_backoff_exp_init(void);
void cdsl_queue_list_lock_based_ttas_backoff_exp_enqueue(unsigned int cpu_id, cdsl_queue_list_lock_based_ttas_backoff_exp *q, void *data);
void* cdsl_queue_list_lock_based_ttas_backoff_exp_dequeue(unsigned int cpu_id, cdsl_queue_list_lock_based_ttas_backoff_exp *q);
/******************************************************************************************/

/**************************************** List - RCL **************************************/

typedef struct cdsl_queue_list_rcl_struct {
	cdsl_queue_node *head;
	cdsl_queue_node *tail;

	void  (*enqueue)(struct cdsl_queue_list_rcl_struct*, void *);
	volatile void* (*dequeue)(struct cdsl_queue_list_rcl_struct*);
} cdsl_queue_list_rcl;

cdsl_queue_list_rcl* cdsl_queue_list_rcl_init(void);
void* cdsl_queue_list_rcl_server_start(void* queue);
void cdsl_queue_list_rcl_server_enqueue(cdsl_queue_list_rcl* q, void* data);
void* cdsl_queue_list_rcl_server_dequeue(cdsl_queue_list_rcl* q);
void cdsl_queue_list_rcl_client_enqueue(cdsl_queue_list_rcl* q, void* data);
volatile void* cdsl_queue_list_rcl_client_dequeue(cdsl_queue_list_rcl* q);
void cdsl_queue_list_rcl_client_exit(void);

/******************************************************************************************/

/**********************************  List - Lockless **************************************/
typedef struct cdsl_queue_list_lockless_struct {
	cdsl_queue_node *head;
	cdsl_queue_node *tail;

	void  (*enqueue)(unsigned int, struct cdsl_queue_list_lockless_struct*, void *);
	void* (*dequeue)(unsigned int, struct cdsl_queue_list_lockless_struct*);

#ifdef FREELISTS_ENABLED
	cdsl_freelist *freelist[NR_CORES];
#endif
} cdsl_queue_list_lockless;

cdsl_queue_list_lockless* cdsl_queue_list_lockless_init(void);
void cdsl_queue_list_lockless_enqueue(unsigned int cpu_id, cdsl_queue_list_lockless* q, void* data);
void* cdsl_queue_list_lockless_dequeue(unsigned int cpu_id, cdsl_queue_list_lockless* q);

/*****************************************************************************************
*************************************	RING BUFFER   ************************************
*****************************************************************************************/
// Buffer size:
#define CDSL_QUEUE_RING_BUFFER_SIZE	3200000
#define Q_MASK = (CDSL_QUEUE_RING_BUFFER_SIZE - 1)

// Auxiliary macros:
#define _MIN(a,b) (((a)<(b))?(a):(b))
#define _MAX(a,b) (((a)>(b))?(a):(b))

/****************************** Ring Buffer - Lock-based **********************************/
/* pthread mutex */
typedef struct cdsl_queue_ring_buffer_lock_based_pthread_struct {
	unsigned int head, tail;
	unsigned int empty, full;
	void **ptr_array;

	void  (*enqueue)(unsigned int, struct cdsl_queue_ring_buffer_lock_based_pthread_struct*, void *);
	void* (*dequeue)(unsigned int, struct cdsl_queue_ring_buffer_lock_based_pthread_struct*);

	cdsl_pthread_lock h_lock;
	cdsl_pthread_lock t_lock;
} cdsl_queue_ring_buffer_lock_based_pthread;

cdsl_queue_ring_buffer_lock_based_pthread* cdsl_queue_ring_buffer_lock_based_pthread_init(void);
void cdsl_queue_ring_buffer_lock_based_pthread_enqueue(unsigned int cpu_id, cdsl_queue_ring_buffer_lock_based_pthread* q, void* data);
void* cdsl_queue_ring_buffer_lock_based_pthread_dequeue(unsigned int cpu_id, cdsl_queue_ring_buffer_lock_based_pthread* q);

/* pthread spinlock */
typedef struct cdsl_queue_ring_buffer_lock_based_pthread_spinlock_struct {
	unsigned int head, tail;
	unsigned int empty, full;
	void **ptr_array;

	void  (*enqueue)(unsigned int, struct cdsl_queue_ring_buffer_lock_based_pthread_spinlock_struct*, void *);
	void* (*dequeue)(unsigned int, struct cdsl_queue_ring_buffer_lock_based_pthread_spinlock_struct*);

	cdsl_pthread_spinlock h_lock;
	cdsl_pthread_spinlock t_lock;
} cdsl_queue_ring_buffer_lock_based_pthread_spinlock;

cdsl_queue_ring_buffer_lock_based_pthread_spinlock* cdsl_queue_ring_buffer_lock_based_pthread_spinlock_init(void);
void cdsl_queue_ring_buffer_lock_based_pthread_spinlock_enqueue(unsigned int cpu_id, cdsl_queue_ring_buffer_lock_based_pthread_spinlock* q, void* data);
void* cdsl_queue_ring_buffer_lock_based_pthread_spinlock_dequeue(unsigned int cpu_id, cdsl_queue_ring_buffer_lock_based_pthread_spinlock* q);

/* tas lock */
typedef struct cdsl_queue_ring_buffer_lock_based_tas_struct {
	unsigned int head, tail;
	unsigned int empty, full;
	void **ptr_array;

	void  (*enqueue)(unsigned int, struct cdsl_queue_ring_buffer_lock_based_tas_struct*, void *);
	void* (*dequeue)(unsigned int, struct cdsl_queue_ring_buffer_lock_based_tas_struct*);

	cdsl_tas_lock h_lock;
	cdsl_tas_lock t_lock;
} cdsl_queue_ring_buffer_lock_based_tas;

cdsl_queue_ring_buffer_lock_based_tas* cdsl_queue_ring_buffer_lock_based_tas_init(void);
void cdsl_queue_ring_buffer_lock_based_tas_enqueue(unsigned int cpu_id, cdsl_queue_ring_buffer_lock_based_tas* q, void* data);
void* cdsl_queue_ring_buffer_lock_based_tas_dequeue(unsigned int cpu_id, cdsl_queue_ring_buffer_lock_based_tas* q);

/* ttas lock */
typedef struct cdsl_queue_ring_buffer_lock_based_ttas_struct {
	unsigned int head, tail;
	unsigned int empty, full;
	void **ptr_array;

	void  (*enqueue)(unsigned int, struct cdsl_queue_ring_buffer_lock_based_ttas_struct*, void *);
	void* (*dequeue)(unsigned int, struct cdsl_queue_ring_buffer_lock_based_ttas_struct*);

	cdsl_ttas_lock h_lock;
	cdsl_ttas_lock t_lock;
} cdsl_queue_ring_buffer_lock_based_ttas;

cdsl_queue_ring_buffer_lock_based_ttas* cdsl_queue_ring_buffer_lock_based_ttas_init(void);
void cdsl_queue_ring_buffer_lock_based_ttas_enqueue(unsigned int cpu_id, cdsl_queue_ring_buffer_lock_based_ttas* q, void* data);
void* cdsl_queue_ring_buffer_lock_based_ttas_dequeue(unsigned int cpu_id, cdsl_queue_ring_buffer_lock_based_ttas* q);

/* ttas backoff linear */
typedef struct cdsl_queue_ring_buffer_lock_based_ttas_backoff_linear_struct {
	unsigned int head, tail;
	unsigned int empty, full;
	void **ptr_array;

	void  (*enqueue)(unsigned int, struct cdsl_queue_ring_buffer_lock_based_ttas_backoff_linear_struct*, void *);
	void* (*dequeue)(unsigned int, struct cdsl_queue_ring_buffer_lock_based_ttas_backoff_linear_struct*);

	cdsl_ttas_backoff_linear_lock h_lock;
	cdsl_ttas_backoff_linear_lock t_lock;
} cdsl_queue_ring_buffer_lock_based_ttas_backoff_linear;

cdsl_queue_ring_buffer_lock_based_ttas_backoff_linear* cdsl_queue_ring_buffer_lock_based_ttas_backoff_linear_init(unsigned int sleep_time);
void cdsl_queue_ring_buffer_lock_based_ttas_backoff_linear_enqueue(unsigned int cpu_id, cdsl_queue_ring_buffer_lock_based_ttas_backoff_linear* q, void* data);
void* cdsl_queue_ring_buffer_lock_based_ttas_backoff_linear_dequeue(unsigned int cpu_id, cdsl_queue_ring_buffer_lock_based_ttas_backoff_linear* q);

/* ttas backoff exp */
typedef struct cdsl_queue_ring_buffer_lock_based_ttas_backoff_exp_struct {
	unsigned int head, tail;
	unsigned int empty, full;
	void **ptr_array;

	void  (*enqueue)(unsigned int, struct cdsl_queue_ring_buffer_lock_based_ttas_backoff_exp_struct*, void *);
	void* (*dequeue)(unsigned int, struct cdsl_queue_ring_buffer_lock_based_ttas_backoff_exp_struct*);

	cdsl_ttas_backoff_exp_lock h_lock;
	cdsl_ttas_backoff_exp_lock t_lock;
} cdsl_queue_ring_buffer_lock_based_ttas_backoff_exp;

cdsl_queue_ring_buffer_lock_based_ttas_backoff_exp* cdsl_queue_ring_buffer_lock_based_ttas_backoff_exp_init(void);
void cdsl_queue_ring_buffer_lock_based_ttas_backoff_exp_enqueue(unsigned int cpu_id, cdsl_queue_ring_buffer_lock_based_ttas_backoff_exp* q, void* data);
void* cdsl_queue_ring_buffer_lock_based_ttas_backoff_exp_dequeue(unsigned int cpu_id, cdsl_queue_ring_buffer_lock_based_ttas_backoff_exp* q);
/******************************************************************************************/

/************************************ Ring Buffer - RCL ***********************************/
/*
volatile void* cdsl_queue_ring_buffer_rcl_buffer[CDSL_QUEUE_RCL_NR_CLIENTS];
pthread_mutex_t cdsl_queue_ring_buffer_rcl_mtx[CDSL_QUEUE_RCL_NR_CLIENTS];
pthread_cond_t cdsl_queue_ring_buffer_rcl_cond_var[CDSL_QUEUE_RCL_NR_CLIENTS];

typedef struct cdsl_queue_ring_buffer_rcl_struct {
	unsigned int head, tail;
	unsigned int empty, full;
	void **ptr_array;

	void  (*enqueue)(struct cdsl_queue_ring_buffer_rcl_struct*, void *);
	volatile void* (*dequeue)(struct cdsl_queue_ring_buffer_rcl_struct*);

} cdsl_queue_ring_buffer_rcl;

cdsl_queue_ring_buffer_rcl* cdsl_queue_ring_buffer_rcl_init(cdsl_queue_ring_buffer_rcl* q);
void* cdsl_queue_ring_buffer_rcl_server_start(cdsl_queue_ring_buffer_rcl* q);
void cdsl_queue_ring_buffer_rcl_server_enqueue(cdsl_queue_ring_buffer_rcl* q, void* data);
void* cdsl_queue_ring_buffer_rcl_server_dequeue(cdsl_queue_ring_buffer_rcl* q);
void cdsl_queue_ring_buffer_rcl_client_enqueue(cdsl_queue_ring_buffer_rcl* q, void* data);
volatile void* cdsl_queue_ring_buffer_rcl_client_dequeue(cdsl_queue_ring_buffer_rcl* q);
void cdsl_queue_ring_buffer_rcl_client_exit(void);
*/
/******************************************************************************************/

/****************************** Ring Buffer - Lockless **********************************/
#define CDSL_QUEUE_RING_BUFFER_RCL_NR_CORES 4

typedef struct cdsl_queue_ring_buffer_lockless_thread_position_struct {
		unsigned long head, tail;
} cdsl_queue_ring_buffer_lockless_thread_position;

typedef struct cdsl_queue_ring_buffer_lockless_struct {
	unsigned long nr_producers, nr_consumers;
	unsigned long head; 		// currently free position (next to insert)
	unsigned long tail; 		// current tail, next to pop
	unsigned long last_head; 	// last not-processed producer's pointer
	unsigned long last_tail; 	// last not-processed consumer's pointer
	cdsl_queue_ring_buffer_lockless_thread_position *thr_p;
    void **ptr_array;

	void  (*enqueue)(unsigned int, struct cdsl_queue_ring_buffer_lockless_struct*, void *);
	void* (*dequeue)(unsigned int, struct cdsl_queue_ring_buffer_lockless_struct*);

} cdsl_queue_ring_buffer_lockless;

cdsl_queue_ring_buffer_lockless_thread_position* cdsl_queue_ring_buffer_lockless_thr_pos(cdsl_queue_ring_buffer_lockless* q);
cdsl_queue_ring_buffer_lockless* cdsl_queue_ring_buffer_lockless_init(unsigned long nr_producers, unsigned long nr_consumers);
void cdsl_queue_ring_buffer_lockless_enqueue(unsigned int cpu_id, cdsl_queue_ring_buffer_lockless* q, void* data);
void* cdsl_queue_ring_buffer_lockless_dequeue(unsigned int cpu_id, cdsl_queue_ring_buffer_lockless* q);
/******************************************************************************************/

#endif
