#include <cstdio>
#include <cstdlib>
#include <pthread.h>

// Typedef a struct for arguments
typedef struct thread_args {
  int arg;
} thread_args_t;

// Number of threads
#define THREAD_NUM 5

// Declare pthread barrier
pthread_barrier_t barrier;

// Thread function
void* thread_fn(void* void_args) {
  thread_args_t* args = (thread_args_t*)void_args;
  
  // Do some computation here
  
  pthread_barrier_wait (&barrier);
  return NULL;
}


/* Main function */
int main(int argc, char** argv) {
  FILE * file1 = fopen("file.txt", "r");
  FILE * file2 = fopen("file2.txt", "w");
  
  // Create thread arrays
  pthread_t threadArr[THREAD_NUM];
  thread_args_t threadArgs[THREAD_NUM];

  // Initialize pthread barrier
  pthread_barrier_init(&barrier, NULL, THREAD_NUM);
  
  // Declare and initialize the threads
  for (int i = 0; i < THREAD_NUM; i++) {
    if(pthread_create(&threadArr[i], NULL, thread_fn, &threadArgs[i]) != 0) {
      perror("Error creating a thread.");
      exit(2);
    }
  }
  
  // Close one of the files
  fclose(file1);
  exit(0);
}
