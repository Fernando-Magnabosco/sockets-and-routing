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
        si_other.sin_port = htons(msg.destiny_port);

        if (inet_aton(msg.destiny_ip, &si_other.sin_addr) == 0)
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