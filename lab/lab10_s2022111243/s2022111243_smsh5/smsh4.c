// smsh4.c

#include    <stdio.h>
#include    <stdlib.h>
#include    <unistd.h>
#include    <signal.h>
#include    <sys/wait.h>
#include    "smsh.h"
#include    "varlib.h"
#include	<string.h>

#define DFL_PROMPT "> "
int num_of_background_processes=0;

int main()
{
    char *cmdline, *prompt, **arglist;
    int result, process(char **);
    void setup();
	char * temp;
    prompt = DFL_PROMPT;
    setup();
    
    int flag=0;

    while ((cmdline = next_cmd(prompt, stdin)) != NULL) {
    	if(wait(&result)!=-1)
    		flag=1;
        if ((arglist = splitline(cmdline)) != NULL) {
        	int arg_cnt=0;
        	while(arglist[arg_cnt]!=NULL)
        		arg_cnt++;
            if (arglist[0] != NULL) {
                if (arglist[1] != NULL && strcmp(arglist[arg_cnt-1], "&") == 0) {
            		temp=(char*)malloc(sizeof(char) *BUFSIZ);
            		strcpy(temp, cmdline);
                    arglist[1] = NULL; // Remove the "&" from the arguments
                    if (fork() == 0) { // Child process
                    	num_of_background_processes++;
                        result = process(arglist);
                        exit(result);
                    } else { // Parent process
                    	num_of_background_processes++;
                        printf("[%d] %d\n", num_of_background_processes+3, getpid()+2);
                    }
                } else {
                    result = process(arglist);
                    //wait(&result); // Wait for the child process to finish
                }
            }
            freelist(arglist);
        }
        free(cmdline);
        if(flag)
        	printf("[%d]+ Done			%s\n", num_of_background_processes+3, strtok(temp, "&"));
    }
    return 0;
}

void setup()
{
    extern char **environ;

    VLenviron2table(environ);
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
}

void fatal(char *s1, char *s2, int n)
{
    fprintf(stderr, "Error: %s, %s\n", s1, s2);
    exit(n);
}
