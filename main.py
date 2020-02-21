import curses
import copy
import math
from chunk import *
from position import *

MODE = 0
MODES = ['SQUARE', 'PUT', 'TEXT', 'SELECT']


canva = list()
x, y = 0, 0
SELECTION = None
charset = 'AZERTYUIOPMLKJHGFDSQWXCVBNazertyuiop mlkjhgfdsqwxcvbn?!\'$",;.:/\\+-*/=|&1230456789'


class point():
    def __init__(self, x, y):
        self.x = x
        self.y = y

class rect():
    def __init__(self, p1, p2):
        self.min, self.max = rect_from_points(p1, p2)

def canva_point_to_screen_point(stdscr, p):
    _p = copy.deepcopy(p)
    height, width = stdscr.getmaxyx()
    down_left = point(math.floor(x-width/2),math.floor(y-height/2))
    up_right = point(math.floor(x+width/2),math.floor(y+height/2))
    return point(_p.x - down_left.x, _p.y - down_left.y)



def draw(stdscr):
    global x, y
    stdscr.addstr(0, 0, 'ascii-drawer by Fran6nd.' + str(x) + ' ' + str(y))
    stdscr.addstr(
        1, 0, 'Press [KEY_F1] so switch beween edit modes (' + MODES[MODE] + ' selected)')
    height, width = stdscr.getmaxyx()
    height -= 2
    start_x =math.floor(x - width/2)
    start_y =math.floor(y - height/2)

    for _x in range(0, width-1):
        for _y in range(0, height-1):
            center = point(math.floor(width/2), math.floor(height/2))
            center = canva_point_to_screen_point(stdscr, point(x, y))
            if start_x + _x == x and start_y + _y == y:
                stdscr.addstr(
                    _y+2, _x, canva[start_x + _x][start_y + height + 1 - _y], curses.color_pair(2))
            else:
                stdscr.addstr(
                    _y+2, _x, canva[start_x + _x][start_y + height + 1 - _y], curses.color_pair(1))

def draw_selection(stdscr):
    height, width = stdscr.getmaxyx()
    if(SELECTION):
        _max = canva_point_to_screen_point(stdscr, SELECTION.max)
        _min = canva_point_to_screen_point(stdscr, SELECTION.min)
        _max.y = height - _max.y
        _min.y = height - _min.y
        for _y in range(_max.y, _min.y+1):
            for _x in range(_min.x, _max.x+1):
                if _x > 0 and _x < width:
                    add_char(stdscr,_y, _x, get_char(stdscr,_y, _x), curses.color_pair(2))

def add_char(stdscr,  _y, _x,c, color = None):
    if c:
        height, width = stdscr.getmaxyx()
        height -= 1
        _y = _y + 2
        if(_x > 1 and _x <= width and _y > 1 and _y <= height):
            if color:
                stdscr.addstr(_y, _x, c, color)
            else:
                stdscr.addstr(_y, _x, c)
def get_char(stdscr,  _y, _x):
    height, width = stdscr.getmaxyx()
    #height -= 2
    _y = _y + 2
    if(_x > 1 and _x <= width and _y > 1 and _y <= height):
        return chr(stdscr.inch(_y, _x) & 0xFF)

def rect_from_points(p1, p2):
    x_min = min(p1.x, p2.x)
    x_max = max(p1.x, p2.x)
    y_min = min(p1.y, p2.y)
    y_max = max(p1.y, p2.y)
    return point(x_min, y_min), point(x_max, y_max)

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
    curses.curs_set(0)
    # Clear screen
    curses.init_pair(1, curses.COLOR_WHITE, curses.COLOR_BLACK)
    curses.init_pair(2, curses.COLOR_WHITE, curses.COLOR_RED)
    height, width = stdscr.getmaxyx()
    height -= 2
    stdscr.addstr(0, 0, 'ascii-drawer by Fran6nd.')
    stdscr.addstr(1, 0, '[MODE] (press [tab] to switch mode)')
    main_chunk = chunk(position(width*2, height*2))
    main_chunk.draw(stdscr, position(width, height))


    stdscr.refresh()
    stdscr.getch()



curses.wrapper(main)
