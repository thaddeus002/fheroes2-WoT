/**
 * \file toPalette.c
 */


#include <stdio.h>
#include <stdlib.h>
#include "palette.h"
#include "yImage.h"
#include "yImage_io.h"


static void usage(char *prog) {
    fprintf(stdout, "Usage : %s <png_image>\n", prog);
    exit(1);
}


static int distance(yColor c1, yColor c2) {

    return (c1.r-c2.r)*(c1.r-c2.r)+(c1.g-c2.g)*(c1.g-c2.g)+(c1.b-c2.b)*(c1.b-c2.b);
}




/**
 * \param rbg a three unsigned chars array representing a rgb color
 * \return the index of the nearest color in the palette
 */
static unsigned char palette_nearest(const unsigned char *rgb) {

    unsigned char index = -1;
    int minSqrDist = 210000;
    yColor color;
    y_set_color(&color, rgb[0], rgb[1], rgb[2], 255);


    for(int i = 0; i < 256; i++) {
        // TODO skip cycling colors
        int sqrDist = distance(color, getColor(i));
        if(sqrDist < minSqrDist) {
            minSqrDist = sqrDist;
            index = i;
        }
        if(sqrDist == 0) break;
    }

    return index;
}


/**
 * \return a newly allocated array of color index
 */
// TODO this not manage transparent pixels
static unsigned char *toPalette(yImage *image) {

    unsigned char *colormap = malloc(sizeof(unsigned char) * image->rgbWidth * image->rgbHeight);

    if(colormap == NULL) return NULL;

    for(int i = 0; i < image->rgbWidth * image->rgbHeight; i++) {
        colormap[i] = palette_nearest(image->rgbData+3*i);
    }
    
    return colormap;
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
    y_destroy_image(input);
    free(bitmap);

    if(err) {
        fprintf(stderr, "An error occured while creating image : %d\n", err);
    } else {
        err = y_save_png(output, "toPalette.png");
    }

    if(err) {
        fprintf(stderr, "An error occured while saving image : %d\n", err);
    }

    return err;
}
