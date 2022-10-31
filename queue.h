#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "message.h"

#define BUFFER_SIZE 256

typedef struct queue
{
    message *head;
    message *tail;
    message buffer[BUFFER_SIZE];

} queue;

queue *init_queue();
void enqueue(queue *q, message msg);
message dequeue(queue *q);