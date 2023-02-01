#include "headers/router.h"
#include <sys/types.h>
#include <sys/stat.h>
#define NO_THREADS 5

router r;

int main(int argc, char const *argv[])
{
    atexit(append_logs);
    if (argc != 2)
    {
        puts("Usage: ./router <id>");
        return 1;
    }

    // if folder logs doesn't exist, create it
    struct stat st = {0};
    if (stat("logs", &st) == -1)
        mkdir("logs", 0700);

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
