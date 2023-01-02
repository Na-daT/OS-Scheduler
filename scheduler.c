#ifndef HEADERS_H
#define HEADERS_H
#include "headers.h"
#endif
#ifndef pqueue
#define pqueue
#include "data_structures/pqueue.c"
#endif
#include "data_structures/cqueue.c"
#include "scheduling_algorithms/MLFL.c"
#include "buddymemory.c"



float utilization = 0, avg_wait = 0, wta = 0;

QNode *Running = NULL;
QNode *Running4 = NULL;
CNode *Running2 = NULL;
QNode *Running3 = NULL;

QNode *finishedque = NULL;

int processcount;
int quantum;
int count;

void ProcessTerminated(int signum);
void ProcessTerminated2(int signum);
void clockchangeMLFL(int signum);
void clockchangeRR(int signum);

FILE *logfile;
FILE *perf;
FILE *memOutlog;

int main(int argc, char *argv[])
{
    int algorithm_id = atoi(argv[1]);
    processcount = atoi(argv[3]);
    quantum = atoi(argv[2]);


    // creating the message queue
    int keyid = ftok(".", 65);
    int msqid = msgget(keyid, 0666 | IPC_CREAT);

    struct msgbuffer message;


    // open the output file
    logfile = fopen("./scheduler.log", "w");
    perf = fopen("./scheduler.perf", "w");
    memOutlog = fopen("./mem.log", "w");

    // printing the header of output file
    fprintf(logfile, "#At time x process y state arr w total z remain y wait k\n");
    fprintf(memOutlog, "#At time x a l l o c a t e d y bytes f o r process z from i to j\n");

    buddy_start();
    initClk();

    // TODO: implement the scheduler.

    ///////////////////////////////// SJF (Non-preemptive)
    if (algorithm_id == SJF)
    {
        signal(SIGUSR1, ProcessTerminated2);
        signal(SIGUSR2, SIG_IGN); // sigusr2 is used in Round Robin, so it's ignored
        QNode *pqHead = NULL; // head of queue

        while (getClk() < 1) {}
        while (processcount > 0)
        {
            int received_value = msgrcv(msqid, &message, sizeof(message.process), 0, IPC_NOWAIT); //msgtyp = 0, accepts all messages

            while (received_value != -1) // if received_value is -1 that means no message(process) was received this clock tick
            {
                // get all processes that arrive at current clock tick
                utilization += message.process.runtime; //add the received process runtime to utilization
                printf("received: %d at time %d \n", message.process.id, getClk());

                node *new = newnode(message.process.id, message.process.priority,
                                                message.process.arrival, message.process.runtime, message.process.memsize, waiting);

                QNode *newqnode = newNodeSJF(new);
                if (pqHead == NULL)
                    pqHead = newqnode;
                else
                    push(&pqHead, newqnode);

                received_value = msgrcv(msqid, &message, sizeof(message.process), 0, IPC_NOWAIT);
            }

            int flag = 0;
            if (isEmptyQNODE(&Running4) && !isEmptyQNODE(&pqHead)) // no running and there is a process waiting
            {
                Running4 = pqHead;
                pop(&pqHead);
                flag = 1;
            }

            if (!isEmptyQNODE(&Running4) && flag == 1)
            {
                // fork and take id of process
                int pid = fork();
                if (pid == 0)
                {
                    char *runtime_char = malloc(sizeof(char));
                    sprintf(runtime_char, "%d", Running4->process->runtime);
                    char *arg[] = {runtime_char, NULL};

                    int execute = execv("./process.out", arg);
                    if (execute == -1)
                        printf("failed to execute process\n");
                    perror("The error is: \n");
                    exit(-1);
                }
                printf("process %d started at time %d \n", Running4->process->id, getClk());
                Running4->process->processPID = pid;
                Running4->process->Status = started;
                Running4->process->WaitingTime = (getClk() - Running4->process->arrivaltime) - (Running4->process->runtime - Running4->process->ReaminingTime);
                fprintf(logfile, "At time %d process %d started arr %d total %d remain %d wait %d\n", getClk(), Running4->process->id, Running4->process->arrivaltime, Running4->process->runtime, Running4->process->ReaminingTime, Running4->process->WaitingTime);

                raise(SIGSTOP);
            }
            fflush(memOutlog); 
            fflush(logfile);
        }
        //TODO: figure out how SJF is supposed to exit loop
        destroyPQ(pqHead);
    }

    if (algorithm_id == HPF) // HPF (Preemptive)
    {
        // printf("starting hpf \n");
        signal(SIGUSR1, ProcessTerminated);
        signal(SIGUSR2, SIG_IGN); // sigusr2 is used in round-robin
        // so it's useless for this and should be ignored
        QNode *pqHead = NULL; // head of prioqueue

        // QNode* Running = NULL; // process currently running
        int last_runclk = 0;
        while (getClk() < 1) {}
        while (processcount > 0)
        {
            // IMPORTANT TO DO
            // MAKE SURE PRIOQUEUE IS USING priority AS PRIORITY VALUE

            int rec_value = msgrcv(msqid, &message, sizeof(message.process), 0, IPC_NOWAIT);

            int process_switched_flag = 0;
            //int newproccess_arrival_flag = 0;
            while (rec_value != -1) // get all process that arrive at current clk time
            {
                //newproccess_arrival_flag = 1;
                utilization += message.process.runtime;
                printf("received: %d at time %d \n", message.process.id, getClk());

                node *new = newnode(message.process.id, message.process.priority,
                                                message.process.arrival, message.process.runtime, message.process.memsize, waiting);

                QNode *newqnode = newNodeHPF(new);
                if (pqHead == NULL)
                    pqHead = newqnode;
                else
                    push(&pqHead, newqnode);


                //newproccess_arrival_flag = 1;
                rec_value = msgrcv(msqid, &message, sizeof(message.process), 0, IPC_NOWAIT);


            }

            process_switched_flag = 0;
            if (isEmptyQNODE(&Running) && !isEmptyQNODE(&pqHead))
            {
                printf("11111111 \n");
                // running the process
                // printf("before dequeuing %d\n", ReadyQ->process.id);
                Running = pqHead;
                pop(&pqHead);

                // printf("after dequeuing running is empty %d %d is empty q %d \n", isEmpty(&Running), Running->process.id, isEmpty(&ReadyQ));
                process_switched_flag = 1;

            }

            else if (process_switched_flag == 0 && !isEmptyQNODE(&Running) && !isEmptyQNODE(&pqHead))
            {
                //printf("tried to compare and switch \n");
                // peek and compare and change if necessary
                if (pqHead->process->Qpriority < Running->process->Qpriority)
                {

                    int iskill = kill(Running->process->processPID, SIGSTOP);

                    Running->process->ReaminingTime += last_runclk - getClk();
                    fprintf(logfile, "At time %d process %d stopped arr %d total %d remain %d wait %d\n", getClk(), Running->process->id, Running->process->arrivaltime, Running->process->runtime, Running->process->ReaminingTime, Running->process->WaitingTime);
                    Running->process->Status = stopped;

                    push(&pqHead, Running);
                    Running = pqHead;
                    pop(&pqHead);
                    //newproccess_arrival_flag = 0;
                    process_switched_flag = 1;

                }
            }

            if (process_switched_flag == 1)
            {
                if (Running->process->processPID == -1) // this proccess has yet to be created
                {

                    // fork and take id of process
                    int pid = fork();
                    if (pid == 0)
                    {
                        char *runtime_char = malloc(sizeof(char));
                        sprintf(runtime_char, "%d", Running->process->runtime);
                        char *arg[] = {runtime_char, NULL};

                        int execute = execv("./process.out", arg);
                        if (execute == -1)
                            printf("failed to execute process\n");
                        perror("The error is: \n");
                        exit(-1);
                    }

                    raise(SIGSTOP);
                    Running->process->processPID = pid;
                    Running->process->Status = started;
                    last_runclk = getClk(); // check waiting time equation correctness
                    Running->process->WaitingTime = (getClk() - Running->process->arrivaltime) - (Running->process->runtime - Running->process->ReaminingTime);
                    printf( "At time %d process %d started arr %d total %d remain %d wait %d\n", getClk(), Running->process->id, Running->process->arrivaltime, Running->process->runtime, Running->process->ReaminingTime, Running->process->WaitingTime);

                    fprintf(logfile, "At time %d process %d started arr %d total %d remain %d wait %d\n", getClk(), Running->process->id, Running->process->arrivaltime, Running->process->runtime, Running->process->ReaminingTime, Running->process->WaitingTime);
                }
                else // this proccess was created and only needed to be continued
                {
                    kill(Running->process->processPID, SIGCONT);
                    Running->process->Status = resumed;
                    last_runclk = getClk();
                    // check waiting time equation correctness
                    Running->process->WaitingTime = (getClk() - Running->process->arrivaltime) - (Running->process->runtime - Running->process->ReaminingTime);
                    printf("At time %d process %d resumed arr %d total %d remain %d wait %d\n", getClk(), Running->process->id, Running->process->arrivaltime, Running->process->runtime, Running->process->ReaminingTime, Running->process->WaitingTime);
                    fprintf(logfile, "At time %d process %d resumed arr %d total %d remain %d wait %d\n", getClk(), Running->process->id, Running->process->arrivaltime, Running->process->runtime, Running->process->ReaminingTime, Running->process->WaitingTime);
                }
            }
            fflush(memOutlog);
            fflush(logfile);
        }

        printf("destroying\n");
        destroyPQ(finishedque);
        printf("done destroying\n");
    }

    if (algorithm_id == RR) // Round Robin
    {
        signal(SIGUSR1, SIG_IGN);
        // won't use on termination here, handled internally on quantum end
        // don't rely on process sending signal each quantum, process will be sent a pause/cont each quantum or terminate when rem time = 0
        signal(SIGUSR2, clockchangeRR);
        // process notifies scheduler of clk changes


        circular_queue *circularque = create_circular_queue(processcount);
        int previd;
        while (processcount > 0)
        {
            while (getClk() < 1) {}
            int rec_value = msgrcv(msqid, &message, sizeof(message.process), 0, IPC_NOWAIT);
            previd = -10;

            while (rec_value != -1)
            {
                if (previd == message.process.id) break;
                previd = message.process.id;
                printf("received: %d at time %d \n", message.process.id, getClk());
                utilization += message.process.runtime;
                node *new = newnode(message.process.id, message.process.priority,
                                    message.process.arrival, message.process.runtime, message.process.memsize, waiting);
                enqueueCQ(circularque, new);

                
                rec_value = msgrcv(msqid, &message, sizeof(message.process), 0, IPC_NOWAIT);
            }

            if (isEmptyCnode(Running2) && !isEmptyCQ(circularque))
            {
                
                Running2 = peekCQ(circularque); // return pointer to process
                count = 0;                      // used to countup to each quantum
                quantum = atoi(argv[2]); // updating quantum cause see comment directly below

                if (Running2->process->ReaminingTime < quantum) // process's remaining time less than quantum so gonna run it for only that
                {
                    quantum = Running2->process->ReaminingTime;
                }

                if (Running2->process->processPID == -1)
                {

                    int pos = buddy_insert(Running2->process->memsize);
                    if (pos == -1) {
                        //printf("failed to allocate  %d, %d is next process \n",Running2->process->id, Running2->next->process->id);
                        fprintf(memOutlog, "At time %d failed to allocate %d bytes for process %d due to lack of space\n",
                            getClk(), Running2->process->memsize, Running2->process->id);
                        advanceCQhead(circularque); 
                        Running2 = NULL;
                    }
                    if(Running2 != NULL){

                        Running2->process->memstart = pos;

                        printf("forking\n");
                        int pid = fork();
                        if (pid == 0)
                        {
                            char *runtime_char = malloc(sizeof(char));
                            sprintf(runtime_char, "%d", Running2->process->runtime);
                            char *arg[] = {runtime_char, NULL};
                            int execute = execv("./process.out", arg);
                            if (execute == -1)
                                printf("failed to execute process\n");
                            perror("The error is: \n");
                            exit(-1);
                        }
                        Running2->process->processPID = pid;
                        Running2->process->Status = started;
                        Running2->process->WaitingTime = (getClk() - Running2->process->arrivaltime) - (Running2->process->runtime - Running2->process->ReaminingTime);
                        raise(SIGSTOP);
                        int NextPowerOfTwo = __nextPowerOf2(Running2->process->memsize);
                        fprintf(memOutlog, "At time %d allocated %d bytes for process %d from %d to %d\n",
                            getClk(), Running2->process->memsize, Running2->process->id, Running2->process->memstart, Running2->process->memstart + NextPowerOfTwo - 1);
                        fprintf(logfile, "At time %d process %d started arr %d total %d remain %d wait %d\n", getClk(), Running2->process->id, Running2->process->arrivaltime, Running2->process->runtime, Running2->process->ReaminingTime, Running2->process->WaitingTime);
                    }
                    
                }
                else
                {
                    kill(Running2->process->processPID, SIGCONT);
                    Running2->process->Status = resumed;
                    Running2->process->WaitingTime = (getClk() - Running2->process->arrivaltime) - (Running2->process->runtime - Running2->process->ReaminingTime);
                    fprintf(logfile, "At time %d process %d resumed arr %d total %d remain %d wait %d\n", getClk(), Running2->process->id, Running2->process->arrivaltime, Running2->process->runtime, Running2->process->ReaminingTime, Running2->process->WaitingTime);
                }
            }
            if (!isEmptyCnode(Running2) && count == quantum) // reached end of quantum
            {
                printf("reached end of quantum, count %d at time %d\n", count, getClk());
                if (Running2->process->ReaminingTime == 0)
                {
                    int status;
                    int rec_child = waitpid(Running2->process->processPID, &status, 0);
                    Running2->process->Status = finished;

                    processcount--;
                    avg_wait += Running2->process->WaitingTime;
                    wta += (float)(getClk() - Running2->process->arrivaltime) / Running2->process->runtime;
                    Running2->process->WaitingTime = (getClk() - Running2->process->arrivaltime) - (Running2->process->runtime - Running2->process->ReaminingTime);
                    fprintf(logfile, "At time %d process %d finished arr %d total %d remain 0 wait %d TA %d WTA %.2f\n", getClk(), Running2->process->id, Running2->process->arrivaltime, Running2->process->runtime, Running2->process->WaitingTime, getClk() - Running2->process->arrivaltime, (float)(getClk() - Running2->process->arrivaltime) / Running2->process->runtime);
                    int SmallestPowerOfTwo = __nextPowerOf2(Running2->process->memsize);
                    fprintf(memOutlog, "At time %d freed %d bytes from process %d from %d to %d\n",
                            getClk(), Running2->process->memsize, Running2->process->id, Running2->process->memstart, Running2->process->memstart + SmallestPowerOfTwo - 1);
                    bool rem = buddy_remove(Running2->process->memstart, Running2->process->memsize);
                    if(rem == false) printf("        removing went wrong      ");

                    Running2 = NULL;
                    dequeueCQ(circularque); // free head (running is always at head) and advances head pointer if appropriate obviously
                } 
                else
                {
                    if (!isEmptyCQ(circularque)) // switiching between process
                    {
                        kill(Running2->process->processPID, SIGSTOP);
                        Running2->process->Status = stopped;
                        Running2->process->WaitingTime = (getClk() - Running2->process->arrivaltime) - (Running2->process->runtime - Running2->process->ReaminingTime);
                        fprintf(logfile, "At time %d process %d stopped arr %d total %d remain %d wait %d\n", getClk(), Running2->process->id, Running2->process->arrivaltime, Running2->process->runtime, Running2->process->ReaminingTime, Running2->process->WaitingTime);

                        int rec_value = msgrcv(msqid, &message, sizeof(message.process), 0, IPC_NOWAIT);
                        while (rec_value != -1)
                        {
                            if (previd == message.process.id) break;
                            previd = message.process.id;
                            printf("received: %d at time %d \n", message.process.id, getClk());
                            utilization += message.process.runtime;
                            node *new = newnode(message.process.id, message.process.priority,
                                                message.process.arrival, message.process.runtime, message.process.memsize, waiting);
                            enqueueCQ(circularque, new);

                            int rec_value = msgrcv(msqid, &message, sizeof(message.process), 0, IPC_NOWAIT);
                        } // check for new arrivals and addong to tail

                        Running2 = NULL;
                        advanceCQhead(circularque); // move head once
                        // if only one process then it will switch to itself
                    }
                    else
                    {
                        count = 0;
                        quantum = atoi(argv[2]);
                        if (Running2->process->ReaminingTime < quantum)
                        {
                            quantum = Running2->process->ReaminingTime;
                        }
                    }
                }
            }
            fflush(memOutlog); 
            fflush(logfile);
        }

        destroyCQ(circularque);
    }

    if (algorithm_id == MFL) // Multiple level Feedback Loop
    {
        // printf("starting mlfl \n");
        signal(SIGUSR1, SIG_IGN); // won't use on termination here, handled internally on quantum end
        // don't rely on process sending signal each quantum, process will be sent a pause/cont each quantum or terminate when rem time = 0
        // PROCESS NOTIFIES SCHEDULAR OF CLK CHANGES
        signal(SIGUSR2, clockchangeMLFL);
        printf("quantum %d \n", quantum);

        // MLFLNode* Running3;
        bool endofthisqueuelevel = true;
        bool process_switched = false;
        int currentchain_index = 10;
        int currentchain;
        // printf("before construtor\n");
        MLFL *mlfl = newMLFL();
        int previd;
        printf("args %s %s %s \n", argv[1], argv[2], argv[3]);
        
        while (processcount > 0)
        {
            while (getClk() < 1){}
            int rec_value = msgrcv(msqid, &message, sizeof(message.process), 0, IPC_NOWAIT);
            
                    previd = -10;
                    while (rec_value != -1)
                    {
                        if (previd == message.process.id) break;
                        previd = message.process.id;

                        printf("received: %d at time %d \n", message.process.id, getClk());
                        utilization += message.process.runtime;

                        node *new = newnode(message.process.id, message.process.priority,
                                                message.process.arrival, message.process.runtime, message.process.memsize, waiting);
                        
                        enqueueMLFL(mlfl, new);

                        rec_value = msgrcv(msqid, &message, sizeof(message.process), 0, IPC_NOWAIT);
                    }
            if (endofthisqueuelevel == true)
            {
                printf("OI1111\n");
                printf("currentchain_index: %d\n", currentchain_index);
                switch (currentchain_index)
                {
                case 0:
                    currentchain = 0;
                    // currentchain = mlfl->Head0;
                    break;
                case 1:
                    currentchain = 1;
                    printf("in case 1 of node2\n");
                    // currentchain = mlfl->Head1;
                    break;
                case 2:
                    currentchain = 2;
                    // currentchain = mlfl->Head2;
                    break;
                case 3:
                    currentchain = 3;
                    // currentchain = mlfl->Head3;
                    break;
                case 4:
                    currentchain = 4;
                    // currentchain = mlfl->Head4;
                    break;
                case 5:
                    currentchain = 5;
                    // currentchain = mlfl->Head5;
                    printf("in case 5 of node 1\n");
                    break;
                case 6:
                    currentchain = 6;
                    // currentchain = mlfl->Head6;
                    break;
                case 7:
                    currentchain = 7;
                    // currentchain = mlfl->Head7;
                    break;
                case 8:
                    currentchain = 8;
                    // currentchain = mlfl->Head8;
                    break;
                case 9:
                    currentchain = 9;
                    // currentchain = mlfl->Head9;
                    break;
                case 10:
                    currentchain = 10;
                    // currentchain = mlfl->Head10;
                    break;
                default:
                    printf("Out of range");
                    break;
                }
                /// PrintQueue(mlfl->linkedlist5);
                process_switched = true;
                printf("try to peek\n");
                Running3 = PeekMLFLChainedList(currentchain, mlfl);
                // printf("running3: %d", Running3->process->id);
                if (isEmptyQNODE(&Running3))
                {
                    printf("OI1\n");
                    printf("current chain before grab next node: %d", currentchain);
                    Running3 = grabnextnode_mlfl(&currentchain, Running3, mlfl); // if this current level is empty, go grab next node
                    printf("current chain after grab next node: %d", currentchain);
                }
                if (Running3 == NULL)
                { // hit that 'double null' so that level of the queue is done and need to advance to next level
                    printf("OI2\n");
                    currentchain_index = (currentchain_index + 1) % 11;
                    // currentchain = currentchain_index;
                    endofthisqueuelevel = true; // end of this queue
                }
                else
                {
                    printf("OI3\n");
                    endofthisqueuelevel = false;
                }
                printf(" :) \n");
            }
            // actual running of a process happens here
            if (Running3 != NULL && process_switched && Running3->process != NULL)
            {
                printf("5: ");
                PrintQueue(mlfl->linkedlist6);
                printf("4: ");
                PrintQueue(mlfl->linkedlist4);
                printf("3: ");
                PrintQueue(mlfl->linkedlist3);
                printf("2: ");
                PrintQueue(mlfl->linkedlist2);
                // printf("actual running of a process \n");
                // if (Running3->process == NULL)
                // {
                //     printf("Running3->process is null \n");
                //     switch (Running3->process->processpriority)
                //     {
                //     case 0:
                //         if (mlfl->linkedlist0 = NULL)
                //             break;
                //     case 1:
                //         if (mlfl->linkedlist1 = NULL)
                //             break;
                //     case 2:
                //         if (mlfl->linkedlist2 = NULL)
                //             break;
                //     case 3:
                //         if (mlfl->linkedlist3 = NULL)
                //             break;
                //     case 4:
                //         if (mlfl->linkedlist4 = NULL)
                //             break;
                //     case 5:
                //         if (mlfl->linkedlist5 = NULL)
                //             break;
                //     case 6:
                //         if (mlfl->linkedlist6 = NULL)
                //             break;
                //     case 7:
                //         if (mlfl->linkedlist7 = NULL)
                //             break;
                //     case 8:
                //         if (mlfl->linkedlist8 = NULL)
                //             break;
                //     case 9:
                //         if (mlfl->linkedlist9 = NULL)
                //             break;
                //     case 10:
                //         if (mlfl->linkedlist10 = NULL)
                //             break;
                //     }
                // }
                endofthisqueuelevel = false;
                process_switched = false;

                count = 0; // used to countup to each quantum
                if (Running3->process != NULL)
                    printf("there is a process in this level");
                printf("running process %d\n", Running3->process->id);
                quantum = atoi(argv[2]); // updating quantum cause see comment directly below

                if (Running3->process->ReaminingTime < quantum) // process's remaining time less than quantum so gonna run it for only that
                {
                    quantum = Running3->process->ReaminingTime;
                }

                if (Running3->process->processPID == -1)
                {
                    printf("forking\n");
                    int pid = fork();
                    if (pid == 0)
                    {
                        char *runtime_char = malloc(sizeof(char));
                        sprintf(runtime_char, "%d", Running3->process->runtime);
                        // printf("runtime %s of process %d time rn %d\n", runtime_char, getpid(), getClk());
                        char *arg[] = {runtime_char, NULL};
                        int execute = execv("./process.out", arg);
                        if (execute == -1)
                            printf("failed to execute process\n");
                        perror("The error is: \n");
                        exit(-1);
                    }
                    Running3->process->processPID = pid;
                    Running3->process->Status = started;
                    Running3->process->WaitingTime = (getClk() - Running3->process->arrivaltime) - (Running3->process->runtime - Running3->process->ReaminingTime);
                    raise(SIGSTOP);
                    fprintf(logfile, "At time %d process %d started arr %d total %d remain %d wait %d\n", getClk(), Running3->process->id, Running3->process->arrivaltime, Running3->process->runtime, Running3->process->ReaminingTime, Running3->process->WaitingTime);
                }
                else
                {
                    printf("continuing process\n");
                    kill(Running3->process->processPID, SIGCONT);
                    Running3->process->Status = resumed;
                    Running3->process->WaitingTime = (getClk() - Running3->process->arrivaltime) - (Running3->process->runtime - Running3->process->ReaminingTime);
                    fprintf(logfile, "At time %d process %d resumed arr %d total %d remain %d wait %d\n", getClk(), Running3->process->id, Running3->process->arrivaltime, Running3->process->runtime, Running3->process->ReaminingTime, Running3->process->WaitingTime);
                }
            }
            // printf("after second if statement\n");

            /////////
            // printf("before third if statement\n");
            if (Running3 != NULL && count == quantum) // reached end of quantum
            {
                printf("reached end of quantum, count %d at time %d\n", count, getClk());
                printf("rem time in sched %d\n", Running3->process->ReaminingTime);
                if (Running3->process->ReaminingTime == 0)
                {
                    int status;
                    int rec_child = waitpid(Running3->process->processPID, &status, 0);
                    printf("rec child %d\n", rec_child);
                    Running3->process->Status = finished;

                    processcount--;
                    avg_wait += Running3->process->WaitingTime;
                    wta += (float)(getClk() - Running3->process->arrivaltime) / Running3->process->runtime;
                    Running3->process->WaitingTime = (getClk() - Running3->process->arrivaltime) - (Running3->process->runtime - Running3->process->ReaminingTime);
                    fprintf(logfile, "At time %d process %d finished arr %d total %d remain 0 wait %d TA %d WTA %.2f\n", getClk(), Running3->process->id, Running3->process->arrivaltime, Running3->process->runtime, Running3->process->WaitingTime, getClk() - Running3->process->arrivaltime, (float)(getClk() - Running3->process->arrivaltime) / Running3->process->runtime);

                    printf("before clear finished processes\n");
                    clearfinishedprocesses(mlfl, Running3->process->processpriority);
                    printf("after clear finished processes\n");

                    Running3->process = NULL;
                    printf("after nulling\n");
                    process_switched = true;
                    if (Running3->next != NULL)
                        Running3 = Running3->next;
                    else
                        Running3 = grabnextnode_mlfl(&currentchain, Running3, mlfl);
                    if (Running3 == NULL)
                    { // hit that 'double null' so that level of the queue is done and need to advance to next level
                        currentchain_index = (currentchain_index + 1) % 11;
                        printf("end of this queue level");
                        endofthisqueuelevel = true;
                        process_switched = false;
                    }
                    printf("after grabbing next node\n");
                }
                else
                {
                    // switiching between process
                    kill(Running3->process->processPID, SIGSTOP);
                    Running3->process->Status = stopped;
                    Running3->process->WaitingTime = (getClk() - Running3->process->arrivaltime) - (Running3->process->runtime - Running3->process->ReaminingTime);
                    fprintf(logfile, "At time %d process %d stopped arr %d total %d remain %d wait %d\n", getClk(), Running3->process->id, Running3->process->arrivaltime, Running3->process->runtime, Running3->process->ReaminingTime, Running3->process->WaitingTime);

                    int rec_value = msgrcv(msqid, &message, sizeof(message.process), 0, IPC_NOWAIT);
            
                    while (rec_value != -1)
                    {
                        if (previd == message.process.id) break;
                        previd = message.process.id;

                        printf("received: %d at time %d \n", message.process.id, getClk());
                        utilization += message.process.runtime;

                        node *new = newnode(message.process.id, message.process.priority,
                                                message.process.arrival, message.process.runtime, message.process.memsize, waiting);
                        
                        enqueueMLFL(mlfl, new);

                        rec_value = msgrcv(msqid, &message, sizeof(message.process), 0, IPC_NOWAIT);
                    }

                    process_switched = true;
                    if (Running3->next != NULL)
                        Running3 = Running3->next;
                    else
                        Running3 = grabnextnode_mlfl(&currentchain, Running3, mlfl);
                    if (Running3 == NULL)
                    { // hit that 'double null' so that level of the queue is done and need to advance to next level
                        currentchain_index = (currentchain_index + 1) % 11;
                        printf("end of this queue level");
                        endofthisqueuelevel = true;
                        process_switched = false;
                    }
                }
            }
            fflush(memOutlog); 
            fflush(logfile);
        }
        destroymlfl(mlfl);
    }


    wta /= atoi(argv[3]);
    avg_wait /= atoi(argv[3]);
    utilization /= getClk();
    utilization *= 100;

    printf("exiting scheduler at time %d\n", getClk());

    // printing to .perf file
    fprintf(perf, "CPU utilization= %.2f %% Avg \nWTA =%.2f \nAvg Waiting =%.2f\n", utilization, wta, avg_wait);

    fclose(perf);
    fclose(logfile);
    fclose(memOutlog);

    // TODO: upon termination release the clock resources.
    destroyClk(1);
    return 0;
}
void ProcessTerminated2(int signum)
{
    avg_wait += Running4->process->WaitingTime;
    wta += (float)(getClk() - Running4->process->arrivaltime) / Running4->process->runtime;
    // calculated at proccess while it was running
    fprintf(logfile, "At time %d process %d finished arr %d total %d remain 0 wait %d TA %d WTA %.2f\n", getClk(), Running4->process->id, Running4->process->arrivaltime, Running4->process->runtime, Running4->process->WaitingTime, getClk() - Running4->process->arrivaltime, (float)(getClk() - Running4->process->arrivaltime) / Running4->process->runtime);
    printf("At time %d process %d finished arr %d total %d remain 0 wait %d TA %d WTA %.2f\n", getClk(), Running4->process->id, Running4->process->arrivaltime, Running4->process->runtime, Running4->process->WaitingTime, getClk() - Running4->process->arrivaltime, (float)(getClk() - Running4->process->arrivaltime) / Running4->process->runtime);
    freeinsideQNODE(Running4);
    free(Running4);
    Running4 = NULL;
    processcount--;
    printf("processcount=%d\n", processcount);
}
void ProcessTerminated(int signum)
{
    avg_wait += Running->process->WaitingTime;
    wta += (float)(getClk() - Running->process->arrivaltime) / Running->process->runtime;
    // calculated at proccess while it was running
    fprintf(logfile, "At time %d process %d finished arr %d total %d remain 0 wait %d TA %d WTA %.2f\n", getClk(), Running->process->id, Running->process->arrivaltime, Running->process->runtime, Running->process->WaitingTime, getClk() - Running->process->arrivaltime, (float)(getClk() - Running->process->arrivaltime) / Running->process->runtime);
    printf("At time %d process %d finished arr %d total %d remain 0 wait %d TA %d WTA %.2f\n", getClk(), Running->process->id, Running->process->arrivaltime, Running->process->runtime, Running->process->WaitingTime, getClk() - Running->process->arrivaltime, (float)(getClk() - Running->process->arrivaltime) / Running->process->runtime);
    //freeinsideQNODE(Running);
    //free(Running);
    if (finishedque == NULL)
        finishedque = Running;
    else
        push(&finishedque, Running);
    Running = NULL;
    processcount--;
    printf("processcount=%d\n", processcount);
}
void clockchangeRR(int signum)
{
    count++;
    Running2->process->ReaminingTime--;
    // printf("count: %d rem time %d\n", count, Running2->process->ReaminingTime);
}
void clockchangeMLFL(int signum)
{
    count++;
    Running3->process->ReaminingTime--;
    // printf("count: %d rem time %d\n", count, Running3->process->ReaminingTime);
}