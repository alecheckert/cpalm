// nd2read.h
#ifndef _ND2READ_H
#define _ND2READ_H

#include <regex.h>
int regex_check(char *expr, char *string);
int partialstrcpy(int n_chars, char *buffer, char *src);
int get_frame_coords(int max_frames, FILE *fp, int *out_coords);
int get_frame(int start_coord, FILE *fp, unsigned short *out);
unsigned long int get_n_pixels(int frame_start_coord, FILE *fp);

#endif
