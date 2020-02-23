#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

void myfunc1 (void *ptr);
void myfunc2 (void *ptr);

char buff[24];
sem_t mutex;

int main(int argc, char *argv[])
{
  pthread_t thread1;
  pthread_t thread2;

  char *mag1 = "Thread 1";
  char *mag2 = "Thread 2";

  //initiallize semaphore
  sem_init(&mutex, 0, 1);

  pthread_create (&thread1, NULL, (void*) &myfunc1, (void*) mag1);
  pthread_create (&thread2, NULL, (void*) &myfunc2, (void*) mag2);

  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);

  // destroy semaphore
  sem_destroy(&mutex);

  return 0;
}

void myfunc1(void* ptr)
{
  char *mag = (char*) ptr;

  printf("%s\n", mag);

  // start protecting data buff
  sem_wait(&mutex);

  sprintf(buff, "%s", "hi");

  // end
  sem_post(&mutex);

  pthread_exit(0);
}

void myfunc2(void* ptr)
{
  char *mag = (char*) ptr;

  printf("%s\n", mag);

  // start
  sem_wait(&mutex);

  printf("%s\n", buff);

  // end
  sem_post(&mutex);

  pthread_exit(0);
}
