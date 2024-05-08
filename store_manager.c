//SSOO-P3 23/24

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stddef.h>
#include <sys/stat.h>
#include <pthread.h>
#include "queue.h"
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>


// Global variables
int profits = 0;
int product_stock[5] = {0};
int values_table[5][2] = {{2, 3}, {5, 10}, {15, 20}, {25, 40}, {100, 125}}; // Table of purchase/sell prices

struct producer_data {
  int num_tasks;
  queue *q;
  struct element *data;
};

void *producer(void *arg)
{
  struct producer_data *data = (struct producer_data *)arg;
  int num_tasks = data->num_tasks;
  queue *q = data->q;

  for (int i = 0; i < num_tasks; i++)
  {
    queue_put(q, &(data->data[i]));
  }

  pthread_exit(NULL);

}


void *consumer(void *arg)
{

  queue *q = (queue *)arg;
  
  
  while (1)
  {
    struct element *elem = queue_get(q);
    
    if (elem->op == -1) // Identify fake element that signals the thread to finish
    {
      break;
    }

    int * curr_stock = &(product_stock[elem->product_id - 1]);
    int amount = (elem->units*values_table[elem->product_id - 1][elem->op]);
    // Purchase
    if (elem->op == 0)
    {
      purchase(&profits, curr_stock, amount, elem->units);
    }

    // Sell
    else
    {
     sale(&profits, curr_stock, amount, elem->units); 
    }
  }

  pthread_exit(NULL);
}


int main (int argc, const char * argv[])
{
  // Check the number of arguments
  if (argc != 5)
  {
    printf("Usage: %s <file> <num_producers> <num_consumers> <queue_size>\n", argv[0]);
    exit(-1);
  }

  // Open the file
  FILE *file = fopen(argv[1], "r");
  if (file == NULL)
  {
    perror("Failed to open the file.");
    exit(-1);
  }

  // Read the number of lines and allocate memory accordingly
  int numLines;
  fscanf(file, "%d", &numLines);
  struct element *data = (struct element *)malloc(numLines * sizeof(struct element));

  // Load the data into the struct
  for (int i = 0; i < numLines; i++)
  {
    char word1[100], word2[100], word3[100];
    fscanf(file, "%s %s %s", word1, word2, word3);
    data[i].product_id = atoi(word1);
    if (strcmp(word2, "PURCHASE") == 0)
    {
      data[i].op = 0;
    }
    else
    {
      data[i].op = 1;
    }
    data[i].units = atoi(word3);
  }

  // Close the file
  fclose(file);

  // Check the queue size
  if (atoi(argv[4]) <= 0)
  {
    printf("The queue size must be greater than 0.\n");
    exit(-1);
  }
  // Create the queue
  queue *q = queue_init(atoi(argv[4]));

  // Get number of producers and consumers
  int num_producers = atoi(argv[2]);
  int num_consumers = atoi(argv[3]);

  // Check the number of producers and consumers
  if (num_producers <= 0 || num_consumers <= 0)
  {
    printf("The number of producers and consumers must be greater than 0.\n");
    exit(-1);
  }

  // Do not create unnecessary threads
  if (num_producers > numLines)
  {
    num_producers = numLines;
  }
  if (num_consumers > numLines)
  {
    num_consumers = numLines;
  }

  // Distribute the tasks among the producers
  int tasks_per_producer[num_producers];
  for (int i = 0; i < num_producers; i++)
  {
    tasks_per_producer[i] = numLines / num_producers;
  }
  if (numLines % num_producers != 0)
  {
    for (int i = 0; i < numLines % num_producers; i++)
    {
      tasks_per_producer[i] += 1;
    }
  }

// Create producer threads
pthread_t producer_threads[num_producers];
struct producer_data producer_args[num_producers];

int index = 0;
for (int i = 0; i < num_producers; i++)
{
  producer_args[i].q = q;
  producer_args[i].num_tasks = tasks_per_producer[i];
  producer_args[i].data = &(data[index]);

  index += tasks_per_producer[i];

  pthread_create(&producer_threads[i], NULL, producer, (void *)&producer_args[i]);
}

// Create consumer threads
pthread_t consumer_threads[num_consumers];
for (int i = 0; i < num_consumers; i++)
{
  pthread_create(&consumer_threads[i], NULL, consumer, (void *)q);
}

// Wait for all producer threads to finish
for (int i = 0; i < num_producers; i++)
{
  pthread_join(producer_threads[i], NULL);
}

while (queue_empty(q) == 0) // Wait for consumers to finish (the queue will end up empty)
{
  continue;
}

// Insert fake elements (as much as consumer threads) to signal the consumers to finish
struct element fake_elem[num_consumers];
for (int i = 0; i < num_consumers; i++)
{
  fake_elem[i].op = -1;
}
pthread_t waking_thread;
struct producer_data waking_args;
waking_args.q = q;
waking_args.num_tasks = num_consumers;
waking_args.data = fake_elem;
pthread_create(&waking_thread, NULL, producer, (void *)&waking_args);


// Wait for all consumer threads to finish
for (int i = 0; i < num_consumers; i++)
{
  pthread_join(consumer_threads[i], NULL);
}
pthread_join(waking_thread, NULL);

  // Output
  printf("Total: %d euros\n", profits);
  printf("Stock:\n");
  printf("  Product 1: %d\n", product_stock[0]);
  printf("  Product 2: %d\n", product_stock[1]);
  printf("  Product 3: %d\n", product_stock[2]);
  printf("  Product 4: %d\n", product_stock[3]);
  printf("  Product 5: %d\n", product_stock[4]);

  // Free the memory
  free(data);
  queue_destroy(q);

  return 0;
}

