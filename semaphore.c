#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define SIZE 10

static char buff[SIZE] = {0};
static int in = 0, out = 0;
static sem_t filled_cnt;
static sem_t avail_cnt;
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

int main() {
  pthread_t t1;
  sem_init(&filled_cnt, 0, 0);
  sem_init(&avail_cnt, 0, SIZE);
  pthread_create(&t1, NULL, thread_func, NULL);
  for (int i = 0;; i++) {
    sem_wait(&avail_cnt
  }
}
