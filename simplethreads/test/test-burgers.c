/*
 * Part 3 of assignment
 */

#include <stdio.h>
#include <stdlib.h>
#include <sthread.h>
#include <assert.h>

/*
 * Simultaneously the burger and the burger stack
 * Each link on this linked list represents a burger
 */
struct burger_stack {
  int burger_id;
  struct burger_stack *next;
};

/****************************/
/* Mutex-controlled objects */
/****************************/

static sthread_mutex_t burger_lock;
static sthread_cond_t burger_ready;

/*
 * Provides unique IDs for burgers
 */
static int burger_counter = 0;

/*
 * The burger on top
 */
static struct burger_stack *yummy = NULL;

/***********************************/
/* End of Mutex-controlled objects */
/***********************************/

/*
 * Value determined by program arguments
 * Read-only afterwards
 */
static int num_burgers;

/*
 * Produces burgers and stacks them up
 */
void *cook_thread(void *arg);

/*
 * De-stacks burgers and consumes them
 */
void *student_thread(void *arg);

int main(int argc, char **argv) {
  // Check arguments
  if (argc != 4) {
    printf("Usage: test-burgers <num cooks> <num students> <num burgers>\n");
    exit(1);
  }

  // Convert arguments to ints
  int num_cooks, num_students;
  num_cooks = atoi(argv[1]);
  num_students = atoi(argv[2]);
  num_burgers = atoi(argv[3]);

  printf("Testing simplethreads, impl: %s\n",
         (sthread_get_impl() == STHREAD_PTHREAD_IMPL) ? "pthread" : "user");

  // Initialize the threading module and the synchronization objects
  sthread_init();
  burger_lock = sthread_mutex_init();
  burger_ready = sthread_cond_init();

  // Keep track of the cook and student threads
  sthread_t *cooks = (sthread_t *) malloc(sizeof(sthread_t) * num_cooks);
  sthread_t *students = (sthread_t *) malloc(sizeof(sthread_t) * num_students);

  // Initialize all the cooks and students
  int i;
  for (i = 0; i < num_cooks; i++) {
    cooks[i] = sthread_create(cook_thread, (void *)NULL, 1);
    if (cooks[i] == NULL) {
      printf("sthread_create failed\n");
      exit(1);
    }
  }
  for (i = 0; i < num_students; i++) {
    students[i] = sthread_create(student_thread, (void *)NULL, 1);
    if (students[i] == NULL) {
      printf("sthread_create failed\n");
      exit(1);
    }
  }
  
  // Wait for the test to complete
  for (i = 0; i < num_cooks; i++) {
    sthread_join(cooks[i]);
  }
  for (i = 0; i < num_students; i++) {
    sthread_join(students[i]);
  }
  
  printf("Test complete\n");
  
  // Some number of burgers should have been made
  // And all burgers should be eaten
  assert(burger_counter == num_burgers);
  assert(yummy == NULL);

  sthread_mutex_free(burger_lock);
  sthread_cond_free(burger_ready);
  return 0;
}

void *cook_thread(void *arg) {
  sthread_mutex_lock(burger_lock);
  
  while (burger_counter < num_burgers) {
    // Produce and stack a burger
    struct burger_stack *patty = (struct burger_stack *) 
                                 malloc(sizeof(struct burger_stack));
    patty->burger_id = burger_counter;
    patty->next = yummy;
    yummy = patty;
    printf("Cooked burger %d!\n", patty->burger_id);
    burger_counter++;
    
    // Notify the students
    sthread_cond_broadcast(burger_ready);
    
    // Give another thread a chance to run
    sthread_mutex_unlock(burger_lock);
    sthread_yield();
    sthread_mutex_lock(burger_lock);
  }
  
  sthread_mutex_unlock(burger_lock);
  return (void *)NULL;
}

void *student_thread(void *arg) {
  sthread_mutex_lock(burger_lock);
  
  while (burger_counter < num_burgers || yummy != NULL) {
    // Wait for a burger (unlocks the mutex)
    if (yummy == NULL) {
      sthread_cond_wait(burger_ready, burger_lock);
      sthread_mutex_lock(burger_lock);
    }
    
    if (yummy != NULL) {
      // Consume and de-stack a burger
      struct burger_stack *patty = yummy;
      yummy = patty->next;
      printf("Ate burger %d!\n", patty->burger_id);
      free(patty);
    }
    
    // Give another thread a chance to run
    sthread_mutex_unlock(burger_lock);
    sthread_yield();
    sthread_mutex_lock(burger_lock);
  }
  
  sthread_mutex_unlock(burger_lock);

  return (void *)NULL;
}
