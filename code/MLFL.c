#include "linkedlist.c"

typedef struct chained_linkedlist
{
    linkedlist* listHead;
    struct chained_linkedlist* next;
    //int baseprio;
} chained_linkedlist;

chained_linkedlist* create_chainedlinkedlist(linkedlist* list, chained_linkedlist* nextcll)
{
    chained_linkedlist *cll = (chained_linkedlist *)malloc(sizeof(chained_linkedlist));
    cll->listHead = list;
    cll->next = nextcll;
    return cll;
}

void advancehead_mlfl(chained_linkedlist* currentchain, MLFLNode* currentlyrunning){
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
        if (currentchain->listHead->Head != NULL){
            currentlyrunning = currentchain->listHead->Head;
            return;
        }
    } 
    
        currentlyrunning = NULL;
    
    // if currentlyrunning returns null then we have hit the 'double null' and then we are at the end of all chains
    // and we should run lesser prio chain next ex: head0 -> head1 (using switch case inside scheduler part)
}

typedef struct MLFL
{
    linkedlist* linkedlist0;
    linkedlist* linkedlist1;
    linkedlist* linkedlist2;
    linkedlist* linkedlist3;
    linkedlist* linkedlist4;
    linkedlist* linkedlist5;
    linkedlist* linkedlist6;
    linkedlist* linkedlist7;
    linkedlist* linkedlist8;
    linkedlist* linkedlist9;
    linkedlist* linkedlist10;

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

    mlfl-> linkedlist0 = create_linkedlist();
    mlfl-> linkedlist1 = create_linkedlist();
    mlfl-> linkedlist2 = create_linkedlist();
    mlfl-> linkedlist3 = create_linkedlist();
    mlfl-> linkedlist4 = create_linkedlist();
    mlfl-> linkedlist5 = create_linkedlist();
    mlfl-> linkedlist6 = create_linkedlist();
    mlfl-> linkedlist7 = create_linkedlist();
    mlfl-> linkedlist8 = create_linkedlist();
    mlfl-> linkedlist9 = create_linkedlist();
    mlfl-> linkedlist10 = create_linkedlist();
    
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
}

void destroymlfl(MLFL* mlfl){
    destroylinkedlist(mlfl->linkedlist0);
    destroylinkedlist(mlfl->linkedlist1);
    destroylinkedlist(mlfl->linkedlist2);
    destroylinkedlist(mlfl->linkedlist3);
    destroylinkedlist(mlfl->linkedlist4);
    destroylinkedlist(mlfl->linkedlist5);
    destroylinkedlist(mlfl->linkedlist6);
    destroylinkedlist(mlfl->linkedlist7);
    destroylinkedlist(mlfl->linkedlist8);
    destroylinkedlist(mlfl->linkedlist9);
    destroylinkedlist(mlfl->linkedlist10);
    
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
    if(n == NULL || mlfl == NULL) return;
    MLFLNode* new =  newNodeMLFL(n);
    switch (n->processpriority) {
                    case 0:
                        enqueuelinkedlist(mlfl->linkedlist0, new);
                        break;
                    case 1:
                        enqueuelinkedlist(mlfl->linkedlist1, new);
                        break;
                    case 2:
                        enqueuelinkedlist(mlfl->linkedlist2, new);
                        break;
                    case 3:
                        enqueuelinkedlist(mlfl->linkedlist3, new);
                        break;
                    case 4:
                        enqueuelinkedlist(mlfl->linkedlist4, new);
                        break;
                    case 5:
                        enqueuelinkedlist(mlfl->linkedlist5, new);
                        break;
                    case 6:
                        enqueuelinkedlist(mlfl->linkedlist6, new);
                        break;
                    case 7:
                        enqueuelinkedlist(mlfl->linkedlist7, new);
                        break;
                    case 8:
                        enqueuelinkedlist(mlfl->linkedlist8, new);
                        break;
                    case 9:
                        enqueuelinkedlist(mlfl->linkedlist9, new);
                        break;
                    case 10:
                        enqueuelinkedlist(mlfl->linkedlist10, new);
                        break;
                    default:
                        printf("Out of range");
                        break;
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

void clearfinishedprocesses_in_a_priolevel(linkedlist* level){
    if(level->Head && level->Head->process->Status == finished){
        MLFLNode* temp = level->Head;
        level -> Head = temp -> next;
        freeinsideMLFL(temp);
        free(temp);
        return;
    } //dealing with head is finished

    MLFLNode* temp = level->Head->next;
    MLFLNode* prev = level->Head; //clearing rest of list
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