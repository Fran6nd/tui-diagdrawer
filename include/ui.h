#ifndef UI_H
#define UI_H

#include "position.h"
void ui_draw_rect(position p1, position p2);
int ui_ask_yes_no(char *question);
void ui_show_text(char *text);
void ui_show_text_info(char *text);
#endif