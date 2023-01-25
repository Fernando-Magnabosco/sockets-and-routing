#ifndef NEIGHBOR_H
#define NEIGHBOR_H

#define MAX_NEIGHBORS 16

typedef struct neighbor
{
    int id;
    int port;
    int cost;
    char ip[16];
    struct neighbor *distance_vector;

} neighbor;

#endif