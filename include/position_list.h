#ifndef POSITION_LIST_H
#define POSITION_LIST_H

#include "position.h"

typedef struct position_list position_list;
struct position_list {
  int size;
  position *list;
};

position_list pl_new();
void pl_add(position_list *pl, position p);
void pl_empty(position_list *pl);
void pl_remove_last(position_list *pl);
position pl_get_last(position_list *pl);
int pl_is_inside(position_list *pl, position p);
char pl_get_line_char(position_list *pl, int index);
char pl_get_arrow_char(position_list *pl, int index);

#endif