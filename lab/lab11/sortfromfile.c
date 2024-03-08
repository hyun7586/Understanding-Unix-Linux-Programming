#include	<stdio.h>
#include  <stdlib.h>
#include  <unistd.h>
#include  <fcntl.h>
#include  <sys/wait.h>

int main(int argc, char* argv[])
{
	int	pid ;
	int	fd;
	
	if(argc!=2)
	{
		fprintf(stderr, "use ./sortfromfile (file_name)\n");
		exit(1);
	}
	
	printf("About to run sort with input from %s\n", argv[1]);

	/* create a new process or quit */
	if( (pid = fork() ) == -1 ){
		perror("fork"); exit(1);
	}
	/* child does the work */
	if ( pid == 0 ){
		close(0);				/* close, */
		fd = open(argv[1], O_RDONLY );		/* then open */
		if( fd == -1 ){
			perror("open"); exit(1);
		}
		execlp( "sort", "sort", NULL );		/* and run	*/
		perror("execlp");
		exit(1);
	}
	/* parent waits then reports */
	if ( pid != 0 ){
		wait(NULL);
		printf("Done running sort < %s\n", argv[1]);
	}

	return 0;
}
