#include "../headers/router.h"

void disconnect(int id)
{
    r.other_routers[id].id = -1;
    r.other_routers[id].cost = -1;
    r.other_routers[id].is_neighbor = false;
    r.neighbor_list = remove_int(r.neighbor_list, id);

    for(int i = 0; i < NETWORK_SIZE; i++)
    {
        if (r.other_routers[i].source == id)
        {
            r.other_routers[i].id = -1;
            r.other_routers[i].cost = -1;
            
        }
    }
}

void update_distance_vector(message msg)
{

    int id, cost;

    char *token = strtok(msg.data + 2, "\n");
    while (token != NULL)
    {

        sscanf(token, "%d %d", &id, &cost);
        if (r.other_routers[id].id == -1)
            r.other_routers[id] = (other_router){
                .id = id,
                .cost = cost,
                .source = msg.source,
                .is_neighbor = false};
        else
        {
            if (r.other_routers[id].source == msg.source)
                r.other_routers[id].cost = cost;
            else if (r.other_routers[id].cost > cost + r.other_routers[msg.source].cost)
            {
                r.other_routers[id].cost = cost + r.other_routers[msg.source].cost;
                r.other_routers[id].source = msg.source;
            }
        }
        token = strtok(NULL, "\n");
    }
}

 
void handle_control_message(message msg)
{
    int type;
    char log[100];
    sscanf(msg.data, "%d", &type);
    switch(type)
    {
        
        case DISCONNECT:
        sprintf(log, "Disconnecting from %d", msg.source);
        write_to_log(log);
        disconnect(msg.source);
        break;
        case DISTANCE_VECTOR:
        sprintf(log, "Distance vector from %d", msg.source);
        write_to_log(log);
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
        char *log = malloc(sizeof(char) * 100);
        sprintf(log, "Packet from %d to %d", msg.source, msg.destiny_id);
        write_to_log(log);

        if (msg.destiny_id != r.id)
        {
            enqueue(r.out, msg);
            continue;
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
