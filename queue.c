//SSOO-P3 23/24

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "queue.h"
#include <pthread.h>

pthread_mutex_t writing_mutex;
pthread_cond_t writing_cond;
pthread_mutex_t reading_mutex;
pthread_cond_t reading_cond;

//To create a queue
queue* queue_init(int size)
{

  queue * q = (queue *)malloc(sizeof(queue));
  if (q == NULL)
  {
    perror("Could not assign memory for queue");
    exit(-1);
  }
  q->in = 0; // Start at the first available position
  q->out = 0; // End at the first available position
  q->size = size; // Max number of elements
  q->elements = 0;
  q->buffer = (struct element *)malloc(size * sizeof(struct element)); // Allocate memory for the buffer

  pthread_mutex_init(&writing_mutex, NULL); // Initialize writing mutex
  pthread_cond_init(&writing_cond, NULL); // Initialize writing condition variable
  pthread_mutex_init(&reading_mutex, NULL); // Initialize reading mutex
  pthread_cond_init(&reading_cond, NULL); // Initialize reading condition variable

  return q;
}

// To Enqueue an element
int queue_put(queue *q, struct element* x)
{

  while (queue_full(q)) // Check if there is still space in the queue
  {
    pthread_cond_wait(&writing_cond, &writing_mutex); // Wait until there is space in the queue
  }
  
  pthread_mutex_lock(&writing_mutex); // Lock the mutex to write to the queue
  int position = (q->in); // Get the position of the element to be put
  q->in = (q->in + 1) % q->size; // Move pointer so the next producer thread can insert correctly
  q->elements = q->elements + 1;  // Increase the number of elements in the queue
  pthread_mutex_unlock(&writing_mutex); // Unlock the mutex

  // Store element
  q->buffer[position] = *x;

  pthread_cond_signal(&reading_cond); // Signal that there is an element in the queue
  return 0;
}

// To Dequeue an element.
struct element* queue_get(queue *q)
{

  // Check if the queue is empty
  while (queue_empty(q))
  {
    pthread_cond_wait(&reading_cond, &reading_mutex); // Wait until there is an element in the queue
  }
  
  pthread_mutex_lock(&reading_mutex); // Lock the mutex to read from the queue
  int position = (q->out); // Get the position of the element to be consumed
  q->out = (q->out + 1) % q->size; // Move pointer so the next consumer thread can consume the next element
  q->elements = q->elements - 1; // Decrease the number of elements in the queue
  pthread_mutex_unlock(&reading_mutex); // Unlock the mutex

  // Get element from the queue
  struct element * element = malloc(sizeof(struct element));
  if (element == NULL) {
    perror("Could not assign memory for element in queue_get");
    exit(-1);
  }
  struct element * getelem = &(q->buffer[position]);
  element->op = getelem->op;
  element->product_id = getelem->product_id;
  element->units = getelem->units;

  pthread_cond_signal(&writing_cond); // Signal that there is space in the queue
  
  return element;
}

//To check queue state
int queue_empty(queue *q)
{
  if (q->elements == 0)
  return 1; // Queue is empty
  
  return 0; // Queue is not empty
}

int queue_full(queue *q)
{
  if (q->elements == q->size)
  return 1; // Queue is full

  return 0; // Queue is not full
}

//To destroy the queue and free the resources
int queue_destroy(queue *q)
{
  free(q->buffer);
  free(q);
  return 0;
}
