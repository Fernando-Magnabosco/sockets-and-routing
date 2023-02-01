#include "../headers/int_list.h"
#include "../headers/router.h"

int_list *init_int_list()
{
    int_list *list = malloc(sizeof(int_list));
    list->value = -1;
    list->next = NULL;
    return list;
}

int_list *add_int(int_list *list, int value)
{
    pthread_mutex_lock(&r.neighbor_list_lock);
    if (list == NULL)
    {
        int_list *newList = init_int_list();
        newList->value = value;
        pthread_mutex_unlock(&r.neighbor_list_lock);
        return newList;
    }

    if (list->value == -1)
    {
        list->value = value;
        pthread_mutex_unlock(&r.neighbor_list_lock);
        return list;
    }

    else
    {
        int_list *new = malloc(sizeof(int_list));
        new->value = value;
        new->next = list;
        pthread_mutex_unlock(&r.neighbor_list_lock);
        return new;
    }
}

int_list *remove_int(int_list *list, int value)
{
    if (list == NULL)
        return NULL;

    pthread_mutex_lock(&r.neighbor_list_lock);
    if (list->value == value)
    {
        int_list *next = list->next;
        free(list);
        pthread_mutex_unlock(&r.neighbor_list_lock);
        return next;
    }

    int_list *current = list;
    while (current->next)
    {
        if (current->next->value == value)
        {
            int_list *next = current->next->next;
            free(current->next);
            current->next = next;
            return list;
        }
        current = current->next;
    }
    pthread_mutex_unlock(&r.neighbor_list_lock);
    return list;
}

int in(int_list *list, int value)
{
    pthread_mutex_lock(&r.neighbor_list_lock);
    for (int_list *iterator = list; iterator != NULL; iterator = iterator->next)
    {
        if (iterator->value == value)
        {
            pthread_mutex_unlock(&r.neighbor_list_lock);
            return true;
        }
    }
    pthread_mutex_unlock(&r.neighbor_list_lock);
    return false;
}