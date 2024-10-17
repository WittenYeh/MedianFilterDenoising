/*
 * @Autor: Witten Yeh
 * @Date: 2024-10-16 11:39:43
 * @LastEditors: Witten Yeh
 * @LastEditTime: 2024-10-17 11:38:10
 * @Description: A simple application that convert origin graph into 
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "lodepng.h"

/**
 * @brief convert origin image into a grapy image
 * 
 * @param image -> 
 * @param width -> 
 * @param height -> 
 */
void rgb_to_grayscale(unsigned char *image, unsigned width, unsigned height) {
    for (unsigned y = 0; y < height; y++) {
        for (unsigned x = 0; x < width; x++) {
            // visit each pixel (RGBA)
            unsigned char *pixel = &image[4 * (y * width + x)];  
            // convert origin pixel to gray pixel
            unsigned char gray = 0.299 * pixel[0] + 0.587 * pixel[1] + 0.114 * pixel[2];
            // set all color channel to the same gray value
            pixel[0] = pixel[1] = pixel[2] = gray;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "usage: %s <input file> <output file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *input_file = argv[1];
    const char *output_file = argv[2];

    unsigned width, height;
    unsigned char *image = NULL;

    // read png graph
    unsigned error = lodepng_decode32_file(&image, &width, &height, input_file);
    if (error) {
        printf("read file error %u: %s\n", error, lodepng_error_text(error));
        return EXIT_FAILURE;
    }

    int level = 2;

    struct timespec start, end;
    
    clock_gettime(CLOCK_MONOTONIC, &start);    // start
    // convert origin image to gray image
    for (int i = 0; i < level; ++i)
        rgb_to_grayscale(image, width, height);

    clock_gettime(CLOCK_MONOTONIC, &end);      // end

    // save gray image
    error = lodepng_encode32_file(output_file, image, width, height);
    if (error) {
        printf("fail to save file %u: %s\n", error, lodepng_error_text(error));
        free(image);
        return EXIT_FAILURE;
    }

    double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) * 1e-9;

    // free image memory
    free(image);

    printf("gray image is successfully save to: %s\n", output_file);
    printf("time taken: %.7lf\n", time_taken);
    
    return 0;
}
