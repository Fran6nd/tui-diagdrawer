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
    f.chunk = malloc(f.lines * sizeof(char *));
    int i;
    for (i = 0; i < f.lines; i++)
    {
        f.chunk[i] = malloc(sizeof(char) * (f.cols + 1));
        memset(f.chunk[i], ' ', f.cols);
        f.chunk[i][f.cols] = 0;
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
chunk chk_copy_chunk(chunk *input)
{
    chunk output;
    if(input->null){
        output.null = 1;
        return output;
    }
    output = chk_new(input->cols, input->lines);
    int x;
    int y;
    for (x = 0; x < input->cols; x++)
    {
        for (y = 0; y < input->lines; y++)
        {
            position tmp = {x, y, 0};
            chk_set_char_at(&output, tmp, chk_get_char_at(input, tmp));
        }
    }
    return output;
}
void chk_fill_chunk(chunk *input, position p1, position p2, char c)
{
    p2.x++;
    p2.y++;
    position min = pos_min(p1, p2);
    position max = pos_max(p1, p2);
    int x;
    int y;
    for (x = min.x; x < max.x; x++)
    {
        for (y = min.y; y < max.y; y++)
        {
            position tmp = {x, y};
            chk_set_char_at(input, tmp, c);
        }
    }
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
        return chk_new(10, 10);
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
    int x;
    /* Find empty columns on left. */
    int global_left_indent = 0;
    for (x = 0; x < file->cols; x++)
    {
        int spaces_only = 1;
        for (y = 0; y < file->lines; y++)
        {
            position tmp = {x, y};
            if (file->chunk[y][x] != ' ')
            {
                spaces_only = 0;
                break;
            }
        }
        if (spaces_only == 1)
        {
            global_left_indent++;
        }
        else
        {
            break;
        }
    }

    /* Find number of empty lines at the end of the file. */
    int empty_lines_at_eof = 0;
    for (y = file->lines - 1; y >= 0; y--)
    {
        char *line = file->chunk[y];
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

    /* Find number of empty lines at the beginnng of the file. */
    int empty_lines_at_bof = 0;
    for (y = 0; y < file->lines; y++)
    {
        char *line = file->chunk[y];
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
        char *line = file->chunk[y];
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
        for (i = global_left_indent; i < strlen(tmp); i++)
        {
            fputc(tmp[i], fOut);
        }

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
            free(f->chunk[i]);
        }
        free(f->chunk);
        f->lines = -1;
        f->null = 1;
    }
}

char chk_get_char_at(chunk *f, position p)
{
    if (p.y < f->lines && p.y >= 0)
    {
        if (p.x < strlen(f->chunk[p.y]) && p.x >= 0)
        {
            return f->chunk[p.y][p.x];
        }
    }
    return 0;
}

void chk_set_char_at(chunk *f, position p, char c)
{
    while (p.y < 0)
    {
        p.y++;
        chk_add_line_up(f);
    }
    while (p.y >= f->lines)
    {
        chk_add_line_down(f);
    }
    while (p.x < 0)
    {
        p.x++;
        chk_add_col_left(f);
    }
    while (p.x >= f->cols)
    {
        chk_add_col_right(f);
    }
    f->chunk[p.y][p.x] = c;
}

void chk_add_line_down(chunk *f)
{
    f->lines++;
    f->chunk = (char **)realloc(f->chunk, f->lines * sizeof(char *));
    f->chunk[f->lines - 1] = malloc(sizeof(char) * f->cols + 1);
    memset(f->chunk[f->lines - 1], ' ', f->cols);
    f->chunk[f->lines - 1][f->cols] = 0;
}

void chk_add_line_up(chunk *f)
{
    chk_add_line_down(f);
    int y;
    for (y = f->lines - 2; y >= 0; y--)
    {
        memcpy(f->chunk[y + 1], f->chunk[y], strlen(f->chunk[y]));
    }
    memset(f->chunk[0], ' ', strlen(f->chunk[0]));
}

void chk_add_col_right(chunk *f)
{
    f->cols++;
    int y;
    for (y = 0; y < f->lines; y++)
    {
        f->chunk[y] = (char *)realloc(f->chunk[y], sizeof(char) * f->cols + 1);
        f->chunk[y][f->cols] = 0;
        f->chunk[y][f->cols - 1] = ' ';
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
            f->chunk[y][x + 1] = f->chunk[y][x];
        }
        f->chunk[y][0] = ' ';
    }
}