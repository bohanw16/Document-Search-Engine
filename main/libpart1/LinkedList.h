/*
 * Copyright ©2022 Dylan Hartono.  All rights reserved.
 */

#ifndef HW1_PUBLIC_LINKEDLIST_H_
#define HW1_PUBLIC_LINKEDLIST_H_

#include <stdbool.h>    // bool
#include <stdint.h>     // uint64_t


///////////////////////////////////////////////////////////////////////////////
// `LinkedList` is a doubly-linked list.
//
// Please view the specifications of the interface of `LinkedList` here and
// implement it in `LinkedList.c`.
//
// Note: `struct ll` is defined in `LinkedList_priv.h` to hide implementation
// details. The `typedef` declares the structure, meaning users can use it as
// a type but cannot access any of the fields found in `struct ll`.
typedef struct ll LinkedList;

// LLPayload_t is the interface type for a payload, or value, in `LinkedList`.
// It is of type `void*` to allow psuedo-generic types (as all pointers can be
// casted to a `void*`).
//
// Note: In some future homeworks, you might see the hw cheat this typedef
// by storing an int64_t instead of a pointer as both are 64 bits, but not now
typedef void* LLPayload_t;

// A function pointer called `LLPayloadFreeFnPtr` that takes in the argument
// `payload` of type `LLPayload`. Used in `LinkedList_Free` for a user to
// determine how to handle payloads currently in `LinkedList`.
//
// Hint: View Pointers lecture for syntax review.
typedef void (*LLPayloadFreeFnPtr) (LLPayload_t payload);

// Allocates and return a heap allocated `LinkedList`.  The caller is
// responsible for calling `LinkedList_Free` to free the entire data structure.
// Stops program execution on memory failures.
LinkedList* LinkedList_Allocate(void);

// Frees `list` and calls `payload_free_function` on all payloads currently in
// `list` (meant to free the elements).
void LinkedList_Free(LinkedList* list,
                     LLPayloadFreeFnPtr payload_free_function);

// Returns the number of elements in `list`
int LinkedList_NumElements(LinkedList* list);

// Adds `payload` as a new element at the head, or start, of `list`.
// Stops program execution on memory failures.
//
// Note: the caller is responsible for interpreting and managing the memory of
// `payload`.
void LinkedList_Push(LinkedList* list, LLPayload_t payload);

// Pops, or removes, an element from the head, or start, of `list` and
// returns true iff element successfully is removed from `list` and false
// otherwise.
// Iff removed, `payload` will store the removed `LLPayload_t` as an
// ouput parameter.
bool LinkedList_Pop(LinkedList* list, LLPayload_t* payload);

// Adds `payload` as a new element at the tail, or end, of `list`.
// Stops program execution on memory failures.
//
// Note: the caller is responsible for interpreting and managing the memory of
// `payload`.
void LinkedList_Append(LinkedList* list, LLPayload_t payload);

// Remove the last element of `list` (the tail), which is similar to
// `LinkedList_Pop`. Returns true iff element is successfully removed and false
// otherwise. Iff element removed, `payload_ptr` will store the `LLPayload_t`
// removed as an output parameter.
bool LinkedList_Slice(LinkedList* list, LLPayload_t* payload_ptr);

// A function pointer called `LLPayloadComparatorFnPtr` with parameters
// `payload_a` and `payload_b`. Here are the expected return values:
//    -1  if payload_a < payload_b
//     0  if payload_a == payload_b
//    +1  if payload_a > payload_b
//
// Hint: View Pointers lecture for syntax review.
typedef int(*LLPayloadComparatorFnPtr)(LLPayload_t payload_a,
                                       LLPayload_t payload_b);

// Sorts `list` in-place using `comparator_function` to determine the sorting
// order in either ascending if `ascending` is true and descending otherwise
// (when ascending is false).
void LinkedList_Sort(LinkedList* list, bool ascending,
                     LLPayloadComparatorFnPtr comparator_function);


///////////////////////////////////////////////////////////////////////////////
// `LLIterator` iterates through all the payloads in `LinkedList` from the
// head, or front, to the tail, or back.

// Remember that `struct ll_iter` is stored in `LinkedList_priv.h` like how
// `struct ll` is stored there to hide implementation.
//
// Note: `LLIterator` may have unexpected results if the `LinkedList` it is
// iterating over mutates at any time outside of `LLIterator_Remove` function.
// Unfortunately, there is no way to error check this as `LLIterator` is not
// coupled with `LinkedList`.
typedef struct ll_iter LLIterator;

// Manufactures and returns an `LLIterator*` that will iterate through `list`.
// The iterator is initialized to point to the first element in `list`.
// Stops program execution if `table` is NULL or any memory failure.
//
// Note: `LLIterator*` could already invalid at the start.
//
// Reminder: caller is responsible for calling LLIterator_Free once finished
// using `LLIterator` to free memory associated to the data structure.
LLIterator* LLIterator_Allocate(LinkedList* list);

// Completely free's `iter`
void LLIterator_Free(LLIterator* iter);

// Returns true iff `iter` is current pointing at a valid element and
// false otherwise.
bool LLIterator_IsValid(LLIterator* iter);

// Advances `iter` to the next element of the `LinkedList` it is iterating over.
// Returns true iff `iter` successfully advances to the next element and
// `false` otherwise (no more elements to advance to).
bool LLIterator_Next(LLIterator* iter);

// Returns the current `payload` `iter` is pointing at in the `LinkedList`
// through the output parameter `payload`.
// Returns true iff the value of `payload` is from `iter` and false
// otherwise (`payload` is not treated as an output parameter)
bool LLIterator_Get(LLIterator* iter, LLPayload_t* payload);

// Removes the current `payload` `iter` is pointing at in the `HashTable`
// and returns true iff the `payload` was successfully removed and false
// otherwise. The `payload` will be removed with the `LLPayloadFreeFnPtr`.
//
// After deletion, the iterator may be in one of the following three states:
// - if there was only one element in the list, the iterator is now invalid
//   and cannot be used.  In this case, the caller is recommended to free
//   the now-invalid iterator.
// - if the deleted node had a successor (ie, it was pointing at the head),
//   the iterator is now pointing at the successor.
// - if the deleted node was the tail, the iterator is now pointing at the
//    predecessor.
bool LLIterator_Remove(LLIterator* iter,
                       LLPayloadFreeFnPtr payload_free_function);

#endif  // HW1_PUBLIC_LINKEDLIST_H_
