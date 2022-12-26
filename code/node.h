#ifndef HEADERS_H
#define HEADERS_H
#include "headers.h"
#endif

enum status{
    stopped = 0,
    running = 1, //only one not used so far
    resumed = 2,
    started = 3,
    waiting = 4,
    finished = 5
};

// Node
typedef struct node { 
    int id;

    int processpriority;
    int arrivaltime;
    int runtime;
    int processPID;

    int WaitingTime;
    int ReaminingTime;

    enum status Status;
    //Status = waiting; 
    int quantum; //RR

    int schedpid;
    //int Live_Priority; //mlfl //surprisngly don't need it

    // Lower values indicate higher Qpriority
    int Qpriority; 

} node;

typedef struct QNode {
	//int data;
    node *process; 
    struct QNode* next;
} QNode;

typedef struct CNode {
	//int data;
    node *process; 
    struct CNode* next;
    struct CNode* prev;
} CNode;

/*typedef struct MLFLNode {
	//int data;
    node* process; 
    struct MLFLNode* next;
} MLFLNode;*/


node* newnode(int id, int Processpriority, int arrival, int runtime, 
                enum status status)
{
    node* temp = (node*)malloc(sizeof(node));
    temp->id = id;
    temp->processpriority = Processpriority;
    temp->processPID=-1;
    temp->runtime=runtime;
    temp->ReaminingTime=runtime;
    temp->arrivaltime=arrival;
    temp->WaitingTime=0;
    temp->Status=status;
    temp->schedpid = 0;
    return temp;
}

CNode* newNodeRR(node* input)
{
    
    CNode* temp = ( CNode*)malloc(sizeof(CNode));
    temp->process = input;
    temp->process->Status = waiting;
    temp->next = NULL;
    temp->prev = NULL;

    return temp;
}

QNode* newNodeSJF(node* input)
{
    
    QNode* temp = ( QNode*)malloc(sizeof(QNode));
    temp->process = input;
    temp->process->Qpriority = input->runtime; //or totaltime coordinate
    temp->process->Status = waiting;
    temp->next = NULL;

    return temp;
}

QNode* newNodeHPF(node* input)
{
    
    QNode* temp = ( QNode*)malloc(sizeof(QNode));
    temp->process = input;
    temp->process->Qpriority = input->processpriority;
    temp->process->Status = waiting;
    temp->next = NULL;

    return temp;
}

QNode* newNodeMLFL(node* input)
{
    
    QNode* temp = ( QNode*)malloc(sizeof(QNode));
    temp->process = input;
    temp->process->Status = waiting;
    temp->process->Qpriority = 0;
    temp->next = NULL;

    return temp;
}

void freeinsideCNODE(CNode* cnode){
    if(cnode->process != NULL) free(cnode->process);
}
void freeinsideQNODE(QNode* qnode){
    if(qnode->process != NULL) free(qnode->process);
}
void freeinsideMLFL(QNode* MLFLnode){
    if(MLFLnode->process != NULL) free(MLFLnode->process);
}