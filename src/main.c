#include <stdlib.h>
#include <curses.h>
#include <stdio.h>

#include "ad_file.h"

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

void draw(){
    clear_screen();
    move(0, 0);
    addstr("Hello!!!");
    move(1, 0);
    addstr("Second line!");
    refresh();
}

int main( int argc, char *argv[] )
{
    initscr();
    curs_set(0);
    noecho();
    ad_file current_file = ad_file_new(COLS, LINES - 2);
    int looping = 1;
    while (looping)
    {
        draw();
        char c = getch();
        if (c == 'q'){
            looping = 0;
        }

    }
    endwin();
    ad_file_free(&current_file);
    return 0;
}