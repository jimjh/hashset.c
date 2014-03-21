#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include "../csapp.h"
#include "../hashset.h"
#include "../hasher.h"

static const char *DICT = "/usr/share/dict/words";
static const char *BIN  = "examples/dict.bin";

static void Persist(set *hs) {
  FILE *out = Fopen(BIN, "w+");
  assert(!set_dump(hs, out));
  Fclose(out);
}

int main() {
  int in = Open(DICT, O_RDONLY, 0644);

  rio_t rio;
  Rio_readinitb(&rio, in);

  set *hs = NULL;
  assert(!set_alloc(&hs, 235000*2, 1024*1024*2.5, hasher));

  char *buffer = Calloc(5, 1024);
  ssize_t len = 0;

  // read each line, add to set
  while ((len = Rio_readlineb(&rio, buffer, 1024*5)) > 0) {
    if ('\n' == buffer[len-1]) len -= 1;
    assert(!set_add(hs, buffer, len));
  }

  Persist(hs);

  assert(!set_free(&hs));
  Close(in);
  return SUCCESS;
}
