# hashset.c

A hashset implementation that uses contiguous memory space. This allows you to
build large sets and load them at runtime using `mmap`.

## Usage

Refer to `hashset.h` for the complete interface.

```c
#include "hashset.h"

int main() {
  set *ptr = NULL;
  set_alloc(&ptr, 5*1024, 20*1024, hasher);
  set_add(ptr, "abc", sizeof("abc"));
  assert(0 == set_find(ptr, "abc", sizeof("abc")));
  return 0;
}
```

## Examples

Refer to `preprocess.c` and `level0.c` for examples.

`preprocess` reads a dictionary of words and adds them to a hashset that is
persisted to disk.

```sh
$ make examples/preprocess
$ examples/preprocess
```

`level0` is a spellchecker that uses the blob created by `preprocess`.

```sh
$ make examples/level0
$ examples/level0
this is a corect string
this is a <corect> string
```

## Tests

Tests live in `tests/`. To run them, use

```sh
$ make test
```
