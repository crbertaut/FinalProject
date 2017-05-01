#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include <interpose.hh>

/* https://gcc.gnu.org/onlinedocs/gcc-4.4.4/gcc/Code-Gen-Options.html */
/* https://gcc.gnu.org/onlinedocs/gcc-4.0.0/gcc/Variable-Attributes.html */

INTERPOSE(thread_fn)(void * args) {


}

INTERPOSE(exit)(int status) {
  printf("\n\nProgram Allocation Stats\n");





  real::exit(status);
}
