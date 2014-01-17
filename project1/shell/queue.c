/* Implements queue abstract data type. */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

/* Each link in the queue stores a queue_element and
 * a pointer to the next link in the queue. */
typedef struct _queue_link {
  queue_element* elem;
  struct _queue_link* next;
} queue_link;

/* This is the actual implementation of the queue struct that
 * is declared in queue.h. */
struct _queue {
  queue_link* head;
};

queue* queue_create() {
  queue* q = (queue*) malloc(sizeof(queue));
  assert(q != NULL);

  q->head = NULL;
  return q;
}

/* Private */
static queue_link* queue_new_element(queue_element* elem) {
  queue_link* ql = (queue_link*) malloc(sizeof(queue_link));
  assert(ql != NULL);

  ql->elem = elem;
  ql->next = NULL;

  return ql;
}

void queue_append(queue* q, queue_element* elem) {
  assert(q != NULL);

  // Bug #1
  // Corner case: Head element has not been initialized
  // i.e. queue size is 0
  if (q->head == NULL) {
    q->head = queue_new_element(elem);
    return;
  }

  // Find the last link in the queue.
  queue_link* cur;
  for (cur = q->head; cur->next != NULL; cur = cur->next) {}

  // Append the new link.
  cur->next = queue_new_element(elem);
}

bool queue_remove(queue* q, queue_element** elem_ptr) {
  queue_link* old_head;

  assert(q != NULL);
  assert(elem_ptr != NULL);
  if (queue_is_empty(q)) {
    return false;
  }

  *elem_ptr = q->head->elem;
  old_head = q->head;
  q->head = q->head->next;

  // Bug #2
  // Memory leak unless the link is freed
  free(old_head);
  return true;
}

bool queue_is_empty(queue* q) {
  assert(q != NULL);
  return q->head == NULL;
}

/* private */
static bool queue_count_one(queue_element* elem, queue_function_args* args) {
  size_t* count = (size_t*) args;
  *count = *count + 1;
  return true;
}

size_t queue_size(queue* q) {
  size_t count = 0;
  queue_apply(q, queue_count_one, &count);
  return count;
}

bool queue_apply(queue* q, queue_function qf, queue_function_args* args) {
  assert(q != NULL && qf != NULL);

  if (queue_is_empty(q))
    return false;

  for (queue_link* cur = q->head; cur; cur = cur->next) {
    if (!qf(cur->elem, args))
      break;
  }

  return true;
}

void queue_reverse(queue* q) {
  assert(q != NULL);

  // Nothing to reverse
  // A queue size of 1 also has nothing to reverse,
  //   but since queue_size does a traversal of the queue
  //   it would be faster to just perform the "reversal"
  if (queue_is_empty(q)) {
    return;
  }

  queue_link* cur = q->head;
  q->head = NULL;
  queue_link* next_elem;

  // Treat new_head as a stack and push the queue onto it
  for (; cur != NULL; cur = next_elem) {
    next_elem = cur->next;

    cur->next = q->head;
    q->head = cur;
  }
}

void queue_sort(queue* q, queue_compare qc) {
  assert(q != NULL && qc != NULL);

  if (queue_is_empty(q)) {
    return;
  }

  // Via insertion sort

  // Disconnect the head from the rest of the queue
  queue_link* cur = q->head->next;
  q->head->next = NULL;

  // Insert each element from the detached queue
  // back into the queue, in order
  while (cur != NULL) {
    // Detach the head of the detached queue
    // to be inserted into the main queue
    queue_link* insert = cur;
    cur = cur->next;
    insert->next = NULL;

    queue_link* prev = NULL;
    for (queue_link* sorted = q->head; sorted != NULL; sorted = sorted->next) {
      // Insert ahead of the the element
      if (qc(insert->elem, sorted->elem) <= 0) {
        // Is the value smaller than all values in the queue?
        if (prev == NULL) {
          insert->next = q->head;
          q->head = insert;
        } else {
          prev->next = insert;
          insert->next = sorted;
        }
        break;
      } else {
        // The value to be inserted is greater than all values in the queue
        if (sorted->next == NULL) {
          sorted->next = insert;
          break;
        }
      }
      prev = sorted;
    }
  }
}

void queue_destroy(queue* q) {
  queue_link* next;
  for (queue_link* cur = q->head; cur != NULL; cur = next) {
    next = cur->next;
    free(cur);
  }
  free(q);
}
