#include "../headers/router.h"

void send_distance_vectors()
{
    message msg = {
        .type = CONTROL,
        .origin = r.id,
        .destiny_id = -1,
        .sequence = 0,
    };
    char buffer[MSG_SIZE];
    msg.data[0] = '\0';
    sprintf(msg.data, "%d\n", DISTANCE_VECTOR);

    pthread_mutex_lock(&r.other_routers_lock);
    pthread_mutex_lock(&r.neighbor_list_lock);

    for (int i = 0; i < NETWORK_SIZE; i++)
    {

        sprintf(buffer, "%d %d\n", r.other_routers[i].id, r.other_routers[i].cost);
        if (strlen(msg.data) + strlen(buffer) > MSG_SIZE)
            break;

        if (r.other_routers[i].id != -1)
            strcat(msg.data, buffer);
    }

    for (int_list *iterator = r.neighbor_list; iterator; iterator = iterator->next)
    {
        msg.destiny_id = iterator->value;
        enqueue(r.out, msg);
    }

    pthread_mutex_unlock(&r.neighbor_list_lock);
    pthread_mutex_unlock(&r.other_routers_lock);
}

void *routine_distance_vector_sender(void *args)
{

    while (1)
    {
        usleep(DISTANCE_VECTOR_DELAY);
        send_distance_vectors();
    }
}

void *sender(void *args)
{

    struct sockaddr_in si_other;
    int s, i, slen = sizeof(si_other);

    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }

    memset((char *)&si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;

    while (1)
    {

        message msg = dequeue(r.out);
        if (msg.destiny_id >= NETWORK_SIZE || msg.destiny_id < 0)
            continue;

        pthread_mutex_lock(&r.other_routers_lock);
        if (r.other_routers[msg.destiny_id].id == -1 || msg.sequence == NETWORK_SIZE)
        {
            pthread_mutex_unlock(&r.other_routers_lock);
            continue;
        }

        msg.sequence++;
        msg.sender = r.id;

        int port;
        char *ip;

        int dest = msg.destiny_id;
        while (r.other_routers[dest].source != -1)
            dest = r.other_routers[dest].source;

        port = r.other_routers[dest].network_info.port;
        ip = r.other_routers[dest].network_info.ip;

        pthread_mutex_unlock(&r.other_routers_lock);

        si_other.sin_port = htons(port);

        if (inet_aton(ip, &si_other.sin_addr) == 0)
        {
            fprintf(stderr, "inet_aton() failed\n");
            exit(1);
        }

        if (sendto(s, &msg, sizeof(message), 0, (struct sockaddr *)&si_other, slen) == -1)
        {
            die("sendto()");
        }

        char buffer[MSG_SIZE];
        sprintf(buffer, "Sent packet to %s:%d\n", ip, port);
        write_to_log(buffer);
    }
}