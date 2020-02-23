#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// sum computed by background threads
 

// Thread Func to generate sum of 0 to N
void* sum_runner(void *arg)
{
  struct sum_runner_struct *arg_struct = (struct sum_runner_struct*) arg;

  long long sum = 0;
  for (long long i = 0; i < arg_struct->limit;  i++) {
    sum += i;
  }

  arg_struct -> answer = sum; // pass sum into struct
  pthread_exit(0); // the null value from pthread_join
}

int main(int argc, char **argv)
{
  if (argc < 2) {
    printf("Usage: %s <num 1> <num 2> ... <num-n>\n", argv[0]);
    exit(-1);
  }
  // number of args
  int num_args = argc - 1;

  struct sum_runner_struct args[num_args];

  // thread id dynamic array
  pthread_t thread_IDs[num_args];

  // create attributes
  for (int i = 0; i < num_args; i++) {

      //long long limit = atoll(argv[i+1]);
      args[i].limit = atoll(argv[i+1]);

      pthread_attr_t attr;
      pthread_attr_init(&attr);
      pthread_create(&thread_IDs[i], &attr, sum_runner, &args[i]);
  }

  // wait until work is done
  for (int i = 0; i < num_args; i++ ){
    pthread_join(thread_IDs[i], NULL);
    printf("sum for thread %d is %lld\n", i,args[i].answer);
  }
  //pthread_attr_destroy(&attr);
}
