#include <pthread.h>
#include <stdio.h>
#include "queue.h"
#include "simple_sync.h"

#define DEBUG 1

static pthread_t threads[NUM_THREADS] = {};
static queue_t data_queues[NUM_QUEUES] = {};


void sync_produce(unsigned long long elem, int val_id) {
#if DEBUG
  //printf("%lld\n", elem);
  printf("value [%lld] produced in queue [%d]\n", elem, val_id);
#endif
  queue_push(&data_queues[val_id], elem);
}

unsigned long long sync_consume(int val_id) {
#if DEBUG
  unsigned long long res = queue_pop(&data_queues[val_id]);
  printf("value [%lld] consumed in queue [%d]\n", res, val_id);
  //printf("%lld\n", res);
  return res;
#else
  return queue_pop(&data_queues[val_id]);
#endif
}

// called by master thread
// sends function pointers to the auxiliary threads
void sync_delegate(int tid, void *(*fp)(void *), void *args) {
  printf("\t >>> create thread!\n");
  pthread_create(&threads[tid], NULL, fp, args);
}

void sync_join() {
  printf("\t >>> join threads!\n");
  int i;
  for (i = 0; i < NUM_THREADS; i++) {
      if (threads[i]) {
          pthread_join(threads[i], NULL);
      }
  }
}

void sync_init() {
  printf("\t >>> init queues!\n");
  int i;
  for (i = 0; i < NUM_QUEUES; i++) {
      queue_init(&data_queues[i]);
  }
}
