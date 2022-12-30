#ifndef HEADERS_H
#define HEADERS_H
#include "headers.h"
#endif

/* Modify this file as needed*/
int remainingtime;

void cont(int signum);
void stop(int signum);

int conttime=0;

int main(int argc, char *argv[])
{
    initClk();
    int prevclk= getClk();
    signal(SIGCONT,cont);
    kill(getppid(),SIGCONT);

    //TODO The process needs to get the remaining time from somewhere
    remainingtime = atoi(argv[0]);
    
    printf("time at beginning for process %d: %d with runtime %d\n",getpid(),prevclk,atoi(argv[0]));

    while (remainingtime>0){
       
      //printf("prev clk %d\n",prevclk);
        while (prevclk==getClk()) {}
        //printf("prevclk=%d getclk %d\n",prevclk,getClk());
        if (conttime != getClk())
            {
              remainingtime--;
              kill(getppid(),SIGUSR2);
              printf("clk changed at time %d rem time %d\n",getClk(),remainingtime);
            } 
          prevclk=getClk();
    }

    printf("time at ending for process %d : %d\n",getpid(),getClk());

    kill(getppid(),SIGUSR1);

    destroyClk(0);
    return 0;
}

void cont(int signum)
{
    //printf("sigcont for process %d at %d remaining time %d\n",getpid(),getClk(),remainingtime);
    conttime=getClk();
    //prevclk=getClk();
    signal(SIGCONT,cont);
}