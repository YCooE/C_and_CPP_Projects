#include "transducers.h"

#include <unistd.h>
#include <assert.h>
#include <stdlib.h>

struct stream {
  FILE *fp;
  int read; // Flag for whether the stream is free or not
};

// Takes 1 argument, an array with two filepointers
static int file_pipe(FILE* files[2]) {
    int fds[2];
    int r = pipe(fds);
    if (r == 0) {
      files[0] = fdopen(fds[0], "r");
      assert(files[0] != NULL);
      files[1] = fdopen(fds[1], "w");
      assert(files[1] != NULL);

      if (files[0] && files[1]) {
        return 0;
      } else {
        return 1;
      }
    } else {
      return r;
    }
}

void transducers_free_stream(stream *s) {
  if (s->read == 1) {
      assert(fclose(s->fp) == 0); // Close the file_pipe's read port
      free(s);  // Free what has been malloc'ed
  }
}

int transducers_link_source(stream **out,
                            transducers_source s, const void *arg) {
  struct stream *new_stream = malloc(sizeof(struct stream));
  assert(new_stream != NULL);
  *out = new_stream;
  FILE* filearray[2];     // Filepointer array with 2 elements
  file_pipe(filearray);   // Sets up a file_pipe
  pid_t ret = fork();
  assert(ret >= 0);
  if(ret == 0) { // Child
    assert(fclose(filearray[0]) == 0); // Close the read port because child only writes
    s(arg, filearray[1]); // transducers source, takes 1 arg and a FILE *out
    assert(fclose(filearray[1]) == 0); // Close the write port
    exit(0);
  }
  else { // Parent
    assert(fclose(filearray[1]) == 0); // Close the write port because the parent only reads
    new_stream->fp = filearray[0];
    new_stream->read = 0; // Sets flag to free (0)
    return 0;
  }
}

int transducers_link_sink(transducers_sink s, void *arg, stream *in) {
  assert(in != NULL);
  if(in->read == 0) { // If it's free (0)
    s(arg, in->fp);
    in->read = 1; // Sets flag to not free (1)
    return 0;
  }
  else {
    return 1; // If the input stream has already been used
  }
}

int transducers_link_1(stream **out, transducers_1 t,
                       const void *arg, stream* in) {
  assert(in != NULL);
  if(in->read == 0) { // If the input stream has been used return 1
      struct stream* new_stream = malloc(sizeof(struct stream));
      assert(new_stream != NULL);
      *out = new_stream;
      FILE* filearray[2];
      file_pipe(filearray);
      pid_t ret = fork();
      assert(ret >= 0);
      if (ret == 0) { // Child
          assert(fclose(filearray[0]) == 0); // Close the read port because child only writes
          t(arg, filearray[1], in->fp);
          assert(fclose(filearray[1]) == 0); // Close the write port
          exit(0);
      }
      else { // Parent
          assert(fclose(filearray[1]) == 0); // Close the write port because the parent only reads
          new_stream->fp = filearray[0];
          new_stream->read = 0; // Sets flag to free (0)
          in->read = 1; // Sets flag to not free (1)
          return 0;
      }
  }
  else {
    return 1; // If the input stream has already been used
  }
}

int transducers_link_2(stream **out,
                       transducers_2 t, const void *arg,
                       stream* in1, stream* in2) {
  assert(in1 != NULL);
  assert(in2 != NULL);
  if (in1->read == 0 && in2->read == 0) {
      struct stream* new_stream = malloc(sizeof(struct stream));
      assert(new_stream != NULL);
      *out = new_stream;
      FILE* filearray[2];
      file_pipe(filearray); // Sets up a file_pipe
      pid_t ret = fork();
      assert(ret >= 0);
      if (ret == 0) { // Child
          assert(fclose(filearray[0]) == 0); // Close the read port because child only writes
          t(arg, filearray[1], in1->fp, in2->fp);
          assert(fclose(filearray[1]) == 0); // Close the write port
          exit(0);
      }
      else { // Parent
          assert(fclose(filearray[1]) == 0); // Close the write port because the parent only reads
          new_stream->fp = filearray[0];
          new_stream->read = 0;
          in1->read = 1; // Sets flag to not free (1)
          in2->read = 1;
          return 0;
      }
  }
  else {
    return 1; // If the input stream has already been used
  }
}

int transducers_dup(stream **out1, stream **out2,
                    stream *in) {
  assert(in != NULL);
  if(in->read == 0) {
      struct stream* new_stream = malloc(sizeof(struct stream));
      struct stream* new_stream_2 = malloc(sizeof(struct stream));
      assert(new_stream != NULL);
      assert(new_stream_2 != NULL);
      *out1 = new_stream;
      *out2 = new_stream_2;
      FILE* filearray[2];
      FILE* filearray2[2];
      file_pipe(filearray); // Sets up a file_pipe
      file_pipe(filearray2);
      pid_t ret = fork();
      assert(ret >= 0);
      if(ret == 0) { // Child
          assert(fclose(filearray[0]) == 0); // Close the read ports because child only writes
          assert(fclose(filearray2[0]) == 0);
          unsigned char k;
          while (fread(&k, sizeof(unsigned char), 1, in->fp) == 1) { // Read input from the input stream
            if (fwrite(&k, sizeof(unsigned char), 1, filearray[1]) != 1) { // Write input from the input stream to filearray
                break;
            }
            if (fwrite(&k, sizeof(unsigned char), 1, filearray2[1]) != 1) { // Write input from the input stream to filearray2
                break;
            }
          }
          assert(fclose(filearray[1]) == 0); // Close the write ports
          assert(fclose(filearray2[1]) == 0);
          exit(0);
      }
      else { // Parent
          assert(fclose(filearray[1]) == 0); // Close the write ports because the parent only reads
          assert(fclose(filearray2[1]) == 0);
          new_stream->fp = filearray[0]; 
          new_stream_2->fp = filearray2[0];
          new_stream->read = 0; // Sets flag to free (0)
          new_stream_2->read = 0;
          in->read = 1; // Sets flag to not free (0)
          return 0;
      }
  }
  else {
    return 1; // If the input stream has already been used
  }
}
