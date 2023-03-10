#include "transducers.h"

#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>


void string_stream(const void *arg, FILE *out) {
  fputs((const char*) arg, out);
}

void function(const void *arg, FILE *out, FILE* in1, FILE* in2){
  int d = *(int*)arg;
  unsigned char c;
  unsigned char a;
  while (fread(&c, sizeof(unsigned char), 1, in2) == 1){
      c += d;
      if (fwrite(&c, sizeof(unsigned char), 1, out) != 1) {
        break;
      }
    }
  while (fread(&a, sizeof(unsigned char), 1, in1) == 1){
      a += d;
      if (fwrite(&a, sizeof(unsigned char), 1, out) != 1) {
        break;
    }
  }
}

void save_stream(void *arg, FILE *in) {
  /* We will be writing bytes to this location. */
  unsigned char *d = arg;

  while (fread(d, sizeof(unsigned char), 1, in) == 1) {
    d++; /* Move location ahead by one byte. */
  }
}

int main() {
  stream* s[4];

  char *input = "aaa";
  char *output = malloc(2*strlen(input)+1);
  output[2*strlen(input)] = '\0'; /* Ensure terminating NULL. */
  int inc = 1;
  assert(transducers_link_source(&s[0], string_stream, input) == 0);
  assert(transducers_dup(&s[1], &s[2], s[0]) == 0);
  assert(transducers_link_2(&s[3], function, &inc, s[1], s[2]) == 0);
  assert(transducers_link_sink(save_stream, output, s[3]) == 0);
  /* We cannot use the '==' operator for comparing strings, as strings
     in C are just pointers.  Using '==' would compare the _addresses_
     of the two strings, which is not what we want. */
  assert(strcmp("bbbbbb", output) == 0);

  /* Note the sizeof()-trick to determine the number of elements in
     the array.  This *only* works for statically allocated arrays,
     *not* ones created by malloc(). */
  for (int i = 0; i < (int)(sizeof(s)/sizeof(s[0])); i++) {
    transducers_free_stream(s[i]);
  }
  free(output);
  return 0;
}
