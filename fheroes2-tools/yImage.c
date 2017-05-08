/**
 * \file yImage.c : sauvegarder des images dans différents formats
 * fonctions inspirées de la bibliothèque Imlib 1.x
 *      => yImage est sous licence GPL (GNU GENERAL PUBLIC LICENSE)
 */


#include "yImage.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h> // round()
#include <ctype.h>
#include <string.h> //memset()


/************************************************************/
/*               CREATION / DESTRUCTION DES IMAGES          */
/************************************************************/


/* create an yImage without transparency */
yImage *create_yImage(int *err, const unsigned char *rgbData, int width, int height){

    yImage *im;

    im=(yImage *)malloc(sizeof(yImage));

    if (im==NULL) {
        *err=ERR_ALLOCATE_FAIL;
        return(NULL);
    }

    im->rgbData=(unsigned char *) malloc(3*width*height);
    if(im->rgbData==NULL) {
        *err=ERR_ALLOCATE_FAIL;
        free(im);
        return(NULL);
    }

    im->alphaChanel=(unsigned char *) malloc(width*height);
        if(im->alphaChanel==NULL) {
        *err=ERR_ALLOCATE_FAIL;
        free(im->rgbData);
        free(im);
        return(NULL);
    }

    memset(im->alphaChanel, 255, width*height);

    if(rgbData==NULL) {
        memset(im->rgbData, 0, 3*width*height);
    } else memcpy(im->rgbData, rgbData, 3*width*height);

    im->rgbHeight=height;
    im->rgbWidth=width;

    im->presShapeColor=0;

    im->shapeColor.r=0;
    im->shapeColor.g=0;
    im->shapeColor.b=0;

    *err=0;
    return(im);
}


yImage *create_uniform_yImage(int *err, yColor *background, int width, int height){

    yImage *img = create_yImage(err, NULL, width, height);
    int pix;

    for(pix=0; pix<width*height; pix++) {
        img->rgbData[3*pix+0]=background->r;
        img->rgbData[3*pix+1]=background->g;
        img->rgbData[3*pix+2]=background->b;
    }

    memset(img->alphaChanel, background->alpha, width*height);

    return img;
}




/* libération de la memoire */
void destroy_yImage(yImage *im){
    if(im!=NULL){
        if(im->rgbData!=NULL) free(im->rgbData);
        free(im);
    }
}


/************************************************************/
/*               MANIPULATION DES IMAGES                    */
/************************************************************/


yColor *y_get_color(yImage *im, int x, int y){

    yColor *value = NULL;
    int pos;

    pos = x + y*im->rgbWidth;

    if(pos < im->rgbWidth*im->rgbHeight) {

        value = (yColor *) malloc(sizeof(yColor));
        if(value==NULL) return NULL;

        value->r = im->rgbData[3*pos];
        value->g = im->rgbData[3*pos+1];
        value->b = im->rgbData[3*pos+2];
        value->alpha = im->alphaChanel[pos];
    }

    return value;
}





/* rend l'image transparente */
int transp(yImage *im){
    if(im==NULL) return -1;

    if(im->alphaChanel==NULL) im->alphaChanel=(unsigned char *)malloc(im->rgbWidth*im->rgbHeight);
    if(im->alphaChanel==NULL) return ERR_ALLOCATE_FAIL;

    memset(im->alphaChanel, 0, im->rgbWidth*im->rgbHeight);
    im->presShapeColor=0;

    return 0;
}




void superpose_images(yImage *back, yImage *fore, int x, int y){

    yColor composition;
    int i, j;

    for(i=0; i<fore->rgbWidth; i++)
        for(j=0; j<fore->rgbHeight; j++){

        int xb, yb;
        xb=i+x; yb=j+y;

        if((xb>=0) && (xb<back->rgbWidth) && (y>=0) && (yb<back->rgbHeight)){

            int ab= back->alphaChanel[xb+yb*back->rgbWidth];
            int rb= back->rgbData[3*(xb+yb*back->rgbWidth)];
            int gb= back->rgbData[3*(xb+yb*back->rgbWidth)+1];
            int bb= back->rgbData[3*(xb+yb*back->rgbWidth)+2];

            int af= fore->alphaChanel[i+j*fore->rgbWidth];
            int rf= fore->rgbData[3*(i+j*fore->rgbWidth)];
            int gf= fore->rgbData[3*(i+j*fore->rgbWidth)+1];
            int bf= fore->rgbData[3*(i+j*fore->rgbWidth)+2];

            yColor foreColor;
            y_set_color(&foreColor, rf, gf, bf, af);

            if(!fore->presShapeColor || compare_colors(&(fore->shapeColor), &foreColor)) {
                /* TODO vraie superposition de couleurs */
                composition.r=((255-af)*rb + af*rf)/255;
                composition.b=((255-af)*bb + af*bf)/255;//af>0?bf:bb;
                composition.g=((255-af)*gb + af*gf)/255;//af>0?gf:gb;
                composition.alpha=ab+((255-ab)*af/255);

                yImage_set_pixel(back, &composition, xb, yb);
            }
        }
    }
}



void y_grey_level_to_alpha(yImage *im){

    int i, j;
    yColor *color;


    for(i=0; i<im->rgbWidth; i++) {
        for(j=0; j<im->rgbHeight; j++) {

            color = y_get_color(im, i, j);

            if(color!=NULL) {

                int m = color->r;
                if(color->g>m) m=color->g;
                if(color->b>m) m=color->b;

                if(m>0) {

                    color->r=255;
                    color->g=255;
                    color->b=255;
                }
                color->alpha=m;
                yImage_set_pixel(im, color, i, j);

                free(color);
            }
        }
    }
}





/************************************************************/
/*                   DRAWING FUNCTIONS                      */
/************************************************************/



void yImage_set_pixel(yImage *im, yColor *color, int x, int y){
    int pos; /* position of the pixel (x,y) in the data array */

    if(im==NULL) return;
    if(color==NULL) return;
    if((x<0) || (y<0)) return;
    if((x>=im->rgbWidth) || (y>=im->rgbHeight) )return;

    pos = 3*y*im->rgbWidth + 3*x;

    im->rgbData[pos]=color->r;
    im->rgbData[pos+1]=color->g;
    im->rgbData[pos+2]=color->b;

    im->alphaChanel[y*im->rgbWidth + x]=color->alpha;
}


/** \return 0 if x is not between a and b */
static int entre(int x, int a, int b) {

    if(a<=b && x>=a && x<=b) {
        return 1;
    }

    if(a>b && x>=b && x<=a) {
        return 1;
    }

    return 0;
}



void y_draw_line(yImage *im, yColor *color, int x1, int y1, int x2, int y2) {

    // cross differently if deltaX > deltaY or not
    int horizontal;
    // where start and stop the line
    int begin, end;
    // range in the direction we are not crossing
    int bottom, top;
    // counter
    int i;

    if( abs(y2-y1) > abs(x2-x1) ) {
        horizontal = 0;
        begin = y1;
        end = y2;
        bottom = x1;
        top = x2;
    } else {
        horizontal = 1;
        begin = x1;
        end = x2;
        bottom = y1;
        top = y2;
    }

    if(end == begin) {
        // only one point to draw

        int x, y;

        if(horizontal) {
            x = begin;
            y = bottom;
        } else {
            y = begin;
            x = bottom;
        }

        yImage_set_pixel(im, color, x, y);
        return;
    }


    if(begin > end) {
        int tmp;
        tmp=end;
        end=begin; begin=tmp;
        tmp=top; top=bottom; bottom=tmp;
    }

    // Cross from point1 to point2
    for(i=begin;i<=end;i++) {

        int x, y;
        double tmp;
        int pos;

        // calculate x and y

        tmp = (double)bottom + ((double)(top - bottom))*(double)(i-begin)/(double)(end - begin);
        pos = (int) round(tmp);

        if(horizontal) {
            x = i;
            y = pos;
        } else {
            y = i;
            x = pos;
        }


        if( !entre(x, x1,x2)  || !entre(y, y1, y2)) {
            fprintf(stderr, "bad point calculation : (%d,%d) not between (%d,%d) and (%d,%d)\n", x, y, x1,y1,x2,y2);
        }

        // draw point
        yImage_set_pixel(im, color, x, y);
    }
}



void y_draw_lines(yImage *im, yColor *color, yPoint *points, int nbPoints){

    int i;

    if(nbPoints == 0) return;

    if(nbPoints == 1) {
        yImage_set_pixel(im, color, points->x, points->y);
        return;
    }

    for(i=0; i<nbPoints-1; i++) {

        y_draw_line(im, color, points[i].x, points[i].y,points[i+1].x, points[i+1].y);
    }
}


