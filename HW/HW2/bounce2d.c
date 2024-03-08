#include <stdio.h>
#include <curses.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>

#define MESSAGE "o"
#define BLANK "     "
#define TOP_ROW 5
#define BOT_ROW 20
#define LEFT_EDGE 10
#define RIGHT_EDGE 70
#define TICKS_PER_SEC 50

int row;
int col;
int x_dir;
int y_dir;
int x_delay;
int y_delay;

void move_msg(int);
int set_ticker(int);
void ignore_signal();

int main()
{
    int c;
    void move_msg(int);

    // hide the cursor
    curs_set(0);

    initscr();
    crmode();
    noecho();
    clear();

    row = 10;
    col = 0;
    x_dir = 1;
    y_dir = 1;
    x_delay = y_delay = 50;

    move(row, col);
    addstr(MESSAGE);
    signal(SIGALRM, move_msg);
	ignore_signal();
    set_ticker(x_delay);

    while (1)
    {
        c = getch();
        if (c == 'Q')
            break;
        if (c == ' ')
        {
            x_dir = -x_dir;
            y_dir = -y_dir;
        }
        if (c == 'f' && x_delay > 2)
            x_delay /= 2;
        if (c == 'F' && y_delay > 2)
            y_delay /= 2;
        if (c == 's')
            x_delay *= 2;
        if (c == 'S')
            y_delay *= 2;

        set_ticker(x_delay);
    }
    endwin();
    return 0;
}

void move_msg(int signum)
{
    signal(SIGALRM, move_msg);

    move(row, col);
    addstr(BLANK);

    col += x_dir;
    row += y_dir;

    move(row, col);
    addstr(MESSAGE);
    refresh();
	curs_set(0);

    if (x_dir == -1 && col <= LEFT_EDGE)
        x_dir = 1;
    else if (x_dir == 1 && col + strlen(MESSAGE) >= RIGHT_EDGE)
        x_dir = -1;

    if (y_dir == -1 && row <= TOP_ROW)
        y_dir = 1;
    else if (y_dir == 1 && row >= BOT_ROW)
        y_dir = -1;
}

int set_ticker(int n_msecs)
{
    struct itimerval new_timeset;
    long n_sec, n_usecs;

    n_sec = n_msecs / 1000;
    n_usecs = (n_msecs % 1000) * 1000L;

    new_timeset.it_interval.tv_sec = n_sec;
    new_timeset.it_interval.tv_usec = n_usecs;
    new_timeset.it_value.tv_sec = n_sec;
    new_timeset.it_value.tv_usec = n_usecs;

    return setitimer(ITIMER_REAL, &new_timeset, NULL);
}

void ignore_signal()
{
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
}
