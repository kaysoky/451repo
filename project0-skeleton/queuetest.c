#include <stdio.h>
#include <assert.h>
#include "queue.h"

// Helper for the test helpers used to check queue values
void print_queue_item(queue_element* elem, queue_function_args* args) {
  printf("Item %d => %d\n", *(int*) args, *(int*) elem);
}

/** Test functions **/

// Make some assertions about the state of the initialized queue
void test_queue_create() {
  queue* q = queue_create();

  // Should not return NULL
  assert(q != NULL);

  // Should start out empty
  assert(queue_is_empty(q));
  free(q);
}

// Checks the values of the queue
// Helper for test_queue_append()
bool check_queue_append_elements(queue_element* elem,
    queue_function_args* args) {
  print_queue_item(elem, args);

  if (*(int*) args < 3) {
    assert(*(int*) elem == *(int*) args);
  } else {
    assert(*(int*) elem == 0);
  }
  *(int*) args = *(int*) args + 1;
  return true;
}

// This test appends 4 elements to the queue and checks for persistence
// This is the test that was originally included in this file
// It has been modified as per the assignment
void test_queue_append() {
  queue* q = queue_create();

  int x = 0;
  int y = 1;
  int z = 2;

  queue_append(q, &x);
  assert(queue_size(q) == 1);
  assert(!queue_is_empty(q));

  queue_append(q, &y);
  assert(queue_size(q) == 2);
  assert(!queue_is_empty(q));

  queue_append(q, &z);
  assert(queue_size(q) == 3);
  assert(!queue_is_empty(q));

  queue_append(q, &x);
  assert(queue_size(q) == 4);
  assert(!queue_is_empty(q));
  printf("Queue size is %u\n", queue_size(q));

  int index = 0;
  queue_apply(q, check_queue_append_elements, &index);

  free(q);
}

// This test appends and removes elements from the queue
void test_queue_append_remove() {
  queue* q = queue_create();

  int a = 0;
  int b = 1;
  int c = 2;
  int d = 3;
  int* removed;

  queue_append(q, &a);
  queue_append(q, &b);
  assert(queue_size(q) == 2);
  assert(!queue_is_empty(q));

  // Should be FIFO
  queue_remove(q, (queue_element**) &removed);
  assert(removed == &a);
  assert(queue_size(q) == 1);
  assert(!queue_is_empty(q));

  // Removal should not destroy ability to append
  queue_append(q, &c);
  queue_append(q, &d);
  assert(queue_size(q) == 3);
  assert(!queue_is_empty(q));

  // Should be able to remove all elements
  queue_remove(q, (queue_element**) &removed);
  assert(removed == &b);
  queue_remove(q, (queue_element**) &removed);
  assert(removed == &c);
  queue_remove(q, (queue_element**) &removed);
  assert(removed == &d);

  // This operation should not fail
  assert(!queue_remove(q, (queue_element**) &removed));
  assert(queue_size(q) == 0);
  assert(queue_is_empty(q));

  // Should still be able to append
  queue_append(q, &a);
  assert(queue_size(q) == 1);
  assert(!queue_is_empty(q));

  free(q);
}

// Checks the values of the queue, assuming they count up from *args
// Helper for test_reverse_queue() and test_sort_queue()
bool check_queue_elements_in_order(queue_element* elem,
    queue_function_args* args) {
  print_queue_item(elem, args);
  assert(*(int*) elem == *(int*) args);
  *(int*) args = *(int*) args + 1;
  return true;
}

// Checks the values of the queue, assuming they count down from *args
// Helper for test_reverse_queue()
bool check_queue_elements_reversed(queue_element* elem,
    queue_function_args* args) {
  print_queue_item(elem, args);
  assert(*(int*) elem == *(int*) args);
  *(int*) args = *(int*) args - 1;
  return true;
}

void test_reverse_queue() {
  // This test appends and removes elements from the queue
  queue* q = queue_create();

  int a = 0;
  int b = 1;
  int c = 2;
  int index = 0;

  // Shouldn't do anything
  queue_reverse(q);

  // Shouldn't do anything
  queue_append(q, &a);
  queue_reverse(q);
  queue_apply(q, check_queue_elements_in_order, &index);

  // Should reverse
  queue_append(q, &b);
  queue_reverse(q);
  index = 1;
  queue_apply(q, check_queue_elements_reversed, &index);

  // Should revert the order
  queue_reverse(q);
  index = 0;
  queue_apply(q, check_queue_elements_in_order, &index);

  // Try again with more elements
  queue_append(q, &c);
  queue_reverse(q);
  index = 2;
  queue_apply(q, check_queue_elements_reversed, &index);

  queue_reverse(q);
  index = 0;
  queue_apply(q, check_queue_elements_in_order, &index);
}

// Helper for test_sort_queue()
int int_compare(queue_element* e1, queue_element* e2) {
  return *(int*)e1 - *(int*)e2;
}

void test_sort_queue() {
  // This test appends and sorts elements in the queue
  queue* q = queue_create();

  int a = 0;
  int b = 1;
  int c = 2;
  int index = 0;

  // No-op
  queue_sort(q, int_compare);

  // No-op again
  queue_append(q, &a);
  queue_sort(q, int_compare);

  // Shouldn't mess up the order
  // This exercises appending numbers to the end of the sorted queue
  queue_append(q, &b);
  queue_sort(q, int_compare);
  queue_apply(q, check_queue_elements_in_order, &index);

  // Same here
  queue_append(q, &c);
  queue_sort(q, int_compare);
  index = 0;
  queue_apply(q, check_queue_elements_in_order, &index);

  // Change the order
  // This exercises appending numbers to the beginning
  c = 0;
  b = 1;
  a = 2;
  queue_sort(q, int_compare);
  index = 0;
  queue_apply(q, check_queue_elements_in_order, &index);

  // Change the order again
  // This exercises appending numbers in the middle of the sorted queue
  c = 0;
  a = 1;
  b = 2;
  queue_sort(q, int_compare);
  index = 0;
  queue_apply(q, check_queue_elements_in_order, &index);

  // Just to make sure, pop the elements off
  int* removed;
  queue_remove(q, (queue_element **) &removed);
  assert(&c == removed);
  queue_remove(q, (queue_element **) &removed);
  assert(&a == removed);
  queue_remove(q, (queue_element **) &removed);
  assert(&b == removed);
}

/** Test running helpers **/

// Signature for a test function
typedef void (*test_function)(void);

// Wraps a test function with some print statements
void run_test(test_function tf, char* test_name) {
  printf("Testing %s...\n", test_name);
  tf();
  printf("Passed\n\n");
}

int main(int argc, char* argv[]) {
  printf("Beginning tests...\n\n");
  run_test(test_queue_create, "queue initialization");
  run_test(test_queue_append, "queue pushing (the original test)");
  run_test(test_queue_append_remove, "queue CRU[D]");
  run_test(test_reverse_queue, "queue reversal");
  run_test(test_sort_queue, "queue sorting");

  printf("All tests passed (Yay!)\n");
  return 0;
}

