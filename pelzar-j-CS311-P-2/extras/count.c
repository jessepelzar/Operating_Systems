#include <stdio.h>
#include <pthread.h>

#define NUM_LOOPS 200000

long long sum = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// synchronization using mutex for the shared variable "sum"
// sum is not zero - race condition. order of execusion plays a large role
// critical section: shared data between multiple threads or processes
// all using sum - need to protect access to sum - use pthread mutex for locking


// non pthread function
/*
void counting_function(int offset) {
  for (int i = 0; i < NUM_LOOPS; i++ ) {
    sum += offset;
  }
}
*/

// pthread function
void* counting_function(void *arg) {
  int offset = *(int *) arg;
  for (int i = 0; i < NUM_LOOPS; i++) {

    // start critical section :: using mutex
    pthread_mutex_lock(&mutex);

    sum += offset;

    // end criical section
    pthread_mutex_unlock(&mutex);

  }
  pthread_exit(NULL);
}

int main(void) {

  // non pthread function call
  /*
  counting_function(1);
  */


  // spawn threads
  pthread_t ID_1;
  int offset_1 = 1;
  pthread_create(&ID_1, NULL, counting_function, &offset_1);

  pthread_t ID_2;
  int offset_2 = -1;
  pthread_create(&ID_2, NULL, counting_function, &offset_2);

  // wait for threads to finish
  pthread_join(ID_1, NULL);
  pthread_join(ID_2, NULL);

  printf("%lld\n", sum);
  return 0;
}
