#ifndef UNDO_REDO_H
#define UNDO_REDO_H
#include "chunk.h"

/*
 * This file is intended to allow performing undo/redo operations.
 * This is maybe the most sensitive part of the software because of the risk of
 * memmory leaks. Feel free to improve it :)
 */

void init_undo_redo();
void do_change(chunk *chk);
void undo_change(chunk *chk);
void redo_change(chunk *chk);
void free_undo_redo();
#endif