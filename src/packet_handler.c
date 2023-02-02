#include "../headers/router.h"

void update_distance_vector(message msg)
{

    if (r.other_routers[msg.origin].id == -1)
    {

        r.other_routers[msg.origin].id = msg.origin;
        r.other_routers[msg.origin].is_neighbor = true;
        r.other_routers[msg.origin].last_update = time(NULL);

        r.neighbor_list = add_int(r.neighbor_list, msg.origin);
    }

    int id, cost;
    char *token = strtok(msg.data + 2, "\n");
    bool changed = false;
    while (token != NULL)
    {
        sscanf(token, "%d %d", &id, &cost);
        if (id == r.id)
        {
            token = strtok(NULL, "\n");
            continue;
        }
        else
            changed = true;
        if (r.other_routers[id].id == -1)
        {
            r.other_routers[id] = (other_router){
                .id = id,
                .cost = cost + r.other_routers[msg.origin].cost,
                .source = msg.origin,
                .is_neighbor = false};
        }
        else
        {
            if (r.other_routers[id].source == msg.origin)
                r.other_routers[id].cost = cost + r.other_routers[msg.origin].cost;
            else if (r.other_routers[id].cost > cost + r.other_routers[msg.origin].cost)
            {
                r.other_routers[id].cost = cost + r.other_routers[msg.origin].cost;
                r.other_routers[id].source = msg.origin;
            }
        }
        token = strtok(NULL, "\n");
    }
    if (changed)
        send_distance_vectors();
}

void handle_control_message(message msg)
{
    int type;
    char log[100];
    sscanf(msg.data, "%d", &type);
    switch (type)
    {

    case DISCONNECT:
        sprintf(log, "Disconnecting from %d", msg.origin);
        write_to_log(log);
        disconnect(msg.origin);
        break;
    case DISTANCE_VECTOR:
        sprintf(log, "Distance vector from %d", msg.origin);
        write_to_log(log);
        update_distance_vector(msg);
        break;
    default:
        die("Unknown control message");
    }
}

void handle_data_message(message msg)
{
    enqueue(r.messages, msg);
}

void *packet_handler(void *args)
{
    char log[100];
    while (1)
    {

        message msg = dequeue(r.in);
        sprintf(log, "Packet from %d to %d", msg.origin, msg.destiny_id);
        write_to_log(log);

        if (msg.destiny_id != r.id)
        {
            sprintf(log, "Forwarding packet with destiny %d", msg.destiny_id);
            write_to_log(log);
            enqueue(r.out, msg);
            continue;
        }
        switch (msg.type)
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
