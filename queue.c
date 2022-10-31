#include "queue.h"

queue *init_queue()
{
    queue *q = calloc(1, sizeof(queue));
    q->head = q->buffer;
    q->tail = q->buffer;
    return q;
}

void enqueue(queue *q, message msg)
{

    // se a mensagem nao possui destino, presume-se que nao foi inicializada;
    if (!q->tail->destiny)
    {
        *q->tail = msg;
        q->tail++;
        // se o tail chegou ao fim do buffer, volta para o inicio (fila circular);
        if (q->tail == q->buffer + BUFFER_SIZE)
            q->tail = q->buffer;
    }
    else
    {
        puts("Queue is full");
    }
}

message dequeue(queue *q)
{
    if (!q->head->destiny)
    {
        puts("Queue is empty");
        return (message){0};
    }
    else
    {
        message msg = *q->head;
        memset(q->head, 0, sizeof(message)); // limpa a mensagem
        q->head++;
        // se o head chegou ao fim do buffer, volta para o inicio (fila circular);
        if (q->head == q->buffer + BUFFER_SIZE)
            q->head = q->buffer;
        return msg;
    }
}