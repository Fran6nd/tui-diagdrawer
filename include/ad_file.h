#ifndef AD_FILE_H
#define AD_FILE_H

typedef struct ad_file ad_file;
struct ad_file{
    char ** file;
    int lines;
};
ad_file ad_file_new(int x, int y);
ad_file ad_load_file(char * f);
void ad_file_free(ad_file * f);

#endif