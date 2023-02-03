#ifndef ROUTER_H
#define ROUTER_H

#define DISTANCE_VECTOR_DELAY 1000000                // distance vector delay in microseconds
#define TIME_OUT DISTANCE_VECTOR_DELAY * 5 / 1000000 // timeout in seconds
#define CHECK_NEIGHBORS_DELAY 10000000               // check neighbors delay in microseconds

#define NETWORK_SIZE 16
#define NETWORK_DIAMETER NETWORK_SIZE * 10

#include "queue.h"
#include "int_list.h"
#include "logs.h"
#include "other_routers.h"

#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>

typedef struct router
{
    int id;
    int port;
    char ip[16];

    queue *in;
    queue *out;

    queue *messages;

    logs log;

    pthread_mutex_t neighbor_list_lock;
    pthread_mutex_t other_routers_lock;

    int_list *neighbor_list;
    other_router other_routers[NETWORK_SIZE];

} router;

extern router r;

void *packet_handler(void *args);
void *terminal(void *args);
void *sender(void *args);
void *receiver(void *args);
void *routine_distance_vector_sender(void *args);
void send_distance_vectors();
void *check_neighbors(void *args);

void disconnect(int id);
void init_router(int id);

FILE *open_file(char *folder, char *filename, char *mode);
void write_to_log(char *s);
void append_logs();
void die(char *s);
void clean_stdin(void);

#endif