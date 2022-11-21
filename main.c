#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <semaphore.h>
#include <unistd.h>

#include "queue.h"

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

router *init_router(int id)
{
    router *r = malloc(sizeof(router));
    r->id = id;
    r->in = init_queue();
    r->out = init_queue();
    r->ip = malloc(16);
    r->in_sem = malloc(sizeof(sem_t));
    r->out_sem = malloc(sizeof(sem_t));

    FILE *f = fopen("roteador.config", "r");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }

    while (
        fscanf(f, "%d %d %s", &r->id, &r->port, r->ip) != EOF)
        if (r->id == id)
            break;

    fseek(f, 0, SEEK_SET);
    int noLines = 0;
    while (fgetc(f) != EOF)
        if (fgetc(f) == '\n')
            noLines++;

    r->n_routers = noLines - 1;
    r->distance_vector = calloc(noLines, sizeof(int));
    r->neighbors_distance_vectors = calloc(noLines, 2 * sizeof(int *));
    for (int i = 0; i < 2; i++)
        r->neighbors_distance_vectors[i] = calloc(noLines, sizeof(int));

    fclose(f);

    printf("Router %d: %s:%d\n", r->id, r->ip, r->port);

    return r;
}

void *terminal(void *args)
{
    queue *outqueue = r->out;

    while (1)
    {

        char input[2];
        message msg;

        puts("Send message to neighbour at the (l,r) position:");

        fgets(input, 2, stdin);
        clean_stdin();

        if (*input != 'l' && *input != 'r')
            puts("Invalid input");
        else
        {
            msg.type = DATA;
            msg.source = r->port;
            msg.destiny = *input == 'l' ? (r->port - 1) : (r->port + 1);
            puts("Enter message:");
            fgets(msg.data, MSG_SIZE, stdin);
            enqueue(outqueue, msg);
            sem_post(r->out_sem);
        }
    }
}

void *sender(void *args)
{
    queue *outqueue = r->out;
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
        message msg = dequeue(outqueue);
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
    queue *inqueue = r->in;
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
        enqueue(inqueue, *msg);
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
    queue *outqueue = r->out;

    while (1)
    {
        usleep(SLEEP_TIME);

        message msg;
        msg.type = CONTROL;
        msg.source = r->port;
        msg.destiny = r->port - 1;
        char *data = malloc(MSG_SIZE);
        memset(data, '\0', MSG_SIZE);
        for (int i = 0; i < r->n_routers; i++)
        {
            char buffer[10];
            sprintf(buffer, "%d ", r->distance_vector[i]);
            strcat(data, buffer);
        }
        strcpy(msg.data, data);
        enqueue(outqueue, msg);
        sem_post(r->out_sem);
        msg.destiny = r->port + 1;
        enqueue(outqueue, msg);
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

    void *args[NO_THREADS] = {NULL, NULL, NULL, NULL};

    for (int i = 0; i < NO_THREADS; i++)
        pthread_create(&threads[i], NULL, func[i], args[i]);

    for (int i = 0; i < NO_THREADS; i++)
        pthread_join(threads[i], NULL);

    return 0;
}
