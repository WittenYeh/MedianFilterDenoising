#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <getopt.h>
#include "../deps/lodepng.h"

// Helper function for qsort to compare values
int compare(const void *a, const void *b) {
    return (*(unsigned char*)a - *(unsigned char*)b);
}

// Median filter function
unsigned char * median_filter(unsigned char *image, int width, int height, int filter_size) {
    unsigned char *filtered_image = (unsigned char *)malloc(width * height * 4);  // Allocating memory for filtered image
    int half_size = filter_size / 2;

    int num_pixels = filter_size * filter_size;

    // Dynamically allocate memory for R, G, B arrays
    unsigned char *r_vals = (unsigned char *)malloc(num_pixels * sizeof(unsigned char));
    unsigned char *g_vals = (unsigned char *)malloc(num_pixels * sizeof(unsigned char));
    unsigned char *b_vals = (unsigned char *)malloc(num_pixels * sizeof(unsigned char));

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Arrays to hold R, G, B values of the filter window
            int count = 0;

            // Collect pixel values in the filter window
            for (int fy = -half_size; fy <= half_size; fy++) {
                for (int fx = -half_size; fx <= half_size; fx++) {
                    int iy = y + fy;
                    int ix = x + fx;
                    // printf("(iy=%d, ix=%d)\n", iy, ix);
                    if (iy >= 0 && iy < height && ix >= 0 && ix < width) {
                        int idx = 4 * (iy * width + ix); // 4 channels: RGBA
                        r_vals[count] = image[idx];
                        g_vals[count] = image[idx + 1];
                        b_vals[count] = image[idx + 2];
                        count++;
                    }
                }
            }

            // Sort the R, G, B arrays and find the median
            qsort(r_vals, count, sizeof(unsigned char), compare);
            qsort(g_vals, count, sizeof(unsigned char), compare);
            qsort(b_vals, count, sizeof(unsigned char), compare);

            // Set the median value to the filtered image
            unsigned idx = 4 * (y * width + x);
            filtered_image[idx] = r_vals[count / 2];
            filtered_image[idx + 1] = g_vals[count / 2];
            filtered_image[idx + 2] = b_vals[count / 2];
            filtered_image[idx + 3] = image[idx + 3]; // Keep the alpha channel unchanged
        }
    }
    
    // Free allocated memory
    free(r_vals);
    free(g_vals);
    free(b_vals);

    return filtered_image;
}

// Function to read PNG file using lodepng
unsigned char* read_png(const char *filename, unsigned *width, unsigned *height) {
    unsigned char *image;
    unsigned error = lodepng_decode32_file(&image, width, height, filename); // Load as RGBA (4 channels)
    if (error) {
        printf("Error %u: %s\n", error, lodepng_error_text(error));
        return NULL;
    }
    return image;
}

// Function to save PNG file using lodepng
void save_png(const char *filename, unsigned char *image, unsigned width, unsigned height) {
    unsigned error = lodepng_encode32_file(filename, image, width, height); // Save as RGBA (4 channels)
    if (error) {
        printf("Error %u: %s\n", error, lodepng_error_text(error));
    } else {
        printf("Image saved to %s\n", filename);
    }
}

int main(int argc, char *argv[]) {
    int filter_size = 0;
    int level = 0;
    
    // Define long options
    static struct option long_options[] = {
        {"size", required_argument, 0, 's'},   // --size option
        {"level", required_argument, 0, 'l'},  // --level option
        {0, 0, 0, 0}
    };

    int option_index = 0;
    int opt;
    
    // Parse options using getopt_long
    while ((opt = getopt_long(argc, argv, "s:l:", long_options, &option_index)) != -1) {
        switch (opt) {
            case 's':
                filter_size = atoi(optarg);  // Convert size argument to integer
                break;
            case 'l':
                level = atoi(optarg);  // Convert level argument to integer
                break;
            default:
                fprintf(stderr, "Usage: %s [--size filter_size] [--level level] <input_image> <output_image>\n", argv[0]);
                return 1;
        }
    }

    // Check if input and output file arguments are provided
    if (optind + 2 > argc) {
        fprintf(stderr, "Usage: %s [--size filter_size] [--level level] <input_image> <output_image>\n", argv[0]);
        return 1;
    }

    const char *input_file = argv[optind];      // Input file name (first positional argument)
    const char *output_file = argv[optind + 1]; // Output file name (second positional argument)

    // Validate filter_size
    if (filter_size < 1) {
        printf("Invalid filter size, must be greater than 0\n");
        return 1;
    }

    // Validate level
    if (level < 1) {
        printf("Invalid level, must be greater than 0\n");
        return 1;
    }

    int width, height;
    unsigned char *image = read_png(input_file, &width, &height);  // Read the input image
    if (!image) {
        return 1;
    }

    printf("Applying median filter with size %ux%u...\n", filter_size, filter_size);

    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC, &start);    // start
    
    unsigned char * filtered_image;
    // main logic
    for (int i = 0; i < level; ++i) {
        filtered_image = median_filter(image, width, height, filter_size);  // Apply median filter
        free(image);
        image = filtered_image;
    }

    clock_gettime(CLOCK_MONOTONIC, &end);      // end

    double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) * 1e-9;

    printf("madian filter time taken: %.7lf\n", time_taken);

    save_png(output_file, image, width, height);  // Save the output image

    free(image);  // Free the image memory
    return 0;
}
