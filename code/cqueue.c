#include "node.h"

// Create circular queue struct
typedef struct circular_queue
{
    CNode* Head;
    size_t size;
    int length;
} circular_queue;

// Create circular queue
circular_queue *create_circular_queue(size_t size)
{
    circular_queue *cq = (circular_queue *)malloc(sizeof(circular_queue));
    cq->Head = NULL;
    cq->size = size;
    cq->length = 0;
    return cq;
}

// destroy circular queue
void destroyCQ(circular_queue *cq)
{
    while (cq->Head)
        dequeueCQ(cq);

    free(cq);
}


void advanceCQhead(circular_queue *cq){
    cq->Head = cq->Head->next;
}

// Enqueue
void enqueueCQ(circular_queue *cq, node *data)
{
    CNode *new_node = (CNode *)malloc(sizeof(CNode));
    new_node->process = data;

    if (cq->Head != NULL)
    {
        new_node->prev = cq->Head->prev;
        new_node->next = cq->Head;
        cq->Head->prev->next = new_node;
        cq->Head->prev = new_node;
    }
    else
    {
        new_node->next = new_node;
        new_node->prev = new_node;
        cq->Head = new_node;
    }
    cq->length += 1;
}

// Dequeue
node* dequeueCQ(circular_queue *cq)
{
    if (cq->Head == NULL)
    {
        return NULL;
    }
    CNode *temp = cq->Head;
    node *data = temp->process;
    if (cq->Head->next == cq->Head)
    {
        cq->Head = NULL;
    }
    else
    {
        temp->prev->next = temp->next;
        temp->next->prev = temp->prev;
        cq->Head = temp->next;
    }
    free(temp);
    cq->length -= 1;
    return data;
}

// Front
CNode *peekCQ(circular_queue *cq)
{
    if (cq->Head == NULL)
    {
        //printf("Queue is empty");
        return NULL;
    }
    return cq->Head;
} 

// Is empty
bool isEmptyCQ(circular_queue *cq)
{
    return cq->Head == NULL;
}

bool isEmptyCnode(CNode *cq)
{
    return cq->process == NULL;
}