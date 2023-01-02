#ifndef CIRCULAR_QUEUE_H
#define CIRCULAR_QUEUE_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "headers.h"

// Create node struct
typedef struct circular_node
{
    Process *data;
    struct circular_node *next, *prev;
} circular_node;

// Create circular queue struct
typedef struct circular_queue
{
    circular_node *Head;
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

// Enqueue
void enqueueCQ(circular_queue *cq, Process *data)
{
    circular_node *new_node = (circular_node *)malloc(sizeof(circular_node));
    new_node->data = data;

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
}

// Dequeue
Process *dequeueCQ(circular_queue *cq)
{
    if (cq->Head == NULL)
    {
        return NULL;
    }
    circular_node *temp = cq->Head;
    Process *data = temp->data;
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
    return data;
}

// destroy circular queue
void destroyCQ(circular_queue *cq)
{
    while (cq->Head)
        dequeueCQ(cq);

    free(cq);
}


// Front
Process *peekCQ(circular_queue *cq)
{
    if (cq->Head == NULL)
    {
        printf("Queue is empty");
        return NULL;
    }
    return cq->Head->data;
}

// Is empty
bool isEmptyCQ(circular_queue *cq)
{
    return cq->Head == NULL;
}

#endif