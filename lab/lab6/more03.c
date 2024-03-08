#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>    

#define LINELEN 512

void do_more(FILE *);
int see_more(int screen_rows);

int main(int argc, char *argv[])
{
    FILE *fp;

    if (argc == 1)
        do_more(stdin);
    else
    {
        while (--argc)
        {
            if ((fp = fopen(*++argv, "r")) != NULL)
            {
                do_more(fp);
                fclose(fp);
            }
            else
                exit(1);
        }
    }
    return 0;
}

void do_more(FILE *fp)
{
    char line[LINELEN];
    
    int screen_rows;
    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);
    screen_rows = w.ws_row;	              
    

    int num_of_lines = 0;
    int reply;

    while (fgets(line, LINELEN, fp))
    {
        if (num_of_lines == screen_rows)
        {
            reply = see_more(screen_rows);
            if (reply == 0)
                break;
            num_of_lines -= reply;
        }
        if (fputs(line, stdout) == EOF)
            exit(1);
        num_of_lines++;
    }
}

int see_more(int screen_rows)
{
    int c;

    printf("\033[7m more? \033[m");
    while ((c = getchar()) != EOF)
    {
        if (c == 'q')
            return 0;
        if (c == ' ')
            return screen_rows;
        if (c == '\n')
            return 1;
    }
    return 0;
}
