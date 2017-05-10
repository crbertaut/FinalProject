#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#include <papi.h>

#include "include/interpose.hh"


// Variables
int files_opened = 0;
int files_closed = 0;
int threads_made = 0;
int threads_joined = 0;

int retval, EventSet = PAPI_NULL;
long long values[3];

/// The type of a main function
typedef int (*main_fn_t)(int, char**, char**);

/// The program's real main function
main_fn_t real_main;


void stop(int EventSet, long long values[]) {
  // Stop counting events in EventSet
  if (PAPI_stop(EventSet, values) != PAPI_OK) {
    perror("Error stopping count for EventSet");
  }
}

int custom_main(int argc, char ** argv, char ** envp) {
  // Initialize PAPI library
  retval = PAPI_library_init(PAPI_VER_CURRENT);
  if (retval != PAPI_VER_CURRENT) {
    fprintf(stderr, "PAPI library init error.\n");
    exit(1);
  }
  
  // Create an EventSet
  if (PAPI_create_eventset(&EventSet) != PAPI_OK) {
    perror("Error creating EventSet");
  }

  // Add events to EventSet
  if (PAPI_add_event(EventSet, PAPI_TOT_INS) != PAPI_OK) {
    perror("Error adding event to EventSet");
  }
  if (PAPI_add_event(EventSet, PAPI_LD_INS) != PAPI_OK) {
    perror("Error adding event to EventSet");
  }
  if (PAPI_add_event(EventSet, PAPI_SR_INS) != PAPI_OK) {
    perror("Error adding event to EventSet");
  } 

  // Start counting events in EventSet
  if (PAPI_start(EventSet) != PAPI_OK) {
    perror("Error starting count for EventSet");
  }

  int status = real_main(argc, argv, envp);
  exit(status);
}

// Charlie Cursinger's code
extern "C" int __libc_start_main(main_fn_t, int, char**, void (*)(), void (*)(), void (*)(), void*) __attribute__((weak, alias("replacement_libc_start_main")));

extern "C" int replacement_libc_start_main(main_fn_t main_fn, int argc, char** argv, void (*init)(), void (*fini)(), void (*rtld_fini)(), void* stack_end) {
  // Find the real __libc_start_main
  auto real_libc_start_main = (decltype(__libc_start_main)*)dlsym(RTLD_NEXT, "__libc_start_main");
  // Save the program's real main function
  real_main = main_fn;
  // Run the real __libc_start_main, but pass in the custom main function
  int result = real_libc_start_main(custom_main, argc, argv, init, fini, rtld_fini, stack_end);
  return result;
}

// Interpose on fopen, incrementing files_opened iff successful
INTERPOSE(fopen)(const char * path, const char * mode) {
  FILE * file = real::fopen(path, mode);
  if (file != NULL) {
    files_opened++;
  }
  return file;
}
// Interpose on freopen, incrementing files_opened iff successful
INTERPOSE(freopen)(const char * path, const char * mode, FILE * stream) {
  FILE * file = real::freopen(path, mode, stream);
  if (file != NULL) {
    files_opened++;
  }
  return file;
}

// Interpose on fclose, incrementing files_closed iff successful
INTERPOSE(fclose)(FILE * file) {
  int ret = real::fclose(file);
  if (ret == 0) {
    files_closed++;
  }
  return ret;
}

// Interpose on pthread_create, incrementing threads_made iff successful
INTERPOSE(pthread_create)(pthread_t * thread, const pthread_attr_t * attr, void * (*start_routine) (void *), void * arg) {
  int ret = real::pthread_create(thread, attr, start_routine, arg);
  if (ret == 0) {
    threads_made++;
  }
  return ret;
}

// Interpose on pthread_join, incrementing threads_joined iff successful
INTERPOSE(pthread_join)(pthread_t thread, void ** retVal) {
  int ret = real::pthread_join(thread, retVal);
  if (ret == 0) {
    threads_joined++;
  }
  return ret;
}

// Interpose on exit
INTERPOSE(exit)(int status) {
  // Record EventSet values and stop counting
  PAPI_stop(EventSet, values);

  // Print results
  printf("\n\nProgram Stats:\n");
  printf("----------FILES----------\n");
  printf("Files opened: %d\n", files_opened - 10);
  printf("Files closed: %d\n", files_closed - 10);
  printf("----------THREADS----------\n");
  printf("Threads created: %d\n", threads_made);
  printf("Threads joined: %d\n", threads_joined);
  printf("----------MEMORY----------\n");
  printf("Total writes: %lli\n", values[1]);
  printf("Total reads: %lli\n", values[2]);
  printf("----------INSTRUCTIONS----------\n");
  printf("Total instructions executed: %lli\n\n", values[0]);

  // Exit the program for real
  real::exit(status);
}

