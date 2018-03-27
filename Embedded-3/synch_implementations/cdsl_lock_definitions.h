#ifndef __CDSL_LOCK_DEFINITIONS_H__
#define __CDSL_LOCK_DEFINITIONS_H__ 

// common includes:
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <linux/unistd.h>
#include <stdio.h>

// Lock_val indicates that a thread has explicit control over a specific variable.
// Unlock_val indicates that no thread has control over it
#define CDSL_LOCK_VAL 		1
#define CDSL_UNLOCK_VAL	 	0

/******** Lock structure definition and member declarations *******/
 /* pthread mutex */
 typedef struct cdsl_pthread_lock_struct {
	 pthread_mutex_t lock;
	 void (*request)(struct cdsl_pthread_lock_struct*);
	 void (*release)(struct cdsl_pthread_lock_struct*);	 
 } cdsl_pthread_lock;
 
void cdsl_pthread_lock_request(cdsl_pthread_lock* pthread_lock);
void cdsl_pthread_lock_release(cdsl_pthread_lock*  pthread_lock) ;
void cdsl_pthread_lock_init(cdsl_pthread_lock* pthread_lock);

/* pthread spinlock */
typedef struct cdsl_pthread_spinlock_struct {
	pthread_spinlock_t lock;
	 void (*request)(struct cdsl_pthread_spinlock_struct*);
	 void (*release)(struct cdsl_pthread_spinlock_struct*);
} cdsl_pthread_spinlock;

void cdsl_pthread_lock_request(cdsl_pthread_lock* pthread_lock);
void cdsl_pthread_lock_release(cdsl_pthread_lock*  pthread_lock) ;
void cdsl_pthread_lock_init(cdsl_pthread_lock* pthread_lock);

/* test-and-set lock */
typedef struct cdsl_tas_lock_struct {
	volatile int lock;
	void (*request)(struct cdsl_tas_lock_struct*);
	void (*release)(struct cdsl_tas_lock_struct*);
} cdsl_tas_lock;

void cdsl_tas_lock_request(cdsl_tas_lock* tas_lock);
void cdsl_tas_lock_release(cdsl_tas_lock*  tas_lock) ;
void cdsl_tas_lock_init(cdsl_tas_lock* tas_lock);

/* test-test-and-set lock */
typedef struct cdsl_ttas_lock_struct {
	volatile int lock;
	void (*request)(struct cdsl_ttas_lock_struct*);
	void (*release)(struct cdsl_ttas_lock_struct*);	
} cdsl_ttas_lock;

void cdsl_ttas_lock_request(cdsl_ttas_lock* ttas_lock);
void cdsl_ttas_lock_release(cdsl_ttas_lock*  ttas_lock) ;
void cdsl_ttas_lock_init(cdsl_ttas_lock* ttas_lock);

/* test-test-and-set lock with linear backoff */
typedef struct cdsl_ttas_backoff_linear_lock_struct {
	volatile int lock;
	struct timespec tim, tim2;
	void (*request)(struct cdsl_ttas_backoff_linear_lock_struct*);
	void (*release)(struct cdsl_ttas_backoff_linear_lock_struct*);
} cdsl_ttas_backoff_linear_lock;

void cdsl_ttas_backoff_linear_lock_request(cdsl_ttas_backoff_linear_lock* ttas_backoff_linear_lock);
void cdsl_ttas_backoff_linear_lock_release(cdsl_ttas_backoff_linear_lock*  ttas_backoff_linear_lock) ;
void cdsl_ttas_backoff_linear_lock_init(cdsl_ttas_backoff_linear_lock* ttas_backoff_linear_lock, unsigned int sleep_time);

/* test-test-and-set lock with exp backoff */
typedef struct cdsl_ttas_backoff_exp_lock_struct {
	volatile int lock;
	struct timespec tim, tim2;
	void (*request)(struct cdsl_ttas_backoff_exp_lock_struct*);
	void (*release)(struct cdsl_ttas_backoff_exp_lock_struct*);
} cdsl_ttas_backoff_exp_lock;

void cdsl_ttas_backoff_exp_lock_request(cdsl_ttas_backoff_exp_lock* ttas_backoff_exp_lock);
void cdsl_ttas_backoff_exp_lock_release(cdsl_ttas_backoff_exp_lock*  ttas_backoff_exp_lock) ;
void cdsl_ttas_backoff_exp_lock_init(cdsl_ttas_backoff_exp_lock* ttas_backoff_exp_lock);

/******** Read-Writer Lock structure definition and member declarations *******/
#define NR_READERS	4
#define NR_WRITERS	4

/* pthread rw lock */
 typedef struct cdsl_rw_pthread_lock_struct {
	 pthread_rwlock_t lock;
	 void (*rd_request)(struct cdsl_rw_pthread_lock_struct*);
	 void (*wr_request)(struct cdsl_rw_pthread_lock_struct*);
	 void (*release)(struct cdsl_rw_pthread_lock_struct*);
 } cdsl_rw_pthread_lock;

void cdsl_rw_pthread_lock_rd_request(cdsl_rw_pthread_lock* pthread_rw_lock);
void cdsl_rw_pthread_lock_wr_request(cdsl_rw_pthread_lock* pthread_rw_lock);
void cdsl_rw_pthread_lock_release(cdsl_rw_pthread_lock*  pthread_rw_lock);
void cdsl_rw_pthread_lock_init(cdsl_rw_pthread_lock* pthread_rw_lock);

/******** Recursive (Reentrant) pthread Lock structure definition and member declarations *******/
typedef struct cdsl_pthread_rc_lock_struct {
	 pthread_mutex_t lock;
	 pthread_mutexattr_t attr;
	 void (*request)(struct cdsl_pthread_rc_lock_struct*);
	 void (*release)(struct cdsl_pthread_rc_lock_struct*);
} cdsl_pthread_rc_lock;

void cdsl_pthread_rc_lock_request(cdsl_pthread_rc_lock* pthread_lock);
void cdsl_pthread_rc_lock_release(cdsl_pthread_rc_lock*  pthread_lock) ;
void cdsl_pthread_rc_lock_init(cdsl_pthread_rc_lock* pthread_lock);

#endif

