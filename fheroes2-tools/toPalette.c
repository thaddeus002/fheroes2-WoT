/**
 * \file toPalette.c
 * \brief A program to modify an image for using only the available
 * colors in a palette.
 */


#include <stdio.h>
#include <stdlib.h>
#include "palette.h"
#include "yImage.h"
#include "yImage_io.h"
#include "color_distance.h"


/**
 * The usage function.
 */
static void usage(char *prog) {
    fprintf(stdout, "Usage : %s <png_image>\n", prog);
    exit(1);
}


// may replace the standard distance calculation between colors
static int distance_ciede2000(yColor c1, yColor c2) {

    int rgb1 = c1.r*256*256+c1.g*256+c1.b;
    int rgb2 = c2.r*256*256+c2.g*256+c2.b;

    return color_diff(rgb1, rgb2);
}


// restore transparent and shadow pixels from input image
static void copy_transparency(yImage *input, yImage *output){
    for(int i = 0; i < input->rgbWidth * input->rgbHeight; i++) {
        if(input->alphaChanel[i]==0) {
            output->alphaChanel[i] = 0;
        } else if(input->alphaChanel[i]<250) {
            // put a shadow pixel
            output->alphaChanel[i] = 0x40;
            output->rgbData[3*i] = 0;
            output->rgbData[3*i+1] = 0;
            output->rgbData[3*i+2] = 0;
        } else {
           output->alphaChanel[i] = 255;
        }
    }
}


int main(int argc, char **argv) {

    if(argc != 2) usage(argv[0]);

    char *inputFile = argv[1];
    yImage *input = y_load_png(inputFile);

    if(input == NULL) {
        fprintf(stderr, "Could not read input file %s\n", inputFile);
        return 2;
    }

    unsigned char *colorMap = toPalette(input);
    if(colorMap == NULL) {
        fprintf(stderr, "Could not calculate colorMap - Exit\n");
        y_destroy_image(input);
        return 3;
    }

    unsigned char *bitmap = create_bitmap(colorMap, input->rgbWidth * input->rgbHeight);
    free(colorMap);
    if(bitmap == NULL) {
        fprintf(stderr, "Could not create the bitmap - Exit\n");
        y_destroy_image(input);
        return 4;
    }

    int err = 0;
    yImage *output = y_create_image(&err, bitmap, input->rgbWidth, input->rgbHeight);
    free(bitmap);

    if(err) {
        fprintf(stderr, "An error occured while creating image : %d\n", err);
    } else {
        // restore transparent and shadow pixels from input image
        copy_transparency(input, output);
        err = y_save_png(output, "toPalette.png");
        if(err) {
            fprintf(stderr, "An error occured while saving image : %d\n", err);
        }
    }

    y_destroy_image(input);

    return err;
}
