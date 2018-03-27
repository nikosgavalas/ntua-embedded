#ifndef __CDSL_HAZARD_H__
#define __CDSL_HAZARD_H__

#include <stdlib.h>
#include <glib.h>

// The maximum number of hazard pointers needed by a thread:
#ifndef CDSL_HAZARD_K
#define CDSL_HAZARD_K (2)
#endif

//Hazard pointer list size at which reclaimation should occur:
#ifndef CDSL_HAZARD_R
#define CDSL_HAZARD_R (8)
#endif

// Get hazard pointers list:
#define CDSL_HAZARD_INIT cdsl_hazard *cdsl_thread_local_hazard = (g_private_get(&cdsl_thread_local_hazard_key))
#define CDSL_HAZARD_TLS (cdsl_thread_local_hazard)							

#define CDSL_HAZARD_SET(i,p) G_STMT_START {							\
    if (!cdsl_thread_local_hazard) {                                                 		\
        cdsl_hazard_thread_acquire();                                				\
        cdsl_thread_local_hazard = g_private_get(&cdsl_thread_local_hazard_key);	     	\
        g_assert(cdsl_thread_local_hazard);                                          		\
    }                                                                				\
    cdsl_thread_local_hazard->hp[(i)] = (p);                                         		\
} G_STMT_END

#define CDSL_HAZARD_UNSET(p) G_STMT_START {                          				\
    cdsl_hazard *_head;                                              				\
    cdsl_thread_local_hazard->rlist = g_slist_prepend(cdsl_thread_local_hazard->rlist, (p));    \
    cdsl_thread_local_hazard->rcount++;                                              		\
    _head = _cdsl_hazards;                                           				\
    if (cdsl_thread_local_hazard->rcount >= (_CDSL_H + CDSL_HAZARD_R)) {             		\
        cdsl_hazard_scan(_head);                                     				\
        cdsl_hazard_help_scan();                                     				\
    }                                                                				\
} G_STMT_END

#define G_SLIST_POP(l,d) G_STMT_START {                              \
    if (!(l)) {                                                      \
        *(d) = NULL;                                                 \
    } else {                                                         \
        *(d) = (l)->data;                                            \
        (l) = g_slist_delete_link((l), (l));                         \
    }                                                                \
} G_STMT_END


typedef struct cdsl_hazard_struct {
	void*  hp[CDSL_HAZARD_K];
	struct cdsl_hazard_struct *next;
	int  active;
	GSList   *rlist;
	gint      rcount;
	GTree    *plist;
} cdsl_hazard;


// Thread local hazard pointers.
static GPrivate cdsl_thread_local_hazard_key = G_STATIC_PRIVATE_INIT;

// Global linked-list of all hazard pointers:
static cdsl_hazard *_cdsl_hazards = NULL;

/*
 * Total count of all potential hazard pointers.
 */
static gint _CDSL_H = 0;

/*
 * Hazard pointer callback for reclaimation of memory.  This should
 * be set by the data structure consuming cdsl_hazard.h.
 */
static void (*cdsl_hazard_free) (gpointer data) = NULL;

/*
 * Sorting method for GTree.
 */
static gint cdsl_hazard_pointer_compare(gconstpointer a, gconstpointer b) {
	return (a == b) ? 0 : a - b;
}

/*
 * Method to acquire thread local data structures for hazard pointer
 * operation.  This is called automatically as needed when a new thread
 * enters the arena.
 *
 * Your data structure should provide a method for threads leaving the arena
 * to lose their allocated resources which should in turn call
 * cdsl_hazard_thread_release().
 */
static void cdsl_hazard_thread_acquire(void) {
	cdsl_hazard *hazard, *old_head;
	gint old_count;

	/*
	 * Try to reclaim an existing, unused cdsl_hazard structure.
	 */
	for (hazard = _cdsl_hazards; hazard; hazard = hazard->next) {
		if (hazard->active)
			continue;
		if (!g_atomic_int_compare_and_exchange(&hazard->active, FALSE, TRUE))
			continue;
		g_private_set(&cdsl_thread_local_hazard_key, hazard);
		return;
	}

	/*
	 * No cdsl_hazard could be reused.  We will create one and push it onto
	 * the head of the linked-list.
	 */
	old_count = g_atomic_int_exchange_and_add(&_CDSL_H, CDSL_HAZARD_K);
	hazard = g_slice_new0(cdsl_hazard);
	hazard->active = TRUE;
	hazard->plist = g_tree_new(cdsl_hazard_pointer_compare);
	do {
		old_head = _cdsl_hazards;
		hazard->next = old_head;
	} while (!g_atomic_pointer_compare_and_exchange((gpointer *)&_cdsl_hazards,
	                                                old_head, hazard));
	g_private_set(&cdsl_thread_local_hazard_key, hazard);
	return;
}

/*
 * This method works in two stages.  The first stage scans all neighbor threads
 * for hazard pointers and stores them in a worst-case O(log N) balanced-binary
 * GTree.  The second stage looks to see if any hazard pointers in the threads
 * local hazard pointers are found in the GTree.  If they are not, they are
 * ready to be reclaimed.  If they are found, we store them back into our
 * list of hazard pointers for the next round of reclaimation.
 */
static void cdsl_hazard_scan(cdsl_hazard *head) {
	cdsl_hazard *hazard;
	GSList *tmplist;
	gpointer data = NULL;
	gint i;
	
	CDSL_HAZARD_INIT;

	/*
	 * Stage 1: Collect all the current hazard pointers from active threads.
	 */
	hazard = head;
	while (hazard != NULL) {
		for (i = 0; i < (CDSL_HAZARD_K - 1); i++) {
			data = g_atomic_pointer_get(&hazard->hp[i]);
			if (data != NULL)
				g_tree_insert(CDSL_HAZARD_TLS->plist, data, data);
		}
		hazard = hazard->next;
	}

	/*
	 * Stage 2: Reclaim expired hazard pointers.
	 */
	CDSL_HAZARD_TLS->rcount = 0;
	tmplist = CDSL_HAZARD_TLS->rlist;
	CDSL_HAZARD_TLS->rlist = NULL;
	G_SLIST_POP(tmplist, &data);
	while (data != NULL) {
		if (g_tree_lookup(CDSL_HAZARD_TLS->plist, data)) {
			CDSL_HAZARD_TLS->rlist = g_slist_prepend(CDSL_HAZARD_TLS->rlist, data);
			CDSL_HAZARD_TLS->rcount++;
		} else {
			//printf("f-:%d\n", (unsigned int)data);
			free(data);
		}
		G_SLIST_POP(tmplist, &data);
	}

	/*
	 * This is part of a micro-optimization for GLib 2.22+.  GTree became
	 * reference counted and allows us to reuse the structure and quickly
	 * remove all of the existing pointers from the tree.  However, it also
	 * decrements the reference count in the process.  Therefore, we increment
	 * it before-hand.
	 */
	CDSL_HAZARD_TLS->plist = g_tree_ref(CDSL_HAZARD_TLS->plist);
	g_tree_destroy(CDSL_HAZARD_TLS->plist);
}

static void cdsl_hazard_help_scan(void) {
	cdsl_hazard *hazard, *head;
	gpointer data;

	CDSL_HAZARD_INIT;

	for (hazard = _cdsl_hazards; hazard; hazard = hazard->next) {
		if (hazard->active)
			continue;
		if (!g_atomic_int_compare_and_exchange(&hazard->active, FALSE, TRUE))
			continue;
		while (hazard->rcount > 0) {
			G_SLIST_POP(hazard->rlist, &data);
			hazard->rcount--;
			CDSL_HAZARD_TLS->rlist = g_slist_prepend(CDSL_HAZARD_TLS->rlist, data);
			CDSL_HAZARD_TLS->rcount++;
			head = _cdsl_hazards;
			if (CDSL_HAZARD_TLS->rcount >= (_CDSL_H + CDSL_HAZARD_R))
				cdsl_hazard_scan(head);
		}
		hazard->active = FALSE;
	}
}

#endif
