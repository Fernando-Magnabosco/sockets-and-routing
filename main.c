#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <semaphore.h>

#include "queue.h"

typedef struct router
{
    int id;
    int port;
    char *ip;
    queue *in;
    queue *out;
    sem_t *in_sem;
    sem_t *out_sem;

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

    fclose(f);

    printf("Router %d: %s:%d\n", r->id, r->ip, r->port);

    return r;
}

void *terminal(void *args)
{
    queue *outqueue = (queue *)args;

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
    queue *outqueue = (queue *)args;
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

        if (sendto(s, msg.data, MSG_SIZE, 0, (struct sockaddr *)&si_other, slen) == -1)
        {
            die("sendto()");
        }
        printf("Sent packet to %s:%d\n", r->ip, msg.destiny);
    }
}

void *receiver(void *args)
{
    queue *inqueue = (queue *)args;
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
        message msg;
        memset(msg.data, '\0', MSG_SIZE);

        if ((recv_len = recvfrom(s, msg.data, MSG_SIZE, 0, (struct sockaddr *)&si_other, &slen)) == -1)
            die("recvfrom()");

        puts("Message received :)");
        printf("Message: %s\n", msg.data);
        enqueue(inqueue, msg);
        sem_post(r->in_sem);
    }
}

void *packet_handler(void *args)
{
}

int main(int argc, char const *argv[])
{

    if (argc != 2)
    {
        puts("Usage: ./router <id>");
        return 1;
    }

    r = init_router(atoi(argv[1]));

    pthread_t threads[4];
    void *(*func[4])(void *) = {terminal, sender, receiver, packet_handler};
    void *args[4] = {r->out, r->out, r->in, NULL};

    for (int i = 0; i < 4; i++)
        pthread_create(&threads[i], NULL, func[i], args[i]);

    for (int i = 0; i < 4; i++)
        pthread_join(threads[i], NULL);

    return 0;
}
