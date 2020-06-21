#ifndef POSITION_H
#define POSITION_H

typedef struct position position;
struct position {
  int x, y, null;
};

position pos_min(position p1, position p2);
position pos_max(position p1, position p2);

#endif