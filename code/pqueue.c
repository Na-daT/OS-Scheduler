#ifndef NODE_H
#define NODE_H
#include "node.h"
#endif

// Return the value at head
/*int peek(QNode** head)
{
    return (head);
}*/


// Removes the element with the
// highest Qpriority from the list
void pop(QNode** head)
{
    QNode* temp = *head;
    (*head) = (*head)->next;
    //free(temp); //yes i just want it to update the queue and not free the head
}

bool isEmptyQNODE(struct QNode** head)
{
    return (*head) == NULL;
}

// Function to push according to Qpriority
void push(QNode** head, QNode* temp)
{
    QNode* start = (*head);
    //printf("trying to insert new node in linked list \n");
    //printf("process id %d \n ", temp->process->id);
    //printf("process level %d \n ", temp->process->processpriority);
    // Create new Node
    //QNode* temp = newNode(d, p);
 
    // Special Case: The head of list has lesser
    // Qpriority than new node. So insert new
    // node before head node and change head node.
    if ((*head)->process->Qpriority > temp->process->Qpriority) {
 
        // Insert New Node before head
        temp->next = *head;
        (*head) = temp;
    }
    else {
 
        // Traverse the list and find a
        // position to insert new node
        while (start->next != NULL &&
            start->next->process->Qpriority <= temp->process->Qpriority) { // <= instead of <
            start = start->next;
        }
 
        // Either at the ends of the list
        // or at required position
        temp->next = start->next;
        start->next = temp;
    }
}

void PrintQueue(struct QNode* head){
    QNode* temp = head;
    printf("printing queue \n"); 
    while(temp){
        printf("pid: %d ", temp->process->id);
        temp = temp->next;
    }
    printf("\n");
}

void destroyPQ(QNode *pq)
{
    QNode *temp = pq;
    while (temp != NULL)
    {
        QNode *next = temp->next;
        freeinsideQNODE(temp);
        free(temp);
        temp = next;
    }
}