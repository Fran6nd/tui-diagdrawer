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
    int lines = 0;
    int col = 0;
    int max_col = 0;

    // Open the file
    FILE *fp = fopen(f, "r");

    // Check if file exists
    if (fp == NULL)
    {
        printf("Could not open file %s", f);
    }

    // Extract characters from file and store in character c
    char c;
    for (c = getc(fp); c != EOF; c = getc(fp))
        if (c == '\n') // Increment count if this character is newline
        {
            lines = lines + 1;

            col = 0;
        }
        else
        {
            col++;
            if (col > max_col)
            {
                max_col = col;
            }
        }

    rewind(fp);
    ad_file output = ad_file_new(max_col + 1, lines + 1);
    int y = 0;
    int x = 0;
    for (c = getc(fp); c != EOF; c = getc(fp))
        if (c == '\n') // Increment count if this character is newline
        {
            y++;
            ;
            x = 0;
        }
        else
        {
            x++;
            position tmp = {x, y};
            ad_file_set_char(&output, tmp, c);
        }
    fclose(fp);
    return output;
}

void ad_save_file(ad_file *file, char *f)
{
    int y;
    FILE * fOut = fopen(f, "w");
    for (y = 0; y < file->lines; y++)
    {
        fputs(file->file[y], fOut);
        fputs("\n", fOut);
    }
    fclose (fOut);
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