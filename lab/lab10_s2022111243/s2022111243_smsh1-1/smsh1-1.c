#include	<stdio.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<signal.h>
#include	"smsh.h"
#include	<string.h>

#define	DFL_PROMPT	"> "

int main()
{
	char	*cmdline, *prompt, **arglist;
	int	result;
	
	void	setup();

	prompt = DFL_PROMPT ;
	setup();

	while ( (cmdline = next_cmd(prompt, stdin)) != NULL ){
	char* command=strtok(cmdline, ";");
	while(command!=NULL){
		if ( (arglist = splitline(command)) != NULL  ){
			result = execute(arglist);
			freelist(arglist);
		}
		command=strtok(NULL, ";");
		}
		free(cmdline);
	}
	return 0;
}

void setup()
{
	signal(SIGINT,  SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
}

void fatal(char *s1, char *s2, int n)
{
	fprintf(stderr,"Error: %s,%s\n", s1, s2);
	exit(n);
}
