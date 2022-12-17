#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <semaphore.h>
#include <unistd.h>

#include "queue.h"
#include "neighbor.h"

#define SLEEP_TIME 1000000
#define NO_THREADS 5

typedef struct router
{
    int id;
    int port;
    char *ip;

    queue *in;
    queue *out;
    sem_t *in_sem;
    sem_t *out_sem;

    int n_routers;
    int *distance_vector;
    int **neighbors_distance_vectors;

    int no_neighbors;
    neighbor *neighbors;

} router;

router *r;

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
    FILE *f = fopen(filename, "r");
    if (f == NULL)
    {
        printf("Error opening %s file!\n", filename);
        exit(1);
    }
    return f;
}

router *init_router(int id)
{
    router *r = malloc(sizeof(router));
    r->ip = malloc(16);

    r->in = init_queue();
    r->out = init_queue();
    r->in_sem = malloc(sizeof(sem_t));
    r->out_sem = malloc(sizeof(sem_t));

    r->n_routers = -1;
    r->no_neighbors = 0;

    r->neighbors = calloc(MAX_NEIGHBORS, sizeof(neighbor));
    for (int i = 0; i < MAX_NEIGHBORS; i++)
        r->neighbors[i].ip = malloc(16);

    FILE *f;
    neighbor n;
    n.ip = malloc(16);

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
            r->neighbors[r->no_neighbors].id = enlace.destiny;
            r->neighbors[r->no_neighbors++].cost = enlace.cost;
        }
        else if (enlace.destiny == id)
        {
            r->neighbors[r->no_neighbors].id = enlace.source;
            r->neighbors[r->no_neighbors++].cost = enlace.cost;
        }

        if (r->no_neighbors == MAX_NEIGHBORS)
            break;
    }

    fclose(f);
    f = open_file("roteador.config");
    while (
        fscanf(f, "%d %d %s", &n.id, &n.port, n.ip) != EOF)
    {
        if (n.id == id)
        {
            r->id = n.id;
            r->port = n.port;
            strcpy(r->ip, n.ip);
        }

        for (int i = 0; i < r->no_neighbors; i++)
        {
            if (r->neighbors[i].id == n.id)
            {
                r->neighbors[i].port = n.port;
                strcpy(r->neighbors[i].ip, n.ip);
            }
        }
    }

    fclose(f);

    return r;
}

void *terminal(void *args)
{

    while (1)
    {

        char input[2];
        message msg;

        puts("Send message to a neighbour:");
        for (int i = 0; i < r->no_neighbors; i++)
        {
            printf("%d - %d %s:%d\n", i, r->neighbors[i].id,
                   r->neighbors[i].ip, r->neighbors[i].port);
        }

        fgets(input, 2, stdin);
        clean_stdin();

        if (*input < '0' || *input - '0' >= r->no_neighbors)
            puts("Invalid input");
        else
        {
            msg.type = DATA;
            msg.source = r->port;
            msg.destiny = r->neighbors[*input - '0'].port;
            puts("Enter message:");
            fgets(msg.data, MSG_SIZE, stdin);
            enqueue(r->out, msg);
            sem_post(r->out_sem);
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

    if (inet_aton(r->ip, &si_other.sin_addr) == 0)
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

    while (1)
    {
        sem_wait(r->out_sem);
        message msg = dequeue(r->out);
        si_other.sin_port = htons(msg.destiny);

        if (sendto(s, &msg, sizeof(message), 0, (struct sockaddr *)&si_other, slen) == -1)
        {
            die("sendto()");
        }
        printf("Sent packet to %s:%d\n", r->ip, msg.destiny);
    }
}

void *receiver(void *args)
{

    struct sockaddr_in si_me, si_other;
    int s, i, slen = sizeof(si_other), recv_len;

    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
        die("socket");

    memset((char *)&si_me, 0, sizeof(si_me));
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(r->port);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(s, (struct sockaddr *)&si_me, sizeof(si_me)) == -1)
        die("bind");

    while (1)
    {
        message *msg = calloc(1, sizeof(message));

        if ((recv_len = recvfrom(s, msg, sizeof(message), 0, (struct sockaddr *)&si_other, &slen)) == -1)
            die("recvfrom()");

        puts("Message received :)");
        enqueue(r->in, *msg);
        sem_post(r->in_sem);
    }
}

void *packet_handler(void *args)
{
    while (1)
    {
        sem_wait(r->in_sem);
        message msg = dequeue(r->in);
        printf("Packet from %d to %d\n", msg.source, msg.destiny);
        if (msg.type == DATA)
            printf("Message: %s\n", msg.data);
        else if (msg.type == CONTROL)
        {

            printf("Control message from: %d\n", msg.source);
            int position = msg.source == r->port - 1 ? 0 : 1;

            // read msg.data and update distance vector
            int i = 0;
            char *seek = msg.data;
            while (seek != NULL)
            {
                r->neighbors_distance_vectors[position][i] = atoi(strtok(seek, " "));
                seek = NULL;
                i++;
            }
        }
    }
}

void *send_distance_vectors(void *args)
{
    r->out;

    while (1)
    {
        usleep(SLEEP_TIME);

        message msg;
        msg.type = CONTROL;
        msg.source = r->port;
        // msg.destiny = r->port - 1;
        char *data = malloc(MSG_SIZE);
        memset(data, '\0', MSG_SIZE);
        for (int i = 0; i < r->n_routers; i++)
        {
            char buffer[10];
            sprintf(buffer, "%d ", r->distance_vector[i]);
            strcat(data, buffer);
        }
        strcpy(msg.data, data);
        enqueue(r->out, msg);
        sem_post(r->out_sem);
        // msg.destiny = r->port + 1;
        enqueue(r->out, msg);
        sem_post(r->out_sem);
    }
}

int main(int argc, char const *argv[])
{

    if (argc != 2)
    {
        puts("Usage: ./router <id>");
        return 1;
    }

    r = init_router(atoi(argv[1]));

    pthread_t threads[NO_THREADS];

    void *(*func[NO_THREADS])(void *) = {
        terminal,
        sender,
        receiver,
        packet_handler,
        send_distance_vectors};

    void *args[NO_THREADS];

    for (int i = 0; i < NO_THREADS; i++)
        args[i] = NULL;

    for (int i = 0; i < NO_THREADS; i++)
        pthread_create(&threads[i], NULL, func[i], args[i]);

    for (int i = 0; i < NO_THREADS; i++)
        pthread_join(threads[i], NULL);

    return 0;
}
