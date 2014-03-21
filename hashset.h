/**
 * hashset.h
 * A simple hashset implementation that does not grow and uses quadratic
 * probing. It is intended to keep all data within a compact memory space that
 * can be persisted to disk.
 * @author Jiunn Haur Lim <jim@jimjh.com>
 */
#ifndef HASHSET_H
#define HASHSET_H

#include <stddef.h>

#define SUCCESS        0
#define ERR_PARAMS    -1
#define ERR_MEM       -2
#define ERR_NOT_FOUND -3
#define ERR_SIZE      -4
#define ERR_IO        -5

//// DATA TYPES

/**
 * Hashing Function
 * @param element     pointer to element
 * @param size        number of bytes occupied by element
 * @return hash code
 */
typedef size_t (*hasher_t)(const void *, size_t);

/**
 * Set Type
 */
typedef struct set_t set;

//// FUNCTION PROTOTYPES

/**
 * Allocates and initializes the set.
 * Given a set pointer, initializes it to a freshly allocated set that uses a
 * hash table with size-many buckets.
 *
 * @param ptr       address of a set pointer
 * @param size      number of elements
 * @param capacity  number of bytes
 * @return
 *     ERR_PARAMS if ptr is null, *ptr is not null, size is not positive, or
 *                capacity is not positive
 *     ERR_MEM    if there is insufficient memory
 *     SUCCESS    on success
 */
int set_alloc(set **ptr, size_t size, size_t capacity, hasher_t func);

/**
 * Frees the set.
 * Given a set pointer, frees all remaining nodes, frees the cache, and sets
 * the pointer to NULL.
 *
 * @param ptr     address of a set pointer
 * @return
 *     ERR_PARAMS if ptr is null
 *     SUCCESS    on success
 */
int set_free(set **ptr);

/**
 * Adds the given element to the set.
 * size-many bytes will be copied into the set.
 *
 * @param ptr     address of set pointer
 * @param element element to be added
 * @param size    number of bytes occupied by element
 * @return
 *    ERR_PARAMS if ptr is null, or element is null, or size is 0
 *    ERR_SIZE   if the given element could not be inserted
 *    SUCCESS    on success
 */
int set_add(set *ptr, const void *element, size_t size);

/**
 * Determines if the given element exists in the set.
 * size-many bytes will be compared.
 *
 * @param ptr     address of set pointer
 * @param element element to find
 * @param size    number of bytes occupied by element
 * @return
 *    ERR_PARAMS    if ptr is null, or element is null, or size is 0
 *    ERR_NOT_FOUND if element is not found
 *    SUCCESS       on success
 */
int set_find(const set *ptr, const void *element, size_t size);

/**
 * Serializes the set to the given FILE.
 *
 * @param ptr   address of set pointer
 * @param FILE  destination file
 *
 * @return
 *    ERR_IO    if a write error has occurred
 *    SUCCESS   on success
 */
int set_dump(const set *ptr, FILE *file);

/**
 * Loads the set from the given memory space, assuming it was written using
 * #set_dump.
 *
 * @param ptr      address of set pointer
 * @param mem      start of memory space
 * @return
 *     ERR_PARAMS if ptr is null, *ptr is not null, or mem is null
 *     SUCCESS    on success
 */
int set_load(set **ptr, void *mem, hasher_t func);

#endif
