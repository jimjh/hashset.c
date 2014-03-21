#include "hasher.h"

size_t hasher(const void *element, size_t len) {
  if (NULL ==  element) return 0;

  size_t code = 0;
  const char *buffer = element;

  for (unsigned int i = 0; i < len; i++) {
    code  = buffer[i] + code * 31;
  }

  return code;
}
