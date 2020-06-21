#include "position_list.h"
#include <stdlib.h>

position_list pl_new() {
  position_list pl;
  pl.size = 0;
  return pl;
}
void pl_add(position_list *pl, position p) {
  pl->size++;
  if (pl->size == 1) {
    pl->list = malloc(sizeof(position));
    pl->list[0] = p;
  } else {
    pl->list = (position *)realloc(pl->list, pl->size * sizeof(position));
    pl->list[pl->size - 1] = p;
  }
}
/* Clear the content of a list. */
void pl_empty(position_list *pl) {
  if (pl->size > 0) {
    free(pl->list);
    pl->size = 0;
  }
}
/* Remove last position from a list. */
void pl_remove_last(position_list *pl) {
  if (pl->size == 1) {
    pl_empty(pl);
  } else if (pl->size > 1) {
    pl->size--;
    pl->list = (position *)realloc(pl->list, pl->size * sizeof(position));
  }
}
/* Check if the given position is inside or not.
 * Return -1 if not.
 * Else return index in list.
 */
int pl_is_inside(position_list *pl, position p) {
  int i;
  for (i = 0; i < pl->size; i++) {
    position p1 = pl->list[i];
    if (p.x == p1.x && p.y == p1.y) {
      return i;
    }
  }
  return -1;
}
/* Calculate which char should be displayed from the index of a list containing
 * all line's points. */
char pl_get_line_char(position_list *pl, int i) {
  char to_print = '#';
  position current = pl->list[i];
  if (pl->size == 1) {
    to_print = '+';
  } else if (i - 1 >= 0 && i + 1 < pl->size) {
    position prev = pl->list[i - 1];
    position next = pl->list[i + 1];
    if (prev.x != next.x && prev.y != next.y) {
      to_print = '+';
    } else if (prev.x == next.x) {
      to_print = '|';
    } else {
      to_print = '-';
    }
  } else if (i == 0) {
    position next = pl->list[i + 1];
    if (current.x == next.x) {
      to_print = '|';
    } else {
      to_print = '-';
    }
  } else {
    position prev = pl->list[i - 1];
    if (current.x == prev.x) {
      to_print = '|';
    } else {
      to_print = '-';
    }
  }
  return to_print;
}
/* Calculate which char should be displayed from the index of a list containing
 * all arrow's points. */
char pl_get_arrow_char(position_list *pl, int i) {
  char to_print = '#';
  position current = pl->list[i];
  if (pl->size == 1) {
    to_print = '+';
  } else if (i - 1 >= 0 && i + 1 < pl->size) {
    position prev = pl->list[i - 1];
    position next = pl->list[i + 1];
    if (prev.x != next.x && prev.y != next.y) {
      to_print = '+';
    } else if (prev.x == next.x) {
      to_print = '|';
    } else {
      to_print = '-';
    }
  } else if (i == 0) {
    position next = pl->list[i + 1];
    if (current.x == next.x) {
      to_print = '|';
    } else {
      to_print = '-';
    }
  } else {
    position prev = pl->list[i - 1];
    if (current.x == prev.x) {
      if (current.y < prev.y) {
        to_print = '^';
      } else {
        to_print = 'v';
      }
    } else {
      if (prev.x < current.x) {
        to_print = '>';
      } else {
        to_print = '<';
      }
    }
  }
  return to_print;
}
position pl_get_last(position_list *pl) { return pl->list[pl->size - 1]; }
