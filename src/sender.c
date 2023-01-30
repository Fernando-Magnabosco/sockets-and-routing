#include "../headers/router.h"
#include <arpa/inet.h>

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
        msg.sequence++;

        int port;
        char *ip;

        if (r.other_routers[msg.destiny_id].is_neighbor)
        {
            port = r.other_routers[msg.destiny_id].connection.network_info.port;
            ip = r.other_routers[msg.destiny_id].connection.network_info.ip;
        }
        else
        {
            port = r.other_routers[r.other_routers[msg.destiny_id].connection.source].connection.network_info.port;
            ip = r.other_routers[r.other_routers[msg.destiny_id].connection.source].connection.network_info.ip;
        }

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