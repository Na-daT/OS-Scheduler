#include "linkedlist.c"

typedef struct chained_linkedlist
{
    linkedlist* Head;
    struct chained_linkedlist* next;
    //int baseprio;
} chained_linkedlist;

chained_linkedlist* create_chainedlinkedlist(linkedlist* list, chained_linkedlist* nextcll)
{
    chained_linkedlist *cll = (chained_linkedlist *)malloc(sizeof(chained_linkedlist));
    cll->Head = list;
    cll->next = nextcll;
    return cll;
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

    chained_linkedlist* Head0;
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

/*linkedlist* linkedlist0 = create_linkedlist();
    linkedlist* linkedlist1 = create_linkedlist();
    linkedlist* linkedlist2 = create_linkedlist();
    linkedlist* linkedlist3 = create_linkedlist();
    linkedlist* linkedlist4 = create_linkedlist();
    linkedlist* linkedlist5 = create_linkedlist();
    linkedlist* linkedlist6 = create_linkedlist();
    linkedlist* linkedlist7 = create_linkedlist();
    linkedlist* linkedlist8 = create_linkedlist();
    linkedlist* linkedlist9 = create_linkedlist();
    linkedlist* linkedlist10 = create_linkedlist();
    
    chained_linkedlist* Head0 = create_chainedlinkedlist(linkedlist0, NULL);
    chained_linkedlist* Head1 = create_chainedlinkedlist(linkedlist1, Head0);
    chained_linkedlist* Head2 = create_chainedlinkedlist(linkedlist2, Head1);
    chained_linkedlist* Head3 = create_chainedlinkedlist(linkedlist3, Head2);
    chained_linkedlist* Head4 = create_chainedlinkedlist(linkedlist4, Head3);
    chained_linkedlist* Head5 = create_chainedlinkedlist(linkedlist5, Head4);
    chained_linkedlist* Head6 = create_chainedlinkedlist(linkedlist6, Head5);
    chained_linkedlist* Head7 = create_chainedlinkedlist(linkedlist7, Head6);
    chained_linkedlist* Head8 = create_chainedlinkedlist(linkedlist8, Head7);
    chained_linkedlist* Head9 = create_chainedlinkedlist(linkedlist9, Head8);
    chained_linkedlist* Head10 = create_chainedlinkedlist(linkedlist10, Head9);
    
    */