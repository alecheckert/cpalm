// main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nd2read.h"

int main(int argc, char *argv[]) {

    // Return value for various errors
    int rv = 0;

    // File stream
    FILE * fp;

    // The maximum number of frames to consider reading 
    int max_frames = 30000;

    // The actual number of frame coordinates parsed
    int n_frames = 0;

    // Coordinates of each image frame in the file stream
    int coord_array[max_frames];


    // Parse CLI args
    if (argc != 3) {
        printf("Usage: cpalm <nd2_path> <out_csv_path>\n");
        return 1;
    }

    // Check that the user has actually passed an ND2 file
    rv = regex_check(".nd2", argv[1]);
    if (rv != 0) {
        printf("Error: %s is not a *.nd2 file\n", argv[1]);
        return rv;
    }

    // Open file stream
    fp = fopen(argv[1], "rb");

    // Read the locations of all of the image frames out of the 
    // chunk map 
    n_frames = get_frame_coords(max_frames, fp, coord_array);

    printf("n_frames = %d\n", n_frames);

    // Try to get the first frame
    int frame_idx = 0;
    unsigned long int n_pixels = get_n_pixels(
        coord_array[0],
        fp
    );
    printf("n_pixels = %lu\n", n_pixels);

    // Read the first frame with the get_frame function
    unsigned short frame[n_pixels];
    rv = get_frame(coord_array[0], fp, frame);
    if (rv == 0) {
        printf("get_frame was successful\n");
    } else {
        printf("get_frame failed for some reason\n");
    }
    for (int i=0; i<n_pixels; i++) {
        printf("frame[%d] = %d\n", i, frame[i]);
    }


    // Close the input filestream
    fclose(fp);

    return rv;
}

