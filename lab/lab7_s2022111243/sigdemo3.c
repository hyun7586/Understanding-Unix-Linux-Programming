/* sigdemo3.c */

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <time.h> 
#include <stdlib.h>

#define INPUTLEN 100

void f(int);
void g(int);
static time_t last_time = 0; 

int main(int ac, char *av[])
{
	last_time=time(NULL);
    signal(SIGINT, f);
    signal(SIGQUIT, g);
    
    printf("you can't stop me!\n");
    while (1)
    {
        sleep(1);
        printf("haha\n");
    }
}

void f(int signum)
{

    time_t current_time = time(NULL); 
    
    int elapsed_time = (int)(current_time - last_time);
    printf("Currently elapsed time : %d sec(s)\n", elapsed_time);
    
}

void g(int signum)
{
	printf("Quit\n");
	exit(0);
}
