#include "../headers/router.h"

void update_distance_vector(message msg)
{
    pthread_mutex_lock(&r.other_routers_lock);
    if (r.other_routers[msg.sender].id == -1)
    {
        r.other_routers[msg.sender].id = msg.sender;

        r.other_routers[msg.sender].last_update = time(NULL);
        r.neighbor_list = add_int(r.neighbor_list, msg.sender);
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

        if(r.other_routers[msg.sender].is_neighbor)
            r.other_routers[msg.sender].distance_vector[id] = cost;

        if (r.other_routers[id].id == -1)
        {
            r.other_routers[id].id = id;
            r.other_routers[id].cost = cost + r.other_routers[msg.sender].cost;
            r.other_routers[id].source = msg.sender;

            changed = true;
        }
        else
        {
            if (r.other_routers[id].source == msg.sender &&
                r.other_routers[id].cost != cost + r.other_routers[msg.sender].cost)
            {
                if (cost + r.other_routers[msg.sender].cost > NETWORK_DIAMETER)
                {
                    // solve count-to-infinity problem
                    r.other_routers[id].cost = NETWORK_DIAMETER;
                    token = strtok(NULL, "\n");
                    continue;
                }

                r.other_routers[id].cost = cost + r.other_routers[msg.sender].cost;
                changed = true;
            }
            else if (r.other_routers[id].cost > cost + r.other_routers[msg.sender].cost)
            {
                r.other_routers[id].cost = cost + r.other_routers[msg.sender].cost;
                r.other_routers[id].source = msg.sender;
                changed = true;
            }
        }
        token = strtok(NULL, "\n");
    }
    pthread_mutex_unlock(&r.other_routers_lock);

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
