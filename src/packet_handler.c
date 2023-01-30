#include "../headers/router.h"
#include <stdio.h>

void *packet_handler(void *args)
{
    while (1)
    {

        message msg = dequeue(r.in);
        printf("Packet from %d to %d\n", msg.source, msg.destiny_id);
        if (msg.destiny_id != r.id)
            enqueue(r.out, msg);
        else if (msg.type == CONTROL)
        {
        }
        else if (msg.type == DATA)
        {
            printf("Message from %d: %s", msg.source, msg.data);
        }
    }
}
