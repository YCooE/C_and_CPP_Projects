TESTS=test0 test1 test2 test3 test4 test5
EXAMPLES=divisible

CC=gcc
CFLAGS=-g -Wall -Wextra -pedantic -std=gnu99

.PHONY: all test clean ../src.zip

all: $(TESTS) $(EXAMPLES)

transducers.o: transducers.c transducers.h
	$(CC) -c transducers.c $(CFLAGS)

%: %.c transducers.o
	$(CC) -o $@ $^ $(CFLAGS)

test: $(TESTS)
	@set e; for test in $(TESTS); do echo ./$$test; ./$$test; done
examples: $(EXAMPLES)
	./divisible 16 4 2
clean:
	rm -rf $(TESTS) $(EXAMPLES) *.o core

../src.zip:
	make clean
	cd .. && zip src.zip -r src
