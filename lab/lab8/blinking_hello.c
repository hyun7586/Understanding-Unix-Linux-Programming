#include	<stdio.h>
#include	<curses.h>
#include	<unistd.h>

int main()
{
	initscr() ;		

				
	clear();
	while(1){
	move(10,20);			
	addstr("Hello, world");		
	move(LINES-1,0);
	usleep(300000);
	refresh();
	
	move(10, 20);
	addstr("            ");
	move(LINES-1, 0);
	usleep(300000);
	refresh();
	}
	endwin();		
}
