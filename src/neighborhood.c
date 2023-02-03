#include "../headers/router.h"

void *check_neighbors(void *arg)
{
    char log[100];
    while (1)
    {
        usleep(CHECK_NEIGHBORS_DELAY);
        pthread_mutex_lock(&r.neighbor_list_lock);
    keep_going:
        for (int_list *iterator = r.neighbor_list; iterator; iterator = iterator->next)
        {
            pthread_mutex_lock(&r.other_routers_lock);
            if (difftime(time(NULL), r.other_routers[iterator->value].last_update) > TIME_OUT)
            {
                sprintf(log, "Neighbor %d disconnected\n", iterator->value);
                write_to_log(log);
                pthread_mutex_unlock(&r.other_routers_lock);
                pthread_mutex_unlock(&r.neighbor_list_lock);
                disconnect(iterator->value);
                pthread_mutex_lock(&r.neighbor_list_lock);
                goto keep_going;
            }
            pthread_mutex_unlock(&r.other_routers_lock);
        }
        pthread_mutex_unlock(&r.neighbor_list_lock);
    }
}