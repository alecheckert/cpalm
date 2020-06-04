// main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nd2read.h"

int main(int argc, char *argv[]) {

    // Indexing
    int i;

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

    // A sample frame to retrieve
    int frame_idx = 0;

    // The number of pixels in the image, to be set
    unsigned long int n_pixels;

    // The Y and X dimensions of the data
    int yx[2];


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

    // Get the number of pixels using the first image frame (demonstration)
    n_pixels = get_n_pixels(coord_array[0], fp);

    // Get the Y and X dimensions of the movie
    rv = get_height_width(fp, yx);
    if (rv != 0) {
        printf("Couldn't retrieve height and width\n");
        return 1;
    }   

    // Read the first frame as a 1D array
    unsigned short arr[n_pixels];
    rv = get_frame(coord_array[0], fp, arr);
    for (i=0; i<n_pixels; i++)
        printf("%d: %hu\n", i, arr[i]);

    // // Read the first frame as a 2D array
    // unsigned short **arr = (unsigned short **)malloc(yx[0]*sizeof(unsigned short *));
    // for (i=0; i<yx[0]; i++) 
    //     arr[i] = (unsigned short *)malloc(yx[1]*sizeof(unsigned short));

    // Read from the file
    // rv = get_frame_2D(0, fp, coord_array, yx[0], yx[1], arr);
    // pr_unsigned_short(yx[0], yx[1], arr);

    // // Clear memory
    // for (i=0; i<yx[0]; i++) 
    //     free(arr[i]);
    // free(arr);

    // Close the input filestream
    fclose(fp);

    return rv;
}

