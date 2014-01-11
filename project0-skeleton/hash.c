/* Implements the abstract hash table.
 * Via linear probing.
 */

#include <assert.h>
#include <stdlib.h>

#include "hash.h"

// The starting capacity of a new hash table
#define INITIAL_HASH_TABLE_SIZE 10

// The number of elements within the hash table multiplied by this value
// must be less than the capacity of the hash table.
// If not, then the table is enlarged and rehashed upon insert.  
#define INVERSE_LOAD_FACTOR 2

// Determines how much the hash table's capacity expands upon enlargement.
// (By multiplying this value with the current capacity.)
#define HASH_TABLE_GROWTH_FACTOR 2

typedef struct _hash_entry {
  void* key;
  void* value;
} hash_entry;

struct _hash_table {
  hash_hasher hasher;
  hash_compare predicate;
  hash_entry* entries;
  int capacity;
  int size;
};

hash_table* hash_create(hash_hasher hasher, hash_compare predicate) {
  hash_table* ht = (hash_table*) malloc(sizeof(hash_table));
  assert(ht != NULL);

  ht->hasher = hasher;
  ht->predicate = predicate;
  ht->entries = (hash_entry*) 
      calloc(INITIAL_HASH_TABLE_SIZE, sizeof(hash_entry));
  ht->capacity = INITIAL_HASH_TABLE_SIZE;
  ht->size = 0;

  assert(ht->entries != NULL);
  return htable;
}

/* Private */
// Inserts the key-value into the hash table
// Similar to hash_insert(...) but does not enlarge the table
static void hash_insert_private(hash_table* ht, hash_entry* he
                                void** removed_key_ptr, 
                                void** removed_value_ptr) {
  //TODO
}

/* Private */
static void hash_enlarge_table(hash_table* ht) {
  // Hold onto the state of the passed-in hash table
  int oldCapacity = ht->capacity;
  int oldSize = ht->size;
  hash_entry* old = ht->entries;

  // Update the state of the hash table
  ht->capacity = HASH_TABLE_GROWTH_FACTOR * ht->capacity;
  ht->size = 0;
  ht->entries = (hash_entry*) 
      calloc(ht->capacity, sizeof(hash_entry));
  assert(ht->entries != NULL);

  // Iterate over the old entries and insert them into the new table
  void* staysNull = NULL;
  void* staysNullToo = NULL;
  for (int i = 0; i < oldCapacity; i++) {
    if (old[i]->key != NULL && old[i]->value != NULL) {
      hash_insert_private(ht, old[i], &staysNull, &staysNullToo);

      // No keys should be replaced by the table enlargement
      assert(staysNull == NULL);
      assert(staysNullToo == NULL);
    }
  }

  // Sanity check: did the correct number of elements get rehashed?
  assert(oldSize == ht->size);

  // Don't leak the old entries
  free(old);
}

void hash_insert(hash_table* ht, void* key, void* value,
                 void** removed_key_ptr, void** removed_value_ptr) {
  // Check to see if insertion would have the load 
  // of the table surpass a pre-defined threshold.
  if ((ht->size + 1) * INVERSE_LOAD_FACTOR >= ht->capacity) {
    // If it does, expand the table
    hash_enlarge_table(ht);
  }

  // Combine the key-value
  hash_entry* he = (hash_entry*) malloc(sizeof(hash_entry));
  assert(he != NULL);
  he->key = key;
  he->value = value;

  // Do the insert
  hash_insert_private(ht, he, removed_key_ptr, removed_value_ptr);
}

//TODO
