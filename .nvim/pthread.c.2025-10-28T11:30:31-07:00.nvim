#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

static pthread_mutex_t mutex0 = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

int count = 0;
void *thread0(void *arg) {
  pthread_mutex_lock(&mutex0);
  printf("thread0: mutex0\n");
  pthread_mutex_lock(&mutex1);
  printf("thread0: mutex1\n");
  pthread_mutex_unlock(&mutex0);
  pthread_mutex_unlock(&mutex1);
  printf("thread0 done;\n");
  pthread_exit(0);
}

void *thread1(void *arg) {
  pthread_mutex_lock(&mutex1);
  printf("thread1: mutex1\n");
  pthread_mutex_lock(&mutex0);
  printf("thread1: mutex0\n");
  pthread_mutex_unlock(&mutex1);
  pthread_mutex_unlock(&mutex0);
  printf("thread1 done;\n");
  pthread_exit(0);
}

int main() {

  pthread_t t0;
  pthread_t t1;

  if (pthread_create(&t0, NULL, thread0, NULL) != 0) {
    perror("pthread1");
    return 1;
  }
  if (pthread_create(&t1, NULL, thread1, NULL) != 0) {
    perror("pthread2");
    return 1;
  }

  pthread_join(t0, NULL);
  pthread_join(t1, NULL);

  return 0;
}
