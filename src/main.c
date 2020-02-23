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
position P1, P2;
ad_file CURRENT_FILE;
int MODE = MODE_PUT;
int PREVIOUS_MODE = 0;

position get_cursor_pos()
{
    position tmp = {UP_LEFT_CORNER.x + COLS / 2, UP_LEFT_CORNER.y + (LINES - 2) / 2};
    return tmp;
}

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

int is_on_rect_border(position r1, position r2, position p)
{
    position min = min_pos(r1, r2);
    position max = max_pos(r1, r2);
    if (p.x > min.x && p.x < max.x)
        if (p.y == max.y || p.y == min.y)
            return 1;
    if (p.y > min.y && p.y < max.y)
        if (p.x == max.x || p.x == min.x)
            return 1;
    return 0;
}
int is_in_rect(position r1, position r2, position p)
{
    position min = min_pos(r1, r2);
    position max = max_pos(r1, r2);
    if (p.x >= min.x && p.x <= max.x)
        if (p.y <= max.y && p.y >= min.y)
            return 1;
    return 0;
}
int is_on_rect_corner(position r1, position r2, position p)
{
    position down_left = min_pos(r1, r2);
    position up_right = max_pos(r1, r2);
    position up_left = {down_left.x, up_right.y};
    position down_right = {up_right.x, down_left.y};
    if (p.x == down_left.x && p.y == down_left.y)
        return 1;
    if (p.x == up_right.x && p.y == up_right.y)
        return 1;
    if (p.x == up_left.x && p.y == up_left.y)
        return 1;
    if (p.x == down_right.x && p.y == down_right.y)
        return 1;
    return 0;
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
                if (MODE == MODE_RECT)
                {
                    if (P1.null == 0)
                    {
                        if (is_on_rect_corner(P1, get_cursor_pos(), p))
                        {
                            c = '+';
                        }
                        else if (is_on_rect_border(P1, get_cursor_pos(), p))
                        {
                            if (p.y == P1.y || p.y == get_cursor_pos().y)
                            {
                                c = '-';
                            }
                            if (p.x == P1.x || p.x == get_cursor_pos().x)
                            {
                                c = '|';
                            }
                        }
                    }
                }
                if (pos_on_screen.x == COLS / 2 && pos_on_screen.y == (LINES + 2) / 2)
                {
                    draw_char(pos_on_screen, c, COL_CURSOR);
                }
                else
                {
                    position tmp = P2.null == 0 ? P2 : get_cursor_pos();
                    if (MODE == MODE_SELECT && P1.null == 0 && is_in_rect(P1, tmp, p))
                    {
                        draw_char(pos_on_screen, c, COL_SELECTION);
                    }
                    else
                    {
                        draw_char(pos_on_screen, c, COL_NORMAL);
                    }
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
    addstr("ASCII-Drawer by Fran6nd (press [tab] to swith mode)");
    move(1, 0);
    switch (MODE)
    {
    case MODE_NONE:
        addstr("Press [q] to exit\n"
               "      [p] to enter PUT mode\n"
               "      [s] to enter SELECT mode\n"
               "      [r] to enter RECT mode");
        break;
    case MODE_PUT:
        addstr("[PUT MODE] -> move with arrows and set keys as you wish!");
        break;
    case MODE_RECT:
        addstr("[RECT MODE] -> move with arrows, use [space] to set p1 and p2, use [tab][tab] to abort.");
        break;
    case MODE_SELECT:
        if (!P1.null && !P2.null)
        {
            addstr("[SELECT MODE] -> press [space] to select, [f] to fill then give a character to fill with!");
        }
        else
        {
            addstr("[SELECT MODE] -> move with arrows, use [space] to set p1 and p2 or unselect.");
        }

        break;
    default:
        addstr("Current mode not implemented yet.");
        break;
    }
}

int main(int argc, char *argv[])
{
    WINDOW *w = initscr();
    curs_set(0);
    noecho();
    start_color();
    //keypad(w, TRUE);

    init_pair(COL_CURSOR, COLOR_WHITE, COLOR_RED);
    init_pair(COL_NORMAL, COLOR_WHITE, COLOR_BLACK);
    init_pair(COL_EMPTY, COLOR_BLACK, COLOR_BLUE);
    init_pair(COL_SELECTION, COLOR_BLACK, COLOR_BLUE);

    P1.null = 1;
    P2.null = 1;
    CURRENT_FILE = ad_file_new(COLS - 2, LINES - 2);
    int looping = 1;
    while (looping)
    {
        draw();
        char c = getch();
        if (MODE == MODE_NONE)
        {
            P1.null = 1;
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
            case '\t':
                MODE = PREVIOUS_MODE;
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
            if (c == '\t')
            {
                PREVIOUS_MODE = MODE;
                MODE = MODE_NONE;
            }
            else if (MODE == MODE_PUT)
            {
                if (c == '\033')
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
                    position tmp = get_cursor_pos();
                    ad_file_set_char(&CURRENT_FILE, tmp, c);
                }
            }
            else if (MODE == MODE_RECT)
            {
                if (c == '\033')
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
                else if (c == ' ')
                {
                    position tmp = get_cursor_pos();
                    if (P1.null)
                    {
                        P1 = tmp;
                    }
                    else
                    {
                        position down_left = min_pos(P1, tmp);
                        position up_right = max_pos(P1, tmp);
                        position up_left = {down_left.x, up_right.y};
                        position down_right = {up_right.x, down_left.y};
                        ad_file_set_char(&CURRENT_FILE, down_left, '+');
                        ad_file_set_char(&CURRENT_FILE, up_right, '+');
                        ad_file_set_char(&CURRENT_FILE, down_right, '+');
                        ad_file_set_char(&CURRENT_FILE, up_left, '+');
                        int x;
                        for (x = down_left.x + 1; x < down_right.x; x++)
                        {
                            position tmp1 = {x, up_right.y};
                            ad_file_set_char(&CURRENT_FILE, tmp1, '-');
                        }
                        for (x = down_left.x + 1; x < down_right.x; x++)
                        {
                            position tmp1 = {x, down_right.y};
                            ad_file_set_char(&CURRENT_FILE, tmp1, '-');
                        }
                        int y;
                        for (y = down_left.y + 1; y < up_left.y; y++)
                        {
                            position tmp1 = {up_left.x, y};
                            ad_file_set_char(&CURRENT_FILE, tmp1, '|');
                        }
                        for (y = down_left.y + 1; y < up_left.y; y++)
                        {
                            position tmp1 = {up_right.x, y};
                            ad_file_set_char(&CURRENT_FILE, tmp1, '|');
                        }

                        P1.null = 1;
                    }
                }
            }
            else if (MODE == MODE_SELECT)
            {
                if (P1.null || P2.null)
                {
                    if (c == '\033')
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
                    else if (c == ' ')
                    {
                        position tmp = get_cursor_pos();
                        if (P1.null)
                        {
                            P1 = tmp;
                        }
                        else if (P2.null)
                        {
                            P2 = tmp;
                        }
                        else
                        {
                            P1.null = 1;
                            P2.null = 1;
                        }
                    }
                }
                else if (!P2.null)
                {
                    if (c == ' ')
                    {
                        P2.null = 1;
                        P1.null = 1;
                    }
                    else if (c == 'f')
                    {
                        do
                        {
                            c = getch();
                        } while (!is_writable(c));
                        position min = min_pos(P1, P2);
                        position max = max_pos(P1, P2);
                        int x;
                        int y;
                        for (x = min.x; x <= max.x; x++)
                        {
                            for (y = min.y; y <= max.y; y++)
                            {
                                position tmp = {x, y};
                                ad_file_set_char(&CURRENT_FILE, tmp, c);
                            }
                        }
                        P2.null = 1;
                        P1.null = 1;
                    }
                }
            }
        }
    }
    endwin();
    ad_file_free(&CURRENT_FILE);
    return 0;
}