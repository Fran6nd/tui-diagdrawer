#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "ad_file.h"

#define max(a, b) (a >= b ? a : b)
#define min(a, b) (a <= b ? a : b)

ad_file ad_file_new(int x, int y)
{
    fflush(stdout);
    ad_file f;
    f.lines = y;
    int cols = x;
    f.file = malloc(f.lines * sizeof(char *));
    int i;
    for (i = 0; i < f.lines; i++)
    {
        f.file[i] = malloc(sizeof(char) * (cols + 1));
        memset(f.file[i], ' ', cols);
        f.file[i][cols] = 0;
    }
    return f;
}
ad_file ad_load_file(char *f)
{
}
void ad_file_free(ad_file *f)
{
    int i;
    for (i = 0; i < f->lines; i++)
    {
        free(f->file[i]);
    }
    free(f->file);
    f->lines = -1;
}

char ad_file_get_char(ad_file *f, position p)
{
    if (p.y < f->lines && p.y >= 0)
    {
        if (p.x < strlen(f->file[p.y]) && p.x >= 0)
        {
            return f->file[p.y][p.x];
        }
    }
    return 0;
}
void ad_file_set_char(ad_file *f, position p, char c)
{
    if (p.y < f->lines && p.y >= 0)
    {
        f->file[p.y][p.x] = c;
    }
}

position min_pos(position p1, position p2)
{
    position output = {min(p1.x, p2.x), min(p1.y, p2.y)};
    return output;
}
position max_pos(position p1, position p2)
{
    position output = {max(p1.x, p2.x), max(p1.y, p2.y)};
    return output;
}