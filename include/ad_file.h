#ifndef AD_FILE_H
#define AD_FILE_H

typedef struct ad_file ad_file;
struct ad_file{
    char ** file;
    int lines;
};
typedef struct position position;
struct position{
    int x, y, null;
};
ad_file ad_file_new(int x, int y);
ad_file ad_load_file(char * f);

void ad_save_file(ad_file *file, char *f);
void ad_file_free(ad_file * f);
char ad_file_get_char(ad_file * f, position);
void ad_file_set_char(ad_file * f, position, char c);
position min_pos(position p1, position p2);
position max_pos(position p1, position p2);

#endif