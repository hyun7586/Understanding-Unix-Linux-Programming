#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define MAXARGS 20  
#define ARGLEN 100    

void execute(char *arglist[]);
char *makestring(char *buf);

int main()
{
    char *arglist[MAXARGS + 1]; 
    int numargs;                
    char argbuf[ARGLEN];        

	while(1){
        numargs = 0;
        while (numargs < MAXARGS)
        {
            printf("Arg[%d]? ", numargs);
            if (fgets(argbuf, ARGLEN, stdin) == NULL || *argbuf == '\n')
            {
                if (numargs > 0)
                {                 
                    arglist[numargs] = NULL; 
                    execute(arglist);        
                    numargs = 0;             
                }
                if (feof(stdin))
                {
                    printf("Exit the program\n");
                    exit(0); 
                }
            }
            else
            {
                arglist[numargs++] = makestring(argbuf);
                if (numargs >1 && strcmp(arglist[numargs-1], "exit")==0){
                    printf("Exit the program\n");
                    exit(0); 
                }
				if(strcmp(arglist[0], "exit")==0){
					printf("Exit the program\n");
					exit(0);
				}
            }
        }
    }

    return 0;
}

void execute(char *arglist[])
{
    int pid, exitstatus; 

    pid = fork(); 
    switch (pid)
    {
    case -1:
        perror("fork failed");
        exit(1);
    case 0:
        execvp(arglist[0], arglist);
        perror("execvp failed");
        exit(1);
    default:
        while (wait(&exitstatus) != pid)
            ;
        printf("child exited with status %d,%d\n", exitstatus >> 8, exitstatus & 0377);
    }
}

char *makestring(char *buf)
{
    char *cp;

    buf[strlen(buf) - 1] = '\0';
    cp = malloc(strlen(buf) + 1); 
    if (cp == NULL)
    { 
        fprintf(stderr, "no memory\n");
        exit(1);
    }
    strcpy(cp, buf); 
    return cp;       
}
