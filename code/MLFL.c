// #include "linkedlist.c"
#ifndef pqueue
#define pqueue
#include "pqueue.c"
#endif
typedef struct chained_linkedlist
{
    // linkedlist* listHead;
    QNode *Head;
    struct chained_linkedlist *next;
    // int baseprio;
} chained_linkedlist;

chained_linkedlist *create_chainedlinkedlist(QNode **list, chained_linkedlist *nextcll)
{
    chained_linkedlist *cll = (chained_linkedlist *)malloc(sizeof(chained_linkedlist));
    cll->Head = *list;
    cll->next = nextcll;
    return cll;
}

typedef struct MLFL
{
    QNode *linkedlist0;
    QNode *linkedlist1;
    QNode *linkedlist2;
    QNode *linkedlist3;
    QNode *linkedlist4;
    QNode *linkedlist5;
    QNode *linkedlist6;
    QNode *linkedlist7;
    QNode *linkedlist8;
    QNode *linkedlist9;
    QNode *linkedlist10;

    // chained_linkedlist *Head0; // use an array with currentchain_index for each instead?
    // chained_linkedlist *Head1;
    // chained_linkedlist *Head2;
    // chained_linkedlist *Head3;
    // chained_linkedlist *Head4;
    // chained_linkedlist *Head5;
    // chained_linkedlist *Head6;
    // chained_linkedlist *Head7;
    // chained_linkedlist *Head8;
    // chained_linkedlist *Head9;
    // chained_linkedlist *Head10;

} MLFL;

QNode *grabnextnode_mlfl(int *currentchain, QNode *currentlyrunning, MLFL *mlfl)
{
    // head0 1 process // head0 0 process
    // head1 0 process
    // head2 1 process //corner case
    // a priority level in the middle with no processes
    // printf("grabbing next node\n");
    // if (currentlyrunning != NULL && (*currentlyrunning)->next)
    // { // first null
    //     printf("in first null\n");
    //     currentlyrunning = &((*currentlyrunning)->next);
    //     return;
    // }
    // else
    // {
    //     printf("in else conf in grabnextnode\n");
    //     while (currentchain->next)
    //     { // second null
    //         printf("stuck in while of second null\n");
    //         currentchain = currentchain->next;
    //         if (currentchain->Head != NULL)
    //         {
    //             currentlyrunning = &(currentchain->Head);
    //             return;
    //         }
    //     }
    // }
    // currentlyrunning = NULL;
    // printf("hit double null\n");
    // return;
    // if currentlyrunning returns null then we have hit the 'double null' and then we are at the end of all chains
    // and we should run lesser prio chain next ex: head0 -> head1 (using switch case inside scheduler part)

    // if (currentlyrunning != NULL && currentlyrunning->next != NULL)
    // { // first null
    //     printf("in first null\n");
    //     currentlyrunning = currentlyrunning->next;

    //     if (currentlyrunning != NULL && currentlyrunning->next == NULL)
    //     {
    //         printf("in if loop dec currentchain\n");
    //         (*currentchain)--;
    //     }
    //     // printf("process id after first null: %d\n", currentlyrunning->process->id);
    //     printf("currentchain after first null: %d\n", *currentchain);
    //     return currentlyrunning;
    // }
    // else
    // {
    while (*currentchain > -1)
    {
        printf("while loop in grabnextnode\n");
        switch (*currentchain)
        {
        case 0:
            if (mlfl->linkedlist0 != NULL)
            {
                printf("grabnext node in case 0\n");
                return mlfl->linkedlist0;
            }
            else
                return NULL;
        case 1:
            if (mlfl->linkedlist1 != NULL)
            {
                printf("grabnext node in case 1 \n");
                (*currentchain)--;
                return mlfl->linkedlist1;
            }
            else
            {
                (*currentchain)--;
                break;
            }
        case 2:
            if (mlfl->linkedlist2 != NULL)
            {
                printf("grabnext node in case 2\n");
                (*currentchain)--;
                return mlfl->linkedlist2;
            }
            else
            {
                (*currentchain)--;
                break;
            }
        case 3:
            if (mlfl->linkedlist3 != NULL)
            {
                printf("grabnext node in case3\n");
                (*currentchain)--;
                return mlfl->linkedlist3;
            }
            else
            {
                (*currentchain)--;
                break;
            }
        case 4:
            if (mlfl->linkedlist4 != NULL)
            {
                printf("grabnext node in case 4\n");
                (*currentchain)--;
                return mlfl->linkedlist4;
            }
            else
            {
                (*currentchain)--;
                break;
            }
        case 5:
            if (mlfl->linkedlist5 != NULL)
            {
                printf("grabnext node in case 5\n");
                printf("process id: %d\n", mlfl->linkedlist5->process->id);
                (*currentchain)--;
                return mlfl->linkedlist5;
            }
            else
            {
                (*currentchain)--;
                break;
            }
        case 6:
            if (mlfl->linkedlist6 != NULL)
            {
                printf("grabnext node in case 6\n");
                (*currentchain)--;
                return mlfl->linkedlist6;
            }
            else
            {
                (*currentchain)--;
                break;
            }
        case 7:
            if (mlfl->linkedlist7 != NULL)
            {
                printf("grabnext node in case 7\n");
                (*currentchain)--;
                return mlfl->linkedlist7;
            }
            else
            {
                (*currentchain)--;
                break;
            }
        case 8:
            if (mlfl->linkedlist8 != NULL)
            {
                printf("grabnext node in case 8\n");
                (*currentchain)--;
                return mlfl->linkedlist8;
            }
            else
            {
                (*currentchain)--;
                break;
            }
        case 9:
            if (mlfl->linkedlist9 != NULL)
            {
                printf("grabnext node in case 9\n");
                (*currentchain)--;
                return mlfl->linkedlist9;
            }
            else
            {
                (*currentchain)--;
                break;
            }
        case 10:
            if (mlfl->linkedlist10 != NULL)
            {
                printf("grabnext node in case10\n");
                (*currentchain)--;
                return mlfl->linkedlist10;
            }
            else
            {
                printf("ELSE grabnext node in case10\n");
                (*currentchain)--;
                break;
            }
        }
    }
    //(*currentchain) = 10;
    //}
}

// head0 = linkedlist0
// head1 = linkedlist1 + head0
// head2 = linkedlist2 + head1

MLFL *newMLFL()
{

    MLFL *mlfl = (MLFL *)malloc(sizeof(MLFL));

    mlfl->linkedlist0 = NULL;
    mlfl->linkedlist1 = NULL;
    mlfl->linkedlist2 = NULL;
    mlfl->linkedlist3 = NULL;
    mlfl->linkedlist4 = NULL;
    mlfl->linkedlist5 = NULL;
    mlfl->linkedlist6 = NULL;
    mlfl->linkedlist7 = NULL;
    mlfl->linkedlist8 = NULL;
    mlfl->linkedlist9 = NULL;
    mlfl->linkedlist10 = NULL;
    // mlfl->Head0 = create_chainedlinkedlist(&(mlfl->linkedlist0), NULL);
    // mlfl->Head1 = create_chainedlinkedlist(&(mlfl->linkedlist1), (mlfl->Head0));
    // mlfl->Head2 = create_chainedlinkedlist(&(mlfl->linkedlist2), (mlfl->Head1));
    // mlfl->Head3 = create_chainedlinkedlist(&(mlfl->linkedlist3), (mlfl->Head2));
    // mlfl->Head4 = create_chainedlinkedlist(&(mlfl->linkedlist4), (mlfl->Head3));
    // mlfl->Head5 = create_chainedlinkedlist(&(mlfl->linkedlist5), (mlfl->Head4));
    // mlfl->Head6 = create_chainedlinkedlist(&(mlfl->linkedlist6), (mlfl->Head5));
    // mlfl->Head7 = create_chainedlinkedlist(&(mlfl->linkedlist7), (mlfl->Head6));
    // mlfl->Head8 = create_chainedlinkedlist(&(mlfl->linkedlist8), (mlfl->Head7));
    // mlfl->Head9 = create_chainedlinkedlist(&(mlfl->linkedlist9), (mlfl->Head8));
    // mlfl->Head10 = create_chainedlinkedlist(&(mlfl->linkedlist10), (mlfl->Head9));
    return mlfl;
}

void destroymlfl(MLFL *mlfl)
{
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

    // free(mlfl->Head0);
    // free(mlfl->Head1);
    // free(mlfl->Head2);
    // free(mlfl->Head3);
    // free(mlfl->Head4);
    // free(mlfl->Head5);
    // free(mlfl->Head6);
    // free(mlfl->Head7);
    // free(mlfl->Head8);
    // free(mlfl->Head9);
    // free(mlfl->Head10);

    free(mlfl);
}

void enqueueMLFL(MLFL *mlfl, node *n)
{
    printf("enqueueMLFL\n");
    // if(n == NULL || mlfl == NULL) return;
    QNode *new = newNodeMLFL(n);
    switch (n->processpriority)
    {
    case 0:
        if (isEmptyQNODE(&(mlfl->linkedlist0)))
            mlfl->linkedlist0 = new;
        else
            push(&(mlfl->linkedlist0), new);
        printf("case 0\n");
        break;
    case 1:
        if (isEmptyQNODE(&(mlfl->linkedlist1)))
            mlfl->linkedlist1 = new;
        else
            push(&(mlfl->linkedlist1), new);
        printf("case 1\n");
        break;
    case 2:
        if (isEmptyQNODE(&(mlfl->linkedlist2)))
            mlfl->linkedlist2 = new;
        else
            push(&(mlfl->linkedlist2), new);
        printf("case 2\n");
        break;
    case 3:
        if (isEmptyQNODE(&(mlfl->linkedlist3)))
            mlfl->linkedlist3 = new;
        else
            push(&(mlfl->linkedlist3), new);
        printf("case 3\n");
        break;
    case 4:
        if (isEmptyQNODE(&(mlfl->linkedlist4)))
            mlfl->linkedlist4 = new;
        else
            push(&(mlfl->linkedlist4), new);
        printf("case 4\n");
        break;
    case 5:
        if (isEmptyQNODE(&(mlfl->linkedlist5)))
            mlfl->linkedlist5 = new;
        else
            push(&(mlfl->linkedlist5), new);
        printf("case 5\n");
        break;
    case 6:
        if (isEmptyQNODE(&(mlfl->linkedlist6)))
            mlfl->linkedlist6 = new;
        else
            push(&(mlfl->linkedlist6), new);
        printf("case 6\n");
        break;
    case 7:
        if (isEmptyQNODE(&(mlfl->linkedlist7)))
            mlfl->linkedlist7 = new;
        else
            push(&(mlfl->linkedlist7), new);
        printf("case 7\n");
        break;
    case 8:
        if (isEmptyQNODE(&(mlfl->linkedlist8)))
            mlfl->linkedlist8 = new;
        else
            push(&(mlfl->linkedlist8), new);
        printf("case 8\n");
        break;
    case 9:
        if (isEmptyQNODE(&(mlfl->linkedlist9)))
            mlfl->linkedlist9 = new;
        else
            push(&(mlfl->linkedlist9), new);
        printf("case 9\n");
        break;
    case 10:
        if (isEmptyQNODE(&(mlfl->linkedlist10)))
            mlfl->linkedlist10 = new;
        else
            push(&(mlfl->linkedlist10), new);
        printf("case 10\n");
        break;
    default:
        printf("Out of range");
        break;
    }
    printf("End of enqueueMLFL\n");
}

QNode *PeekMLFLChainedList(int linkedListindex, MLFL *mlfl)
{
    QNode *temp;
    switch (linkedListindex)
    {
    case 0:
        temp = mlfl->linkedlist0;
        break;
    case 1:
        temp = mlfl->linkedlist1;
        break;
    case 2:
        temp = mlfl->linkedlist2;
        break;
    case 3:
        temp = mlfl->linkedlist3;
        break;
    case 4:
        temp = mlfl->linkedlist4;
        break;
    case 5:
        temp = mlfl->linkedlist5;
        break;
    case 6:
        temp = mlfl->linkedlist6;
        break;
    case 7:
        temp = mlfl->linkedlist7;
        break;
    case 8:
        temp = mlfl->linkedlist8;
        break;
    case 9:
        temp = mlfl->linkedlist9;
        break;
    case 10:
        temp = mlfl->linkedlist10;
        break;
    }
    if (temp == NULL)
    {
        printf("empty linkedlist \n");
        return NULL;
    }
    else
        return temp;
}

void clearfinishedprocesses_in_a_priolevel(QNode *level, MLFL *mlfl)
{
    if(level == NULL)
        return;
    printf("clearfinishedprocesses_in_a_priolevel\n");
    if (level && level->process->Status == finished)
    {
        printf("inside if of clearfinishedprocesses_in_a_priolevel\n");
        switch (level->process->processpriority)
        {
        case 0:
            if(level->next != NULL)
            {
                mlfl->linkedlist0 = level->next;
                break;
            }
            else
            {
                mlfl->linkedlist0 = NULL;
                level = NULL;
                break;
            }
        case 1:
            if(level->next != NULL)
            {
                mlfl->linkedlist1 = level->next;
                break;
            }
            else
            {
                mlfl->linkedlist1 = NULL;
                level = NULL;
                break;
            }
            // mlfl->linkedlist1 = NULL;
            // level = NULL;
            // break;
        case 2:
            if(level->next != NULL)
            {
                mlfl->linkedlist2 = level->next;
                break;
            }
            else
            {
                mlfl->linkedlist2 = NULL;
                level = NULL;
                break;
            }
            // mlfl->linkedlist2 = NULL;
            // level = NULL;
            // break;
        case 3:
            if(level->next != NULL)
            {
                mlfl->linkedlist3 = level->next;
                break;
            }
            else
            {
                mlfl->linkedlist3 = NULL;
                level = NULL;
                break;
            }
            // mlfl->linkedlist3 = NULL;
            // level = NULL;
            // break;
        case 4:
            if(level->next != NULL)
            {
                mlfl->linkedlist4 = level->next;
                break;
            }
            else
            {
                mlfl->linkedlist4 = NULL;
                level = NULL;
                break;
            }
            // mlfl->linkedlist4 = NULL;
            // level = NULL;
            // break;
        case 5:
            if(level->next != NULL)
            {
                mlfl->linkedlist5 = level->next;
                break;
            }
            else
            {
                mlfl->linkedlist5 = NULL;
                level = NULL;
                break;
            }
            // mlfl->linkedlist5 = NULL;
            // level = NULL;
            // break;
        case 6:
            if(level->next != NULL)
            {
                mlfl->linkedlist6 = level->next;
                break;
            }
            else
            {
                mlfl->linkedlist6 = NULL;
                level = NULL;
                break;
            }
            // mlfl->linkedlist6 = NULL;
            // level = NULL;
            // break;
        case 7:
            if(level->next != NULL)
            {
                mlfl->linkedlist7 = level->next;
                break;
            }
            else
            {
                mlfl->linkedlist7 = NULL;
                level = NULL;
                break;
            }
            // mlfl->linkedlist7 = NULL;
            // level = NULL;
            // break;
        case 8:
            if(level->next != NULL)
            {
                mlfl->linkedlist8 = level->next;
                break;
            }
            else
            {
                mlfl->linkedlist8 = NULL;
                level = NULL;
                break;
            }
            // mlfl->linkedlist8 = NULL;
            // level = NULL;
            // break;
        case 9:
            if(level->next != NULL)
            {
                mlfl->linkedlist9 = level->next;
                break;
            }
            else
            {
                mlfl->linkedlist9 = NULL;
                level = NULL;
                break;
            }
            // mlfl->linkedlist9 = NULL;
            // level = NULL;
            // break;
        case 10:
            if(level->next != NULL)
            {
                mlfl->linkedlist10 = level->next;
                break;
            }
            else
            {
                mlfl->linkedlist10 = NULL;
                level = NULL;
                break;
            }   
            // mlfl->linkedlist10 = NULL;
            // level = NULL;
            // break;
        }
        // printf("clearfinishedprocesses_in_a_priolevel\n");
        // printf("level->process->id %d\n", level->process->id);

        // printf("clearing head of list");
        // // QNode *temp = level;
        //  level = temp->next;
        //  freeinsideMLFL(level);
        free(level);
        printf("done clearfinishedprocesses_in_a_priolevel\n");
        if (level == NULL)
            printf("level is null\n");
        return;
    } // dealing with head is finished

    QNode *temp = level->next;
    QNode *prev = level; // clearing rest of list
    while (temp)
    {
        if (temp->process->Status == finished)
        {
            prev->next = temp->next;
            //freeinsideMLFL(temp);
            //free(temp);
            return;
        }
        prev = temp;
        temp = temp->next;
    }
}

void clearfinishedprocesses(MLFL *mlfl, int prio)
{

    // head0 1 unfinished
    // head1 1 finished 0 unfinished
    // corner case with dequeueing
    printf("clearfinishedprocesses priority: %d\n", prio);
    if (prio == 0)
        clearfinishedprocesses_in_a_priolevel(mlfl->linkedlist0, mlfl);
    if (prio == 1)
        clearfinishedprocesses_in_a_priolevel(mlfl->linkedlist1, mlfl);
    if (prio == 2)
        clearfinishedprocesses_in_a_priolevel(mlfl->linkedlist2, mlfl);
    if (prio == 3)
    {
        if(mlfl->linkedlist3 == NULL)
            printf("linked list 3 is null\n");
        clearfinishedprocesses_in_a_priolevel(mlfl->linkedlist3, mlfl);
    }
    if (prio == 4)
        clearfinishedprocesses_in_a_priolevel(mlfl->linkedlist4, mlfl);
    if (prio == 5)
        clearfinishedprocesses_in_a_priolevel(mlfl->linkedlist5, mlfl);
    if (prio == 6)
        clearfinishedprocesses_in_a_priolevel(mlfl->linkedlist6, mlfl);
    if (prio == 7)
        clearfinishedprocesses_in_a_priolevel(mlfl->linkedlist7, mlfl);
    if (prio == 8)
        clearfinishedprocesses_in_a_priolevel(mlfl->linkedlist8, mlfl);
    if (prio == 9)
        clearfinishedprocesses_in_a_priolevel(mlfl->linkedlist9, mlfl);
    if (prio == 10)
        clearfinishedprocesses_in_a_priolevel(mlfl->linkedlist10, mlfl);
}
