#include "node.h"

typedef struct linkedlist
{
    MLFLNode* Head;
    int prio;
} linkedlist;


linkedlist* create_linkedlist()
{
    linkedlist *cq = (linkedlist *)malloc(sizeof(linkedlist));
    cq->Head = NULL;
    return cq;
}

void enqueuelinkedlist(linkedlist *ll, MLFLNode *data)
{
    MLFLNode *new_node = (MLFLNode *)malloc(sizeof(MLFLNode));
    new_node->process = data;

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