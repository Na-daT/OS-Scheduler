#include "priority_queue.c"
#include "circular_queue.c"

void clearResources();
bool readFile(char *path);
circular_queue *processQueue;
int messageQueue;
size_t numberOfProcesses = 0;

int main(int argc, char *argv[])
{
    if (argc != 3)
        {
            printf("count: %d\n", argc);
            return fprintf(stderr, "Incorrect input\n");
        }

    printf("The process generator has started\n");
    signal(SIGINT, clearResources);
    // TODO Initialization
    printf("signal initilized\n");
    // 1. Read the input file processes.txt and get the processes and their parameters.
    processQueue = create_circular_queue(sizeof(Process));
    if (!readFile("./processes.txt"))
    {
        perror("Error in file, please try again");
        exit(-1);
    }
    // 2. Read the chosen scheduling algorithm and its parameters, if there are any from the argument list.
    // 3. Initiate and create the scheduler and clock processes.
    
    pid_t clockProcess = fork();
    if (clockProcess == 0) // clk process
    {
        // clk code
        // clk.out
        char *argv[] = {"./clk.out", 0};
        execve(argv[0], &argv[0], NULL); // mmkn nst3ml execve lw masht8ltsh!!
    }
    printf("clock process created\n");

    printf("create scheduler\n");
    pid_t schedulerProcess = fork();
    if (schedulerProcess == 0) // scheduler process
       {
        // scheduler code
        // scheduler.out
        char * count = malloc(10);
        sprintf(count, "%ld", numberOfProcesses);
        char *argv[] = {"./scheduler.out", argv[1], argv[2], count, 0};
        printf("right before execve\n");
        execve(argv[0], &argv[0], NULL);
       }

    printf("scheduler process created\n");

    initClk();
    // 4. Use this function after creating the clock process to initialize clock.

    key_t msgKey = ftok("./clk.c", 'Z');
    messageQueue = msgget(msgKey, 0666 | IPC_CREAT);
    if (messageQueue == -1)
    {
        perror("Error in creating message queue");
        exit(-1);
    }

    while (processQueue->size > 0)
    {
        Process *processP = peekCQ(processQueue);
        Process p = *processP;
        while (getClk() < p.arrival)
            ;
        dequeueCQ(processQueue);
        free(processP);

        struct msgbuffer msg;
        msg.mtype = 1;
        msg.process = p;
        int flag = msgsnd(messageQueue, &msg, sizeof(msg.process), !IPC_NOWAIT);
        if (flag == -1)
        {
            perror("error in sending to schedular");
            exit(-1);
        }
        else
        {
            printf("Process %d is sent to schedular", p.id);
            kill(schedulerProcess, SIGCONT);
        }
    }

    wait(NULL);
    clearResources();
    destroyClk(true);
    // To get time use this function.
    // int x = getClk();
    // printf("Current Time is %d\n", x);
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // 6. Send the information to the scheduler at the appropriate time.
    // 7. Clear clock resources
}

void clearResources()
{
    // TODO Clears all resources in case of interruption
    printf("The process generator has stopped\n");
    msgctl(messageQueue, IPC_RMID, NULL);
    destroyClk(true);
    exit(0);
}

bool readFile(char *path)
{
    printf("Reading file ");
    FILE *inFile = fopen(path, "r");
    if (inFile == NULL)
        return false;

    char *line;
    size_t len; //, numberOfProcesses = 0;
    while (getline(&line, &len, inFile) != -1)
        if (line[0] != '#')
        {
            numberOfProcesses++;
            Process *p = malloc(sizeof(*p));
            sscanf(line, "%d\t%d\t%d\t%d=\n", &p->id, &p->arrival, &p->runtime, &p->priority);
            enqueueCQ(processQueue, p);
        }

    fclose(inFile);
    return true;
}