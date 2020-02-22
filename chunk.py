import curses
import time

from COLORS import *

from position import *

class chunk():
    def __init__(self, p1, p2 = None, chunk = None):
        if p2 == None:
            p2 = position(0,0)
        self.min =position.min(p1, p2)
        self.max =position.max(p1, p2)
        self.width = self.max.x - self.min.x
        self.height = self.max.y - self.min.y
        self.chunk = list()
        if chunk:
            for x in range(0, self.width):
                self.chunk.append(list())
                for y in range(0, self.height):
                    self.chunk[x].append(chunk.get_char(position(x, y) + self.min))
        else:
            for x in range(0, self.width):
                self.chunk.append([' '] * self.height)       

    def get_char(self, pos):
        if self.is_inside(pos):
            return self.chunk[pos.x][pos.y]
        else:
            return None

    def set_char(self, pos, c):
        if self.is_inside(pos):
            self.chunk[pos.x][pos.y] = c

    def is_inside(self, pos):
        if(pos.x >= 0 and pos.x < self.width and pos.y >=0 and pos.y < self.height) :
            return True
        return False

    def get_snapshot(self, pos, width, height):
        output = list()
        for x in range(pos.x, pos.x + width):
            output.append(list())
            for y in range(pos.y, pos.y + height):
                output[x - pos.x ].append(self.get_char(position(x, y)))
        return output

    def draw(self, stdscr, down_left_corner):
        height, width = stdscr.getmaxyx()
        height -= 2
        snap = self.get_snapshot(down_left_corner, width, height)
        for x in range(0, width):
            for y in range(0, height-2):
                if(snap[x][y]):
                    stdscr.addstr(y, x, snap[x][y], curses.color_pair(COLOR_NORMAL))
                else:
                    stdscr.addstr(y, x, ' ', curses.color_pair(COLOR_EMPTY))
        


