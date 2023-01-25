#include <unistd.h>
#include "../headers/router.h"
#include "../headers/distance_vectors.h"

void *send_distance_vectors(void *args)
{

    char data[MSG_SIZE];
    message msg;
    msg.type = CONTROL;
    msg.source = r.port;

    while (1)
    {
        usleep(DISTANCE_VECTOR_DELAY);
        memset(data, '\0', MSG_SIZE);

        for (int i = 0; i < r.no_neighbors; i++)
        {
            char buffer[10];
            sprintf(buffer, "%d %d ", r.neighbors[i].id, r.neighbors[i].cost);
            strcat(data, buffer);
        }
        strcpy(msg.data, data);

        for (int i = 0; i < r.no_neighbors; i++)
        {
            msg.destiny_port = r.neighbors[i].port;
            strcpy(msg.destiny_ip, r.neighbors[i].ip);

            if (enqueue(r.out, msg) == QUEUE_FULL)
                break;
        }
    }
}