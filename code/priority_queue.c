#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// Create node struct
typedef struct PQ_node
{
    void *data;
    int priority;
    struct PQ_node *next;
} PQ_node;

// Create priority queue struct
typedef struct priority_queue
{
    PQ_node *Head;
    size_t size;
    int length;
} priority_queue;

// Create priority queue
priority_queue *create_priority_queue(size_t size)
{
    priority_queue *pq = (priority_queue *)malloc(sizeof(priority_queue));
    pq->Head = NULL;
    pq->size = size;
    pq->length = 0;
    return pq;
}

// destroy priority queue
void destroyPQ(priority_queue *pq)
{
    PQ_node *temp = pq->Head;
    while (temp != NULL)
    {
        PQ_node *next = temp->next;
        free(temp);
        temp = next;
    }
    free(pq);
}

// Enqueue
void enqueuePQ(priority_queue *pq, void *data, int priority)
{
    PQ_node *new_node = (PQ_node *)malloc(sizeof(PQ_node));
    new_node->data = malloc(pq->size);
    new_node->priority = priority;
    new_node->next = NULL;

    if (pq->Head == NULL)
    {
        new_node->next = NULL;
        pq->Head = new_node;
        return;
    }

    if (pq->Head->priority > priority)
    {
        new_node->next = pq->Head;
        pq->Head = new_node;
        pq->length++;
        return;
    }

    PQ_node *start = pq->Head;

    while (start->next != NULL && start->next->priority < priority)
    {
        start = start->next;
    }
    new_node->next = start->next;
    start->next = new_node;
    pq->length++;
}

// dequeue
bool dequeuePQ(priority_queue *pq, void **data)
{
    if (pq->Head == NULL)
    {
        return false;
    }
    PQ_node *temp = pq->Head;
    pq->Head = pq->Head->next;
    memcpy(*data, temp->data, pq->size);
    free(temp);
    pq->length--;
    return true;
}

// peek priority queue
bool peekPQ(priority_queue *pq, void **data)
{
    if (pq->Head == NULL)
    {
        return false;
    }
    memcpy(*data, pq->Head->data, pq->size);
    return true;
}