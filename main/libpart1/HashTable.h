/*
 * Copyright ©2022 Dylan Hartono.  All rights reserved.
 */

#ifndef HW1_PUBLIC_HASHTABLE_H_
#define HW1_PUBLIC_HASHTABLE_H_

#include <stdbool.h>    // bool
#include <stdint.h>     // uint64_t

#include "LinkedList.h"

///////////////////////////////////////////////////////////////////////////////
// `HashTable` is a automatically-resizing chained hash table.
//
// `HashTable` emulates generic types by saving values as `void*` and having the
// user be responsible for its allocation. However, the keys are `uint64_t` and
// rely on `FNVHash64` function to generate `HTKey_t`.
//
// Note `struct ht` is actually defined in `HashTable_priv.h`. It is meant to
// prevent users from accessing fields of `struct ht` when using `HashTable`
// type.
//
// Extra Note: Resizing is mostly already handled in the starter code. It will
// resize the table when the load factor (num elements / num buckets) is
// greater than 3 by creating a new table with 9 times more buckets.
typedef struct ht HashTable;

// Key, Value, and Key-Value types
typedef uint64_t HTKey_t;   // hash table key type   (number)

typedef void* HTValue_t;    // hash table value type (pointer)

typedef struct {
  HTKey_t   key;
  HTValue_t value;
} HTKeyValue_t;

// Function pointer called `ValueFreeFnPtr` that takes in the argument `value`
// (Hint: View Pointers lecture for syntax review). Used in `HashTable_Free`.
typedef void (*ValueFreeFnPtr)(HTValue_t value);

// Hashes `len` bytes from `buffer` into a 64-bit key to use as a hash key.
// Returns `HTKey_t` to use for `HashTable`
// FNV Hashing: http://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function
HTKey_t FNVHash64(unsigned char* buffer, int len);

// Allocates and returns a `HashTable` with `num_buckets` initial buckets.
// Asserts `num_buckets < 1`.
// Returns a pointer to a newly heap allocated HashTable.
HashTable* HashTable_Allocate(int num_buckets);

// Deallocates `table` and deallocates its entries with `value_free_function`.
// Note `value_free_function` is a function pointer typedef'd above.
void HashTable_Free(HashTable* table, ValueFreeFnPtr value_free_function);

// Returns the number of elements in `table`
int HashTable_NumElements(HashTable* table);

// Inserts `new_key_value` to `table`. Returns true iff `new_key_value` is
// replacing a current key-value pair in `table` and false otherwise.
//
// If the function returns `true`, `old_key_value` will store the previous
// key-value pair in `table` as an output parameter and allow the user to take
// ownership of it.
bool HashTable_Insert(HashTable* table, HTKeyValue_t new_key_value,
                      HTKeyValue_t* old_key_value);

// Finds the key-value pair that's key is equal to `key` in `table`.
// Returns true iff `key` is in `table` and false otherwise.
//
// If the function returns true, `key_value` will store the associated
// key-value pair in `table` as an output parameter.
bool HashTable_Find(HashTable* table, HTKey_t key, HTKeyValue_t* key_value);

// Removes the key-value pair that's key is equal to `key` in `table` and
// returns true iff the key was found and false otherwise.
//
// If the function returns true, `key_value` will store the associated
// key-value pair removed in `table` as an output parameter. Remember that
// even though the pair is removed from the `table`, the user has ownership
// over the memory associated with it.
bool HashTable_Remove(HashTable* table, HTKey_t key, HTKeyValue_t* key_value);


///////////////////////////////////////////////////////////////////////////////
// `HTIterator` iterates through all the `HTKeyValue_t` in `HashTable`
// emulating it's Java equivalent.
//
// Remember that `struct ht_it` is stored in `HashTable_priv.h` like how
// `struct ht` is stored there to hide implementation.
//
// Note: `HTIterator` may have unexpected results if the `HashTable` it is
// iterating over mutates at any time outside of `HTIterator_Remove` function.
// Unfortunately, there is no way to error check this as `HTIterator` is not
// coupled with `HashTable`.
typedef struct ht_it HTIterator;

// Creates and returns an `HTIterator*` that will iterate through `table`. The
// iterator is initialized to point to the first `HTKeyValue_t`.
// Stops program execution if `table` is NULL or any memory failure.
//
// Note: `HTIterator*` could already invalid at the start
HTIterator* HTIterator_Allocate(HashTable* table);

// Completely free's `iter`.
void HTIterator_Free(HTIterator* iter);

// Returns true iff `iter` is current pointing at a valid `HTKeyValue_t` and
// false otherwise.
bool HTIterator_IsValid(HTIterator* iter);

// Advance `iter` to the next element of the `HashTable` it is iterating over.
// Returns true iff `iter` successfully advances to the next element and
// `false` otherwise (no more elements to advance to).
bool HTIterator_Next(HTIterator* iter);

// Returns the current `HTKeyValue_t` `iter` is pointing at in the `HashTable`
// through the output parameter `key_value`.
// Returns true iff the value of `key_value` is from `iter` and false
// otherwise (`key_value` is not treated as an output parameter)
bool HTIterator_Get(HTIterator* iter, HTKeyValue_t* key_value);

// Removes the current `HTKeyValue_t` `iter` is pointing at in the `HashTable`
// and returns true iff the `HTKeyValue_t` was successfully removed and false
// otherwise. If removed, `key_value` will store the removed `HTKeyValue_t` as
// an output parameter, and advance the iterator (or if there is no next
// `HTKeyValue_t`, invalidate `iter`).
bool HTIterator_Remove(HTIterator* iter, HTKeyValue_t* key_value);


#endif  // HW1_PUBLIC_HASHTABLE_H_
