#include <ordered_array.h>
#include <memory.h>
#include <panic.h>

// Default predicate for ordered arrays.
static bool default_predicate(type_t a, type_t b) {
    return (a < b);
}

ordered_array_t create_ordered_array(size_t max_size, less_than_predicate_t less_than) {
    ordered_array_t ret;

    // set a default less-than predicate if a custom one is not specified.
    if (less_than == NULL) {
        less_than = default_predicate;
    }

    // allocate memory for the ordered array.
    ret.array = (void*)kmalloc(max_size * sizeof(type_t));
    memset(ret.array, 0, max_size * sizeof(type_t));

    // set the rest of the fields.
    ret.size = 0;
    ret.max_size = max_size;
    ret.less_than = less_than;

    return ret;
}

ordered_array_t place_ordered_array(void* addr, size_t max_size, less_than_predicate_t less_than) {
    ordered_array_t ret;

    // set a default less-than predicate if a custom one is not specified.
    if (less_than == NULL) {
        less_than = default_predicate;
    }

    // set the address of the ordered array and zero out the memory.
    ret.array = (type_t*)addr;
    memset(ret.array, 0, max_size * sizeof(type_t));

    // set the rest of the fields.
    ret.size = 0;
    ret.max_size = max_size;
    ret.less_than = less_than;

    return ret;
}

void destroy_ordered_array(ordered_array_t* array) {
    // TODO: this shouldn't be commented, but we don't have a kfree function that does not use ordered arrays.
    //kfree(array->array);
    (void)array;
}

void insert_ordered_array(type_t item, ordered_array_t* array) {
    // check if we have a valid predicate.
    assert(array->less_than, "Invalid Ordered Array", "No valid predicate was specified for ordered array.");

    // find the position to insert.
    size_t iterator = 0;
    while (iterator < array->size && array->less_than(array->array[iterator], item))
        iterator++;
    
    // insert at the end if the item is bigger than the array size.
    if (iterator == array->size) {
        array->array[array->size++] = item;
    }

    else {
        // insert into the middle.
        type_t tmp = array->array[iterator];
        array->array[iterator] = item;

        // shift elements to the right.
        while (iterator < array->size) {
            iterator++;
            type_t tmp2 = array->array[iterator];
            array->array[iterator] = tmp;
            tmp = tmp2;
        }

        // update size.
        array->size++;
    }
}

type_t lookup_ordered_array(size_t i, ordered_array_t* array) {
    // check if the index is valid.
    assert(i < array->size, "Invalid Ordered Array Index", "The index of a value in the ordered array was out of bounds.");

    // return the element at the index.
    return array->array[i];
}

void remove_ordered_array(size_t i, ordered_array_t* array) {
    // shift all elements before the index left.
    while (i < array->size) {
        array->array[i] = array->array[i + 1];
        i++;
    }

    // decrease the array size.
    array->size--;
}