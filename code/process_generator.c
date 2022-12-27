#ifndef HEADERS_H
#define HEADERS_H
#include "headers.h"
#endif

void handler(int signum);

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("wrong arg count\n");
        return 0;
    }
    // assigning a handler to clear ipc resources at termination
    signal(SIGINT, handler);

    // Reading process information from file & entering into an array based on arrival time
    char *filename = argv[1];
    // opening the file
    FILE *fp = fopen(filename, "r");
    int filechar = 0;

    // getting the number of processes in the file
    int size = 0;

    // skipping comment line
    fscanf(fp, "%*[^\n]\n");

    while (fscanf(fp, "%d", &filechar) != EOF)
    {
        size++;
    }
    size /= 4;

    // creating the array which will hold the processes
    struct Process *ProcessArray = (struct Process *)malloc(size * sizeof(struct Process));

    // resetting file pointer
    fseek(fp, 0, SEEK_SET);

    int index = 0;
    // skipping comment line
    fscanf(fp, "%*[^\n]\n");

    while (fscanf(fp, "%d", &filechar) != EOF)
    {
        ProcessArray[index].id = filechar;
        fscanf(fp, "%d", &filechar);
        ProcessArray[index].arrival = filechar;
        fscanf(fp, "%d", &filechar);
        ProcessArray[index].runtime = filechar;
        fscanf(fp, "%d", &filechar);
        ProcessArray[index].priority = filechar;

        index++;
    }

    // closing the file
    fclose(fp);

    char pcount[10] = "0";
    sprintf(pcount, "%d", index);
    // creating a message queue
    int keyid = ftok(".", 65);
    int msqid = msgget(keyid, 0666 | IPC_CREAT);
    // creating the message
    struct msgbuffer message;
    message.mtype = 1; // any arbitary number to send and receive on
    int i = 0;

    // NO FORKING
    system("./clk.out &");

    // establishing communication with the clock module

    char string[100];
    snprintf(string, sizeof(string), "./scheduler.out %s %s %s &", argv[2], argv[3], pcount);
    system(string);

    initClk();
    // loop and send the arrived processes to the scheduler using message queue
    while (i < index)
    {
        message.process = ProcessArray[i];

        while (getClk() < message.process.arrival)
        {
            // sleep(message.process.arrival - getClk());
            //  sleep(1);
        }

        int send_val = msgsnd(msqid, &message, sizeof(message.process), !IPC_NOWAIT);

        if (send_val == -1)
        {
            perror("Failed to send. The error is ");
        }
        printf("sent: %d at time %d\n", message.process.id, getClk());
        printf("I am here in process generator \n");
        i++;
    }

    pause(); // why?
    // releasing communication with the clock module
    destroyClk(1);

    // freeing the dynamically allocated array
    free(ProcessArray);

    // NO FORKING
    // process generator waiting till the exit signal at clock destruction (end of scheduler)

    return 0;
}

// Freeing the ipc resources at termination
void handler(int signum)
{
    // getting the queue ID
    int keyid = ftok(".", 65);
    int msqid = msgget(keyid, 0666 | IPC_CREAT);

    struct msqid_ds *buf;
    // deleting message queue
    msgctl(msqid, IPC_RMID, buf);

    struct shmid_ds *shmbuf;
    // deleting shared memory
    int shmid = shmget(SHKEY, 4, IPC_CREAT | 0644);
    shmctl(shmid, IPC_RMID, shmbuf);

    exit(1);
}