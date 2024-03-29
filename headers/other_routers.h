#ifndef OTHER_ROUTERS_H
#define OTHER_ROUTERS_H
#include <stdbool.h>
#include <time.h>

typedef struct other_router
{
    int id;
    int cost;
    struct
    {
        int port;
        char ip[16];
    } network_info;
    int source;
    int *distance_vector;
    time_t last_update;

    bool is_neighbor;
} other_router;

#endif