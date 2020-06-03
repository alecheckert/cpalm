// main.c
#include <stdio.h>
#include "nd2read.h"

int main(int argc, char *argv[]) {

    // CLI
    if (argc != 3) {
        printf("Usage: cpalm <nd2_path> <out_csv_path>\n");
        return 1;
    }

    printf("argv[1] = %s\n", argv[1]);
    printf("argv[2] = %s\n", argv[2]);

    return 0;
}
