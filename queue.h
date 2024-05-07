//SSOO-P3 23/24

#ifndef HEADER_FILE
#define HEADER_FILE


struct element {
  int product_id; //Product identifier
  int op;         //Operation
  int units;      //Product units
};

typedef struct queue {
  // Define the struct yourself
  int size; // Size of the queue (max number of elements)
  int in; // Position (number) of the start of the queue (empty space for the next elem in)
  int out; // Position (number) of the end of the queue (next elem out)
  int elements; // Number of elements in the queue
  struct element * buffer; // Array of elements
}queue;

queue* queue_init (int size);
int queue_destroy (queue *q);
int queue_put (queue *q, struct element* elem);
struct element * queue_get(queue *q);
int queue_empty (queue *q);
int queue_full(queue *q);
int purchase(int *total, int *stock, int amount, int units);
int sale(int *total, int *stock, int amount, int units);

#endif
