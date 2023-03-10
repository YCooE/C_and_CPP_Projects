#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "job_queue.h"

int job_queue_init(struct job_queue *job_queue, int capacity) {
  job_queue->capacity = capacity;
  job_queue->first = 0;
  job_queue->num_used = 0;
  job_queue->buffer = malloc(capacity * sizeof(void*));
  job_queue->dead = 0;

  pthread_mutex_init(&job_queue->mutex, NULL);
  pthread_cond_init(&job_queue->cond, NULL);

  return 0;
}

int job_queue_destroy(struct job_queue *job_queue) {
  pthread_mutex_lock(&job_queue->mutex);

  // The return value
  int retval = 0;

  // Wait until num_used == 0
  while(job_queue->num_used != 0) {
    pthread_cond_wait(&job_queue->cond, &job_queue->mutex);
  }

  job_queue->dead = 1;

  pthread_cond_broadcast(&job_queue->cond);
  free(job_queue->buffer);
  pthread_mutex_unlock(&job_queue->mutex);
  return retval;
}

int job_queue_push(struct job_queue *job_queue, void *data) {
  pthread_mutex_lock(&job_queue->mutex);

  // The return value
  int retval = 0;

  // If capacity is full, wait until release
  while (job_queue->capacity == job_queue->num_used) {
    if (job_queue->dead == 1) {
      retval = 1;
      break;
    }
    // Release mutex
    pthread_cond_wait(&job_queue->cond, &job_queue->mutex);
  }

  // Add data to job queue
  if (retval == 0) {
    job_queue->buffer[((job_queue->num_used + job_queue->first) % job_queue->capacity)] = data;
    job_queue->num_used += 1;
  }

  // Broadcast and unlock
  pthread_cond_broadcast(&job_queue->cond);
  pthread_mutex_unlock(&job_queue->mutex);
  return retval;
}

int job_queue_pop(struct job_queue *job_queue, void **data) {
  pthread_mutex_lock(&job_queue->mutex);

  // the return value
  int retval = 0;

  // If queue is empty
  while(job_queue->num_used == 0){
    if (job_queue->dead == 1) {
      retval = 1;
      break;
    }
    // Releases mutex
    pthread_cond_wait(&job_queue->cond, &job_queue->mutex);
  }

  if (retval == 0) {
    *data = job_queue->buffer[job_queue->first];
    job_queue->num_used -= 1;
    if ((job_queue->first + 1) >= job_queue->capacity) {
      job_queue->first = 0;
    }
    else {
      job_queue->first = job_queue->first + 1;
    }
  }

  pthread_cond_broadcast(&job_queue->cond);
  pthread_mutex_unlock(&job_queue->mutex);
  return retval;
}
