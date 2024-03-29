#ifndef HEADERS_H
#define HEADERS_H
#include "../headers.h"
#endif

enum status
{
    stopped = 0,
    running = 1, // only one not used so far
    resumed = 2,
    started = 3,
    waiting = 4,
    finished = 5
};

// Node: simulates a process
typedef struct node
{
    int id;

    int processpriority;
    int arrivaltime;
    int runtime;
    int processPID;

    int WaitingTime;
    int ReaminingTime;

    enum status Status;
    // Status = waiting;
    int schedpid;
    int memsize;        // memory size taken by process
    int memstart;       // index of starting location in memory for process

    // Lower values indicate higher Qpriority
    int Qpriority;

} node;

typedef struct QNode
{
    node *process;
    struct QNode *next;
} QNode;

typedef struct CNode
{
    node *process;
    struct CNode *next;
    struct CNode *prev;
} CNode;

/*typedef struct MLFLNode {
    //int data;
    node* process;
    struct MLFLNode* next;
} MLFLNode;*/

node *newnode(int id, int Processpriority, int arrival, int runtime,
              int Memsize, enum status status)
{
    node *temp = (node *)malloc(sizeof(node));
    temp->id = id;
    temp->processpriority = Processpriority;
    temp->processPID = -1;
    temp->runtime = runtime;
    temp->ReaminingTime = runtime;
    temp->arrivaltime = arrival;
    temp->WaitingTime = 0;
    temp->Status = status;
    temp->schedpid = 0;
    temp->memsize = Memsize;
    temp->memstart = -1;

    return temp;
}

CNode *newNodeRR(node *input)
{

    CNode *temp = (CNode *)malloc(sizeof(CNode));
    temp->process = input;
    temp->process->Status = waiting;
    temp->next = NULL;
    temp->prev = NULL;

    return temp;
}

QNode *newNodeSJF(node *input)
{

    QNode *temp = (QNode *)malloc(sizeof(QNode));
    temp->process = input;
    temp->process->Qpriority = input->runtime; // or totaltime coordinate
    temp->process->Status = waiting;
    temp->next = NULL;

    return temp;
}

QNode *newNodeHPF(node *input)
{

    QNode *temp = (QNode *)malloc(sizeof(QNode));
    temp->process = input;
    temp->process->Qpriority = input->processpriority;
    temp->process->Status = waiting;
    temp->next = NULL;

    return temp;
}

QNode *newNodeMLFL(node *input)
{

    QNode *temp = (QNode *)malloc(sizeof(QNode));
    temp->process = input;
    temp->process->Status = waiting;
    temp->process->Qpriority = 0;
    temp->next = NULL;

    return temp;
}

void freeinsideCNODE(CNode *cnode)
{
    if (cnode == NULL)
        return;
    if (cnode->process != NULL)
        free(cnode->process);
    return;
}
void freeinsideQNODE(QNode *qnode)
{
    if (qnode == NULL)
        return;
    if (qnode->process != NULL)
        free(qnode->process);
    return;
}
void freeinsideMLFL(QNode *MLFLnode)
{
    if (MLFLnode == NULL)
        return;
    if (MLFLnode->process != NULL)
        free(MLFLnode->process);
    return;
}