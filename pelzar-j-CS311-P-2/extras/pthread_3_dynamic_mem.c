#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// sum computed by background threads


// Thread Func to generate sum of 0 to N
void* sum_runner(void *arg)
{
  long long *limit_ptr = (long long*) arg; // type casting void* to long long*
  long long limit = *limit_ptr; // dereference limit_ptr to get the value and store it in limit;

  // use it then clear it
  free(arg);



  long long sum = 0;
  for (long long i = 0; i < limit;  i++) {
    sum += i;
  }

  // pass back data in dynamically allocated mem
  long long *answer = malloc(sizeof(*answer));
  *answer = sum;

  // giving out a pointer to the memory we allocated
  pthread_exit(answer); // return answer
}

int main(int argc, char **argv)
{
  if (argc < 2) {
    printf("Usage: %s <num>\n", argv[0]);
    exit(-1);
  }

  long long *limit = malloc(sizeof(*limit));
  *limit = atoll(argv[1]);
  //long long limit = atoll(argv[1]);

  // thread id
  pthread_t thread_ID;

  // create attributes
  pthread_attr_t attr;
  pthread_attr_init(&attr);

  //pthread_create(&thread_ID, &attr, sum_runner, &limit); // dont need address of &limit because its already a pointer
  pthread_create(&thread_ID, &attr, sum_runner, limit);

  // wait until work is done
  long long *result;

  pthread_join(thread_ID, (void**)&result);
  printf("sum is %lld\n", *result);

  free(result);
}
