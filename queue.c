/**********************************
 * @author      Diego Sosa
 * description
 **********************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"

queue queue_init()
{
    queue* q = malloc(sizeof(struct queue));
    q->front = NULL;
    q->back = NULL;

    q->size = 0;

    return *q;
}

void queue_destroy(queue q)
{
    while(queue_num_size(&q) > 0)
    {
        dequeue(&q);
    }
}

qnode* peek(queue* q)
{
    qnode* n = malloc(sizeof(struct qnode));
    memcpy(n, q->front, sizeof(struct qnode));
    return n;   
}

int dequeue(queue* q)
{
    if(q->front == NULL && q->back == NULL)
    {
        return 0;
    }

    // free memory !!!!

    q->front = q->front->prev;
    free(q->front->next);

    q->size = q->size - 1;
    return 0;
}

int enqueue(queue* q, void* data)
{
    qnode* n = malloc(sizeof(struct qnode));
    n->data = data;

    if(q->front == NULL && q->back == NULL)     //  Empty queue
    {
        q->front = n;
        q->back = n;

        return 1;
    }

    q->back->prev = n;
    n->next = q->back;
    q->back = n;

    q->size = q->size + 1;
    return 0;
}

int queue_num_size(queue* q){
    return q->size;
}