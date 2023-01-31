#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "headers/router.h"

#define NO_THREADS 5

router r;

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

int main(int argc, char const *argv[])
{
    atexit(append_logs);
    if (argc != 2)
    {
        puts("Usage: ./router <id>");
        return 1;
    }

    init_router(atoi(argv[1]));

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
