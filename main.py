import curses
import copy

MODE = 0
MODES = ['SQUARE', 'PUT', 'TEXT']


canva = list()
x, y = 0, 0


class point():
    def __init__(self, x, y):
        self.x = x
        self.y = y

def canva_point_to_screen_point(stdscr, p):
    _p = copy.deepcopy(p)
    height, width = stdscr.getmaxyx()
    #height -= 2
    down_left = point(int(x-width/2), int(y-height/2))
    up_right = point(int(x+width/2), int(y+height/2))
    return point(_p.x - down_left.x, _p.y - down_left.y)



def draw(stdscr):
    global x, y
    stdscr.addstr(0, 0, 'ascii-drawer by Fran6nd.' + str(x) + ' ' + str(y))
    stdscr.addstr(
        1, 0, 'Press [KEY_F1] so switch beween edit modes (' + MODES[MODE] + ' selected)')
    height, width = stdscr.getmaxyx()
    height -= 2
    start_x = int(x - width/2)
    start_y = int(y - height/2)

    for _x in range(0, width-1):
        for _y in range(0, height-1):
            if start_x + _x == x and start_y + _y == y:
                stdscr.addstr(
                    _y+2, _x, canva[start_x + _x][start_y + height + 1 - _y], curses.color_pair(2))
            else:
                stdscr.addstr(
                    _y+2, _x, canva[start_x + _x][start_y + height + 1 - _y], curses.color_pair(1))

def add_char(stdscr,  _y, _x,c, color = None):
    height, width = stdscr.getmaxyx()
    height -= 2
    if(_x > 1 and _x <= width and _y > 1 and _y <= height):
        stdscr.addstr(_y, _x, c)

def main(stdscr):
    curses.curs_set(0)
    # Clear screen
    curses.init_pair(1, curses.COLOR_WHITE, curses.COLOR_BLACK)
    curses.init_pair(2, curses.COLOR_WHITE, curses.COLOR_RED)
    height, width = stdscr.getmaxyx()
    for i in range(0, width*10):
        canva.append([' '] * (height*10))
    global x
    global y
    x = int(len(canva)/2)
    y = int(len(canva[0])/2)
    c = 0
    p1 = None
    p2 = None
    global MODE
    global MODES
    global buffer
    while chr(c) != 'q':
        stdscr.clear()
        draw(stdscr)
        if(MODES[MODE] == 'SQUARE'):
            if p1:
                x_min = min(p1.x, x)
                x_max = max(p1.x, x)
                y_min = min(p1.y-2, y-2)
                y_max = max(p1.y-2, y-2)
                _max = point(x_max, y_max)
                _min = point(x_min, y_min)
                _max = canva_point_to_screen_point(stdscr,  _max)
                _min = canva_point_to_screen_point(stdscr, _min)
                _max.y = height - _max.y
                _min.y = height - _min.y
                for _x in range(_min.x, _max.x):
                    if _x > 0 and _x < width:
                        add_char(stdscr,_min.y, _x, '-')
                for _x in range(_min.x, _max.x):
                    if _x > 0 and _x < width:
                        add_char(stdscr,_max.y, _x, '-')
                for _y in range(_max.y, _min.y):
                    if _y > 0 and _y < height:
                        add_char(stdscr,_y, _min.x, '|')
                for _y in range(_max.y, _min.y):
                    if _y > 0 and _y < height:
                        add_char(stdscr,_y, _max.x, '|')
                add_char(stdscr,_max.y, _max.x, '+')
                add_char(stdscr,_min.y, _max.x, '+')
                add_char(stdscr,_max.y, _min.x, '+')
                add_char(stdscr,_min.y, _min.x, '+')
        stdscr.refresh()

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
        else:
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
            elif(MODES[MODE] == 'TEXT'):
                canva[x][y] = chr(c)
                x += 1
            else:
                canva[x][y] = chr(c)


curses.wrapper(main)
