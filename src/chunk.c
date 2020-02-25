#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "chunk.h"

chunk chk_new(int x, int y)
{
    fflush(stdout);
    chunk f;
    f.lines = y;
    f.cols = x;
    f.file = malloc(f.lines * sizeof(char *));
    int i;
    for (i = 0; i < f.lines; i++)
    {
        f.file[i] = malloc(sizeof(char) * (f.cols + 1));
        memset(f.file[i], ' ', f.cols);
        f.file[i][f.cols] = 0;
    }
    f.null = 0;
    return f;
}
chunk chk_extract_chunk(chunk *input, position p1, position p2)
{
    p2.x++;
    p2.y++;
    position min = pos_min(p1, p2);
    position max = pos_max(p1, p2);

    chunk output = chk_new(max.x - min.x, max.y - min.y);
    int x;
    int y;
    for (x = min.x; x < max.x; x++)
    {
        for (y = min.y; y < max.y; y++)
        {
            position in_input = {x, y, 0};
            position in_output = {x - min.x, y - min.y, 0};
            chk_set_char_at(&output, in_output, chk_get_char_at(input, in_input));
        }
    }
    return output;
}
void chk_blit_chunk(chunk *main, chunk *clipboard, position p)
{
    if (!main->null)
    {
        int x;
        int y;
        for (x = 0; x < clipboard->cols; x++)
        {
            for (y = 0; y < clipboard->lines; y++)
            {
                position pos_in_main = {x + p.x, y + p.y};
                position pos_in_clipboard = {x, y};
                chk_set_char_at(main, pos_in_main, chk_get_char_at(clipboard, pos_in_clipboard));
            }
        }
    }
}
chunk chk_new_from_file(char *f)
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
    chunk output = chk_new(max_col + 1, lines + 1);
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
            chk_set_char_at(&output, tmp, c);
        }
    fclose(fp);
    return output;
}

void chk_save_to_file(chunk *file, char *f)
{

    int y;
    int empty_lines_at_eof = 0;
    /* Find number of empty lines at the end of the file. */
    for (y = file->lines - 1; y >= 0; y--)
    {
        char *line = file->file[y];
        int i;
        int empty = 1;
        for (i = 0; i < strlen(line); i++)
        {
            if (line[i] != ' ')
            {
                empty = 0;
            }
        }
        if (empty)
        {
            empty_lines_at_eof++;
        }
        else
        {
            y = -1;
        }
    }
    int empty_lines_at_bof = 0;
    /* Find number of empty lines at the beginnng of the file. */
    for (y = 0; y < file->lines; y++)
    {
        char *line = file->file[y];
        int i;
        int empty = 1;
        for (i = 0; i < strlen(line); i++)
        {
            if (line[i] != ' ')
            {
                empty = 0;
            }
        }
        if (empty)
        {
            empty_lines_at_bof++;
        }
        else
        {
            y = file->lines;
        }
    }
    /* Now write the file. */

    FILE *fOut = fopen(f, "w");
    for (y = empty_lines_at_bof; y < file->lines - empty_lines_at_eof; y++)
    {
        char *line = file->file[y];
        char *tmp = malloc((sizeof(char)) * (strlen(line) + 1));
        memcpy(tmp, line, strlen(line) + 1);
        int i;
        for (i = strlen(line) - 1; i >= 0; i--)
        {
            if (tmp[i] == ' ')
            {
                tmp[i] = 0;
            }
            else
            {
                i = -1;
            }
        }
        fputs(tmp, fOut);
        fputs("\n", fOut);
        free(tmp);
    }
    fclose(fOut);
}

void chk_free(chunk *f)
{
    if (!f->null)
    {
        int i;
        for (i = 0; i < f->lines; i++)
        {
            free(f->file[i]);
        }
        free(f->file);
        f->lines = -1;
        f->null = 1;
    }
}

char chk_get_char_at(chunk *f, position p)
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
void chk_set_char_at(chunk *f, position p, char c)
{
    if (p.y < f->lines && p.y >= 0)
    {
        f->file[p.y][p.x] = c;
    }
}

void chk_add_line_down(chunk *f)
{
    f->lines++;
    f->file = (char **)realloc(f->file, f->lines * sizeof(char *));
    f->file[f->lines - 1] = malloc(sizeof(char) * f->cols + 1);
    memset(f->file[f->lines - 1], ' ', f->cols);
    f->file[f->lines - 1][f->cols] = 0;
}
void chk_add_line_up(chunk *f)
{
    chk_add_line_down(f);
    int y;
    for (y = f->lines - 2; y >= 0; y--)
    {
        memcpy(f->file[y + 1], f->file[y], strlen(f->file[y]));
    }
    memset(f->file[0], ' ', strlen(f->file[0]));
}
void chk_add_col_right(chunk *f)
{
    f->cols++;
    int y;
    for (y = 0; y < f->lines; y++)
    {
        f->file[y] = (char *)realloc(f->file[y], sizeof(char) * f->cols + 1);
        f->file[y][f->cols] = 0;
        f->file[y][f->cols - 1] = ' ';
    }
}
void chk_add_col_left(chunk *f)
{
    chk_add_col_right(f);
    int y;
    for (y = 0; y < f->lines; y++)
    {
        int x;
        for (x = f->cols - 2; x >= 0; x--)
        {
            f->file[y][x + 1] = f->file[y][x];
        }
        f->file[y][0] = ' ';
    }
}