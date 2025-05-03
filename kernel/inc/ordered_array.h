#ifndef ORDERED_ARRAY_H
#define ORDERED_ARRAY_H

#include <common.h>

// Void pointer type.
typedef void* type_t;

// Less-than predicate type.
typedef bool (*less_than_predicate_t)(type_t, type_t);

// Ordered array structure.
typedef struct {
    type_t* array;
    size_t size;
    size_t max_size;
    less_than_predicate_t less_than;
} ordered_array_t;

// Creates an ordered array. Pass `NULL` to `less_than` if you want to use the default predicate.
ordered_array_t create_ordered_array(size_t max_size, less_than_predicate_t less_than);

// Places an ordered array at the address `addr`.
ordered_array_t place_ordered_array(void* addr, size_t max_size, less_than_predicate_t less_than);

// Destroys an ordered array.
void destroy_ordered_array(ordered_array_t* array);

// Adds an item to an ordered array.
void insert_ordered_array(type_t item, ordered_array_t* array);

// Finds the item at index `i`.
type_t lookup_ordered_array(size_t i, ordered_array_t* array);

// Deletes the item at location `i` from the array.
void remove_ordered_array(size_t i, ordered_array_t* array);

#endif