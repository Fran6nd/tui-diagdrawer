#ifndef AD_FILE_H
#define AD_FILE_H

#include "position.h"

typedef struct chunk chunk;
struct chunk
{
    char **chunk;
    int lines;
    int cols;
    int null;
};

chunk chk_new(int x, int y);
chunk chk_new_from_file(char *f);
void chk_save_to_file(chunk *file, char *f);
void chk_free(chunk *f);
char chk_get_char_at(chunk *f, position);
void chk_set_char_at(chunk *f, position, char c);
chunk chk_extract_chunk(chunk *f, position p1, position p2);
chunk chk_copy_chunk(chunk *input);
void chk_fill_chunk(chunk *input, position p1, position p2, char c);
void chk_blit_chunk(chunk *background, chunk *clipboard, position p);
void chk_add_line_down(chunk *f);
void chk_add_line_up(chunk *f);
void chk_add_col_right(chunk *f);
void chk_add_col_left(chunk *f);

#endif