#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "queue.h"

//#define NUMBER 100000
#define NUMBER   100000
#define NUM_THREADS 10
#define ON 1 // a dummy switch
#define VALUE 55

static queue_t *q;

static void *produce(void *arg) {
  int i;
  unsigned int id;
  id = arg;
  printf("\t\t\tproduce working thread [%u] - Entry\n", id);
  for (i = 0; i < NUMBER; i++) {
      // enqueue 1 for NUMBER of times
      queue_push(q, 1);
  }
  printf("\t\t\tproduce working thread [%u] - Done\n", id);
  //return NULL;
  pthread_exit(NULL);
}

static void *consume(void *arg) {
  int i;
  unsigned int id;
  id = arg;
  printf("\t\t\tconsume working thread [%u] - Entry\n", id);
  unsigned long long sum = 0;
  for (i = 0; i < NUMBER; i++) {
      // sum to NUMBER during dequeue
      sum += queue_pop(q);
  }
  printf("\t\t\tconsume working thread [%u] - Done sum = %lld\n", id, sum);
  //return (void *)sum;
  pthread_exit((void *) sum);
  //pthread_exit(NULL);
}

static void *block_consume(void *arg) {
  unsigned long long result;
  printf("\t\t\tblock_consume working thread - Entry\n");
  result = queue_pop(q);
  printf("\t\t\tblock_consume working thread - Done\n");
  //return (void *)result;
  pthread_exit((void *) result);
}

static void *block_produce(void *arg) {
  int i;
  printf("\t\t\tblock_produce working thread - Entry\n");
  // why +1 here? I think this is to make it full exceeding the size
  for (i = 0; i < QUEUE_MAXLEN + 1; i++) {
      queue_push(q, i);
  }
  //printf("Blocked producer done!\n");
  printf("\t\t\tblock_produce working thread - Done\n");
  //return NULL;
  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  // 10 threads for each and each queue size is 32 elements
  pthread_t producers[NUM_THREADS];
  pthread_t consumers[NUM_THREADS];
  pthread_attr_t attr;
  unsigned int i;
  unsigned int p_id[NUM_THREADS];
  unsigned int c_id[NUM_THREADS];
  int rc_p, rc_c, rc;
  unsigned long long ret_val;

  q = (queue_t *)malloc(sizeof(queue_t));

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  printf("\n\n\nInitialize queue...\n");
  queue_init(q);
  printf("Done.\n\n");

  printf("Creating producers and consumers...\n");
  for (i = 0; i < NUM_THREADS; i++) {
      // 10 threads for producer, each thread enqueue data in the for loop
      // so there is a chance that produce threads gets conditional wait if
      // max queue length is reached, with less threads, this is not a big deal

      // i still don't know why make runtime-tests works but run separately doesn't
      // work...

      // as soon as the 20 threads (10 for each) are created,
      // they start produce and consume
      //pthread_create(&producers[i], NULL, produce, NULL);
      //pthread_create(&consumers[i], NULL, consume, NULL);

      // using GDB and find always stuck at the following
      // Program received signal SIGSEGV, Segmentation fault.
      // 0x00007ffff7bc55da in pthread_join (threadid=140737328924416, thread_return=0x0)
      // at pthread_join.c:47 47      pthread_join.c: No such file or directory.

      p_id[i] = i;
      c_id[i] = i;

      printf("\t\t>>> creating produce thread [%d] ID: %lu\n", i, &producers[i]);
      rc = pthread_create(&producers[i], &attr, produce, (void *)p_id[i]);
      if (rc) {
          printf("ERROR; return code from produce thread creation is %d\n", rc);
          exit(-1);
      }

      printf("\t\t>>> creating consume thread [%d] ID: %lu\n", i, &consumers[i]);
      rc = pthread_create(&consumers[i], &attr, consume, (void *)c_id[i]);
      if (rc) {
          printf("ERROR; return code from consume thread creation is %d\n", rc);
          exit(-1);
      }
  }
  printf("Done.\n\n");

  printf("Waiting for sum answers and checking...\n");

  // this is NON-deterministic and free-style threads join
  // some issues if running from the binary directly, still mysterious to me

  // ok the possible issue is: it's a for loop but sometimes
  // the thread can finish very early based on resource allocation by
  // CPU, therefore, when join, it can't find that thread ID any further
  // thus causing the ESRCH=3 return code

  pthread_attr_destroy(&attr);
  for (i = 0; i < NUM_THREADS; i++) {
      //printf("here 1111 = %d, %d\n", i, ret_val);
      //if (producers[i])
      rc_p = pthread_join(producers[i], NULL);
      //printf("rc_p = %d", rc_p);
//      rc_p = pthread_join(producers[i], &status);
#if ON
      if (rc_p) {
          printf("ERROR return code from produce working thread [%d] join is %d\n", i, rc_p);
          exit(-1);
      }
#endif
      //if (consumers[i])
      //rc_c = pthread_join(consumers[i], ((void **)&ret_val));
      rc_c = pthread_join(consumers[i], (void **)&ret_val);
      //printf("rc_c = %d\n", rc_c);
#if ON
      if (rc_c) {
          printf("ERROR return code from consume working thread [%d] join is %d\n", i, rc_c);
          exit(-1);
      }
#endif
      //printf("here = %d, %d\n", i, ret_val);
      if (ret_val != NUMBER) {
          printf("ERROR: values mismatched: %lld\n", ret_val);
      }
  }
  printf("Done.\n\n");

#if ON
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  printf("Checking blocking functionality...\n");
  pthread_create(&consumers[0], &attr, block_consume, NULL);
  sleep(2);
  // push only 1 element
  // using this one to replace produce worker
  queue_push(q, VALUE);

  pthread_attr_destroy(&attr);
  pthread_join(consumers[0], ((void **)&ret_val));

  if (ret_val != VALUE) {
      printf("ERROR: value mismatched: %lld\n", ret_val);
  }

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  // then create a producer thread
  pthread_create(&producers[0], &attr, block_produce, NULL);
  printf("Sleeping: producer shouldn't finish during this time.\n");
  sleep(2);
  printf("Done sleeping, consuming so producer can finish...\n");
  queue_pop(q);

  pthread_attr_destroy(&attr);
  pthread_join(producers[0], NULL);

  printf("Done.\n");
#endif
  return 0;
}
