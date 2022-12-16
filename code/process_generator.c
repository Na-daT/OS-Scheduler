#include "headers.h"
#include "priority_queue.c"
#include "circular_queue.c"

void clearResources(int);

int main(int argc, char *argv[])
{
    if (argc != 2)
        return fprintf(stderr, "Incorrect input\n");

    signal(SIGINT, clearResources);
    // TODO Initialization

    // 1. Read the input file processes.txt and get the processes and their parameters.
    FILE *inFile = fopen("processes.txt", "r");
    if (inFile == NULL)
    {
        perror("Error opening processes file");
        exit(-1);
    }

    char *line;
    size_t len, numberOfProcesses = 0;
    circular_queue *processQueue = create_circular_queue(sizeof(Process));
    while (getline(&line, &len, inFile) != -1)
        if (line[0] != '#')
        {
            numberOfProcesses++;

            Process *p = malloc(sizeof(*p));
            sscanf(line, processFormatString, &p->id, &p->arrivaltime, &p->remainingtime, &p->priority);

            enqueueCQ(processQueue, p);
        }

    fclose(inFile);

    // 2. Read the chosen scheduling algorithm and its parameters, if there are any from the argument list.
    // 3. Initiate and create the scheduler and clock processes.
    // 4. Use this function after creating the clock process to initialize clock.
    initClk();
    // To get time use this function.
    int x = getClk();
    printf("Current Time is %d\n", x);
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // 6. Send the information to the scheduler at the appropriate time.
    // 7. Clear clock resources
    destroyClk(true);
}

void clearResources(int signum)
{
    // TODO Clears all resources in case of interruption
}
