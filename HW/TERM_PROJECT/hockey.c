#include <stdio.h>
#include <curses.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <stdlib.h>
#include <sys/wait.h>

//공 모양 설정
#define MESSAGE "o"
#define BLANK "     "

//윈도우 크기, 골대크기 설정
#define TOP_ROW 2
#define BOT_ROW LINES-2
#define LEFT_EDGE 3
#define RIGHT_EDGE COLS-3
#define UpperPost LINES / 2 + 2
#define LowerPost LINES / 2 - 2

//END_SCORE를 먼저 내는 사람이 이김.
#define END_SCORE 3	

//tick 설정
#define TICKS_PER_SEC 50


//player struct
typedef struct
{
    int x;
    int y;
    int score;
} Player;

Player p1, p2;

// ball features
int row;
int col;
int x_dir;
int y_dir;
int x_delay;
int y_delay;

//global var
int goal_in = 0;
int countdown = 3;
int winner=0;
pid_t pid;

void draw();
void draw_walls();
void init();
int set_ticker(int n_msecs);
void ignore_signal();
void draw_goal_message();
void countdown_and_reset();
void game_over_and_exit();


int main()
{
    int c;

    // hide the cursor
    curs_set(0);

    initscr();
    crmode();
    noecho();
    clear();
    ignore_signal();

    init();

    // 공의 초기 위치를 그림
    mvprintw(row, col, MESSAGE);

    // 시그널 핸들러 설정
    signal(SIGALRM, draw);
    set_ticker(x_delay);
    draw();
	
	//fork
	pid=fork();
	
	if(pid==0){
	//chile process
		if(winner){
			draw();
			game_over_and_exit();
		}
	}
	else if(pid){
	//parent process
		while (p1.score<END_SCORE && p2.score <END_SCORE)
		{
			//게임의 전반적인 진행을 담당하는 부분 : END_SCORE 도달할 때까지 반복
			draw();
		    usleep(1000000);
		   
		   	//하키스틱의 움직임을 조절하는 부분
		   	//Player 1 : w, a, s, d
		   	//Player 2 : i, j, k, l
		    int ch=getch();
		    switch (ch)
		    {
		    // player 1
		    case 'w':
		        if (p1.y > 1){
		        	mvprintw(p1.y, p1.x, BLANK);
		            p1.y--;
		            mvprintw(p1.y, p1.x, "|");
		            }
		            
		        break;
		    case 's':
		        if (p1.y < LINES - 2){
		        	mvprintw(p1.y, p1.x, BLANK);
		            p1.y++;
		            mvprintw(p1.y, p1.x, "|");
		            }
		        break;
		    case 'a':
		        if (p1.x > 1){
		        	mvprintw(p1.y, p1.x, BLANK);
		            p1.x--;
		            mvprintw(p1.y, p1.x, "|");
		            }
		        break;
		    case 'd':
		        if (p1.x < COLS / 2 - 1){
		        	mvprintw(p1.y, p1.x, BLANK);
		            p1.x++;
		            mvprintw(p1.y, p1.x, "|");
		            }
		        break;

		    // player 2
		    case 'i':
		        if (p2.y > 1){
		            mvprintw(p2.y, p2.x, BLANK);
		            p2.y--;
		            mvprintw(p2.y, p2.x, "|");
		            }
		        break;
		    case 'k':
		        if (p2.y < LINES - 2){
		            mvprintw(p2.y, p2.x, BLANK);
		            p2.y++;
		            mvprintw(p2.y, p2.x, "|");
		            }
		        break;
		    case 'j':
		        if (p2.x > COLS / 2 + 1){
		        	mvprintw(p2.y, p2.x, BLANK);
		            p2.x--;
		            mvprintw(p2.y, p2.x, "|");
		            }
		        break;
		    case 'l':
		        if (p2.x < COLS - 2){
		        	mvprintw(p2.y, p2.x, BLANK);
		            p2.x++;
		            mvprintw(p2.y, p2.x, "|");
		            }
		        break;
		    case 'z':
		    	x_delay*=2;
		    	break;
		    case 'm':
		    	x_delay/=2;
		    	break;
		    
		    case 'Q':
		    // Q 입력받으면 강제종료
		    	endwin();
		    	exit(1);
		   
		    }
		    set_ticker(x_delay);
		}
		//child processs 수행을 기다림.
		waitpid(pid, NULL, 0);
		endwin();
		
		//윈도우가 닫히고 어떤 플레이어가 이겼는지 출력해 줌.
		printf("Plyer %d win !\n", winner);
	}
	else{
		perror("fork Fail!\n");
		return -1;
	}
    return 0;
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

void draw()
{
    signal(SIGALRM, draw); // 시그널 핸들러 재설정

    // 현재 위치에 있는 공 및 스틱을 지움
    mvprintw(row, col, BLANK);

    mvprintw(p1.y, p1.x, "|");
    mvprintw(p2.y, p2.x, "|");

	
    // 공의 새로운 위치 계산
    col += x_dir;
    row += y_dir;

    // 새로운 위치에 벽과 공 및 스틱을 그림
    draw_walls();
    mvprintw(row, col, MESSAGE);

    // 점수 출력
    mvprintw(0, COLS / 2 - 30, "Player 1: %d", p1.score);
    mvprintw(0, COLS / 2 + 10, "Player 2: %d", p2.score);

    refresh();
    curs_set(0);

	if(winner){
		mvprintw(LINES/2, COLS/2 - 5, "Game Over");
		mvprintw(LINES/2+1, COLS/2 - 10, "Press any key to exit");
		return;
	}

    // 벽에 부딪힌 경우 방향을 반대로 변경
    if (x_dir == -1 && col <= LEFT_EDGE)
        x_dir = 1;
    else if (x_dir == 1 && col + strlen(MESSAGE) >= RIGHT_EDGE)
        x_dir = -1;

    if (y_dir == -1 && row <= TOP_ROW)
        y_dir = 1;
    else if (y_dir == 1 && row >= BOT_ROW)
        y_dir = -1;
    
    
    //공이 스틱에 부딪힌 경우
    
    int new_col=col + x_dir;
    int new_row=row;
      if ((new_col == p1.x - 1 && (new_row >= p1.y - 1 && new_row <= p1.y + 1)) ||
        (new_col == p2.x + 1 && (new_row >= p2.y - 1 && new_row <= p2.y + 1)) ||
        ((new_col >= p1.x - 1 && new_col <= p1.x + 1) && (new_row >= p1.y - 1 && new_row <= p1.y + 1)) ||
        ((new_col >= p2.x - 1 && new_col <= p2.x + 1) && (new_row >= p2.y - 1 && new_row <= p2.y + 1))){
        	x_dir = -x_dir;
        	if(y_dir==0)
        		y_dir=1;
        	}
        
        
    
    if(LowerPost <= new_row && new_row <= UpperPost && new_col==RIGHT_EDGE-1){
    	p1.score+=1;
    	goal_in = 1;
    	if(p1.score ==END_SCORE){
    		winner=1;
    		return;
    	}
        draw_goal_message();
        countdown_and_reset();
        }
    else if(LowerPost <= new_row && new_row <= UpperPost && new_col==LEFT_EDGE){
    	p2.score+=1;
    	goal_in = 2;
    	if(p2.score ==END_SCORE){
    		winner=2;
    		return;
    	}
        draw_goal_message();
        countdown_and_reset();
        }
    
    
}

void draw_walls()
{
    for (int i = 0; i < LowerPost; i++)
        mvprintw(i, 0, "||");
    for (int i = UpperPost + 1; i < LINES; i++)
        mvprintw(i, 0, "||");

    for (int i = 0; i < LowerPost; i++)
        mvprintw(i, COLS - 2, "||");
    for (int i = UpperPost + 1; i < LINES; i++)
        mvprintw(i, COLS - 2, "||");

    for (int i = LowerPost; i <= UpperPost; i++)
    {
        mvprintw(i, 0, "#");
    }

    for (int i = LowerPost; i <= UpperPost; i++)
    {
        mvprintw(i, COLS - 1, "#");
    }

    for (int i = 0; i < COLS; i++)
    {
        mvprintw(0, i, "=");
        mvprintw(LINES - 1, i, "=");
    }

    for (int i = 1; i <= 5; i++)
    {
        mvprintw(i, COLS / 2, "|");
        mvprintw(LINES - 1 - i, COLS / 2, "|");
    }
}

void init()
{
    row = LINES/2;
    col = COLS/2;
    x_dir = -1;
    y_dir = 0;
    x_delay = y_delay=100;

    p1.x = COLS / 4;
    p1.y = LINES / 2;
    p1.score = 0;

    p2.x = 3 * COLS / 4;
    p2.y = LINES / 2;
    p2.score = 0;
    
    mvprintw(p1.y, p1.x, "|");
    mvprintw(p2.y, p2.x, "|");
}

void draw_goal_message() {
    //clear();  // 화면 초기화

    // "GOAL IN" 메시지를 화면 중앙에 출력
    mvprintw(LINES / 2, COLS / 2 - 4, "GOAL IN");
    refresh();
}

void countdown_and_reset() {
    // 3초 카운트다운
    while (countdown > 0) {
        mvprintw(LINES / 2+1, COLS / 2, "%d", countdown);
        refresh();
        sleep(1);
        countdown--;
    }

    // 스틱 초기 위치로 이동
    p1.y = LINES / 2;
    p2.y = LINES / 2;

    // 공을 골을 먹힌 플레이어 앞에 멈춘 상태로 설정
    row = LINES / 2;
    col = COLS/2;
    x_dir = (goal_in==1) ?  1 : -1;
    y_dir = 0;
    
    p1.x = COLS / 4;
    p1.y = LINES / 2;

    p2.x = 3 * COLS / 4;
    p2.y = LINES / 2;

    // 초기화 후 1초 대기
    refresh();
    sleep(1);

    // 초기화된 상태에서 스틱을 조작할 수 있도록 타이머 재설정
    set_ticker(x_delay);
    goal_in = 0;
    countdown = 3;
    clear();
    draw();  // 초기 상태 그리기
    
}

void game_over_and_exit() {
    if (pid > 0) {
        kill(pid, SIGTERM);  // Terminate the child process
    }

    refresh();
    sleep(5);
    exit(0);
}

