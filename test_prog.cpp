#include <cstdio>
#include <cstdlib>
#include <pthread.h>

// Typedef a struct for arguments
typedef struct thread_args {
  int arg;
} thread_args_t;

// Number of threads
#define THREAD_NUM 3

pthread_barrier_t barrier;

void* thread_fn(void* void_args) {
  thread_args_t* args = (thread_args_t*)void_args;

  for(int i = 0; i < THREAD_NUM; i++) {
    printf("Hello, I am number %d. ", i);
    int* p = (int*)malloc(sizeof(int));
    printf("I live at %p.\n", p);
    free(p);
  }
  
  pthread_barrier_wait (&barrier);
  return NULL;
}

  
int main(int argc, char** argv) {

  // Create thread arrays
  pthread_t threadArr[THREAD_NUM];
  thread_args_t threadArgs[THREAD_NUM];

  pthread_barrier_init(&barrier, NULL, THREAD_NUM);
  
  // Declare and initialize the threads
  for (int i = 0; i < THREAD_NUM; i++) {
    if(pthread_create(&threadArr[i], NULL, thread_fn, &threadArgs[i]) != 0) {
      perror("Error creating a thread.");
      exit(2);
    }
  }
  exit(0);
}
