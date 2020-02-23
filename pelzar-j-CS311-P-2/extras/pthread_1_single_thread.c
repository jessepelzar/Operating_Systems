#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// sum computed by background threads
long long sum = 0;

// Thread Func to generate sum of 0 to N
void* sum_runner(void *arg)
{
  long long *limit_ptr = (long long*) arg; // type casting void* to long long*
  long long limit = *limit_ptr; // dereference limit_ptr to get the value and store it in limit;


  for (long long i = 0; i < limit;  i++) {
    sum += i;
  }

  // what to do with answer

  pthread_exit(0); // the null value from pthread_join
}

int main(int argc, char **argv)
{
  if (argc < 2) {
    printf("Usage: %s <num>\n", argv[0]);
    exit(-1);
  }

  long long limit = atoll(argv[1]);

  // thread id
  pthread_t thread_ID;

  // create attributes
  pthread_attr_t attr;
  pthread_attr_init(&attr);

  pthread_create(&thread_ID, &attr, sum_runner, &limit);

  // wait until work is done
  pthread_join(thread_ID, NULL);
  printf("sum is %lld\n", sum);
  pthread_attr_destroy(&attr);
}
