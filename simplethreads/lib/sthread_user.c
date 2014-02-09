/* Simplethreads Instructional Thread Package
 *
 * sthread_user.c - Implements the sthread API using user-level threads.
 *
 *    You need to implement the routines in this file.
 *
 * Change Log:
 * 2002-04-15        rick
 *   - Initial version.
 * 2014-02-08        Joseph
 *   - Completing implementation.
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
 * This is used to detach threads that have completed
 * since a finished thread cannot deallocate it's own context
 */
static sthread_ctx_t *detached_context;

/*
 * Helper for sthread_user_create() and sthread_user_init()
 * Allocates and initializes a blank _sthread object
 */
static struct _sthread *sthread_user_create_empty();

/*
 * Helper for ctx_start_function(),
 *            sthread_user_exit(),
 *            and sthread_user_join()
 * Detaches the given thread and deallocates any memory not in use
 * This can be used to detach the current context
 *   (Current context is saved in 'detached_context')
 */
static void sthread_detach(struct _sthread *thread);

/*
 * Helper for ctx_start_function() and sthread_user_exit()
 * Switches from the given context to another ready thread
 * If no such thread is available, this instead calls exit()
 */
static void sthread_switch_from(sthread_ctx_t *ctx);

/*
 * Helper for sthread_user_create()
 * This is what each thread actually executes
 * This method wrapper handles execution of the thread's function,
 *   joining/detaching, and context switching after completion
 * If no thread is available, then calls exit() instead.
 */
static void ctx_start_function(void);

/*
 * Helper for sthread_user_yield() and sthread_user_cond_wait()
 * Yields the current thread 
 *   and stores the current thread on the given queue
 *   For a normal yield, 'wait_queue' == 'ready_queue'
 * The next ready thread is always taken from the 'ready_queue'
 * Note: Assumes that the ready_queue is NOT empty
 */
static void sthread_yield_onto_queue(sthread_queue_t wait_queue);

void sthread_user_init(void) {
  current_thread = sthread_user_create_empty();
  ready_queue = sthread_new_queue();
  detached_context = NULL;
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
  // Kill the current thread regardless of its join-ability
  sthread_detach(current_thread);
  
  // Switch to another thread
  sthread_switch_from(detached_context);

  // Not reachable
  assert(0);
}

void* sthread_user_join(sthread_t thread) {
  // Yield until the other thread completes
  while (!thread->done) {
    sthread_user_yield();
  }
  
  // Save the return value
  void *ret_val = thread->ret_val;
  
  // Deallocate the joined thread
  sthread_detach(thread);
  
  return ret_val;
}

void sthread_user_yield(void) {
  if (sthread_queue_is_empty(ready_queue)) {
    // Nothing to yield to
    return;
  }

  sthread_yield_onto_queue(ready_queue);
  
  // Note: When the calling thread resumes, it will begin here.
}

/*******************************/
/* Helper function definitions */
/*******************************/

static struct _sthread *sthread_user_create_empty() {
  // Allocate memory for the new thread
  struct _sthread *thread = (struct _sthread *)
                            malloc(sizeof(struct _sthread));

  // Did the malloc succeed?
  if (thread == NULL) {
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

static void sthread_detach(struct _sthread *thread) {
  // Is there an inactive context ready to be freed?
  if (detached_context != NULL) {
    sthread_free_ctx(detached_context);
  }

  detached_context = thread->saved_ctx;
  free(thread);
}

static void sthread_switch_from(sthread_ctx_t *ctx) {
  // If there are no remaining threads, exit the program
  if (sthread_queue_is_empty(ready_queue)) {
    exit(0);
  }

  // Switch to another ready thread
  current_thread = sthread_dequeue(ready_queue);
  sthread_switch(ctx, current_thread->saved_ctx);

  // Not reachable
  assert(0);
}

static void ctx_start_function(void) {
  current_thread->ret_val =
          current_thread->start_routine(current_thread->start_arg);

  // Mark the thread as complete
  current_thread->done = 1;

  // Deallocate the current thread if it is not joinable
  // Note: We assume that all joinable threads will have join called on them.
  //       That is where their memory is freed.
  sthread_ctx_t *old_ctx;
  if (current_thread->joinable) {
    old_ctx = current_thread->saved_ctx;
  } else {
    sthread_detach(current_thread);
    old_ctx = detached_context;
  }
  
  // Switch to another thread
  sthread_switch_from(old_ctx);

  // Not reachable
  assert(0);
}

static void sthread_yield_onto_queue(sthread_queue_t wait_queue) {
  if (current_thread->saved_ctx == NULL) {
    current_thread->saved_ctx = sthread_new_blank_ctx();
  }
  
  // Keep a reference to the current context
  sthread_ctx_t *old_context = current_thread->saved_ctx;

  // Swap the current thread with another ready thread
  sthread_enqueue(wait_queue, current_thread);
  current_thread = sthread_dequeue(ready_queue);
  
  // Switch contexts
  // This saves the current context onto the queue
  sthread_switch(old_context, current_thread->saved_ctx);
  
  // Note: When the calling thread resumes, it will begin here.
}

/*********************************************************************/
/* Part 2: Synchronization Primitives                                */
/*********************************************************************/

struct _sthread_mutex {
  int locked;
};

sthread_mutex_t sthread_user_mutex_init() {
  // Allocate memory for the new mutex
  struct _sthread_mutex *mutex = (struct _sthread_mutex *)
                                 malloc(sizeof(struct _sthread_mutex));

  // Did the malloc succeed?
  if (mutex == NULL) {
    fprintf(stderr, "Out of memory (sthread_user_mutex_init)\n");
    return NULL;
  }
  
  // Mutex starts unlocked
  mutex->locked = 0;
  return mutex;
}

void sthread_user_mutex_free(sthread_mutex_t lock) {
  // Assuming no waiters
  free(lock);
}

void sthread_user_mutex_lock(sthread_mutex_t lock) {
  // If blocking, make sure there's something else scheduled
  if (sthread_queue_is_empty(ready_queue)) {
    fprintf(stderr, "Infinite blocking detected (sthread_user_mutex_lock)\n");
    exit(0);
  }
  
  // Block until the lock is free
  while (lock->locked) {
    sthread_user_yield();
  }
  
  lock->locked = 1;
}

void sthread_user_mutex_unlock(sthread_mutex_t lock) {
  lock->locked = 0;
}


struct _sthread_cond {
  sthread_queue_t wait_queue;
};

sthread_cond_t sthread_user_cond_init(void) {
  // Allocate memory for the new condition
  struct _sthread_cond *cond = (struct _sthread_cond *)
                                malloc(sizeof(struct _sthread_cond));

  // Did the malloc succeed?
  if (cond == NULL) {
    fprintf(stderr, "Out of memory (sthread_user_cond_init)\n");
    return NULL;
  }
  
  cond->wait_queue = sthread_new_queue();
  return cond;
}

void sthread_user_cond_free(sthread_cond_t cond) {
  // Assuming no waiters
  sthread_free_queue(cond->wait_queue);
  free(cond);
}

void sthread_user_cond_signal(sthread_cond_t cond) {
  // No threads to signal
  if (sthread_queue_is_empty(cond->wait_queue)) {
    return;
  }
  
  // Move a single thread into the 'ready_queue'
  sthread_enqueue(ready_queue, sthread_dequeue(cond->wait_queue));
}

void sthread_user_cond_broadcast(sthread_cond_t cond) {
  // Move all waiting threads onto the 'ready_queue'
  while (!sthread_queue_is_empty(cond->wait_queue)) {
    sthread_enqueue(ready_queue, sthread_dequeue(cond->wait_queue));
  }
}

void sthread_user_cond_wait(sthread_cond_t cond,
                            sthread_mutex_t lock) {
  // If blocking, make sure there's something else scheduled
  if (sthread_queue_is_empty(ready_queue)) {
    fprintf(stderr, "Infinite blocking detected (sthread_user_cond_wait)\n");
    exit(0);
  }
  
  // Unlock the given mutex, assuming the caller holds the lock
  sthread_user_mutex_unlock(lock);
  
  // Have the current thread wait on a separate queue
  // Signalling/broadcasting the condition 
  //   will move the thread onto the 'ready_queue'
  sthread_yield_onto_queue(cond->wait_queue);
  
  // Note: When the calling thread is signalled, it will begin here.
}
