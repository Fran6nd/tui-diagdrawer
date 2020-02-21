import curses
import copy
import math
from chunk import *
from position import *
from COLORS import *

MODE = 0
MODES = ['SQUARE', 'PUT', 'TEXT', 'SELECT']


canva = list()
x, y = 0, 0
SELECTION = None
charset = 'AZERTYUIOPMLKJHGFDSQWXCVBNazertyuiop mlkjhgfdsqwxcvbn?!\'$",;.:/\\+-*/=|&1230456789'

def _main(stdscr):
    curses.curs_set(0)
    # Clear screen
    curses.init_pair(1, curses.COLOR_WHITE, curses.COLOR_BLACK)
    curses.init_pair(2, curses.COLOR_WHITE, curses.COLOR_RED)
    height, width = stdscr.getmaxyx()
    for i in range(0, width*10):
        canva.append([' '] * (height*10))
    global x
    global y
    x =math.floor(len(canva)/2)
    y =math.floor(len(canva[0])/2)
    c = 0
    p1 = None
    p2 = None
    global MODE
    global MODES
    global buffer
    global SELECTION
    while chr(c) != 'q':
        stdscr.clear()
        draw(stdscr)
        if(MODES[MODE] == 'SQUARE'):
            if p1:
                _min, _max = rect_from_points(p1, point(x, y))
                _max = canva_point_to_screen_point(stdscr,  _max)
                _min = canva_point_to_screen_point(stdscr, _min)
                _max.y = height - _max.y
                _min.y = height - _min.y
                for _x in range(_min.x, _max.x):
                    add_char(stdscr,_min.y, _x, '-')
                for _x in range(_min.x, _max.x):
                    add_char(stdscr,_max.y, _x, '-')
                for _y in range(_max.y, _min.y):
                    add_char(stdscr,_y, _min.x, '|')
                for _y in range(_max.y, _min.y):
                    add_char(stdscr,_y, _max.x, '|')
                add_char(stdscr,_max.y, _max.x, '+')
                add_char(stdscr,_min.y, _max.x, '+')
                add_char(stdscr,_max.y, _min.x, '+')
                add_char(stdscr,_min.y, _min.x, '+')
        elif(MODES[MODE] == 'SELECT'):
            if p1:
                SELECTION = rect(p1, point(x, y))
        draw_selection(stdscr)
        stdscr.refresh()
        c = stdscr.getch()
        if c == curses.KEY_UP:
            y = y + 1
        elif c == curses.KEY_DOWN:
            y = y - 1
        elif c == curses.KEY_RIGHT:
            x = x + 1
        elif c == curses.KEY_LEFT:
            x = x - 1
        elif c == 9:
            p1 = None
            p2 = None
            MODE += 1
            if MODE == len(MODES):
                MODE = 0
        elif c == 27:
            if p1 or p2:
                p1 = None
                p2 = None
            else:
                SELECTION = None
        stdscr.refresh()

        if(MODES[MODE] == 'SQUARE'):
            if(c == 32):
                if p1 == None:
                    p1 = point(x, y)
                else:
                    p2 = point(x, y)
                    x_min = min(p1.x, p2.x)
                    x_max = max(p1.x, p2.x)
                    y_min = min(p1.y, p2.y)
                    y_max = max(p1.y, p2.y)
                    for _x in range(x_min, x_max):
                        canva[_x][y_min] = '-'
                    for _x in range(x_min, x_max):
                        canva[_x][y_max] = '-'
                    for _y in range(y_min, y_max):
                        canva[x_min][_y] = '|'
                    for _y in range(y_min, y_max):
                        canva[x_max][_y] = '|'
                    canva[x_min][y_min] = '+'
                    canva[x_min][y_max] = '+'
                    canva[x_max][y_min] = '+'
                    canva[x_max][y_max] = '+'
                    p1 = None
                    p2 = None
        elif(MODES[MODE] == 'SELECT'):
            if(c == 32):
                if p1 == None:
                    p1 = point(x, y)
                else:
                    p2 = point(x, y)
                    SELECTION = rect(p1, p2)
                    p1 = None
                    p2 = None
        elif(MODES[MODE] == 'TEXT'):
            if(chr(c) in charset):
                canva[x][y] = chr(c)
                x += 1
        elif(MODES[MODE] == 'PUT'):
            if(chr(c) in charset):
                canva[x][y] = chr(c)

def main(stdscr):
    global MODE, MODES
    curses.curs_set(0)
    # Clear screen
    curses.init_pair(COLOR_NORMAL, curses.COLOR_WHITE, curses.COLOR_BLACK)
    curses.init_pair(COLOR_CURSOR, curses.COLOR_WHITE, curses.COLOR_RED)
    curses.init_pair(COLOR_EMPTY, curses.COLOR_BLUE, curses.COLOR_BLUE)
    height, width = stdscr.getmaxyx()
    height -= 2
    main_chunk = chunk(position(width, height))
    looping = True
    pos = position(width/2, height/2)
    center = position(width/2, height/2)
    while looping:
        mode = MODES[MODE]
        stdscr.addstr(0, 0, 'ascii-drawer by Fran6nd.' + str(pos.x) + ' ' + str(pos.y))
        stdscr.addstr(1, 0, '[' + MODES[MODE] + '] (press [tab] to switch mode)')
        main_chunk.draw(stdscr, pos)
        stdscr.addstr(center.y, center.x, chr(stdscr.inch(center.y, center.x) & 0xFF), curses.color_pair(COLOR_CURSOR))
        stdscr.refresh()
        c = stdscr.getch()
        if c == 9:
            p1 = None
            p2 = None
            MODE += 1
            if MODE == len(MODES):
                MODE = 0
        else:
            x, y = 0, 0
            if mode == 'PUT' or mode == 'TEXT' or mode == 'SQUARE':
                if c == curses.KEY_UP:
                    y += 1
                elif c == curses.KEY_DOWN:
                    y += - 1
                elif c == curses.KEY_RIGHT:
                    x += 1
                elif c == curses.KEY_LEFT:
                    x += - 1
                elif chr(c) == 'q':
                    looping = False
                if mode == "PUT" :
                    if chr(c) in charset:
                        main_chunk.set_char(pos, chr(c))
                elif mode == 'TEXT':
                    if c in (10, curses.KEY_ENTER):
                        y -= 1
                    elif chr(c) in charset:
                        main_chunk.set_char(pos, chr(c))
                        x+=1
            pos = pos + position(x, y)
        







curses.wrapper(main)
