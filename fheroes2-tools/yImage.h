/**
 * \file yImage.h
 * \brief images representation and basic operations. Plus load or save actions.
 *
 * Can save images in various formats with functions which are inspired by the
 * imlib library.
 *
 * This file is under GPL license (GNU GENERAL PUBLIC LICENSE).
 */

#include <stdint.h>
#include <stdio.h>
#include "yColor.h"


#ifndef __Y_IMAGE_H_
#define __Y_IMAGE_H_


/**
 * \brief A point define a pixel position in the image
 */
typedef struct {
    int x;
    int y;
} yPoint;



/** \brief A raster image */
typedef struct {
    unsigned char *rgbData; /* tableau RVBRVGRVB... */
    unsigned char *alphaChanel; /* array of alpha (8bits) values */
    int rgbWidth, rgbHeight; /* largeur et hauteur */
    int presShapeColor; /* indicate if shape_color is use or not */
    /* available if alpha_chanel == NULL and presShapeColor != 0 */
    yColor shapeColor; /* couleur correspondant à un pixel transparent */
} yImage;



/************************************************************/
/*                    CREATE / DESTROY IMAGES               */
/************************************************************/

/**
 * Create an yImage without transparency.
 * \param rbg_data the background image. Background will be black if NULL
 */
yImage *create_yImage(int *err, const unsigned char *rgb_data, int width, int height);


/**
 * create an yImage with an uniform background color
 */
yImage *create_uniform_yImage(int *err, yColor *background, int width, int height);


/**
 * free memory
 */
void destroy_yImage(yImage *im);

/************************************************************/
/*                   HANDLING IMAGES                        */
/************************************************************/


/**
 * \brief retrive the color of a pixel in the image
 * \param im the image
 * \param x x coordinate of the pixel
 * \param y y coordinate of the pixel
 * \return a newly allocated yColor or NULL in case of fail
 */
yColor *y_get_color(yImage *im, int x, int y);


/**
 * \brief set the max transparency to the image
 * \return 0 in case of success, or a negative error code
 */
int transp(yImage *im);


/**
 * \brief superimpose two images.
 */
void superpose_images(yImage *back, yImage *fore, int x, int y);


/**
 * \brief Each pixel of the image will become white with alpha=max(r,g,b).
 *
 * So, black will become full transparency, white will remain white.
 */
void y_grey_level_to_alpha(yImage *im);



/************************************************************/
/*                   DRAWING FUNCTIONS                      */
/************************************************************/

/**
 * \brief Set a color on a pixel of the image.
 * \param im the image to modify
 * \param color the color for the pixel
 * \param x x coordinate of pixel : 0 is left border
 * \param y y coordinate of pixel : 0 is top border
 */
void yImage_set_pixel(yImage *im, yColor *color, int x, int y);


/**
 * \brief Draw a line between two pixels on the image.
 * \param im the image where to draw
 * \param color the color for the line
 * \param x1 x coordinate of first pixel : 0 is left border
 * \param y1 y coordinate of first pixel : 0 is top border
 * \param x2 x coordinate of second pixel
 * \param y2 y coordinate of second pixel
 */
void y_draw_line(yImage *im, yColor *color, int x1, int y1, int x2, int y2);


/**
 * \brief draw some lines by linking points.
 * \param im the image where to draw
 * \param color the color for the lines
 * \param points a points table
 * \param nbPoints size of the points table
 */
void y_draw_lines(yImage *im, yColor *color, yPoint *points, int nbPoints);



#endif
