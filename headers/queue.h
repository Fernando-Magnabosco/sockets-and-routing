#ifndef QUEUE_H
#define QUEUE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "message.h"
#include <semaphore.h>

#define BUFFER_SIZE 512

typedef struct queue
{
    message *head;
    message *tail;
    message buffer[BUFFER_SIZE];
    sem_t *sem;

} queue;

typedef enum queue_status
{
    QUEUE_OK,
    QUEUE_FULL,
    QUEUE_EMPTY
} queue_status;

queue *init_queue();
queue_status enqueue(queue *q, message msg);
message dequeue(queue *q);

#endif