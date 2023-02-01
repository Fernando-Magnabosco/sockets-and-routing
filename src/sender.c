#include "../headers/router.h"

#define DISTANCE_VECTOR_DELAY 1000000

void *send_distance_vectors(void *args)
{
    message msg = {
        .type = CONTROL,
        .source = r.id,
        .destiny_id = -1,
        .sequence = 0,
    };

    char buffer[MSG_SIZE];

    while (1)
    {
        usleep(DISTANCE_VECTOR_DELAY);

        msg.data[0] = '\0';
        sprintf(msg.data, "%d\n", DISTANCE_VECTOR);
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

        msg.sequence++;

        int port;
        char *ip;

        int dest = msg.destiny_id;
        while (r.other_routers[dest].source != -1)
            dest = r.other_routers[dest].source;

        port = r.other_routers[dest].network_info.port;
        ip = r.other_routers[dest].network_info.ip;

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
        // printf("Sent packet to %s:%d\n", msg.destiny_ip, msg.destiny_port);
    }
}