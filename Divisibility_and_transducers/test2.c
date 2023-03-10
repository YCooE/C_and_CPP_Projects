#include "transducers.h"

#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

//
// This is our negative test, we check if the library functions returns
// the expected value when a stream has already been read. It's based on
// the test0.c file.
//


void string_stream(const void *arg, FILE *out) {
  fputs((const char*) arg, out);
}

// These functions are not meant to do anything special, we simply
// test if the library functions return the expected values of either 0
// or 1.
void string_stream_1(const void *arg, FILE *out, FILE *in) {
  in = in;
  out = out;
  fputs((const char*) arg, out);
}

void string_stream_2(const void *arg, FILE *out, FILE *in1, FILE *in2) {
  in1 = in1;
  in2 = in2;
  out = out;
  fputs((const char*) arg, out);
}

void save_stream(void *arg, FILE *in) {
  unsigned char *d = arg;

  while (fread(d, sizeof(unsigned char), 1, in) == 1) {
    d++;
  }
}

int main() {
  stream* s[5];

  char *input = "Hello, World!";
  char *output = malloc(strlen(input)+1);
  output[strlen(input)] = '\0'; /* Ensure terminating NULL. */

  assert(transducers_link_source(&s[0], string_stream, input) == 0);
  assert(transducers_link_1(&s[1], string_stream_1, input, s[0]) == 0);
  assert(transducers_link_1(&s[1], string_stream_1, input, s[0]) == 1); // s[0]'s flag should be 1 now, and the function should return 1 because of that.
  assert(transducers_dup(&s[2], &s[3], s[1]) == 0);
  assert(transducers_dup(&s[2], &s[3], s[1]) == 1); // Should be 1 because streams have already been handled.
  assert(transducers_link_2(&s[4], string_stream_2, input, s[2], s[3]) == 0);
  assert(transducers_link_2(&s[4], string_stream_2, input, s[2], s[3]) == 1);
  assert(transducers_link_sink(save_stream, output, s[4]) == 0);
  assert(transducers_link_sink(save_stream, output, s[4]) == 1); // Sink should return 1 now.

  assert(strcmp(input,output) == 0); // For good measure we still keep this.

  for (int i = 0; i < (int)(sizeof(s)/sizeof(s[0])); i++) {
    transducers_free_stream(s[i]);
  }
  free(output);
  return 0;
}
