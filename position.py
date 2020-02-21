import math

class position():
    def __init__(self, x, y):
        self.x = math.floor(x)
        self.y =  math.floor(y)
    def __add__(self, other):
        return position(self.x + other.x, self.y + other.y)
    def __sub__(self, other):
        return position(self.x - other.x, self.y - other.y)
    def min(p1, p2):
        return position(min(p1.x, p2.x), min(p1.y, p2.y))
    def max(p1, p2):
        return position(max(p1.x, p2.x), max(p1.y, p2.y))