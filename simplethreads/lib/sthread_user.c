/* Simplethreads Instructional Thread Package
 *
 * sthread_user.c - Implements the sthread API using user-level threads.
 *
 *    You need to implement the routines in this file.
 *
 * Change Log:
 * 2002-04-15        rick
 *   - Initial version.
 */

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <sthread.h>
#include <sthread_queue.h>
#include <sthread_user.h>
#include <sthread_ctx.h>
#include <sthread_user.h>

struct _sthread {
  sthread_ctx_t *saved_ctx;
  sthread_start_func_t start_routine;
  void *start_arg;
  
  // TODO: We probably need to have a separate void* here
  int joinable;
};



/*********************************************************************/
/* Part 1: Creating and Scheduling Threads                           */
/*********************************************************************/

void sthread_user_init(void) {
}

sthread_t sthread_user_create(sthread_start_func_t start_routine, void *arg,
                              int joinable) {
  // Allocate memory for the new thread
  struct _sthread *thread = (struct _sthread *)
                            malloc(sizeof(struct _sthread));

  // Did the malloc succeed?
  if (thread == NULL) {
    fprintf(stderr, "Out of memory (sthread_user_create)\n");
    return NULL;
  }
  
  // Initialize the context
  thread->saved_ctx = sthread_new_ctx(sthread_user_init);
  
  // Did the initialization succeed?
  if (thread->saved_ctx == NULL) {
    // sthread_new_ctx prints the problem to stderr
    free(thread);
    return NULL;
  }
  
  // Save the passed-in state
  thread->start_routine = start_routine;
  thread->start_arg = arg;
  
  // TODO: Not sure if this is correct
  thread->joinable = joinable;
  
  return thread;
}

void sthread_user_exit(void *ret) {
}

void* sthread_user_join(sthread_t t) {
  return NULL;
}

void sthread_user_yield(void) {
}

/* Add any new part 1 functions here */


/*********************************************************************/
/* Part 2: Synchronization Primitives                                */
/*********************************************************************/

struct _sthread_mutex {
  /* Fill in mutex data structure */
};

sthread_mutex_t sthread_user_mutex_init() {
  return NULL;
}

void sthread_user_mutex_free(sthread_mutex_t lock) {
}

void sthread_user_mutex_lock(sthread_mutex_t lock) {
}

void sthread_user_mutex_unlock(sthread_mutex_t lock) {
}


struct _sthread_cond {
  /* Fill in condition variable structure */
};

sthread_cond_t sthread_user_cond_init(void) {
  return NULL;
}

void sthread_user_cond_free(sthread_cond_t cond) {
}

void sthread_user_cond_signal(sthread_cond_t cond) {
}

void sthread_user_cond_broadcast(sthread_cond_t cond) {
}

void sthread_user_cond_wait(sthread_cond_t cond,
                            sthread_mutex_t lock) {
}
