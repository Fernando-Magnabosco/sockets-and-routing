#ifndef OTHER_ROUTERS_H
#define OTHER_ROUTERS_H
#include <stdbool.h>

#define NETWORK_SIZE 16

typedef struct other_router
{
    int id;
    int cost;
    union
    {
        struct
        {
            int port;
            char ip[16];
        } network_info;
        int source;
    } connection;
    bool is_neighbor;
} other_router;

#endif