#include "../headers/queue.h"

queue *init_queue()
{
    queue *q = malloc(sizeof(queue));
    memset(q->buffer, -1, sizeof(message) * BUFFER_SIZE);
    q->head = q->buffer;
    q->tail = q->buffer;
    q->sem = calloc(1, sizeof(sem_t));
    sem_init(q->sem, 0, 0);
    pthread_mutex_init(&q->mutex, NULL);
    return q;
}

queue_status enqueue(queue *q, message msg)
{
    pthread_mutex_lock(&q->mutex);
    int sem_value;
    sem_getvalue(q->sem, &sem_value);
    if (sem_value >= BUFFER_SIZE || q->tail->destiny_id != -1) // queue is full
    {
        pthread_mutex_unlock(&q->mutex);
        return QUEUE_FULL;
    }
    // se a mensagem nao possui destino, presume-se que nao foi inicializada;
    *q->tail = msg;
    q->tail++;
    // se o tail chegou ao fim do buffer, volta para o inicio (fila circular);
    if (q->tail == q->buffer + BUFFER_SIZE)
        q->tail = q->buffer;

    sem_post(q->sem);
    pthread_mutex_unlock(&q->mutex);
    return QUEUE_OK;
}

message dequeue(queue *q)
{
    sem_wait(q->sem);
    pthread_mutex_lock(&q->mutex);
    if (q->head->destiny_id == -1)
    {
        pthread_mutex_unlock(&q->mutex);
        return (message){0};
    }

    message msg = *q->head;
    memset(q->head, -1, sizeof(message)); // limpa a mensagem
    q->head++;
    // se o head chegou ao fim do buffer, volta para o inicio (fila circular);
    if (q->head == q->buffer + BUFFER_SIZE)
        q->head = q->buffer;

    pthread_mutex_unlock(&q->mutex);
    return msg;
}

void print_queue(queue *q)
{
    pthread_mutex_lock(&q->mutex);
    for (message *i = q->head; i != q->tail; i++)
    {
        if (i == q->buffer + BUFFER_SIZE)
            i = q->buffer;
        printf("Message from %d: %s", i->origin, i->data);
    }
    pthread_mutex_unlock(&q->mutex);
}