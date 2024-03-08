#include  <stdio.h>
#include  <pthread.h>
#include  <unistd.h>

#define	NUM	5

int     counter = 0;

int main()
{
	pthread_t t1;			/* one thread */
	void	  *print_count(void *); /* its function */
	int       i;

	pthread_create(&t1, NULL, print_count, NULL);
	for( i = 0 ; i<NUM ; i++ ){
		counter++;
		sleep(1);
	}

	pthread_join(t1, NULL);
	
	return 0;
}
void *print_count(void *m)
{
	int i;
	for(i=0 ; i<NUM ; i++){
		printf("count = %d\n", counter);
		sleep(1);
	}
	return NULL;
}
