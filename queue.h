/**********************************
 * @author      Diego Sosa
 * description
 **********************************/

#ifndef _QUEUE_
#define _QUEUE_

/* Node */
typedef struct qnode{
    void* data;             /* data */
    struct qnode* prev;      /* pointer to previous node */
    struct qnode* next;      /* pointer to next node -> */
} qnode;

/* Queue */
typedef struct queue{
    qnode* front;      /*  first element ->1 */
    qnode* back;       /*  last element */
    int size;
} queue;

queue queue_init();

int dequeue(queue* q);

int enqueue(queue* q, void* data);

int queue_num_size(queue* q);

qnode* peek(queue* q);

void queue_destroy(queue q);

#endif