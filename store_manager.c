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


int main (int argc, const char * argv[])
{
  // Check the number of arguments
  if (argc != 5) {
    printf("Usage: %s <file> <num_producers> <num_consumers> <queue_size>\n", argv[0]);
    exit(-1);
  }

  int profits = 0;
  int product_stock [5] = {0};
  int values_table[5][2] = {{2, 3}, {5, 10}, {15, 20}, {25, 40}, {100, 125}}; // Table of purchase/sell prices

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
  for (int i = 0; i < numLines; i++) {
    char word1[100], word2[100], word3[100];
    fscanf(file, "%s %s %s", word1, word2, word3);
    data[i].product_id = atoi(word1);
    if (strcmp(word2, "PURCHASE") == 0) {
      data[i].op = 0;
    } else {
      data[i].op = 1;
    }
    data[i].units = atoi(word3);
  }

  // Close the file
  fclose(file);





  // Output
  printf("Total: %d euros\n", profits);
  printf("Stock:\n");
  printf("  Product 1: %d\n", product_stock[0]);
  printf("  Product 2: %d\n", product_stock[1]);
  printf("  Product 3: %d\n", product_stock[2]);
  printf("  Product 4: %d\n", product_stock[3]);
  printf("  Product 5: %d\n", product_stock[4]);

  return 0;
}

// 
int producer(){

  return 0;
}


int consumer(){

  return 0;
}

