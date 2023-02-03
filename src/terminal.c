#include "../headers/router.h"

#define NO_OPTIONS 7

void list_neighbors()
{
    pthread_mutex_lock(&r.other_routers_lock);
    pthread_mutex_lock(&r.neighbor_list_lock);
    for (int_list *iterator = r.neighbor_list; iterator; iterator = iterator->next)
    {
        printf("%d %s:%d\n",
               iterator->value,
               r.other_routers[iterator->value].network_info.ip,
               r.other_routers[iterator->value].network_info.port);
    }
    pthread_mutex_unlock(&r.neighbor_list_lock);
    pthread_mutex_unlock(&r.other_routers_lock);
}

void list_reachable()
{
    pthread_mutex_lock(&r.other_routers_lock);
    for (int i = 0; i < NETWORK_SIZE; i++)
    {
        if (r.other_routers[i].id != -1)
            if (r.other_routers[i].source == -1)
                printf(
                    "id %d with cost %d, it's a neighbor\n",
                    r.other_routers[i].id,
                    r.other_routers[i].cost);
            else
                printf(
                    "id %d with cost %d, source %d\n",
                    r.other_routers[i].id,
                    r.other_routers[i].cost,
                    r.other_routers[i].source);
    }
    pthread_mutex_unlock(&r.other_routers_lock);
}

void list_routing_table()
{
    pthread_mutex_lock(&r.other_routers_lock);
    pthread_mutex_lock(&r.neighbor_list_lock);
    printf("%d %d", -1, r.id);
    for (int i = 0; i < 16; i++)
        printf("\t%d", i);
    putchar('\n');
    for (int_list *iterator = r.neighbor_list; iterator != NULL; iterator = iterator->next)
    {
        printf("%d %d", iterator->value, r.other_routers[iterator->value].cost);
        for (int i = 0; i < 16; i++)
            printf("\t%d", r.other_routers[iterator->value].distance_vector[i]);
        putchar('\n');
    }
    pthread_mutex_unlock(&r.neighbor_list_lock);
    pthread_mutex_unlock(&r.other_routers_lock);
}

void list_messages()
{
    print_queue(r.messages);
}

void send_message()
{
    puts("To whom?");
    list_reachable();

    char input[2];
    fgets(input, 2, stdin);
    clean_stdin();

    message msg = {
        .type = DATA,
        .destiny_id = *input - '0',
        .origin = r.id,
        .sequence = 0,
    };

    puts("Enter message:");

    fgets(msg.data, MSG_SIZE, stdin);
    clean_stdin();

    puts("message sent :)");
    write_to_log("message sent :) \n");
    enqueue(r.out, msg);
}

void show_logs()
{
    pthread_mutex_lock(&r.log.lock);
    if (r.log.size == 0)
        puts("No logs to show :(");
    for (int i = 0; i < r.log.size; i++)
        printf("%c", r.log.log[i]);
    pthread_mutex_unlock(&r.log.lock);
}

void exit_router()
{
    exit(0);
}

void menu()
{
    printf("\n");
    char *options[NO_OPTIONS] = {"List neighbors",
                                 "List reachable",
                                 "List routing table",
                                 "Show messages",
                                 "Send message",
                                 "Show logs",
                                 "Exit"};

    void (*functions[NO_OPTIONS])(void) = {list_neighbors, list_reachable, list_routing_table, list_messages, send_message, show_logs, exit_router};

    for (int i = 0; i < NO_OPTIONS; i++)
        printf("%d - %s\n", i, options[i]);

    printf(": ");
    char input[2];
    fgets(input, 2, stdin);
    clean_stdin();

    if (*input < '0' || *input - '0' >= NO_OPTIONS)
        puts("Invalid input");
    else
        functions[*input - '0']();
}

void *terminal(void *args)
{

    while (1)
        menu();
}