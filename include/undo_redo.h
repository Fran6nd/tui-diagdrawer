#ifndef UNDO_REDO_H
#define UNDO_REDO_H
#include "chunk.h"
void init_undo_redo();
void do_change(chunk *chk);
void undo_change(chunk *chk);
void redo_change(chunk *chk);
void free_undo_redo();
#endif