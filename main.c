#include "headers/router.h"

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
