#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "queue.h"
#include "simple_sync.h"

static pthread_t threads[NUM_THREADS] = {};
static queue_t data_queues[NUM_QUEUES] = {};
int rc = 0;

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
  printf("\t\t>>> creating thread [%d] with ID: %lu\n", tid, &threads[tid]);
  rc = pthread_create(&threads[tid], NULL, fp, args);
  if (rc) {
      printf("ERROR; return code from thread [%d] creation is %d\n", tid, rc);
      exit(-1);
  }
  printf("\t\t>>> Done...\n");
}

void sync_join() {
  printf("\t\t>>> join threads!\n");
  unsigned int i;
  for (i = 0; i < NUM_THREADS; i++) {
      if (threads[i]) {
          pthread_join(threads[i], NULL);
          if (rc) {
              printf("ERROR return code from thread [%d] join is %d\n", i, rc);
              exit(-1);
          }
      }
  }
  printf("\t\t>>> Done...\n");
}

void sync_init() {
  printf("\t\t>>> init queues!\n");
  unsigned int i;
  for (i = 0; i < NUM_QUEUES; i++) {
      queue_init(&data_queues[i]);
  }
  printf("\t\t>>> Done...\n");
}
