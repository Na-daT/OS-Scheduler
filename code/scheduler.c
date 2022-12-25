#include "headers.h"
#include "pqueue.c"
#include "cqueue.c"
#include "MLFL.c"

// COORDINATE WITH PROCCESS GENERATOR DUDES:
// forking parameters for scheduler and process 
// modify file path for message queue key


// remaining of main work:
// check/review equations for calculating time
// make sure all nodes after done running are freed (mlfl and rr done)
// debug all

float utilization = 0, avg_wait = 0, wta = 0;

int processcount;
int quantum; 
int count;

void ProcessTerminated(int signum);
void clockchange(int signum);


    FILE *logfile;
    FILE *perf;
    

int main(int argc, char *argv[])
{
    initClk(); 
    int clk = getClk();
    
    printf("Sheduler started\n");
    // creating the message queue
    int keyid = ftok(".", 65);
    int msqid = msgget(keyid, 0666 | IPC_CREAT);
    
    // open the output file
    logfile = fopen("./scheduler.log", "w");
    perf = fopen("./scheduler.perf", "w");

    // printing the header of output file
    fprintf(logfile, "#At time x process y state arr w total z remain y wait k\n");

    //TODO: implement the scheduler.
    
    ///////////////////////////////// SJF //non premitave
    if (atoi(argv[1]) == 1)
    {
        signal(SIGUSR1, ProcessTerminated);
        signal(SIGUSR2,SIG_IGN); // sigusr2 is used in roundrobin 
        //so its useless for this and should be ignored
        processcount = atoi(argv[3]);
        QNode* pqHead = NULL; //headofqueue
        QNode* Running = NULL; // process currently running
        
        while(processcount > 0){

            struct msgbuff message;
            int rec_value = msgrcv(msqid, &message, sizeof(message.process), 0, IPC_NOWAIT);

            while (rec_value != -1) //get all process that arrive at current clk time
            {
                utilization += message.process.runtime;
                printf("received: %d at time %d \n", message.process.id, getClk());
                node* new = newnode(message.process.id, message.process.Processpriority, 
                message.process.arrival, message.process.runtime,  waiting);
                //needs to be checked or modified with message.process
                QNode* newqnode = newNodeSJF(new);
                if(pqHead == NULL) pqHead = newqnode; 
                else push(&pqHead, newqnode); 

                rec_value = msgrcv(msqid, &message, sizeof(message.process), 0, IPC_NOWAIT);
            }

            //int flag = 0; //whats the use for the flag if we are already checking if something is running ???????
            // because he checks if running is empty and then sets variable running
            //and then he wants to immediatly fork while isempty() will return that there is a process running
            // but it hasn't ran yet and its only there from immediate prev running = pop(pqueue)
            // thats why set flag to 1 to notify that the 'running' hasn't actually ran yet and needs to be started/forked  

            if (isEmpty(&Running) && !isEmpty(&pqHead)){ //if nothing is running and there are proccess waiting to run
                // save current process to running
                Running = pqHead;
                pop(&pqHead);
                // fork process
                int pid = fork();
                    if (pid == 0)
                    {
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
                    //rest isn't needed here because not preemptive ???? " - (Running->process->runtime - Running->process->ReaminingTime) " -------------
fprintf(logfile, "At time %d process %d started arr %d total %d remain %d wait %d\n", getClk(), Running->process->id, Running->process->arrivaltime, Running->process->runtime, Running->process->ReaminingTime, Running->process->WaitingTime);
            }
        
        }
        
        destroyPQ(pqHead);

        wta /= atoi(argv[3]);
        avg_wait /= atoi(argv[3]);
        utilization /= getClk();
        utilization *= 100;
        // check if those calcs are correct :to do: ------------------------------
    }

    if (atoi(argv[1]) == 2) //hpf Preemptive 
    {
        signal(SIGUSR1, ProcessTerminated);
        signal(SIGUSR2,SIG_IGN); // sigusr2 is used in roundrobin 
        //so its useless for this and should be ignored
        processcount = atoi(argv[3]);
        QNode* pqHead = NULL; //head of prioqueue
        QNode* Running = NULL; // process currently running
        int last_runclk = 0;

        while(processcount > 0){
            // IMPORTANT TO DO 
            // MAKE SURE PRIOQUEUE IS USING priority AS PRIORITY VALUE

            struct msgbuff message;
            int rec_value = msgrcv(msqid, &message, sizeof(message.process), 0, IPC_NOWAIT);
            
            int newproccess_arrival_flag = 0;
            while (rec_value != -1) //get all process that arrive at current clk time
            {
                newproccess_arrival_flag = 1;
                utilization += message.process.runtime;
                printf("received: %d at time %d \n", message.process.id, getClk());

                node* new = newnode(message.process.id, message.process.Processpriority, 
                message.process.arrival, message.process.runtime,  waiting);
                
                QNode* newqnode = newNodeHPF(new);
                if(pqHead == NULL) pqHead =  newqnode; //needs to be checked or modified with message.process
                else push(&pqHead, newqnode); 

                rec_value = msgrcv(msqid, &message, sizeof(message.process), 0, IPC_NOWAIT);
            }

            int process_switched_flag = 0;

            if(isEmpty(&Running) && !isEmpty(&pqHead)){
                Running = pqHead;
                pop(&pqHead);
                process_switched_flag = 1;
            }
            
            // if there isn't a process running and a new process arrives 
            // it will be placed in its correct place (order of running) therefore
            //next if condition will not affect it because the list was ordered correctly in the first place
            // ex : if arrived placed at front of prioqueue when check happens it will have higher prio than the current head
            // (current head was after it in queue)
            //if not first then same situation above applies in reverse so everything will be okay still

            if(!isEmpty(&Running) && !isEmpty(&pqHead) && newproccess_arrival_flag == 1){
                // peek and compare and change if necessary
                if (pqHead->process->Qpriority < Running->process->Qpriority){

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

            if(process_switched_flag == 1){
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
                    last_runclk = getClk();         //check waiting time equation correctness
                    Running->process->WaitingTime = (getClk() - Running->process->arrivaltime) - (Running->process->runtime - Running->process->ReaminingTime);
                    fprintf(logfile, "At time %d process %d started arr %d total %d remain %d wait %d\n", getClk(), Running->process->id, Running->process->arrivaltime, Running->process->runtime, Running->process->ReaminingTime, Running->process->WaitingTime);
                }
                else //this proccess was created and only needed to be continued
                {
                    kill(Running->process->processPID, SIGCONT);
                    Running->process->Status = resumed;
                    last_runclk = getClk();
                     //check waiting time equation correctness
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

    if (atoi(argv[1]) == 3) //Round Robin (RR) 
    {
        signal(SIGUSR1, SIG_IGN); // won't use on termination here, handled internally on quantum end
        // don't rely on process sending signal each quantum, process will be sent a pause/cont each quantum or terminate when rem time = 0
        // PROCESS NOTIFIES SCHEDULAR OF CLK CHANGES
        signal(SIGUSR2, clockchange);
        int process_count = atoi(argv[3]);
        circular_queue* circularque = create_circular_queue(process_count);
        CNode* Running = NULL;
        int quantum = atoi(argv[2]);

        while (process_count > 0)
        {
            struct msgbuff message;
            int rec = msgrcv(msqid, &message, sizeof(message.process), 0, IPC_NOWAIT);
            while (rec != -1)
            {
                printf("received: %d at time %d \n", message.process.id, getClk());
                utilization += message.process.runtime;
                node* new = newnode(message.process.id, message.process.Processpriority, 
                message.process.arrival, message.process.runtime,  waiting);
                
                //CNode* newcnode = newNodeRR(new); //enqueue takes node not cnode
                enqueueCQ(circularque, new);
                
                rec = msgrcv(msqid, &message, sizeof(message.process), 0, IPC_NOWAIT);
            }

            //add RR LOGIC
            if(isEmptyCnode(Running) && !isEmptyCQ(circularque)){
                Running = peekCQ(circularque); //return pointer to process
                count = 0; //used to countup to each quantum
                printf("running process %d\n", Running->process->id);
                quantum = atoi(argv[2]); //updating quantum cause see comment directly below

                if (Running->process->ReaminingTime < quantum) //process's remaining time less than quantum so gonna run it for only that
                {
                    quantum = Running->process->ReaminingTime;
                }

                if (Running->process->processPID == -1)
                {
                    printf("forking\n");
                    int pid = fork();
                    if (pid == 0)
                    {
                        char *runtime_char = malloc(sizeof(char));
                        sprintf(runtime_char, "%d", Running->process->runtime);
                        // printf("runtime %s of process %d time rn %d\n", runtime_char, getpid(), getClk());
                        char *arg[] = {runtime_char, NULL};
                        int execute = execv("./process.out", arg);
                        if (execute == -1)
                            printf("failed to execute process\n");
                        perror("The error is: \n");
                        exit(-1);
                    }
                    Running->process->processPID = pid;
                    Running->process->Status = started;
                    Running->process->WaitingTime = (getClk() - Running->process->arrivaltime) - (Running->process->runtime - Running->process->ReaminingTime);
                    raise(SIGSTOP);
                    fprintf(logfile, "At time %d process %d started arr %d total %d remain %d wait %d\n", getClk(), Running->process->id, Running->process->arrivaltime, Running->process->runtime, Running->process->ReaminingTime, Running->process->WaitingTime);
                }
                else
                {
                    printf("continuing process\n");
                    kill(Running->process->processPID, SIGCONT); 
                    Running->process->Status = resumed;
                    Running->process->WaitingTime = (getClk() - Running->process->arrivaltime) - (Running->process->runtime - Running->process->ReaminingTime);
                    fprintf(logfile, "At time %d process %d resumed arr %d total %d remain %d wait %d\n", getClk(), Running->process->id, Running->process->arrivaltime, Running->process->runtime, Running->process->ReaminingTime, Running->process->WaitingTime);
                }
            }
            //something is running

            if (!isEmptyCnode(Running) && count == quantum) //reached end of quantum
            {
                printf("reached end of quantum, count %d at time %d\n", count, getClk());
                printf("rem time in sched %d\n", Running->process->ReaminingTime);
                if (Running->process->ReaminingTime == 0)
                {
                    int status;
                    int rec_child = waitpid(Running->process->processPID, &status, 0);
                    printf("rec child %d\n", rec_child);
                    Running->process->Status = finished;

                    process_count--;
                    avg_wait += Running->process->WaitingTime;
                    wta += (float)(getClk() - Running->process->arrivaltime) / Running->process->runtime;
                    Running->process->WaitingTime = (getClk() - Running->process->arrivaltime) - (Running->process->runtime - Running->process->ReaminingTime);
                    fprintf(logfile, "At time %d process %d finished arr %d total %d remain 0 wait %d TA %d WTA %.2f\n", getClk(), Running->process->id, Running->process->arrivaltime, Running->process->runtime, Running->process->WaitingTime, getClk() - Running->process->arrivaltime, (float)(getClk() - Running->process->arrivaltime) / Running->process->runtime);
                    
                    Running = NULL; 
                    dequeueCQ(circularque); //free head (running is always at head) and advances head pointer if appropriate obviously
                }
                else
                {
                    if (!isEmptyCQ(circularque)) //switiching between process
                    {
                        kill(Running->process->processPID, SIGSTOP);
                        Running->process->Status = stopped;
                        Running->process->WaitingTime = (getClk() - Running->process->arrvialtime) - (Running->process->runtime - Running->process->ReaminingTime);
                        fprintf(logfile, "At time %d process %d stopped arr %d total %d remain %d wait %d\n", getClk(), Running->process->id, Running->process->arrvialtime, Running->process->runtime, Running->process->ReaminingTime, Running->process->WaitingTime);

                        int rec = msgrcv(msqid, &message, sizeof(message.process), 0, IPC_NOWAIT);
                        while (rec != -1)
                        {
                            printf("received: %d at time %d \n", message.process.id, getClk());
                            utilization += message.process.runtime;

                            node* new = newnode(message.process.id, message.process.Processpriority, 
                            message.process.arrival, message.process.runtime,  waiting);
                            
                            //CNode* newcnode = newNodeRR(new); //enqueue takes node not cnode
                            enqueueCQ(circularque, new);

                            rec = msgrcv(msqid, &message, sizeof(message.process), 0, IPC_NOWAIT);
                        } //check for new arrivals and addong to tail

                        
                        Running = NULL; 
                        advanceCQhead(circularque); //move head once
                        //if only one process then it will switch to itself
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
    
    if (atoi(argv[1]) == 4) //Multiple level Feedback Loop 
    {
        signal(SIGUSR1, SIG_IGN); // won't use on termination here, handled internally on quantum end
        // don't rely on process sending signal each quantum, process will be sent a pause/cont each quantum or terminate when rem time = 0
        // PROCESS NOTIFIES SCHEDULAR OF CLK CHANGES
        signal(SIGUSR2, clockchange);
        int process_count = atoi(argv[3]);
        int quantum = atoi(argv[2]);

        chained_linkedlist* currentchain;
        MLFLNode* Running;
        bool endofthisqueuelevel = true;
        bool process_switched = false;
        int currentchain_index = 0;
        MLFL* mlfl = newMLFL();

        while (process_count > 0){

            struct msgbuff message;
            int rec = msgrcv(msqid, &message, sizeof(message.process), 0, IPC_NOWAIT);
            while (rec != -1)
            {
                printf("received: %d at time %d \n", message.process.id, getClk());
                utilization += message.process.runtime;
                node* new = newnode(message.process.id, message.process.Processpriority, 
                message.process.arrival, message.process.runtime,  waiting);
                
                enqueueMLFL(mlfl, new);
                
                rec = msgrcv(msqid, &message, sizeof(message.process), 0, IPC_NOWAIT);
                //process_switched = true;
            }

            if(endofthisqueuelevel){
            switch (currentchain_index) {
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
                Running = currentchain->listHead->Head;
                if(Running == NULL) grabnextnode_mlfl(currentchain, Running); //if this current level is empty, go grab next node
                if(Running == NULL) { //hit that 'double null' so that level of the queue is done and need to advance to next level
                    currentchain_index = currentchain_index +1 % 11;
                    endofthisqueuelevel = true; // end of this queue
                }
                else {
                    endofthisqueuelevel = false;
                }
            }
            //actual running of a process happens here

            if(Running != NULL && process_switched){ 
                endofthisqueuelevel = false;
                process_switched = false;
                
                count = 0; //used to countup to each quantum
                printf("running process %d\n", Running->process->id);
                quantum = atoi(argv[2]); //updating quantum cause see comment directly below

                if (Running->process->ReaminingTime < quantum) //process's remaining time less than quantum so gonna run it for only that
                {
                    quantum = Running->process->ReaminingTime;
                }

                if (Running->process->processPID == -1)
                {
                    printf("forking\n");
                    int pid = fork();
                    if (pid == 0)
                    {
                        char *runtime_char = malloc(sizeof(char));
                        sprintf(runtime_char, "%d", Running->process->runtime);
                        // printf("runtime %s of process %d time rn %d\n", runtime_char, getpid(), getClk());
                        char *arg[] = {runtime_char, NULL};
                        int execute = execv("./process.out", arg);
                        if (execute == -1)
                            printf("failed to execute process\n");
                        perror("The error is: \n");
                        exit(-1);
                    }
                    Running->process->processPID = pid;
                    Running->process->Status = started;
                    Running->process->WaitingTime = (getClk() - Running->process->arrivaltime) - (Running->process->runtime - Running->process->ReaminingTime);
                    raise(SIGSTOP);
                    fprintf(logfile, "At time %d process %d started arr %d total %d remain %d wait %d\n", getClk(), Running->process->id, Running->process->arrivaltime, Running->process->runtime, Running->process->ReaminingTime, Running->process->WaitingTime);
                }
                else
                {
                    printf("continuing process\n");
                    kill(Running->process->processPID, SIGCONT); 
                    Running->process->Status = resumed;
                    Running->process->WaitingTime = (getClk() - Running->process->arrivaltime) - (Running->process->runtime - Running->process->ReaminingTime);
                    fprintf(logfile, "At time %d process %d resumed arr %d total %d remain %d wait %d\n", getClk(), Running->process->id, Running->process->arrivaltime, Running->process->runtime, Running->process->ReaminingTime, Running->process->WaitingTime);
                }
            }

            /////////

            if (Running != NULL && count == quantum) //reached end of quantum
            {
                printf("reached end of quantum, count %d at time %d\n", count, getClk());
                printf("rem time in sched %d\n", Running->process->ReaminingTime);
                if (Running->process->ReaminingTime == 0)
                {
                    int status;
                    int rec_child = waitpid(Running->process->processPID, &status, 0);
                    printf("rec child %d\n", rec_child);
                    Running->process->Status = finished;

                    process_count--;
                    avg_wait += Running->process->WaitingTime;
                    wta += (float)(getClk() - Running->process->arrivaltime) / Running->process->runtime;
                    Running->process->WaitingTime = (getClk() - Running->process->arrivaltime) - (Running->process->runtime - Running->process->ReaminingTime);
                    fprintf(logfile, "At time %d process %d finished arr %d total %d remain 0 wait %d TA %d WTA %.2f\n", getClk(), Running->process->id, Running->process->arrivaltime, Running->process->runtime, Running->process->WaitingTime, getClk() - Running->process->arrivaltime, (float)(getClk() - Running->process->arrivaltime) / Running->process->runtime);
                    
                    clearfinishedprocesses(mlfl, Running->process->processpriority);

                    process_switched = true;
                    grabnextnode_mlfl(currentchain, Running);
                    if(Running == NULL) { //hit that 'double null' so that level of the queue is done and need to advance to next level
                        currentchain_index = currentchain_index +1 % 11;
                        endofthisqueuelevel = true;
                    }
                }
                else
                {
                    //switiching between process
                        kill(Running->process->processPID, SIGSTOP);
                        Running->process->Status = stopped;
                        Running->process->WaitingTime = (getClk() - Running->process->arrivaltime) - (Running->process->runtime - Running->process->ReaminingTime);
                        fprintf(logfile, "At time %d process %d stopped arr %d total %d remain %d wait %d\n", getClk(), Running->process->id, Running->process->arrivaltime, Running->process->runtime, Running->process->ReaminingTime, Running->process->WaitingTime);

                        int rec = msgrcv(msqid, &message, sizeof(message.process), 0, IPC_NOWAIT);
                        while (rec != -1)
                        {
                            printf("received: %d at time %d \n", message.process.id, getClk());
                            utilization += message.process.runtime;

                            node* new = newnode(message.process.id, message.process.Processpriority, 
                            message.process.arrival, message.process.runtime,  waiting);
                            
                            enqueueMLFL(mlfl, new);

                            rec = msgrcv(msqid, &message, sizeof(message.process), 0, IPC_NOWAIT);
                        } 

                        process_switched = true;
                        grabnextnode_mlfl(currentchain, Running);
                        if(Running == NULL) { //hit that 'double null' so that level of the queue is done and need to advance to next level
                            currentchain_index = currentchain_index +1 % 11;
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

    printf("exiting scheduler at time %d\n", getClk());

    // printing to .perf file
    fprintf(perf, "CPU utilization= %.2f %% Avg \nWTA =%.2f \nAvg Waiting =%.2f\n", utilization, wta, avg_wait);

    fclose(perf);
    fclose(logfile);

    //TODO: upon termination release the clock resources.
    destroyClk(1);
}

void ProcessTerminated(int signum)
{
    avg_wait += Running->process->WaitingTime;
    wta += (float)(getClk() - Running->process->process.arrivaltime) / Running->process->process.runtime;
    // calculated at proccess while it was running
    fprintf(logfile, "At time %d process %d finished arr %d total %d remain 0 wait %d TA %d WTA %.2f\n", getClk(), Running->process->process.id, Running->process->process.arrivaltime, Running->process->process.runtime, Running->process->WaitingTime, getClk() - Running->process->process.arrivaltime, (float)(getClk() - Running->process->process.arrivaltime) / Running->process->process.runtime);
    printf("At time %d process %d finished arr %d total %d remain 0 wait %d TA %d WTA %.2f\n", getClk(), Running->process->process.id, Running->process->process.arrivaltime, Running->process->process.runtime, Running->process->WaitingTime, getClk() - Running->process->process.arrivaltime, (float)(getClk() - Running->process->process.arrivaltime) / Running->process->process.runtime);
    freeinsideQNODE(Running);
    free(Running); // like delete in c++ ? yes it deallocates memory
    Running = NULL;
    process_count--;
    printf("process_count=%d\n", process_count);

}
void clockchange(int signum){
    count++;
    Running->ReaminingTime--;
    printf("count: %d rem time %d\n", count, Running->ReaminingTime);
    
}