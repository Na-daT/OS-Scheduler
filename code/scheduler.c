#ifndef pqueue
#define pqueue
#include "pqueue.c"
#endif
#include "MLFL.c"
#include "cqueue.c"
#ifndef HEADERS_H
#define HEADERS_H
#include "headers.h"
#endif
// COORDINATE WITH PROCCESS GENERATOR DUDES:
// forking parameters for scheduler and process
// modify file path for message queue key

// remaining of main work:
// check/review equations for calculating time
// make sure all nodes after done running are freed (mlfl and rr done)
// debug all

float utilization = 0, avg_wait = 0, wta = 0;

QNode *Running = NULL;
CNode *Running2 = NULL;
QNode *Running3 = NULL;

int processcount;
int quantum;
int count;

void ProcessTerminated(int signum);
void clockchangeMLFL(int signum);
void clockchangeRR(int signum);

FILE *logfile;
FILE *perf;

int main(int argc, char *argv[])
{
    

    printf("Sheduler started\n");
    printf("scheduler args count %d \n", argc);
    printf("scheduler args %s %s %s \n", argv[1], argv[2], argv[3]);
    
    //int clk = getClk();

    // creating the message queue
    int keyid = ftok(".", 65);
    int msqid = msgget(keyid, 0666 | IPC_CREAT);

    // open the output file
    logfile = fopen("./scheduler.log", "w");
    perf = fopen("./scheduler.perf", "w");

    // printing the header of output file
    fprintf(logfile, "#At time x process y state arr w total z remain y wait k\n");

    // TODO: implement the scheduler.
    initClk();
    ///////////////////////////////// SJF //non premitave
    if (atoi(argv[1]) == 1)
    {
        printf("starting sjf \n");
        signal(SIGUSR1, ProcessTerminated);
        signal(SIGUSR2, SIG_IGN); // sigusr2 is used in roundrobin
        // so its useless for this and should be ignored
        processcount = atoi(argv[3]);
        QNode *pqHead = NULL; // headofqueue
        // QNode* Running = NULL; // process currently running

        while (processcount > 0)
        {

            struct msgbuffer message;
            int rec_value = msgrcv(msqid, &message, sizeof(message.process), 0, IPC_NOWAIT);

            while (rec_value != -1) // get all process that arrive at current clk time
            {
                //printf("reciving sjf \n");
                utilization += message.process.runtime;
                printf("received: %d at time %d \n", message.process.id, getClk());
                node *new = newnode(message.process.id, message.process.priority,
                                    message.process.arrival, message.process.runtime, waiting);
                // needs to be checked or modified with message.process
                QNode *newqnode = newNodeSJF(new);
                if (isEmptyQNODE(&pqHead))
                    pqHead = newqnode;
                else
                    push(&pqHead, newqnode);

                rec_value = msgrcv(msqid, &message, sizeof(message.process), 0, IPC_NOWAIT);
            }

            int flag = 0;
            if (isEmptyQNODE(&Running) && !isEmptyQNODE(&pqHead)) // no running and there is a process waiting
            {
                Running = pqHead;
                pop(&pqHead);
                flag = 1;
            }

            //if (isEmptyQNODE(&Running) && !isEmptyQNODE(&pqHead))
            if (!isEmptyQNODE(&Running) && flag == 1)
            { 
                int pid = fork();
                if (pid == 0)
                {
                    printf("forking process \n");
                    char *runtime_char = malloc(sizeof(char));
                    sprintf(runtime_char, "%d", Running->process->runtime);
                    char *arg[] = {runtime_char, NULL};
                    // instead of listing args one by one in execv, they are grouped in var "arg" and sent to function that way
                    // TO DO MAKE SURE ITS CORRECT AND UNDERSTAND IT WELL

                    int execute = execv("./process.out", arg);
                    if (execute == -1)
                        printf("failed to execute process\n");
                    perror("The error is: \n");
                    exit(-1);
                }
                raise(SIGSTOP); // schedular waits till child is done forking process
                // when process continues it send a sigcont to schedular
                Running->process->processPID = pid;
                Running->process->Status = started;
                Running->process->WaitingTime = (getClk() - Running->process->arrivaltime) - (Running->process->runtime - Running->process->ReaminingTime);
                printf("At time %d process %d started arr %d total %d remain %d wait %d\n", getClk(), Running->process->id, Running->process->arrivaltime, Running->process->runtime, Running->process->ReaminingTime, Running->process->WaitingTime);
                fprintf(logfile, "At time %d process %d started arr %d total %d remain %d wait %d\n", getClk(), Running->process->id, Running->process->arrivaltime, Running->process->runtime, Running->process->ReaminingTime, Running->process->WaitingTime);
            }
        }

        //destroyPQ(pqHead);

        wta /= atoi(argv[3]);
        avg_wait /= atoi(argv[3]);
        utilization /= getClk();
        utilization *= 100;
        
    }

    if (atoi(argv[1]) == 2) // hpf Preemptive
    {
        printf("starting hpf \n");
        signal(SIGUSR1, ProcessTerminated);
        signal(SIGUSR2, SIG_IGN); // sigusr2 is used in roundrobin
        // so its useless for this and should be ignored
        processcount = atoi(argv[3]);
        QNode *pqHead = NULL; // head of prioqueue
        // QNode* Running = NULL; // process currently running
        int last_runclk = 0;

        while (processcount > 0)
        {
            // IMPORTANT TO DO
            // MAKE SURE PRIOQUEUE IS USING priority AS PRIORITY VALUE

            struct msgbuffer message;
            int rec_value = msgrcv(msqid, &message, sizeof(message.process), 0, IPC_NOWAIT);

            int newproccess_arrival_flag = 0;
            while (rec_value != -1) // get all process that arrive at current clk time
            {
                newproccess_arrival_flag = 1;
                utilization += message.process.runtime;
                printf("received: %d at time %d \n", message.process.id, getClk());

                node *new = newnode(message.process.id, message.process.priority,
                                    message.process.arrival, message.process.runtime, waiting);

                QNode *newqnode = newNodeHPF(new);
                if (pqHead == NULL)
                    pqHead = newqnode; // needs to be checked or modified with message.process
                else
                    push(&pqHead, newqnode);

                rec_value = msgrcv(msqid, &message, sizeof(message.process), 0, IPC_NOWAIT);
            }

            int process_switched_flag = 0;

            if (isEmptyQNODE(&Running) && !isEmptyQNODE(&pqHead))
            {
                Running = pqHead;
                pop(&pqHead);
                process_switched_flag = 1;
            }

            // if there isn't a process running and a new process arrives
            // it will be placed in its correct place (order of running) therefore
            // next if condition will not affect it because the list was ordered correctly in the first place
            // ex : if arrived placed at front of prioqueue when check happens it will have higher prio than the current head
            // (current head was after it in queue)
            // if not first then same situation above applies in reverse so everything will be okay still

            if (!isEmptyQNODE(&Running) && !isEmptyQNODE(&pqHead) && newproccess_arrival_flag == 1)
            {
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
                    fprintf(logfile, "At time %d process %d started arr %d total %d remain %d wait %d\n", getClk(), Running->process->id, Running->process->arrivaltime, Running->process->runtime, Running->process->ReaminingTime, Running->process->WaitingTime);
                }
                else // this proccess was created and only needed to be continued
                {
                    kill(Running->process->processPID, SIGCONT);
                    Running->process->Status = resumed;
                    last_runclk = getClk();
                    // check waiting time equation correctness
                    Running->process->WaitingTime = (getClk() - Running->process->arrivaltime) - (Running->process->runtime - Running->process->ReaminingTime);
                    fprintf(logfile, "At time %d process %d resumed arr %d total %d remain %d wait %d\n", getClk(), Running->process->id, Running->process->arrivaltime, Running->process->runtime, Running->process->ReaminingTime, Running->process->WaitingTime);
                }
            }
        }

        wta /= atoi(argv[3]);
        avg_wait /= atoi(argv[3]);
        utilization /= getClk();
        utilization *= 100;
    }

    if (atoi(argv[1]) == 3) // Round Robin (RR)
    {
        printf("starting rr \n");
        signal(SIGUSR1, SIG_IGN); // won't use on termination here, handled internally on quantum end
        // don't rely on process sending signal each quantum, process will be sent a pause/cont each quantum or terminate when rem time = 0
        // PROCESS NOTIFIES SCHEDULAR OF CLK CHANGES
        signal(SIGUSR2, clockchangeRR);
        processcount = atoi(argv[3]);
        circular_queue *circularque = create_circular_queue(processcount);
        // CNode* Running2 = NULL;
        int quantum = atoi(argv[2]);

        while (processcount > 0)
        {

            struct msgbuffer message;
            int rec_value = msgrcv(msqid, &message, sizeof(message.process), 0, IPC_NOWAIT);

            while (rec_value != -1)
            {
                printf("received: %d at time %d \n", message.process.id, getClk());
                utilization += message.process.runtime;
                node *new = newnode(message.process.id, message.process.priority,
                                    message.process.arrival, message.process.runtime, waiting);

                // CNode* newcnode = newNodeRR(new); //enqueue takes node not cnode
                enqueueCQ(circularque, new);
                //printf("heyy %d \n", circularque->Head->process->id);

                //printf("running empty? %d cque empty? %d \n", isEmptyCnode(Running2), isEmptyCQ(circularque));

                rec_value = msgrcv(msqid, &message, sizeof(message.process), 0, IPC_NOWAIT);
            }
            
            // add RR LOGIC
            if (isEmptyCnode(Running2) && !isEmptyCQ(circularque))
            {
                //printf("heyy %d \n", circularque->Head->process->id);
                Running2 = peekCQ(circularque); // return pointer to process
                count = 0;                      // used to countup to each quantum
                printf("running process %d\n", Running2->process->id);
                quantum = atoi(argv[2]); // updating quantum cause see comment directly below

                if (Running2->process->ReaminingTime < quantum) // process's remaining time less than quantum so gonna run it for only that
                {
                    quantum = Running2->process->ReaminingTime;
                }

                if (Running2->process->processPID == -1)
                {
                    printf("forking\n");
                    int pid = fork();
                    if (pid == 0)
                    {
                        char *runtime_char = malloc(sizeof(char));
                        sprintf(runtime_char, "%d", Running2->process->runtime);
                        // printf("runtime %s of process %d time rn %d\n", runtime_char, getpid(), getClk());
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
                    fprintf(logfile, "At time %d process %d started arr %d total %d remain %d wait %d\n", getClk(), Running2->process->id, Running2->process->arrivaltime, Running2->process->runtime, Running2->process->ReaminingTime, Running2->process->WaitingTime);
                }
                else
                {
                    printf("continuing process\n");
                    kill(Running2->process->processPID, SIGCONT);
                    Running2->process->Status = resumed;
                    Running2->process->WaitingTime = (getClk() - Running2->process->arrivaltime) - (Running2->process->runtime - Running2->process->ReaminingTime);
                    fprintf(logfile, "At time %d process %d resumed arr %d total %d remain %d wait %d\n", getClk(), Running2->process->id, Running2->process->arrivaltime, Running2->process->runtime, Running2->process->ReaminingTime, Running2->process->WaitingTime);
                }
            }
            // something is running

            if (!isEmptyCnode(Running2) && count == quantum) // reached end of quantum
            {
                printf("reached end of quantum, count %d at time %d\n", count, getClk());
                printf("rem time in sched %d\n", Running2->process->ReaminingTime);
                if (Running2->process->ReaminingTime == 0)
                {
                    int status;
                    int rec_child = waitpid(Running2->process->processPID, &status, 0);
                    printf("rec child %d\n", rec_child);
                    Running2->process->Status = finished;

                    processcount--;
                    avg_wait += Running2->process->WaitingTime;
                    wta += (float)(getClk() - Running2->process->arrivaltime) / Running2->process->runtime;
                    Running2->process->WaitingTime = (getClk() - Running2->process->arrivaltime) - (Running2->process->runtime - Running2->process->ReaminingTime);
                    fprintf(logfile, "At time %d process %d finished arr %d total %d remain 0 wait %d TA %d WTA %.2f\n", getClk(), Running2->process->id, Running2->process->arrivaltime, Running2->process->runtime, Running2->process->WaitingTime, getClk() - Running2->process->arrivaltime, (float)(getClk() - Running2->process->arrivaltime) / Running2->process->runtime);

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
                            printf("received: %d at time %d \n", message.process.id, getClk());
                            utilization += message.process.runtime;

                            node *new = newnode(message.process.id, message.process.priority,
                                                message.process.arrival, message.process.runtime, waiting);

                            // CNode* newcnode = newNodeRR(new); //enqueue takes node not cnode
                            enqueueCQ(circularque, new);

                            int rec_value = msgrcv(msqid, &message, sizeof(message.process), 0, IPC_NOWAIT);
                        } // check for new arrivals and addong to tail

                        Running2 = NULL;
                        advanceCQhead(circularque); // move head once
                        // if only one process then it will switch to itself
                    }
                }
            }
        }
        
        destroyCQ(circularque);

        wta /= atoi(argv[3]);
        avg_wait /= atoi(argv[3]);
        utilization /= getClk();
        utilization *= 100;
    }

    if (atoi(argv[1]) == 4) // Multiple level Feedback Loop
    {
        printf("starting mlfl \n");
        signal(SIGUSR1, SIG_IGN); // won't use on termination here, handled internally on quantum end
        // don't rely on process sending signal each quantum, process will be sent a pause/cont each quantum or terminate when rem time = 0
        // PROCESS NOTIFIES SCHEDULAR OF CLK CHANGES
        signal(SIGUSR2, clockchangeMLFL);
        processcount = atoi(argv[3]);
        int quantum = atoi(argv[2]);

        chained_linkedlist *currentchain;
        // MLFLNode* Running3;
        bool endofthisqueuelevel = true;
        bool process_switched = false;
        int currentchain_index = 0;
        MLFL *mlfl = newMLFL();
        printf("args %s %s %s \n", argv[1], argv[2], argv[3]);
        while (processcount > 0)
        {
            printf(" :) \n");
            struct msgbuffer message;
            int rec_value = msgrcv(msqid, &message, sizeof(message.process), 0, !IPC_NOWAIT);
            if(rec_value == -1) printf(" :) \n");
            while (rec_value != -1)
            {
                printf("received: %d at time %d \n", message.process.id, getClk());
                utilization += message.process.runtime;
                node *new = newnode(message.process.id, message.process.priority,
                                    message.process.arrival, message.process.runtime, waiting);

                enqueueMLFL(mlfl, new);
                printf("oi\n");
                rec_value = 1;
                //int rec_value = msgrcv(msqid, &message, sizeof(message.process), 0, IPC_NOWAIT);
                //process_switched = true;
            }
            printf(" :) \n");
            if (endofthisqueuelevel)
            {
                printf("OI1111\n");
                switch (currentchain_index)
                {
                case 0:
                    currentchain = mlfl->Head0;
                    break;
                case 1:
                    currentchain = mlfl->Head1;
                    break;
                case 2:
                    currentchain = mlfl->Head2;
                    break;
                case 3:
                    currentchain = mlfl->Head3;
                    break;
                case 4:
                    currentchain = mlfl->Head4;
                    break;
                case 5:
                    currentchain = mlfl->Head5;
                    break;
                case 6:
                    currentchain = mlfl->Head6;
                    break;
                case 7:
                    currentchain = mlfl->Head7;
                    break;
                case 8:
                    currentchain = mlfl->Head8;
                    break;
                case 9:
                    currentchain = mlfl->Head9;
                    break;
                case 10:
                    currentchain = mlfl->Head10;
                    break;
                default:
                    printf("Out of range");
                    break;
                }
                process_switched = true;
                Running3 = currentchain->Head;
                if (Running3 == NULL)
                printf("OI1\n");
                    grabnextnode_mlfl(currentchain, Running3); // if this current level is empty, go grab next node
                if (Running3 == NULL)
                { // hit that 'double null' so that level of the queue is done and need to advance to next level
                printf("OI2\n");
                    currentchain_index = currentchain_index + 1 % 11;
                    endofthisqueuelevel = true; // end of this queue
                }
                else
                {
                    endofthisqueuelevel = false;
                }
                printf(" :) \n");
            }
            // actual running of a process happens here

            if (Running3 != NULL && process_switched)
            {
                endofthisqueuelevel = false;
                process_switched = false;

                count = 0; // used to countup to each quantum
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

            /////////

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

                    clearfinishedprocesses(mlfl, Running3->process->processpriority);

                    process_switched = true;
                    grabnextnode_mlfl(currentchain, Running3);
                    if (Running3 == NULL)
                    { // hit that 'double null' so that level of the queue is done and need to advance to next level
                        currentchain_index = currentchain_index + 1 % 11;
                        endofthisqueuelevel = true;
                    }
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
                        printf("received: %d at time %d \n", message.process.id, getClk());
                        utilization += message.process.runtime;

                        node *new = newnode(message.process.id, message.process.priority,
                                            message.process.arrival, message.process.runtime, waiting);

                        enqueueMLFL(mlfl, new);

                        int rec_value = msgrcv(msqid, &message, sizeof(message.process), 0, IPC_NOWAIT);
                    }

                    process_switched = true;
                    grabnextnode_mlfl(currentchain, Running3);
                    if (Running3 == NULL)
                    { // hit that 'double null' so that level of the queue is done and need to advance to next level
                        currentchain_index = currentchain_index + 1 % 11;
                        endofthisqueuelevel = true;
                    }
                }
            }

            destroymlfl(mlfl);

            wta /= atoi(argv[3]);
            avg_wait /= atoi(argv[3]);
            utilization /= getClk();
            utilization *= 100;
        }
    }
    printf("exiting scheduler at time %d\n", getClk());

    // printing to .perf file
    fprintf(perf, "CPU utilization= %.2f %% Avg \nWTA =%.2f \nAvg Waiting =%.2f\n", utilization, wta, avg_wait);

    fclose(perf);
    fclose(logfile);

    // TODO: upon termination release the clock resources.
    destroyClk(1);
    return 0;
}

void ProcessTerminated(int signum)
{
    avg_wait += Running->process->WaitingTime;
    wta += (float)(getClk() - Running->process->arrivaltime) / Running->process->runtime;
    // calculated at proccess while it was running
    fprintf(logfile, "At time %d process %d finished arr %d total %d remain 0 wait %d TA %d WTA %.2f\n", getClk(), Running->process->id, Running->process->arrivaltime, Running->process->runtime, Running->process->WaitingTime, getClk() - Running->process->arrivaltime, (float)(getClk() - Running->process->arrivaltime) / Running->process->runtime);
    printf("At time %d process %d finished arr %d total %d remain 0 wait %d TA %d WTA %.2f\n", getClk(), Running->process->id, Running->process->arrivaltime, Running->process->runtime, Running->process->WaitingTime, getClk() - Running->process->arrivaltime, (float)(getClk() - Running->process->arrivaltime) / Running->process->runtime);
    freeinsideQNODE(Running);
    free(Running); // like delete in c++ ? yes it deallocates memory
    Running = NULL;
    processcount--;
    printf("processcount=%d\n", processcount);
}
void clockchangeRR(int signum)
{
    count++;
    Running2->process->ReaminingTime--;
    printf("count: %d rem time %d\n", count, Running2->process->ReaminingTime);
}
void clockchangeMLFL(int signum)
{
    count++;
    Running3->process->ReaminingTime--;
    printf("count: %d rem time %d\n", count, Running3->process->ReaminingTime);
}