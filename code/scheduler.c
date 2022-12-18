#include "headers.h"
#include "pqueue.c"
#include "cqueue.c"

// COORDINATE WITH PROCCESS GENERATOR DUDES:
// ipc message check for correctness TO DO (done)
// message.process structure -> convert message.process to QNode,cnode,noderr type (done)
// adjust constructors if neccesary (done)
// forking parameters for scheduler and process 
// modify file path for message queue key


// remaining of main work:
// make circular queue (done)
// write RR (done)
// write clockchange handler (done)
// check RR Logic regarding advancing head
// check/review equations for calculating time
// check for weird enum definition in the node constructor error (done)
// write MLFL

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

            if(!isEmpty(&Running)){ //something is running
                continue;
            }
            if(isEmpty(&Running) && isEmpty(&pqHead)){ // no proccesses are waiting and nothing is running
                continue;
                // cpu utilzation calc here?? likely no
            } //old code didn't take this into account
            
        }

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

            if(!isEmpty(&Running) && isEmpty(&pqHead)){
                continue;
            }

        wta /= atoi(argv[3]);
        avg_wait /= atoi(argv[3]);
        utilization /= getClk();
        utilization *= 100;

    }

    if (atoi(argv[1]) == 3) //Round Robin (RR) 
    {
        signal(SIGUSR1, SIG_IGN);
        signal(SIGUSR2, clockchange);
        int process_count = atoi(argv[3]);
        circular_queue* circularque_head = create_circular_queue(process_count);
        CNode* Running = NULL;
        int quantum = atoi(argv[2]);
        
        int flag = 0;
        int last_runclk = 0;
        int prevClk = getClk();

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
                
                CNode* newcnode = newNodeRR(new);
                enqueueCQ(circularque_head, newcnode);
                
                rec = msgrcv(msqid, &message, sizeof(message.process), 0, IPC_NOWAIT);
            }

            //add RR LOGIC
            if(isEmptyCnode(&Running) && !isEmptyCQ(circularque_head)){
                Running = peekCQ(circularque_head); //return pointer to process
                count = 0; //used to countup to each quantum
                printf("running process %d\n", Running->process->id);
                quantum = atoi(argv[2]); //updating quantum cause see comment directly below

                if (Running->process->ReaminingTime < quantum) // process's remaining time less than quantum so gonna run it for only that
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
                    kill(Running->process->processPID, SIGCONT); ///////////////////////----------------------------------
                    Running->process->Status = resumed;
                    Running->process->WaitingTime = (getClk() - Running->process->arrivaltime) - (Running->process->runtime - Running->process->ReaminingTime);
                    fprintf(logfile, "At time %d process %d resumed arr %d total %d remain %d wait %d\n", getClk(), Running->process->id, Running->process->arrivaltime, Running->process->runtime, Running->process->ReaminingTime, Running->process->WaitingTime);
                }
            }
            //something is running

            if (!isEmptyCnode(&Running) && count == quantum) //reached end of quantum
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
                    dequeueCQ(circularque_head); 

                    count=0;
                }
                else
                {
                    if (!isEmptyCQ(circularque_head)) //switiching between process
                    {
                        kill(Running->process->processPID, SIGSTOP);
                        Running->process->Status = stopped;
                        Running->process->WaitingTime = (getClk() - Running->process->arrvialtime) - (Running->process->runtime - Running->process->ReaminingTime);
                        fprintf(logfile, "At time %d process %d stopped arr %d total %d remain %d wait %d\n", getClk(), Running->process->id, Running->process->arrvialtime, Running->process->runtime, Running->process->ReaminingTime, Running->process->WaitingTime);
                        // enqueuing arrived processes

                        int rec = msgrcv(msqid, &message, sizeof(message.process), 0, IPC_NOWAIT);
                        while (rec != -1)
                        {
                            printf("received: %d at time %d \n", message.process.id, getClk());
                            utilization += message.process.runtime;

                            node* new = newnode(message.process.id, message.process.Processpriority, 
                            message.process.arrival, message.process.runtime,  waiting);
                            
                            CNode* newcnode = newNodeRR(new);
                            enqueueCQ(circularque_head, newcnode);

                            rec = msgrcv(msqid, &message, sizeof(message.process), 0, IPC_NOWAIT);
                        } //check for new arrivals and add to tail

                        
                        Running = NULL; 
                        advanceCQhead(circularque_head); //move head once
                        //if only one process then it will switch to itself
                    }
                    /*else // rerun old process for another quantum since there is no one else
                    {
                        count = 0;
                        quantum = atoi(argv[2]);
                        if (Running->ReaminingTime < quantum)
                        {
                            quantum = Running->ReaminingTime;
                        }
                    } */
                }
            }
        
        }

        wta /= atoi(argv[3]);
        avg_wait /= atoi(argv[3]);
        utilization /= getClk();
        utilization *= 100;

    }
    if (atoi(argv[1]) == 4) //Multiple level Feedback Loop 
    {
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