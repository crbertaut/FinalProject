#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#include <papi.h>

#include "include/interpose.hh"

/* https://github.com/ccurtsinger/interpose */
/* http://jayconrod.com/posts/23/tutorial-function-interposition-in-linux */
/* http://icl.cs.utk.edu/projects/papi/wiki/PAPIC:EventSets */
/* http://icl.cs.utk.edu/projects/papi/wiki/PAPIC:Preset_Event_Definitions */
/* http://icl.cs.utk.edu/projects/papi/wiki/PAPIC:Overview#Example_Code */

int files_opened = 0;
int files_closed = 0;
int threads_made = 0;
int threads_joined = 0;
int threads_used = 0;

int retval, EventSet = PAPI_NULL;
long long values[1];


void stop(int EventSet, long long values[]) {
    /* Stop counting events in EventSet */
  if (PAPI_stop(EventSet, values) != PAPI_OK) {
    perror("Error stopping count for EventSet");
  }
}

int main(int argc, char ** argv) {
    /* Initialize PAPI library */
  retval = PAPI_library_init(PAPI_VER_CURRENT);
  if (retval != PAPI_VER_CURRENT) {
    fprintf(stderr, "PAPI library init error.\n");
    exit(1);
  }

  /* Create an EventSet */
  if (PAPI_create_eventset(&EventSet) != PAPI_OK) {
    perror("Error creating EventSet");
  }

  /* Add event to EventSet */
  if (PAPI_add_event(EventSet, PAPI_TOT_INS) != PAPI_OK) {
    perror("Error adding event to EventSet");
  }

  /* Start counting events in EventSet */
  if (PAPI_start(EventSet) != PAPI_OK) {
    perror("Error starting count for EventSet");
  }
  
  return 0;
}




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

INTERPOSE(pthread_create)(pthread_t * thread, const pthread_attr_t * attr, void * (*start_routine) (void *), void * arg) {
  int ret = real::pthread_create(thread, attr, start_routine, arg);
  if (ret == 0) {
    threads_made++;
  }
  return ret;
}

INTERPOSE(pthread_join)(pthread_t thread, void ** retVal) {
  int ret = real::pthread_join(thread, retVal);
  if (ret == 0) {
    threads_joined++;
  }
  return ret;
}

INTERPOSE(exit)(int status) {
  stop(EventSet, values);
  
  printf("\n\nProgram Stats:\n");
  printf("----------FILES----------\n");
  printf("Files opened: %d\n", files_opened);
  printf("Files closed: %d\n", files_closed);
  printf("----------THREADS----------\n");
  printf("Threads created: %d\n", threads_made);
  printf("Threads joined: %d\n", threads_joined);
  printf("Threads used: %d\n", threads_used);
  printf("----------MEMORY----------\n");
  printf("Memory accesses: \n");
  printf("----------INSTRUCTIONS----------\n");
  printf("Total instructions executed: %lli\n", values[0]);
  
  
  real::exit(status);
}
