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
  hash_compare comparer;
  hash_entry* entries;
  int capacity;
  int size;
};

hash_table* hash_create(hash_hasher hasher, hash_compare comparer) {
  assert(hasher != NULL);
  assert(comparer != NULL);

  // Allocate the table
  hash_table* ht = (hash_table*) malloc(sizeof(hash_table));
  assert(ht != NULL);

  // Save and initialize
  ht->hasher = hasher;
  ht->comparer = comparer;
  ht->entries = (hash_entry*)
      calloc(INITIAL_HASH_TABLE_SIZE, sizeof(hash_entry));
  ht->capacity = INITIAL_HASH_TABLE_SIZE;
  ht->size = 0;

  assert(ht->entries != NULL);
  return ht;
}

/* Private */
// Inserts the key-value into the hash table
// Similar to hash_insert(...) but does not enlarge the table
static void hash_insert_private(hash_table* ht, hash_entry he,
                                void** removed_key_ptr,
                                void** removed_value_ptr) {
  // Iterate over the table and find where to insert the key-value
  int startIndex = ht->hasher(he.key) % ht->capacity;
  int iter = startIndex;
  do {
    // Insert if:
    //    1) There is no entry in this slot
    // OR 2) The entry in this slot has been deleted
    // OR 3) The key in this slot matches
    if (ht->entries[iter].key == NULL
        || ht->entries[iter].value == NULL
        || ht->comparer(he.key, ht->entries[iter].key) == 0) {
      // Increment the size
      // Case 1: A manual delete
      if (he.value == NULL) {
        ht->size--;
      }

      // Case 2: An overwrite
      // No change

      // Case 3: A normal insert
      if (he.value != NULL && ht->entries[iter].value == NULL) {
        ht->size++;
      }

      // Return the (possibly NULL) overwritten key-value
      *removed_key_ptr = ht->entries[iter].key;
      *removed_value_ptr = ht->entries[iter].value;

      // Perform the insert
      ht->entries[iter] = he;

      break;
    }

    // Have the iterator loop around if necessary
    iter = (iter + 1) % ht->capacity;

    // If the iteration goes through all the slots,
    // then something is fatally wrong with the table
    assert(iter != startIndex);
  } while (iter != startIndex);
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
    if (old[i].key != NULL && old[i].value != NULL) {
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
  assert(ht != NULL);
  assert(key != NULL);
  assert(removed_key_ptr != NULL);
  assert(removed_value_ptr != NULL);

  // Check to see if insertion would have the load
  // of the table surpass a pre-defined threshold.
  if ((ht->size + 1) * INVERSE_LOAD_FACTOR >= ht->capacity) {
    // If it does, expand the table
    hash_enlarge_table(ht);
  }

  // Combine the key-value
  hash_entry he;
  he.key = key;
  he.value = value;

  // Do the insert
  hash_insert_private(ht, he, removed_key_ptr, removed_value_ptr);
}

/* Private */
// Returns the index of the key within the table
// Or -1 if not found
static int hash_lookup_index(hash_table* ht, const void* key) {
  assert(ht != NULL);
  assert(key != NULL);

  int startIndex = ht->hasher(key) % ht->capacity;
  int iter = startIndex;
  do {
    // Match the keys, ignoring deleted values
    if (ht->entries[iter].value != NULL
        && ht->comparer(key, ht->entries[iter].key) == 0) {
      return iter;
    }

    // Have the iterator loop around if necessary
    iter = (iter + 1) % ht->capacity;
  } while (iter != startIndex);

  // Didn't find a matching key
  return -1;
}

bool hash_lookup(hash_table* ht, const void* key, void** value_ptr) {
  assert(value_ptr != NULL);

  int index = hash_lookup_index(ht, key);

  if (index >= 0) {
    // Found a matching key
    *value_ptr = ht->entries[index].value;
    return true;
  }

  // Didn't find a matching key
  return false;
}

bool hash_is_present(hash_table* ht, const void* key) {
  return hash_lookup_index(ht, key) >= 0;
}

bool hash_remove(hash_table* ht, const void* key,
                 void** removed_key_ptr, void** removed_value_ptr) {
  assert(removed_key_ptr != NULL);
  assert(removed_value_ptr != NULL);

  int index = hash_lookup_index(ht, key);

  if (index >= 0) {
    // Found a matching key
    *removed_key_ptr = ht->entries[index].key;
    *removed_value_ptr = ht->entries[index].value;
    ht->entries[index].value = NULL;
    return true;
  }

  // Didn't find a matching key
  return false;
}

void hash_destroy(hash_table* ht, bool free_keys, bool free_values) {
  if (free_values) {
    // Free each non-NULL value
    for (int i = 0; i < ht->capacity; i++) {
      if (ht->entries[i].value != NULL) {
        free(ht->entries[i].value);
      }
    }
  }

  if (free_keys) {
    // Free each non-NULL key with a non-NULL value
    for (int i = 0; i < ht->capacity; i++) {
      if (ht->entries[i].key != NULL && ht->entries[i].value != NULL) {
        free(ht->entries[i].key);
      }
    }
  }

  // Clean up the rest of the table
  free(ht->entries);
  free(ht);
}
