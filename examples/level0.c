#include <assert.h>
#include <ctype.h>
#include "../csapp.h"
#include "../hasher.h"
#include "../hashset.h"

//extern unsigned char _binary_dict_bin_start;
//extern unsigned char _binary_dict_bin_end;
//extern unsigned char _binary_dict_bin_size;

static const char *DICT  = "examples/dict.bin";

static size_t Get_size(int dict) {
  struct stat statb;
  Fstat(dict, &statb);
  return statb.st_size;
}

static set *Open_dict(int dict, size_t size) {
  set *hs   = NULL;
  void *mem = Mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE, dict, 0);
  assert(!set_load(&hs, mem, hasher));
  //assert(!set_load(&hs, &_binary_dict_bin_start, hasher));
  return hs;
}

static void Close_dict(set *hs, int dict, size_t size) {
  Munmap(hs, size);
  Close(dict);
}

static void strtolower(const char *str, char *buf, size_t len) {
  for (int i=0; i<len; i++) {
    buf[i] = tolower(str[i]);
  }
}

int main() {
  // inspect dict
  int dict = Open(DICT, O_RDONLY, 0644);
  size_t size = Get_size(dict);

  // load hashset
  set *hs = Open_dict(dict, size);

  char buffer[5*1024];
  char lowered[5*1024];
  ssize_t len = 0;

  // read from STDIN, split on [ \n]
  rio_t rio;
  Rio_readinitb(&rio, 0);
  char last = '\0';
  while ((len = Rio_readtokenb(&rio, buffer, 1024*5)) > 0) {
    len -= 1;
    last = buffer[len];
    buffer[len] = '\0';

    strtolower(buffer, lowered, len);

    if (!set_find(hs, lowered, len)) {
      printf("%s%c", buffer, last);
    } else {
      printf("<%s>%c", buffer, last);
    }
  }

  Close_dict(hs, dict, size);
  return 0;
}
