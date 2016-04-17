#include <stdlib.h>
#include <stdio.h>
#include "queue.h"

void queue_init(queue_t *q) {
  q->head = 0;
  q->tail = 0;
  q->size = 0;
  pthread_mutex_init(&(q->mutex), NULL);
  pthread_cond_init(&(q->empty_cvar), NULL);
  pthread_cond_init(&(q->full_cvar), NULL);
}

void queue_destroy(queue_t *q) {
  pthread_mutex_destroy(&(q->mutex));
  pthread_cond_destroy(&(q->empty_cvar));
  pthread_cond_destroy(&(q->full_cvar));
}
/* block only when full for push or empty for pop */
void queue_push(queue_t *q, unsigned long long val) {
  pthread_mutex_lock(&(q->mutex));
  while (q->size == QUEUE_MAXLEN) {
      //printf("queue_push: queue full\n");
      /* block the calling thread until the full condition is signaled */
      pthread_cond_wait(&(q->full_cvar), &(q->mutex));
  }

  q->arr[q->tail] = val;
  q->tail = (q->tail + 1) % QUEUE_MAXLEN;
  q->size += 1;
  /* should be called after mutex is locked */
  pthread_cond_signal(&(q->empty_cvar));
  pthread_mutex_unlock(&(q->mutex));
}

unsigned long long queue_pop(queue_t *q) {
  pthread_mutex_lock(&(q->mutex));
  while (q->size == 0) {
      //printf("queue_pop: queue empty\n");
      pthread_cond_wait(&(q->empty_cvar), &(q->mutex));
  }

  unsigned long long ret_val = q->arr[q->head];
  q->head = (q->head + 1) % QUEUE_MAXLEN;
  q->size -= 1;
  pthread_cond_signal(&(q->full_cvar));
  pthread_mutex_unlock(&(q->mutex));

  return ret_val;
}
