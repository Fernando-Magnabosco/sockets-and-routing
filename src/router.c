#include "../headers/router.h"

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

FILE *open_file(char *folder, char *filename, char *mode)
{

    int path_len = strlen(folder) + strlen(filename) + 1;

    char path[path_len];
    strcpy(path, folder);
    strcat(path, filename);

    FILE *f = fopen(path, mode);
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

void write_to_log(char *s)
{
    pthread_mutex_lock(&r.log.lock);
    if (r.log.size + strlen(s) + 1 > LOG_SIZE)
    {
        append_logs();
        strcpy(r.log.log, "");
        r.log.size = 0;
    }

    strcat(r.log.log, s);
    strcat(r.log.log, "\n");
    r.log.size += strlen(s);
    pthread_mutex_unlock(&r.log.lock);
}

void append_logs()
{
    if (r.id == -1 || r.log.size == 0)
        return;

    char filename[50] = "router%d.log";
    sprintf(filename, filename, r.id);
    FILE *f = open_file("logs/", filename, "a");

    if (f == NULL)
        write_to_log("Log file not found, creating new one\n");
    else
        fwrite(r.log.log, r.log.size, 1, f);
}

void load_logs()
{

    pthread_mutex_lock(&r.log.lock);
    char filename[50] = "router%d.log";
    sprintf(filename, filename, r.id);
    FILE *f = open_file("logs/", filename, "w+");

    if (f == NULL)
        write_to_log("Log file not found, creating new one\n");
    else

    {
        fseek(f, -LOG_SIZE, SEEK_END);
        fread(r.log.log, LOG_SIZE, 1, f);
        r.log.size = strlen(r.log.log);
    }
    pthread_mutex_unlock(&r.log.lock);
}

void init_router(int id)
{
    r.in = init_queue();
    r.out = init_queue();

    r.id = -1;
    r.neighbor_list = init_int_list();

    for (int i = 0; i < NETWORK_SIZE; i++)
    {
        r.other_routers[i] = (other_router){
            .id = -1,
            .cost = -1,
            .source = -1,
            .is_neighbor = false,
        };
    }

    FILE *f;
    other_router o;

    f = open_file("config/", "enlaces.config", "r");

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
    f = open_file("config/", "roteador.config", "r");
    while (
        fscanf(f, "%d %d %s", &o.id, &o.network_info.port, o.network_info.ip) != EOF)
    {
        if (o.id == id)
        {
            r.id = o.id;
            r.port = o.network_info.port;
            strcpy(r.ip, o.network_info.ip);
        }

        if (in(r.neighbor_list, o.id))
        {
            r.other_routers[o.id].network_info.port = o.network_info.port;
            strcpy(r.other_routers[o.id].network_info.ip, o.network_info.ip);
        }
    }
    fclose(f);
    if (r.id == -1)
        die("Router not found");

    pthread_mutex_init(&r.neighbor_list_lock, NULL);
    pthread_mutex_init(&r.log.lock, NULL);
    load_logs();

    return;
}
