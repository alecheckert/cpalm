// nd2read.c
#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>
int regex_check(char *expr, char *string);
int partialstrcpy(int n_chars, char *buffer, char *src);
int get_frame_coords(int max_frames, FILE *fp, int *out_coords);
int get_frame(int start_coord, FILE *fp, unsigned short *out);
unsigned long int get_n_pixels(int frame_start_coord, FILE *fp);

/*
 *  Function: regex_check
 *  ---------------------
 *  Determine whether a regular expression exists in a string
 *
 *  Parameters
 *  ----------
 *  
*/
int regex_check(char *expr, char *string) {
    int return_value;
    regex_t regex;

    // Compile the regular expression
    return_value = regcomp(&regex, expr, 0);

    // Check for errors in regex compilation
    if (return_value != 0) {
        return return_value; 
    }

    // Look for the regular expression in the string
    return_value = regexec(&regex, string, 0, NULL, 0);
    return return_value;
}

/*
 *  Function: partialstrcpy
 *  -----------------------
 *  Copy the first *n* characters of a large string
 *  to a buffer of size *n*.
 *
 *  Parameters
 *  ----------
 *  n_chars     :   the number of characters to copy
 *  buffer      :   destination
 *  src         :   source
 *
 *  Returns
 *  -------
 *  0 if success, 1 if '\0' was encountered before
 *    reaching n_chars
 *
*/
int partialstrcpy(int n_chars, char *buffer, char *src) {
    for (int i=0; i<n_chars; i++) {
        if (!src[i]) {
            return 1;
        } else {
            buffer[i] = src[i];
        }
    }
    return 0;
}

/*
 *  Function: get_frame_coords
 *  --------------------------
 *  Get the coordinates of the image frames from an ND2 file
 *  stream. These coordinates specify exactly where we need 
 *  to read in order to get image data.
 *
 *  Parameters
 *  ----------
 *  max_frames      :   maximum number of frames to retrieve,
 *                      equal to the size of the array out_coords
 *  fp              :   ND2 file stream (use "rb")
 *  out_coords      :   pointer to array of integers to store the 
 *                      coordinates
 *
 *  Returns
 *  -------
 *  int, the total number of frames retrieved
 *
*/
int get_frame_coords(int max_frames, FILE *fp, int *out_coords) {

    int r;
    long int file_size, chunk_map_start, chunk_map_size;
    int n_frames = 0;
    int i, j;

    // Header for the chunk map elements corresponding to 
    // image frames
    const char *query = "ImageDataSeq|";
    long int query_len = strlen(query);

    // A buffer for reading the file stream
    char buffer[query_len];

    // A short buffer to parse the frame index string
    char frame_index[10];
    int int_frame_index;

    // The information we're trying to retrieve: the start 
    // coordinate (coord) and length (frame_length) of each 
    // image frame
    unsigned long int coord, frame_length;

    // Find the coordinate of the chunk map start, which is encoded
    // in the last 8 bytes of the file
    r = fseek(fp, -8, SEEK_END);
    file_size = ftell(fp) + 8;
    r = fread(&chunk_map_start, 8, 1, fp);
    chunk_map_size = file_size - chunk_map_start;

    // Go to the chunk map start
    r = fseek(fp, chunk_map_start, SEEK_SET);

    // Iteratively proceed through the chunk map, looking for 
    // the string that specifies the start of each image group
    for (i=0; i<chunk_map_size-query_len; i++) {

        // Read the next 13 characters into the buffer
        r = fread(&buffer, query_len, 1, fp);

        // See if this matches the query
        if ((r==1) & (!strcmp(buffer, query))) {
            n_frames++;

            // Find the ! character, which terminates the header,
            // and read the integer immediately before it, which
            // encodes the frame index
            for (j=0; j<10; j++) {
                frame_index[j] = getc(fp);
                if (frame_index[j] == '!') {
                    frame_index[j+1] = '\0';
                    break;
                }
            }
            int_frame_index = atoi(frame_index);

            // Get the start coordinate of the image frame
            r = fread(&coord, 8, 1, fp);

            // Get the length of the image frame
            r = fread(&frame_length, 8, 1, fp);

            // printf("match: %s, coord = %lu, frame_length = %lu, frame_index = %d\n",
            //     buffer, coord, frame_length, int_frame_index);

            // Assign to the corresponding position in the output 
            // array
            if (int_frame_index < max_frames) {
                out_coords[int_frame_index] = coord;
            }
        }

        // Seek to the next query
        r = fseek(fp, chunk_map_start+i, SEEK_SET);
    }

    return n_frames;
}

/*
 *  Function: get_frame
 *  -------------------
 *  Get a single 2D image frame from the filestream. This 
 *  assumes that the user knows the size of the image frames
 *  and where they are located in the file.
 * 
 *  Parameters
 *  ----------
 *    start_coord     : the starting coordinate of the image 
 *                      frame relative to the start of the file
 *    fp              : ND2 filestream
 *    out             : array, output. Must match the number of 
 *                      pixels in this image frame
 *
 *  Returns
 *  -------
 *    0 if no errors
 * 
*/
int get_frame(int start_coord, FILE *fp, unsigned short *out) 
{
    int return_value = 0;
    int offset; 
    unsigned long int frame_length, n_pixels;
    float timestamp;

    // Go to the start coordinate in the filestream
    return_value = fseek(fp, start_coord+4, SEEK_SET);
    if (return_value != 0) {
        printf("failed at step 0\n");
        return return_value;
    }

    // Read the offset, encoded in bytes 4-8
    return_value = fread(&offset, 4, 1, fp);
    if (return_value != 1) {
        printf("failed at step 1\n");
        return 1;
    }

    // Read the frame length, encoded in bytes 8-16
    return_value = fread(&frame_length, 8, 1, fp);
    if (return_value != 1) {
        printf("failed at step 2\n");
        return 1;
    }

    // Get the actual number of pixels
    n_pixels = (frame_length - 8) / 2;  
    printf("n_pixels = %lu\n", n_pixels);

    // Use the offset to find the start of the actual data
    return_value = fseek(fp, start_coord+offset+16, SEEK_SET);
    if (return_value != 0) {
        printf("failed at step 3\n");
        return return_value;
    }

    // The first 4 bytes after this encode a timestamp
    return_value = fread(&timestamp, 4, 1, fp);
    if (return_value != 1) {
        printf("failed at step 4\n");
        return return_value;
    }
    printf("timestamp = %f\n", timestamp);

    // Read the rest into an array
    return_value = fseek(fp, start_coord+offset+24, SEEK_SET);
    if (return_value != 0) {
        printf("failed at step 5\n");
        return return_value;
    }

    return_value = fread(out, 2, n_pixels, fp);
    if (return_value != n_pixels) {
        printf("failed at step 6\n");
        return 1;
    }

    // 0 if successful, 1 if failed at last step
    return 1-(return_value==n_pixels);
}


/* 
 *  Function: get_n_pixels
 *  ----------------------
 *  Get the number of pixels per frame
 *  
 *  Parameters
 *  ----------
 *    frame_start_coord   :  the location of the trial frame
 *                           in the file stream
 *    fp                  :  ND2 file stream
 * 
 *  Returns
 *  -------
 *    unsigned long int, the number of pixels per frame
 *
*/
unsigned long int get_n_pixels(int frame_start_coord, FILE *fp) {
    unsigned long int frame_length, n_pixels;
    int r = 0;

    // Go to the location of the frame in the filestream
    fseek(fp, frame_start_coord+8, SEEK_SET);

    // Read the next 8 bytes, which encode the size of the frame
    fread(&frame_length, 8, 1, fp);

    // Each pixel is 2 bytes, and 8 bytes encode other stuff
    n_pixels = (frame_length - 8) / 2;
    return n_pixels;
}












