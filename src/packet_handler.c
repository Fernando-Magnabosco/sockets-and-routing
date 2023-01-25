#include "../headers/router.h"
#include <stdio.h>

void *packet_handler(void *args)
{
    while (1)
    {

        message msg = dequeue(r.in);
        printf("Packet from %d to %d\n", msg.source, msg.destiny_port);
        if (msg.type == DATA)
            printf("Message: %s\n", msg.data);
        else if (msg.type == CONTROL)
        {
            printf("Control message from: %d\n", msg.source);

            int id, cost;
            char *seek = msg.data;

            while (sscanf(seek, "%d %d ", &id, &cost) != EOF)
            {
            }
        }
    }
}