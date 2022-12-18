#include "headers.h"

/* Modify this file as needed*/
int remainingtime;

void cont(int signum);
int prevclk;
int conttime=0;

int main(int agrc, char *argv[])
{
    initClk();

    
    signal(SIGCONT,cont);
    kill(getppid(),SIGCONT);

    //TODO The process needs to get the remaining time from somewhere
    remainingtime = atoi(argv[0]); //(quantum time or runtime)

    while (remainingtime > 0)
    {
        prevclk=getClk();
        while (prevclk==getClk()) {} //wait till next clk pulse

        if (conttime != getClk()) //in case of switiching, process gets a signal of new time (conttime) 
            {
                remainingtime--; // decrements total time
                kill(getppid(),SIGUSR2); // for RR, informs scheduler of clk changes (nothing for hpf and sjf)
                printf("clk changed at time %d rem time %d\n",getClk(),remainingtime);
            }
    }

    printf("time at ending for process %d : %d\n",getpid(),getClk());
    kill(getppid(),SIGUSR1); //notify of termintaion

    destroyClk(false);

    return 0;
}

void cont(int signum)
{
    conttime=getClk(); // new time for hpf and rr switiching
    signal(SIGCONT,cont);
}
