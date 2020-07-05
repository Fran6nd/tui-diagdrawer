#include "ui.h"
#include "position.h"
#include <curses.h>
#include <string.h>

void ui_draw_rect(position p1, position p2) {
  position min = pos_min(p1, p2);
  position max = pos_max(p1, p2);
  move(min.y, min.x);
  hline(ACS_HLINE, max.x - min.x);
  vline(ACS_VLINE, max.y - min.y);
  addch(ACS_ULCORNER);
  move(max.y, min.x);
  hline(ACS_HLINE, max.x - min.x);
  addch(ACS_LLCORNER);
  move(min.y, max.x);
  vline(ACS_VLINE, max.y - min.y);
  addch(ACS_URCORNER);
  move(max.y, max.x);
  addch(ACS_LRCORNER);
}
void ui_draw_and_fill(position p1, position p2) {
  int x;
  int y;

  position min = pos_min(p1, p2);
  position max = pos_max(p1, p2);
  for (x = min.x; x < max.x; x++) {
    for (y = min.y; y < max.y; y++) {
      move(y, x);
      addch(' ');
    }
  }
  ui_draw_rect(p1, p2);
}
int ui_ask_yes_no(char *question) { return 0; }
void ui_show_text(char *text) {
  int lines = 0;
  int max_line_len = 0;
  int line_len = 0;
  int i;
  for (i = 0; i < strlen(text) + 1; i++) {
    char c = text[i];
    if (c == '\n')
      lines++;
    if (c == '\n' || c == '\0') {
      if (line_len > max_line_len) {
        max_line_len = line_len;
      }
      line_len = 0;
    } else {
      line_len++;
    }
  }
  position p1 = {2, 2};
  position p2 = {p1.x + max_line_len + 2, p1.y + lines + 2};
  ui_draw_and_fill(p1, p2);
  move(p1.y + 1, p1.x + 1);
  lines = 0;
  for (i = 0; i < strlen(text); i++) {
    char c = text[i];
    if (c == '\n') {
      lines++;
      move(p1.y + lines + 1, p1.x + 1);
    } else {
      addch(c);
    }
  }
}
/* #TODO: enable viewing strings bigger than the screen. */
void ui_show_text_info(char *text) {
  ui_show_text(text);
  getch();
}