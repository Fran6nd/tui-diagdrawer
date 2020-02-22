#ifndef AD_FILE_H
#define AD_FILE_H

typedef struct ad_file ad_file;
struct ad_file{
    char ** file;
    int lines;
};
typedef struct position position;
struct position{
    int x, y;
};
ad_file ad_file_new(int x, int y);
ad_file ad_load_file(char * f);
void ad_file_free(ad_file * f);
char ad_file_get_char(ad_file * f, position);
char ad_file_set_char(ad_file * f, position, char c);

#endif