#include "../headers/router.h"
#include <stdio.h>


void disconnect(int id)
{
    r.other_routers[id].id = -1;
    r.other_routers[id].cost = -1;
    r.other_routers[id].is_neighbor = false;
    r.neighbor_list = remove_int(r.neighbor_list, id);

    for(int i = 0; i < NETWORK_SIZE; i++)
    {
        if(r.other_routers[i].connection.source == id)
        {
            r.other_routers[i].id = -1;
            r.other_routers[i].cost = -1;
            
        }
    }
}

void update_distance_vector(message msg)
{
    //
}

 
void handle_control_message(message msg)
{
    int type;
    sscanf(msg.data, "%d", &type);
    switch(type)
    {
        
        case DISCONNECT:
            printf("Disconnecting from %d\n", msg.source);
            disconnect(msg.source);
            break;
        case DISTANCE_VECTOR:
            printf("Distance vector from %d\n", msg.source);
            update_distance_vector(msg);
            break;
        default:
            die("Unknown control message");
    }
}

void handle_data_message(message msg)
{
    printf("Data from %d\n", msg.source);
    printf("Data: %s\n", msg.data);
}



void *packet_handler(void *args)
{
    while (1)
    {

        message msg = dequeue(r.in);
        printf("Packet from %d to %d\n", msg.source, msg.destiny_id);
        if (msg.destiny_id != r.id)
        {
            enqueue(r.out, msg);
            return;
        }
        switch(msg.type)
        {
            case CONTROL:
                handle_control_message(msg);
                break;
            case DATA:
                handle_data_message(msg);
                break;
            default:
                die("Unknown message type");
        }
        
    }
}
