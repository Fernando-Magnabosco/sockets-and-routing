#include "../headers/router.h"

void *terminal(void *args)
{

    while (1)
    {

        char input[2];

        puts("Send message to a neighbour:");

        for (int i = 0; i < r.no_neighbors; i++)
        {
            printf("%d - %d %s:%d\n", i, r.neighbors[i].id,
                   r.neighbors[i].ip, r.neighbors[i].port);
        }
        fgets(input, 2, stdin);
        clean_stdin();

        if (*input < '0' || *input - '0' >= r.no_neighbors)
            puts("Invalid input");
        else
        {
            puts("Enter message:");

            message msg = {
                .type = DATA,
                .source = r.port,
                .destiny_port = r.neighbors[*input - '0'].port,
            };

            strcpy(msg.destiny_ip, r.neighbors[*input - '0'].ip);
            fgets(msg.data, MSG_SIZE, stdin);
            enqueue(r.out, msg);
        }
    }
}