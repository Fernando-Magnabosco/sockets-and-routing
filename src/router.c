#include "../headers/router.h"

void init_router(int id)
{
    r.in = init_queue();
    r.out = init_queue();

    r.id = -1;
    r.no_neighbors = 0;

    r.neighbors = calloc(MAX_NEIGHBORS, sizeof(neighbor));
    for (int i = 0; i < MAX_NEIGHBORS; i++)
    {
        r.neighbors[i].distance_vector = malloc(MAX_NEIGHBORS * sizeof(neighbor));
        memset(r.neighbors[i].distance_vector, -1, MAX_NEIGHBORS * sizeof(neighbor));
    }

    FILE *f;
    neighbor n;

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
        if (enlace.source == id)
        {
            r.neighbors[r.no_neighbors].id = enlace.destiny;
            r.neighbors[r.no_neighbors++].cost = enlace.cost;
        }
        else if (enlace.destiny == id)
        {
            r.neighbors[r.no_neighbors].id = enlace.source;
            r.neighbors[r.no_neighbors++].cost = enlace.cost;
        }

        if (r.no_neighbors == MAX_NEIGHBORS)
            break;
    }

    fclose(f);
    f = open_file("roteador.config");
    while (
        fscanf(f, "%d %d %s", &n.id, &n.port, n.ip) != EOF)
    {
        if (n.id == id)
        {
            r.id = n.id;
            r.port = n.port;
            strcpy(r.ip, n.ip);
        }

        for (int i = 0; i < r.no_neighbors; i++)
        {
            if (r.neighbors[i].id == n.id)
            {
                r.neighbors[i].port = n.port;
                strcpy(r.neighbors[i].ip, n.ip);
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
        printf("Error opening %s file!\n", path);
        exit(1);
    }
    return f;
}
