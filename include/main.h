#ifndef MAIN_H
#define MAIN_H

#include "position.h"
#include "chunk.h"

#define COL_SELECTION 1
#define COL_EMPTY 2
#define COL_CURSOR 3
#define COL_NORMAL 4

#define MODE_NONE 0
#define MODE_SELECT 1
#define MODE_PUT 2
#define MODE_TEXT 3
#define MODE_RECT 4
#define MODE_LINE 5
#define MODE_ARROW 6

/* [Ctrl] + h for help. */
#define K_HELP 8
/* [Ctrl] + u for undo. */
#define K_UNDO 21
/* [Ctrl] + r for redo. */
#define K_REDO 18
/* [return] / [enter] */
#define K_ENTER 10

position get_cursor_pos();
position move_cursor();
int is_writable(char);
int is_diag_char(char);
extern chunk CURRENT_FILE;
extern position UP_LEFT_CORNER;

#endif