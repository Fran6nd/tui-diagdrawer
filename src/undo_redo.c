#include "chunk.h"

chunk history[10];
int index;
int can_redo = 0;
void init_undo_redo()
{
    int i;
    for (i = 0; i < 10; i++)
    {
        history[i].null = 0;
    }
}
void do_change(chunk *chk)
{
    can_redo = 0;
    if (index == 10)
    {
        chk_free(&history[0]);
        int i;
        for (i = 0; i < 9; i++)
        {
            history[i] = history[i + 1];
        }
        index = 9;
    }
    chk_free(&history[index]);
    history[index] = chk_copy_chunk(chk);
    index++;
}
void undo_change(chunk *chk)
{
    if (index > 0)
    {
        index--;
        can_redo = 1;
        chk_free(chk);
;        *chk = chk_copy_chunk(&history[index]);
    }
}
void redo_change(chunk *chk)
{
}
void free_undo_redo(){
    int i;
    for (i = 0; i < 10; i++)
    {
        chk_free(&history[i]);
    }
}