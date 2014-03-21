CC = gcc
CFLAGS = -Wall -Werror -O3 -std=gnu99 #-pthread

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

tests/test_hashset: hashset.o tests/test_hashset.o hasher.o
	$(CC) $(CFLAGS) $^ -o $@

examples/preprocess: hashset.o csapp.o hasher.o examples/preprocess.o
	$(CC) $(CFLAGS) $^ -o $@

examples/level0: hashset.o csapp.o hasher.o examples/level0.o
	$(CC) $(CFLAGS) $^ -o $@

examples/dict.bin: examples/preprocess
	examples/preprocess

.PHONY: test
test: tests/test_hashset
	tests/test_hashset

.PHONY: clean
clean:
	rm -f level0
	rm -f test_hashset
	rm -f preprocess
	rm -f *.o

.PHONY: help
help:
	@echo "make test                 run tests"
	@echo "make examples/preprocess  build example"
	@echo "make examples/level0      build example"
	@echo "make dict.bin             use examples/preprocess to construct a blob"

.DEFAULT_GOAL := help
