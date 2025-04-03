#ifndef ORDERED_ARRAY_H
#define ORDERED_ARRAY_H

#include <common.h>

typedef void* type_t;
typedef bool (*lessthan_predicate_t)(type_t, type_t);

typedef struct {
    type_t* array;
    size_t size;
    size_t max_size;
    lessthan_predicate_t lessthan;
} ordered_array_t;

// Standard less-than predicate.
bool std_lessthan_predicate(type_t a, type_t b);

// Creates an ordered array.
ordered_array_t create_ordered_array(size_t max_size, lessthan_predicate_t lessthan);
ordered_array_t place_ordered_array(void* addr, size_t max_size, lessthan_predicate_t lessthan);

// Destroys an ordered array.
void destroy_ordered_array(ordered_array_t* array);

// Adds an item to an ordered array.
void insert_ordered_array(type_t item, ordered_array_t* array);

// Finds the item at index i.
type_t lookup_ordered_array(size_t i, ordered_array_t* array);

// Deletes the item at location i from the array.
void remove_ordered_array(size_t i, ordered_array_t* array);

#endif