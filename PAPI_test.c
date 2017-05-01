#include <papi.h>
#include <stdio.h>

int main(int argc, char ** argv) {
  int retval, EventSet = PAPI_NULL;
  long long values[];

  /* Initialize PAPI library */
  retval = PAPI_library_init(PAPI_VER_CURRENT);
  if (retval != PAPI_VER_CURRENT) {
    fprintf(stderr, "PAPI library init error.\n");
    exit(1);
  }

  /* Create an EventSet */
  if (PAPI_create_eventset(&EventSet) != PAPI_OK) {
    handle_error(1);
  }

  /* Add event to EventSet */
  if (PAPI_add_event(EventSet, EVENT) != PAPI_OK) {
    handle_error(1);
  }

  /* Start counting events in EventSet */
  if (PAPI_start(EventSet) != PAPI_OK) {
    handle_error(1);
  }

  /
  if (PAPI_read(EventSet, values) != PAPI_OK) {
    handle_error(1);
  }

  /* Stop counting events in EventSet */
  if (PAPI_stop(EventSet, values) != PAPI_OK) {
    handle_error(1);
  }
}
