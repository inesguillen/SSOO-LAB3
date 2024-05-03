//SSOO-P3 23/24

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "queue.h"
#include <semaphore.h>

//To create a queue
queue* queue_init(int size)
{

  queue * q = (queue *)malloc(sizeof(queue));
  if (q == NULL)
  {
    perror("Could not assign memory for queue");
    exit(-1);
  }
  q->start = size; // start at the last available position
  q->end = size; // end at the last available position
  q->size = size;
  q->elements = 0;
  q->buffer = (struct element *)malloc(size * sizeof(struct element)); // Allocate memory for the buffer
  if (q->buffer == NULL) // Check buffer memory was allocated correctly
  {
    perror("Could not assign memory for queue buffer");
    exit(-1);
  }
  if (sem_init(&(q->write), 0, 1) != 0) //  Create semaphore and check it was created properly
  {
    perror("Could not create the semaphore");
    exit(-1);
  }

  return q;
}

// To Enqueue an element
int queue_put(queue *q, struct element* x)
{

  if (queue_full(q)) // Check if there is still space in the queue
  {
    perror("Queue is full");
    return -1;
  }
  
  // Store element
  struct element *putelem = &(q->buffer[q->end]);
  putelem->op = x->op;
  putelem->product_id = x->product_id;
  putelem->units = x->units;

  //Move pointer
  q->end = (q->end + 1) % q->size;

  // Increase the number of elements in the queue
  q->elements = q->elements + 1;

  return 0;
}

// To Dequeue an element.
struct element* queue_get(queue *q)
{

  // Check if the queue is empty
  if (queue_empty(q))
  {
    perror("Queue is empty");
    exit(-1);
  }

  struct element *element = malloc(sizeof(struct element));
  if (element == NULL) {
    perror("Could not assign memory for element in queue_get");
    exit(-1);
  }
  struct element getelem = q->buffer[q->end];
  element->op = getelem.op;
  element->product_id = getelem.product_id;
  element->units = getelem.units;

  //Move pointer
  q->end = (q->end + 1) % q->size;
  if (q->end < 0) // Check if we are at the end of the queue
  {
    q->end = q->size - 1; // set to the end of the circular queue
  }

  // Decrease the number of elements in the queue
  q->elements = q->elements - 1;
  if (q->elements < 0) // End of queue, set to 19
  q->elements = q->size -1;
  
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
  if (sem_destroy(&(q->write)) != 0)
  {
    perror("Could not destroy semaphore");
    exit(-1);
  }
  free(q);
  return 0;
}
