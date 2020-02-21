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
                self.chunk.append(['x'] * self.height)       

    def get_char(self, pos):
        if self.is_inside(pos):
            return self.chunk[pos.x][pos.y]
        else:
            return None

    def is_inside(self, pos):
        if(pos.x >= 0 and pos.x < self.width and pos.y >=0 and pos.y < self.height) :
            return True
        return False

    def get_snapshot(self, pos, width, height):
        output = list()
        for x in range(0, width):
            output.append(list())
            for y in range(0, height):
                output[x].append(self.get_char(position(x, y) + pos))
        return output

    def draw(self, stdscr, center):
        height, width = stdscr.getmaxyx()
        height -= 2
        offset = position(center.x - width / 2, center.y - height / 2)
        for x in range(0, width):
            for y in range(0, height - 1):
                c = self.get_char(offset + position(x, y))
                if c:
                    stdscr.addstr(height -y ,x, c)
                else:
                    stdscr.addstr(height -y ,x, ' ', COLOR_NORMAL)


