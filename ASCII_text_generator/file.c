#define UTF8_1B(x) ((x & 128) == 0 ? 1 : 0)
#define UTF8_2B(x) ((x & 224) == 192 ? 1 : 0)
#define UTF8_3B(x) ((x & 240) == 224 ? 1 : 0)
#define UTF8_4B(x) ((x & 248) == 240 ? 1 : 0)
#define UTF8_CONT(x) ((x & 192) == 128 ? 1 : 0)
#define ASCII(x) (((x >= 0x07 && x <= 0x0D) || (x == 0x1B) || (x >= 0x20 && x <= 0x7E)) ? 1 : 0)

#include <stdio.h>  // fprintf, stdout
#include <stdlib.h> // EXIT_FAILURE, EXIT_SUCCESS.
#include <string.h> // strerror
#include <errno.h> // errnum
#include <stdbool.h> // bool

enum file_type{
  DATA,
  EMPTY,
  ASCII,
  ISO8859,
  UTF8,
  UTF16L,
  UTF16B,
};

const char* const FILE_TYPE_STRINGS[] = {
  "data",
  "empty",
  "ASCII text",
  "ISO-8859 text",
  "UTF-8 Unicode text",
  "Little-endian UTF-16 Unicode text",
  "Big-endian UTF-16 Unicode text",
};


int print_error(char *path, int max_length ,int errnum) {
  return fprintf(stdout, "%s:%*sERROR: cannot open `%s' (%s)\n", path, max_length - (int)strlen(path) + 1, "", path,  strerror(errnum));
}

bool file_exists();

void determine_file_type(FILE* file, char* path, int max_length);

bool determine_if_empty(FILE* file);

bool determine_if_ASCII(FILE* file);

bool determine_if_ISO(FILE* file);

bool determine_if_UTF8(FILE* file);

bool determine_if_Little_UTF16(FILE* file);

bool determine_if_Big_UTF16(FILE* file);

int determine_max_length(int argc, char* argv[]);

int main (int argc, char* argv[]) {
  char* path;

  // The program takes 1 or more arguments, if it has been given less
  // return false.
  if (argc < 2) {
    fprintf (stderr, "Usage: %s path\n", argv[0]);
    return EXIT_FAILURE;
  }
  int max_length = determine_max_length(argc, argv);
  // If correct amount of arguments have been entered, try to open the file.
  FILE* fp;
  for(int i = 1; i < argc; i++){
    path = argv[i];
    fp = fopen(path,"r");
    // If file could not be opened, fp will be NULL.
    if (fp == NULL) {
      print_error(path, max_length, errno);
    }
    else {
      determine_file_type(fp, argv[i], max_length);
      fclose(fp);
    }
  }

  return EXIT_SUCCESS;
}

int determine_max_length(int argc, char* argv[]) {
  int max_length;
  // Goes through each argument, only saves the length if it is longer than currect max_length
  for(int i = 1; i < argc; i++){
    if (max_length < (int)strlen(argv[i])) {
      max_length = (int)strlen(argv[i]);
    }
  }
  return max_length;
}

void determine_file_type(FILE* file, char* path, int max_length){
  FILE* fp;
  fp = file;
    if (determine_if_empty(fp)) {
      printf("%s:%*s%s\n", path, max_length - (int)strlen(path) + 1, "", FILE_TYPE_STRINGS[1]);
    }
    else if (determine_if_ASCII(fp)) {
      printf("%s:%*s%s\n", path, max_length - (int)strlen(path) + 1, "", FILE_TYPE_STRINGS[2]);
    }
    else if (determine_if_UTF8(fp)) {
      printf("%s:%*s%s\n", path, max_length - (int)strlen(path) + 1, "", FILE_TYPE_STRINGS[4]);
    }
    else if (determine_if_ISO(fp)) {
      printf("%s:%*s%s\n", path, max_length - (int)strlen(path) + 1, "", FILE_TYPE_STRINGS[3]);
    }
    else if (determine_if_Little_UTF16(fp)){
      printf("%s:%*s%s\n", path, max_length - (int)strlen(path) + 1, "", FILE_TYPE_STRINGS[5]);
    }
    else if (determine_if_Big_UTF16(fp)){
      printf("%s:%*s%s\n", path, max_length - (int)strlen(path) + 1, "", FILE_TYPE_STRINGS[6]);
    }
    // If file type has not been determined yet, the file type is data.
    else {
      printf("%s:%*s%s\n", path, max_length - (int)strlen(path) + 1, "", FILE_TYPE_STRINGS[0]);
    }
}

bool determine_if_empty(FILE* file){
  FILE* fp;
  fp = file;

  int size;

  fseek(fp, 0, SEEK_END);

  size = ftell(fp);

  // If size is 0, return true.
  if (size == 0) {
    return true;
  }
  else {
    rewind(fp);

    return false;
  }
}

bool determine_if_ASCII(FILE* file){
  bool retval;
  retval = true;

  FILE* fp;
  fp = file;

  int size;
  fseek(fp, 0, SEEK_END);
  size = ftell(fp);
  rewind(fp);

  unsigned char buffer[1];

  for (int i = 0; i <= (size-1); i++) {
    fread(buffer, 1, 1, fp);
    if (!ASCII(*buffer)) {
      // Since a character that is not ASCII has been found, set retval to false;
      retval = false;
      break;
    }
  }

  rewind(fp);

  return retval;
}

bool determine_if_ISO(FILE* file){
  bool retval;
  retval = true;

  FILE* fp;
  fp = file;

  int size;
  fseek(fp, 0, SEEK_END);
  size = ftell(fp);
  rewind(fp);

  unsigned char buffer[1];

  for (int i = 0; i <= (size-1); i++) {
    fread(buffer, 1, 1, fp);
    if (!ASCII(*buffer) && !(*buffer >= 0xA0)) {
      // Since a character that is not ISO-8859 or ASCII has been found, set retval to false.
      retval = false;
      break;
    }
  }

  rewind(fp);

  return retval;
}

bool determine_if_UTF8(FILE* file){
  bool retval;
  retval = true;

  FILE* fp;
  fp = file;

  unsigned char buffer[1];

  // Checks the bits using macros, if they fit the 1 of the 4 variable encoding
  // that UTF-8 uses, simply advance to the next relevant byte.
  while (1) {
    fread(buffer, 1, 1, fp);
    // If at the end of the file, close loop.
    if (feof(fp)) {
      break;
    }
    if (UTF8_4B(*buffer)) {
      // Checks the next 3 bytes if the begin with 10xxxxxx.
      for (int i = 0; i < 3; i++) {
        fread(buffer, 1, 1, fp);
        if (UTF8_CONT(*buffer)){
          continue;
        }
        else {
          return false;
        }
      }
      continue;
    }
    else if (UTF8_3B(*buffer)) {
      // Checks the next 2 bytes if the begin with 10xxxxxx.
      for (int i = 0; i < 2; i++) {
        fread(buffer, 1, 1, fp);
        if (UTF8_CONT(*buffer)){
          continue;
        }
        else {
          return false;
        }
      }
      continue;
    }
    else if (UTF8_2B(*buffer)) {
      // Checks the next byte if the begin with 10xxxxxx.
      for (int i = 0; i < 1; i++) {
        fread(buffer, 1, 1, fp);
        if (UTF8_CONT(*buffer)){
          continue;
        }
        else {
          return false;
        }
      }
      continue;
    }
    else if (UTF8_1B(*buffer) && ASCII(buffer[0])) {
      continue;
    }
    else {
      retval = false;
      break;
    }
  }

  rewind(fp);

  return retval;
}

bool determine_if_Little_UTF16(FILE* file){
  bool retval;
  retval = false;

  FILE* fp;
  fp = file;

  unsigned char buffer[3];

  fread(buffer, 1, 3, fp);

  // Checks the byte order mark
  if (buffer[0] == 0xFF && buffer[1] == 0xFE) {
    retval = true;
  }

  rewind(fp);

  return retval;
}

bool determine_if_Big_UTF16(FILE* file){
  bool retval;
  retval = false;

  FILE* fp;
  fp = file;

  unsigned char buffer[3];

  fread(buffer, 1, 3, fp);

  // Checks the byte order mark
  if (buffer[0] == 0xFE && buffer[1] == 0xFF) {
    retval = true;
  }

  rewind(fp);

  return retval;
}
