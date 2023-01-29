#ifndef ROUTER_H
#define ROUTER_H

#include "queue.h"
#include "int_list.h"
#include "other_routers.h"

typedef struct router
{
    int id;
    int port;
    char ip[16];

    queue *in;
    queue *out;

    int_list *neighbor_list;
    other_router other_routers[NETWORK_SIZE];

} router;

extern router r;

void *packet_handler(void *args);
void *terminal(void *args);
void *sender(void *args);
void *receiver(void *args);
void *send_distance_vectors(void *args);
void init_router(int id);

FILE *open_file(char *filename);
void die(char *s);
void clean_stdin(void);

#endif