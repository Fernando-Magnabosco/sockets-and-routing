#include "../headers/router.h"

void init_router(int id)
{
    r.in = init_queue();
    r.out = init_queue();

    r.id = -1;
    r.neighbor_list = init_int_list();

    for (int i = 0; i < NETWORK_SIZE; i++)
    {
        r.other_routers[i].id = -1;
        r.other_routers[i].cost = -1;
        r.other_routers[i].is_neighbor = false;
    }

    FILE *f;
    other_router o;

    f = open_file("enlaces.config");

    struct enlace
    {
        int source;
        int destiny;
        int cost;
    } enlace;

    while (
        fscanf(f, "%d %d %d", &enlace.source, &enlace.destiny, &enlace.cost) != EOF)
    {
        int addTo = -1;

        if (enlace.source == id)
            addTo = enlace.destiny;
        else if (enlace.destiny == id)
            addTo = enlace.source;
        else
            continue;

        r.neighbor_list = add_int(r.neighbor_list, addTo);
        r.other_routers[addTo].id = addTo;
        r.other_routers[addTo].cost = enlace.cost;
        r.other_routers[addTo].is_neighbor = true;
    }

    fclose(f);
    f = open_file("roteador.config");
    while (
        fscanf(f, "%d %d %s", &o.id, &o.connection.network_info.port, o.connection.network_info.ip) != EOF)
    {
        if (o.id == id)
        {
            r.id = o.id;
            r.port = o.connection.network_info.port;
            strcpy(r.ip, o.connection.network_info.ip);
        }

        for (int_list *iterator = r.neighbor_list; iterator != NULL; iterator = iterator->next)
        {
            if (iterator->value == o.id)
            {
                r.other_routers[o.id].connection.network_info.port = o.connection.network_info.port;
                strcpy(r.other_routers[o.id].connection.network_info.ip, o.connection.network_info.ip);
            }
        }
    }
    fclose(f);
    if (r.id == -1)
        die("Router not found");

    return;
}

void clean_stdin(void)
{
    int c;
    do
    {
        c = getchar();
    } while (c != '\n' && c != EOF);
}

void die(char *s)
{
    perror(s);
    exit(1);
}

FILE *open_file(char *filename)
{

    int path_len = strlen(filename) + strlen("config/") + 1;

    char path[path_len];
    strcpy(path, "config/");
    strcat(path, filename);

    FILE *f = fopen(path, "r");
    if (f == NULL)
    {
        char *template = "Error opening %s file!\n";
        int len = strlen(template) + strlen(path) + 1;
        char msg[len];
        snprintf(msg, len, template, path);
        die(msg);
    }
    return f;
}
