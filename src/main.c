#include <stdlib.h>
#include <curses.h>
#include <string.h>
#include <stdio.h>

#include "ad_file.h"

#define COL_SELECTION 1
#define COL_EMPTY 2
#define COL_CURSOR 3
#define COL_NORMAL 4

#define MODE_NONE 0
#define MODE_SELECT 1
#define MODE_PUT 2
#define MODE_TEXT 3
#define MODE_RECT 4

position UP_LEFT_CORNER = {0, 0};
ad_file CURRENT_FILE;
int MODE = MODE_PUT;

void clear_screen()
{
    int x;
    for (x = 0; x < COLS; x++)
    {
        int y;
        for (y = 0; y < LINES; y++)
        {
            move(y, x);
            addch(' ');
        }
    }
}

int is_writable(char c)
{
    char *charset = "azertyuiopqsdfghjklmwxcvbn?,.;/:§!\\_-+*=()[]{}^$&1234567890AZERTYUIOPQSDFGHJKLMWXCVBN <>";
    int i;
    for (i = 0; i < strlen(charset); i++)
    {
        if (charset[i] == c)
            return 1;
    }
    return 0;
}

void draw_char(position p, char c, int col)
{
    attron(COLOR_PAIR(col));
    move(p.y, p.x);
    addch(c);
    attroff(COLOR_PAIR(col));
}

void draw_file()
{
    int x;
    for (x = 0; x < COLS; x++)
    {
        int y;
        for (y = 2; y < LINES; y++)
        {
            position pos_on_screen = {x, y};
            position p = {x, y - 2};
            p.x += UP_LEFT_CORNER.x;
            p.y += UP_LEFT_CORNER.y;
            char c = ad_file_get_char(&CURRENT_FILE, p);
            if (c != 0)
            {
                if (pos_on_screen.x == COLS / 2 && pos_on_screen.y == (LINES + 2) / 2)
                {
                    draw_char(pos_on_screen, c, COL_CURSOR);
                }
                else
                {
                    draw_char(pos_on_screen, c, COL_NORMAL);
                }
            }
            else
            {
                draw_char(pos_on_screen, ' ', COL_EMPTY);
            }
        }
    }
}

void draw()
{
    clear_screen();
    draw_file();
    move(0, 0);
    addstr("Hello!!!");
    move(1, 0);
    switch (MODE)
    {
    case MODE_NONE:
        addstr("Press [q] to exit\n"
               "      [p] to enter PUT mode\n"
               "      [s] to enter SELECT mode\n"
               "      [r] to enter RECT mode"
        );
        break;
    
    default:
        addstr("Second line!");
        break;
    }
}

int main(int argc, char *argv[])
{
    initscr();
    curs_set(0);
    noecho();
    start_color();

    init_pair(COL_CURSOR, COLOR_WHITE, COLOR_RED);
    init_pair(COL_NORMAL, COLOR_WHITE, COLOR_BLACK);
    init_pair(COL_EMPTY, COLOR_BLACK, COLOR_BLUE);

    CURRENT_FILE = ad_file_new(COLS - 2, LINES - 2);
    int looping = 1;
    while (looping)
    {
        draw();
        char c = getch();
        if (MODE == MODE_NONE)
        {
            switch (c)
            {
            case 's':
                MODE = MODE_SELECT;
                break;
            case 'p':
                MODE = MODE_PUT;
                break;
            case 'q':
                looping = 0;
                break;
            case 'r':
                MODE = MODE_RECT;
                break;
            case '\033':
                getch();
                getch();
                break;
            default:
                break;
            }
        }
        else
        {
            if (c == 'q')
            {
               MODE = MODE_NONE;
            }
            else if (c == '\033')
            {
                getch();
                switch (getch())
                {
                case 'A':
                    UP_LEFT_CORNER.y--;
                    break;
                case 'B':
                    UP_LEFT_CORNER.y++;
                    break;
                case 'C':
                    UP_LEFT_CORNER.x++;
                    break;
                case 'D':
                    UP_LEFT_CORNER.x--;
                    break;
                }
            }
            else if (is_writable(c))
            {
                position tmp = {UP_LEFT_CORNER.x + COLS / 2, UP_LEFT_CORNER.y + (LINES - 2) / 2};
                ad_file_set_char(&CURRENT_FILE, tmp, c);
            }
        }
    }
    endwin();
    ad_file_free(&CURRENT_FILE);
    return 0;
}