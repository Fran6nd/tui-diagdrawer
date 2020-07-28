#include <curses.h>

#include "edit_mode.h"
#include "main.h"
#include "position.h"
#include "position_list.h"
#include "ui.h"
#include "undo_redo.h"

position_list PATH;

static void on_key_event(edit_mode *em, int c)
{
  position delta = move_cursor(c);
  if (!delta.null)
  {
    if (PATH.size != 0)
    {
      position previous_pos = {.x = get_cursor_pos().x - delta.x, .y = get_cursor_pos().y - delta.y, .null = 0};
      position p_min = pos_min(previous_pos, get_cursor_pos());
      position p_max = pos_max(previous_pos, get_cursor_pos());
      while (delta.x < 0)
      {
        delta.x++;
        position tmp = get_cursor_pos();
        tmp.x = tmp.x + delta.x;

        while (pl_is_inside(&PATH, tmp) != -1)
        {
          pl_remove_last(&PATH);
        }
        pl_add(&PATH, tmp);
      }
      while (delta.y < 0)
      {
        delta.y++;
        position tmp = get_cursor_pos();
        tmp.y = tmp.y + delta.y;
        while (pl_is_inside(&PATH, tmp) != -1)
        {
          pl_remove_last(&PATH);
        }
        pl_add(&PATH, tmp);
      }
      while (delta.x > 0)
      {
        delta.x--;
        position tmp = get_cursor_pos();
        tmp.x = tmp.x + delta.x;

        while (pl_is_inside(&PATH, tmp) != -1)
        {
          pl_remove_last(&PATH);
        }
        pl_add(&PATH, tmp);
      }
      while (delta.y > 0)
      {
        delta.y--;
        position tmp = get_cursor_pos();
        tmp.y = tmp.y + delta.y;
        while (pl_is_inside(&PATH, tmp) != -1)
        {
          pl_remove_last(&PATH);
        }
        pl_add(&PATH, tmp);
      }
    }
  }
  else if (c == ' ')
  {
    if (PATH.size != 0)
    {
      int i;
      do_change(&CURRENT_FILE);
      for (i = 0; i < PATH.size; i++)
      {

        chk_set_char_at(&CURRENT_FILE, PATH.list[i],
                        pl_get_arrow_char(&PATH, i));
      }
      pl_empty(&PATH);
      PATH = pl_new();
    }
    else
    {
      pl_add(&PATH, get_cursor_pos());
    }
  }
}

static char *get_help(edit_mode *em)
{
  return "You are in the ARROW mode.\n"
         "You can use [space] to select the starting point\n"
         "and then [space] again to select the ending point\n"
         "and draw the arrow!\n"
         "\n"
         "Press any key to continue.";
}

static void on_free(edit_mode *em) { pl_empty(&PATH); }

static character on_draw(edit_mode *em, position p, character c)
{
  int i = pl_is_inside(&PATH, p);
  if (i != -1)
  {
    c.c = pl_get_arrow_char(&PATH, i);
  }
  return c;
}

static void on_left_column_add(edit_mode *em)
{
  int i = 0;
  for (i = 0; i < PATH.size; i++)
  {
    PATH.list[i].x++;
  }
}
static void on_top_line_add(edit_mode *em)
{
  int i = 0;
  for (i = 0; i < PATH.size; i++)
  {
    PATH.list[i].y++;
  }
}

static int on_abort(edit_mode *em)
{
  if (PATH.size != 0)
  {
    pl_empty(&PATH);
    return 1;
  }
  return 0;
}

edit_mode arrow_mode()
{
  edit_mode EDIT_MODE_RECT = {.name = "ARROW",
                              .key = (int)'a',
                              .on_key_event = on_key_event,
                              .on_free = on_free,
                              .on_draw = on_draw,
                              .on_left_column_add = on_left_column_add,
                              .on_top_line_add = on_top_line_add,
                              .on_abort = on_abort,
                              .get_help = get_help,
                              .null = 0};
  return EDIT_MODE_RECT;
}