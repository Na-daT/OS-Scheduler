#ifndef NODE_H
#define NODE_H
#include "node.h"
#endif 
typedef struct linkedlist
{
    MLFLNode* Head;
    //int prio;
} linkedlist;


linkedlist* create_linkedlist()
{
    linkedlist *cq = (linkedlist *)malloc(sizeof(linkedlist));
    cq->Head = NULL;
    return cq;
}

void enqueuelinkedlist(linkedlist *ll, node *data)
{
    MLFLNode* new_node = newNodeMLFL(data);

    if (ll->Head != NULL)
    {
        MLFLNode *temp = ll->Head;
        while (temp->next)
        {
            temp = temp->next;
        }
        temp->next = new_node;
    }
    else
    {
        new_node->next = NULL;
        ll->Head = new_node;
    }
    
}

void destroylinkedlist(linkedlist *ll)
{
    MLFLNode *temp = ll->Head;
    while (temp != NULL)
    {
        MLFLNode *next = temp->next;
        freeinsideMLFL(temp);
        free(temp);
        temp = next;
    }
    free(ll);
}