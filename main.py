import curses
import copy

MODE = 1
MODES = ['SQUARE', 'LINE', 'MANUAL']


canva = list()
buffer = None
x, y = 0, 0


class point():
    def __init__(self, x, y):
        self.x = x
        self.y = y


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
def draw_buffer(stdscr):
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
                    _y+2, _x, buffer[start_x + _x][start_y + height + 1 - _y], curses.color_pair(2))
            else:
                stdscr.addstr(
                    _y+2, _x, buffer[start_x + _x][start_y + height + 1 - _y], curses.color_pair(1))

def main(stdscr):
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
                buffer = copy.deepcopy(canva)
                x_min = min(p1.x, x)
                x_max = max(p1.x, x)
                y_min = min(p1.y, y)
                y_max = max(p1.y, y)
                for _x in range(x_min, x_max):
                    buffer[_x][y_min] = '-'
                for _x in range(x_min, x_max):
                    buffer[_x][y_max] = '-'
                for _y in range(y_min, y_max):
                    buffer[x_min][_y] = '|'
                for _y in range(y_min, y_max):
                    buffer[x_max][_y] = '|'
                buffer[x_min][y_min] = '+'
                buffer[x_min][y_max] = '+'
                buffer[x_max][y_min] = '+'
                buffer[x_max][y_max] = '+'
                draw_buffer(stdscr)
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
            else:
                canva[x][y] = chr(c)


curses.wrapper(main)
