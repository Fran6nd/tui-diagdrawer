#include "position.h"

#define max(a, b) (a >= b ? a : b)
#define min(a, b) (a <= b ? a : b)

position pos_min(position p1, position p2) {
  position output = {min(p1.x, p2.x), min(p1.y, p2.y)};
  return output;
}
position pos_max(position p1, position p2) {
  position output = {max(p1.x, p2.x), max(p1.y, p2.y)};
  return output;
}