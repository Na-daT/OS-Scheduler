//#include "linkedlist.c"
#ifndef pqueue
#define pqueue
#include "pqueue.c"
#endif
typedef struct chained_linkedlist
{
    //linkedlist* listHead;
    QNode* Head;
    struct chained_linkedlist* next;
    //int baseprio;
} chained_linkedlist;

chained_linkedlist* create_chainedlinkedlist(QNode* list, chained_linkedlist* nextcll)
{
    chained_linkedlist *cll = (chained_linkedlist *)malloc(sizeof(chained_linkedlist));
    cll->Head = list;
    cll->next = nextcll;
    return cll;
}

void grabnextnode_mlfl(chained_linkedlist* currentchain, QNode* currentlyrunning){
    // head0 1 process // head0 0 process 
    // head1 0 process
    // head2 1 process //corner case
    // a priority level in the middle with no processes

    if(currentlyrunning->next){ //first null
        currentlyrunning = currentlyrunning->next;
        return;
    }
    else while(currentchain -> next) { //second null
        currentchain = currentchain ->next;
        if (currentchain->Head != NULL){
            currentlyrunning = currentchain->Head;
            return;
        }
    } 
    
        currentlyrunning = NULL;
    
    // if currentlyrunning returns null then we have hit the 'double null' and then we are at the end of all chains
    // and we should run lesser prio chain next ex: head0 -> head1 (using switch case inside scheduler part)
}

typedef struct MLFL
{
    QNode* linkedlist0;
    QNode* linkedlist1;
    QNode* linkedlist2;
    QNode* linkedlist3;
    QNode* linkedlist4;
    QNode* linkedlist5;
    QNode* linkedlist6;
    QNode* linkedlist7;
    QNode* linkedlist8;
    QNode* linkedlist9;
    QNode* linkedlist10;

    chained_linkedlist* Head0; //use an array with currentchain_index for each instead?
    chained_linkedlist* Head1;
    chained_linkedlist* Head2;
    chained_linkedlist* Head3;
    chained_linkedlist* Head4;
    chained_linkedlist* Head5;
    chained_linkedlist* Head6;
    chained_linkedlist* Head7;
    chained_linkedlist* Head8;
    chained_linkedlist* Head9;
    chained_linkedlist* Head10;

} MLFL;

// head0 = linkedlist0
// head1 = linkedlist1 + head0
// head2 = linkedlist2 + head1



MLFL* newMLFL(){
    
    MLFL *mlfl = (MLFL *)malloc(sizeof(MLFL));

    mlfl-> linkedlist0 = NULL;
    mlfl-> linkedlist1 = NULL;
    mlfl-> linkedlist2 = NULL;
    mlfl-> linkedlist3 = NULL;
    mlfl-> linkedlist4 = NULL;
    mlfl-> linkedlist5 = NULL;
    mlfl-> linkedlist6 = NULL;
    mlfl-> linkedlist7 = NULL;
    mlfl-> linkedlist8 = NULL;
    mlfl-> linkedlist9 = NULL;
    mlfl-> linkedlist10 = NULL;
    
    mlfl-> Head0 = create_chainedlinkedlist(mlfl->linkedlist0, NULL);
    mlfl-> Head1 = create_chainedlinkedlist(mlfl->linkedlist1, mlfl->Head0);
    mlfl-> Head2 = create_chainedlinkedlist(mlfl->linkedlist2, mlfl->Head1);
    mlfl-> Head3 = create_chainedlinkedlist(mlfl->linkedlist3, mlfl->Head2);
    mlfl-> Head4 = create_chainedlinkedlist(mlfl->linkedlist4, mlfl->Head3);
    mlfl-> Head5 = create_chainedlinkedlist(mlfl->linkedlist5, mlfl->Head4);
    mlfl-> Head6 = create_chainedlinkedlist(mlfl->linkedlist6, mlfl->Head5);
    mlfl-> Head7 = create_chainedlinkedlist(mlfl->linkedlist7, mlfl->Head6);
    mlfl-> Head8 = create_chainedlinkedlist(mlfl->linkedlist8, mlfl->Head7);
    mlfl-> Head9 = create_chainedlinkedlist(mlfl->linkedlist9, mlfl->Head8);
    mlfl-> Head10 = create_chainedlinkedlist(mlfl->linkedlist10, mlfl->Head9);
    return mlfl;
}

void destroymlfl(MLFL* mlfl){
    destroyPQ(mlfl->linkedlist0);
    destroyPQ(mlfl->linkedlist1);
    destroyPQ(mlfl->linkedlist2);
    destroyPQ(mlfl->linkedlist3);
    destroyPQ(mlfl->linkedlist4);
    destroyPQ(mlfl->linkedlist5);
    destroyPQ(mlfl->linkedlist6);
    destroyPQ(mlfl->linkedlist7);
    destroyPQ(mlfl->linkedlist8);
    destroyPQ(mlfl->linkedlist9);
    destroyPQ(mlfl->linkedlist10);
    
    free(mlfl->Head0);
    free(mlfl->Head1);
    free(mlfl->Head2);
    free(mlfl->Head3);
    free(mlfl->Head4);
    free(mlfl->Head5);
    free(mlfl->Head6);
    free(mlfl->Head7);
    free(mlfl->Head8);
    free(mlfl->Head9);
    free(mlfl->Head10);

    free(mlfl);
}

void enqueueMLFL(MLFL* mlfl, node* n){
    //if(n == NULL || mlfl == NULL) return;
    QNode* new =  newNodeMLFL(n);
    switch (n->processpriority) {
                    case 0:
                        if (isEmptyQNODE(&(mlfl->linkedlist0))) mlfl->linkedlist0 = new;
                        else push(&(mlfl->linkedlist0), new);
                        break;
                    case 1:
                        if (isEmptyQNODE(&(mlfl->linkedlist1))) mlfl->linkedlist1 = new;
                        else push(&(mlfl->linkedlist1), new);
                        break;
                    case 2:
                        if (isEmptyQNODE(&(mlfl->linkedlist2))) mlfl->linkedlist2 = new;
                        else push(&(mlfl->linkedlist2), new);
                        break;
                    case 3:
                        if (isEmptyQNODE(&(mlfl->linkedlist3))) mlfl->linkedlist3 = new;
                        else push(&(mlfl->linkedlist3), new);
                        break;
                    case 4:
                        if (isEmptyQNODE(&(mlfl->linkedlist4))) mlfl->linkedlist4 = new;
                        else push(&(mlfl->linkedlist4), new);
                        break;
                    case 5:
                        if (isEmptyQNODE(&(mlfl->linkedlist5))) mlfl->linkedlist5 = new;
                        else push(&(mlfl->linkedlist5), new);
                        break;
                    case 6:
                        if (isEmptyQNODE(&(mlfl->linkedlist6))) mlfl->linkedlist6= new;
                        else push(&(mlfl->linkedlist6), new);
                        break;
                    case 7:
                        if (isEmptyQNODE(&(mlfl->linkedlist7))) mlfl->linkedlist7 = new;
                        else push(&(mlfl->linkedlist7), new);
                        break;
                    case 8:
                        if (isEmptyQNODE(&(mlfl->linkedlist8))) mlfl->linkedlist8 = new;
                        else push(&(mlfl->linkedlist8), new);
                        break;
                    case 9:
                        if (isEmptyQNODE(&(mlfl->linkedlist9))) mlfl->linkedlist9 = new;
                        else push(&(mlfl->linkedlist9), new);
                        break;
                    case 10:
                        if (isEmptyQNODE(&(mlfl->linkedlist10))) mlfl->linkedlist10 = new;
                        else push(&(mlfl->linkedlist10), new);
                        break;
                    default:
                        printf("Out of range");
                        break;
                }
}

void clearfinishedprocesses_in_a_priolevel(QNode* level){
    if(level&& level->process->Status == finished){
        QNode* temp = level;
        level = temp -> next;
        freeinsideMLFL(temp);
        free(temp);
        return;
    } //dealing with head is finished

    QNode* temp = level->next;
    QNode* prev = level; //clearing rest of list
    while(temp){
        if(temp->process->Status == finished){
                prev->next = temp->next;
                freeinsideMLFL(temp);
                free(temp);
                return;
        }
        prev = temp;
        temp = temp ->next; 
    }
}

void clearfinishedprocesses(MLFL* mlfl, int prio){
    
    //head0 1 unfinished
    //head1 1 finished 0 unfinished
    //corner case with dequeueing

    if(prio == 0) clearfinishedprocesses_in_a_priolevel(mlfl->linkedlist0);
    if(prio == 1) clearfinishedprocesses_in_a_priolevel(mlfl->linkedlist1);
    if(prio == 2) clearfinishedprocesses_in_a_priolevel(mlfl->linkedlist2);
    if(prio == 3) clearfinishedprocesses_in_a_priolevel(mlfl->linkedlist3);
    if(prio == 4) clearfinishedprocesses_in_a_priolevel(mlfl->linkedlist4);
    if(prio == 5) clearfinishedprocesses_in_a_priolevel(mlfl->linkedlist5);
    if(prio == 6) clearfinishedprocesses_in_a_priolevel(mlfl->linkedlist6);
    if(prio == 7) clearfinishedprocesses_in_a_priolevel(mlfl->linkedlist7);
    if(prio == 8) clearfinishedprocesses_in_a_priolevel(mlfl->linkedlist8);
    if(prio == 9) clearfinishedprocesses_in_a_priolevel(mlfl->linkedlist9);
    if(prio == 10) clearfinishedprocesses_in_a_priolevel(mlfl->linkedlist10);
}

