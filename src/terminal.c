#include "../headers/router.h"

void *terminal(void *args)
{

    while (1)
    {

        char input[2];
        int enumerator = 0;

        puts("Send message to a neighbour:");
        for (int_list *iterator = r.neighbor_list; iterator; iterator = iterator->next)
        {
            printf("%d - %d %s:%d\n", enumerator++,
                   iterator->value,
                   r.other_routers[iterator->value].connection.network_info.ip,
                   r.other_routers[iterator->value].connection.network_info.port);
        }

        fgets(input, 2, stdin);
        clean_stdin();

        if (*input < '0' || *input - '0' >= NETWORK_SIZE)
            puts("Invalid input");
        else
        {
            puts("Enter message:");

            message msg = {
                .type = DATA,
                .source = r.port,
                .destiny_id = *input - '0',
                .sequence = 0,
            };

            fgets(msg.data, MSG_SIZE, stdin);
            enqueue(r.out, msg);
        }
    }
}