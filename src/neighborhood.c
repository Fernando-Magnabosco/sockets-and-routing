#include "../headers/router.h"

void *check_neighbors(void *arg)
{
    char log[100];
    while (1)
    {
        usleep(CHECK_NEIGHBORS_DELAY);
    keep_going:
        pthread_mutex_lock(&r.other_routers_lock);
        pthread_mutex_lock(&r.neighbor_list_lock);
        for (int_list *iterator = r.neighbor_list; iterator; iterator = iterator->next)
        {
            if (difftime(time(NULL), r.other_routers[iterator->value].last_update) > TIME_OUT)
            {
                sprintf(log, "Neighbor %d disconnected\n", iterator->value);
                write_to_log(log);
                pthread_mutex_unlock(&r.neighbor_list_lock);
                pthread_mutex_unlock(&r.other_routers_lock);
                disconnect(iterator->value);
                goto keep_going;
            }
                }
        pthread_mutex_unlock(&r.neighbor_list_lock);
        pthread_mutex_unlock(&r.other_routers_lock);
    }
}