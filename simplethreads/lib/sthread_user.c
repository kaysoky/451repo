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
  // Stack state
  sthread_ctx_t *saved_ctx;
  
  // Function that the thread should run
  sthread_start_func_t start_routine;
  void *start_arg;
  
  // Is 'ret_val' returnable?
  int joinable;
  
  // Undefined if 'joinable' is false
  void *ret_val;
  
  // Has the thread finished running?
  int done;
};

/*********************************************************************/
/* Part 1: Creating and Scheduling Threads                           */
/*********************************************************************/

/*
 * This holds the currently running thread
 * Initialized with a blank context
 */
static struct _sthread *current_thread;

/*
 * This holds all the threads that are ready to be run
 */
static sthread_queue_t ready_queue;

/*
 * Helper for sthread_user_create() and sthread_user_init()
 * Allocates and initializes a blank _sthread object
 */
static struct _sthread *sthread_user_create_empty() {
  // Allocate memory for the new thread
  struct _sthread *thread = (struct _sthread *)
                            malloc(sizeof(struct _sthread));

  // Did the malloc succeed?
  if (current_thread == NULL) {
    fprintf(stderr, "Out of memory (sthread_user_create_empty)\n");
    return NULL;
  }
  
  // Except for the thread that initializes this user thread module, 
  // all threads initialize the context on creation.
  // A NULL context denotes that we are switching from the initial thread
  //   to one of the new user threads.
  thread->saved_ctx = NULL;
  
  // Except for the thread that initializes this user thread module, 
  // all threads have a defined start routine. 
  // This forces a SEGFAULT if the start routine 
  //   for the initializing thread is somehow called.
  thread->start_routine = NULL;
  
  // By default, a thread is not joinable
  thread->joinable = 0;
  
  // By default, a thread has work to do
  thread->done = 0;
  
  return thread;
}

void sthread_user_init(void) {
  current_thread = sthread_user_create_empty();
  ready_queue = sthread_new_queue();
}

/*
 * This is what each thread actually executes
 */
static void ctx_start_function(void) {
    current_thread->ret_val = 
            current_thread->start_routine(current_thread->start_arg);
    
    // Nothing left to do
    current_thread->done = 1;
}

sthread_t sthread_user_create(sthread_start_func_t start_routine, void *arg,
                              int joinable) {
  struct _sthread *thread = sthread_user_create_empty();
  
  // Initialize the context
  thread->saved_ctx = sthread_new_ctx(ctx_start_function);
  
  // Did the initialization succeed?
  if (thread->saved_ctx == NULL) {
    // sthread_new_ctx prints the problem to stderr
    free(thread);
    return NULL;
  }
  
  // Save the passed-in state
  thread->start_routine = start_routine;
  thread->start_arg = arg;
  thread->joinable = joinable;
  
  // Queue the thread and return it
  sthread_enqueue(ready_queue, thread);
  return thread;
}

void sthread_user_exit(void *ret) {
}

void* sthread_user_join(sthread_t t) {
  return NULL;
}

void sthread_user_yield(void) {
  if (sthread_queue_is_empty(ready_queue)) {
    // Nothing to yield to
    return;
  }
  
  if (current_thread->saved_ctx == NULL) {
    current_thread->saved_ctx = sthread_new_blank_ctx();
    
    //TODO: switch?
  }
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
