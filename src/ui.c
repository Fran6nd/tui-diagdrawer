#include <curses.h>
#include "ui.h"
#include "position.h"

void ui_draw_rect(position p1, position p2){
    position min = pos_min(p1, p2);
    position max = pos_max(p1, p2);
    move(min.y, min.x);
    hline(ACS_HLINE, max.x - min.x);
    vline(ACS_VLINE, max.x - min.x);
    addch(ACS_ULCORNER);
    move(max.y, min.x);
    hline(ACS_HLINE, max.x-min.x);
    addch(ACS_LLCORNER);
    move(min.y, max.x-min.x);
    vline(ACS_VLINE,max.y - min.y);
    addch(ACS_URCORNER);
    move(max.y - min.y, max.x-min.x);
    addch(ACS_LRCORNER);
}
int ui_ask_yes_no(char * question){

}