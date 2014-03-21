#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "hashset.h"

#define CHECK_NOT_NULL(x) if (NULL == (x)) { return ERR_PARAMS; }
#define CHECK_NULL(x)     if (NULL != (x)) { return ERR_PARAMS; }
#define CHECK_POSITIVE(x) if (0 >= (x))    { return ERR_PARAMS; }
#define CHECK_MALLOC(x)   if (NULL == (x)) { return ERR_MEM; }

//// IMPLEMENTATION
/* IDEA - keep elements in a managed heap, and keep relative addresses in
 * the hash buckets.
 */
// TODO deserialize with objcopy

struct set_t {
  size_t size;     // maximum number of elements, number of buckets
  size_t capacity; // maximum number of bytes
  hasher_t hasher; // hashing function
  size_t *map;     // array of pointers to elements
  void *heap;      // pointer to the heap of elements
  void *tail;      // pointer to the start of free space in the heap
};

int set_alloc(set **ptr, size_t size, size_t capacity, hasher_t func) {
  if (NULL == ptr  ||
      NULL != *ptr ||
      NULL == func ||
      0 >= size ||
      0 >= capacity) return ERR_PARAMS;

  // [struct | map | heap]
  size_t total = sizeof(set) + size * sizeof(size_t) + capacity;
  void *mem = calloc(1, total);
  CHECK_MALLOC(mem);

  set *candidate      = mem;

  candidate->size     = size;
  candidate->capacity = capacity;
  candidate->hasher   = func;
  candidate->map      = mem + sizeof(set);
  candidate->heap     = mem + sizeof(set) + size * sizeof(size_t);
  candidate->tail     = candidate->heap + 1;

  *ptr = candidate;
  return SUCCESS;
}

int set_free(set **ptr) {
  CHECK_NOT_NULL(ptr);
  CHECK_NOT_NULL(*ptr);

  free(*ptr);
  *ptr = NULL;
  return SUCCESS;
}

// TODO use masking instead of mod for power of 2
// TODO try double hashing
static inline int find_index(size_t hash, unsigned int probe, size_t size) {
  float i = probe;
  return (hash + (int)(0.5 * i + 0.5 * i * i)) % size;
}

static inline bool has_space(set *ptr, size_t len) {
  size_t room = ptr->capacity - (ptr->tail - ptr->heap);
  return room > len;
}

int set_add(set *ptr, const void *element, size_t len) {
  CHECK_NOT_NULL(ptr);
  CHECK_NOT_NULL(element);
  CHECK_POSITIVE(len);

  if (!has_space(ptr, len)) return ERR_MEM;

  size_t hash = ptr->hasher(element, len);
  for (unsigned int i=0; i < ptr->size; i++) {
    size_t index = find_index(hash, i, ptr->size);
    if (!ptr->map[index]) { // insert
      ptr->map[index] = ptr->tail - ptr->heap;
      memcpy(ptr->tail, element, len);
      ptr->tail += len;
      return SUCCESS;
    }
  }

  return ERR_SIZE;
}

int set_find(const set *ptr, const void *needle, size_t len) {
  CHECK_NOT_NULL(ptr);
  CHECK_NOT_NULL(needle);
  CHECK_POSITIVE(len);

  size_t hash = ptr->hasher(needle, len);
  for (unsigned int i=0; i < ptr->size; i++) {
    size_t index = find_index(hash, i, ptr->size);
    if (!ptr->map[index]) return ERR_NOT_FOUND;
    // get offset, add to heap
    const void *candidate = ptr->heap + ptr->map[index];
    if (!memcmp(needle, candidate, len)) return SUCCESS;
  }

  return ERR_NOT_FOUND;
}

int set_dump(const set *ptr, FILE *file) {
  CHECK_NOT_NULL(ptr);
  CHECK_NOT_NULL(file);

  size_t total   = sizeof(set) + ptr->size * sizeof(size_t) + ptr->capacity;
  size_t written = fwrite(ptr, total, 1, file);
  if (written < 1) return ERR_IO;
  return SUCCESS;
}

int set_load(set **ptr, void *mem, hasher_t func) {
  CHECK_NOT_NULL(ptr);
  CHECK_NOT_NULL(mem);
  CHECK_NULL(*ptr);
  CHECK_NOT_NULL(func);

  set *candidate = mem;
  size_t size    = candidate->size;
  candidate->hasher = func;
  candidate->map    = mem + sizeof(set);
  candidate->heap   = mem + sizeof(set) + size * sizeof(size_t);
  candidate->tail   = candidate->heap + 1;

  *ptr = candidate;
  return SUCCESS;
}
