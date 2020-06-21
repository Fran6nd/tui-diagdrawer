#include "chunk.h"

/*
 * This file is intended to allow performing undo/redo operations.
 * This is maybe the most sensitive part of the software because of the risk of
 * memmory leaks. Feel free to improve it :)
 */

chunk history[10];
int index;
int can_redo = 0;
void init_undo_redo() {
  int i;
  for (i = 0; i < 10; i++) {
    history[i].null = 1;
  }
}
void do_change(chunk *chk) {
  can_redo = 0;
  if (index == 10) {
    /* We free the first chunk */
    chk_free(&history[0]);
    int i;
    for (i = 0; i < 9; i++) {
      /* Moving reference for high index to low. */
      history[i] = history[i + 1];
    }
    index = 9;
    /* The last cell is containing same pointers as the previous one.
     * So we set it to null to do not free the previous one.
     */
    history[index].null = 1;
  }
  /* If the chunk is not empty we free it. */
  if (!history[index].null) {
    chk_free(&history[index]);
  }
  history[index] = chk_copy_chunk(chk);
  index++;
}
void undo_change(chunk *chk) {
  if (index > 0) {
    chk_free(&history[index]);
    history[index] = chk_copy_chunk(chk);
    index--;
    can_redo++;
    chk_free(chk);
    *chk = chk_copy_chunk(&history[index]);
  }
}
void redo_change(chunk *chk) {
  if (can_redo > 0) {
    chk_free(chk);
    can_redo--;
    index++;
    *chk = chk_copy_chunk(&history[index]);
  }
}
void free_undo_redo() {
  int i;
  for (i = 0; i < 10; i++) {
    chk_free(&history[i]);
  }
}