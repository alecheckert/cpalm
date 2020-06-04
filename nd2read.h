// nd2read.h
#ifndef _ND2READ_H
#define _ND2READ_H

#include <regex.h>
int regex_check(char *expr, char *string);
unsigned long int get_chunkmap_start(FILE *fp);
int partialstrcpy(int n_chars, char *buffer, char *src);
int get_frame_coords(int max_frames, FILE *fp, int *out_coords);
int get_frame(int start_coord, FILE *fp, unsigned short *out);
int get_frame_2D(int frame_idx, FILE *fp, int *frame_coords, int height, int width, unsigned short **out);
void pr_unsigned_short(int height, int width, unsigned short **arr);
unsigned long int get_n_pixels(int frame_start_coord, FILE *fp);
long int seek_string(char *query, FILE *fp, unsigned long int start_coord);
int get_height_width(FILE *fp, int *out);

#endif
