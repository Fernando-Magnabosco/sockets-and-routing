#include "../headers/router.h"

void *check_neighbors(void *arg)
{
    char log[100];
    while (true)
    {
        usleep(CHECK_NEIGHBORS_DELAY);
        for (int_list *iterator = r.neighbor_list; iterator; iterator = iterator->next)
        {
            if (difftime(time(NULL), r.other_routers[iterator->value].last_update) > TIME_OUT)
            {
                disconnect(iterator->value);
                sprintf(log, "Neighbor %d disconnected\n", iterator->value);
                write_to_log(log);
            }
        }
    }
}