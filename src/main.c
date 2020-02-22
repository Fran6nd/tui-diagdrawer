#include <stdlib.h>
#include <curses.h>
#include <stdio.h>

#include "ad_file.h"

position CURSOR_POSITION = {0, 0};
ad_file CURRENT_FILE;

void clear_screen(){
    int x;
    for (x=0; x < COLS; x++){
        int y;
        for(y = 0; y < LINES; y ++){
            move(y, x);
            addch(' ');
        }
    }
}

void draw_file(){
    int x;
    for (x=0; x < COLS; x++){
        int y;
        for(y = 2; y < LINES; y ++){
            move(y, x);
            position p = {x, y-2};
            p.x += CURSOR_POSITION.x;
            p.y += CURSOR_POSITION.y;
            char c = ad_file_get_char(&CURRENT_FILE, p);
            if(c != 0){
                addch(c);
            }
            else{
                addch('#');
            }
        }
    }  
}

void draw(){
    clear_screen();
    move(0, 0);
    addstr("Hello!!!");
    move(1, 0);
    addstr("Second line!");
    draw_file();
}

int main( int argc, char *argv[] )
{
    initscr();
    curs_set(0);
    noecho();
    CURRENT_FILE = ad_file_new(COLS - 2, LINES - 2);
    int looping = 1;
    while (looping)
    {
        draw();
        char c = getch();
        if (c == 'q'){
            looping = 0;
        }
        else if (c == '\033')
        {
            getch();
            switch (getch())
            {
            case 'A':
                CURSOR_POSITION.y++;
                break;
            case 'B':
                CURSOR_POSITION.y--;
                break;
            case 'C':
                CURSOR_POSITION.x++;
                break;
            case 'D':
                CURSOR_POSITION.x--;
                break;
            }
        }

    }
    endwin();
    ad_file_free(&CURRENT_FILE);
    return 0;
}