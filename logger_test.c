#include <c++/4.9.2/cstdint>
#include <c++/4.9.2/cstdio>
#include <c++/4.9.2/cstdlib>

#include "interpose.hh"

/* https://github.com/ccurtsinger/interpose */
/* https://gcc.gnu.org/onlinedocs/gcc-4.4.4/gcc/Code-Gen-Options.html */
/* https://gcc.gnu.org/onlinedocs/gcc-4.0.0/gcc/Variable-Attributes.html */
/* http://jayconrod.com/posts/23/tutorial-function-interposition-in-linux */
/* https://gcc.gnu.org/onlinedocs/gcc-4.0.0/gcc/Thread_002dLocal.html#Thread_002dLocal */

int files_opened = 0;
int files_closed = 0;


INTERPOSE(fopen)(const char * path, const char * mode) {
  FILE * file = real::fopen(path, mode);
  if (file != NULL) {
    files_opened++;
  }
  return file;
}
INTERPOSE(freopen)(const char * path, const char * mode, FILE * stream) {
  FILE * file = real::freopen(path, mode, stream);
  if (file != NULL) {
    files_opened++;
  }
  return file;
}

INTERPOSE(fclose)(FILE * file) {
  int ret = real::fclose(file);
  if (ret == 0) {
    files_closed++;
  }
  return ret;
}

INTERPOSE(exit)(int status) {
  printf("\n\nProgram Stats:\n");
  printf("Files left open: %d\n", files_opened - files_closed);
  
  real::exit(status);
}
