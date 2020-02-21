import curses

canva = list()
x, y = 0, 0


def add_line_up():
    pass


def add_line_down():
    canva.append([' '] * len(canva[0]))
    pass


def add_column_left():
    pass


def add_column_right():
    pass


def draw(stdscr):
    stdscr.addstr(0, 0, 'ascii-drawer by Fran6nd.' + str(x) + ' ' + str(y))
    stdscr.addstr(
        1, 0, 'Press [tab] so switch beween edit mode and screen mode.')
    height, width = stdscr.getmaxyx()
    height -= 2
    start_x = int(x - width/2)
    start_y = int(y - height/2)
    for _x in range(0, width-1):
        for _y in range(0, height-1):
            if start_x + _x == x and start_y + _y == y:
                stdscr.addstr(
                    _y+2, _x, canva[start_x + _x][start_y + height + 1 -_y], curses.color_pair(2))
            else:
                stdscr.addstr(
                    _y+2, _x, canva[start_x + _x][start_y + height + 1 -_y], curses.color_pair(1))
    stdscr.refresh()


def main(stdscr):
    # Clear screen
    curses.init_pair(1, curses.COLOR_WHITE, curses.COLOR_BLACK)
    curses.init_pair(2, curses.COLOR_WHITE, curses.COLOR_RED)
    height, width = stdscr.getmaxyx()
    for i in range(0, width+10):
        canva.append([' '] * (height+10))
    global x
    global y
    x = int(len(canva)/2)
    y = int(len(canva[0])/2)
    c = 0
    while chr(c) != 'q':
        stdscr.clear()
        draw(stdscr)

        stdscr.refresh()
        c = stdscr.getch()
        if c == curses.KEY_UP:
            y = y + 1
            add_line_up()
        elif c == curses.KEY_DOWN:
            y = y - 1
            add_line_down()
        elif c == curses.KEY_RIGHT:
            x = x + 1
            add_column_right()
        elif c == curses.KEY_LEFT:
            x = x - 1
            add_column_left()
        else:
            canva[x][y] = chr(c)


curses.wrapper(main)
