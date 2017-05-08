/**
 * \file yColor.c
 * \brief Implementation of four channels color representation.
 */



#include "yColor.h"
#include <stdlib.h>


void init_palette(yColorPalette_t palette, const uint8_t *pal){
    int i, j; /* compteur */

    if(pal==NULL) return;

    for(i=0; i<=255; i++){
        for(j=0; j<=2; j++) {
            palette[3*i+j]=pal[3*i+j]*4;
        }
    }
}



int y_get_color_index(yColor *color, yColorPalette_t palette, int index){
    if(color==NULL) return(ERR_NULL_COLOR);
    if(palette==NULL) return(ERR_NULL_PALETTE);

    if((index<0)||(index>255)) return(ERR_BAD_INDEX);

    color->r=palette[3*index];
    color->g=palette[3*index+1];
    color->b=palette[3*index+2];
    color->alpha=255;

    return(0);
}


// yColor functions

yColor *y_color(ySimpleColor color){

    int r,g,b;
    yColor *result;

    result = (yColor *) malloc(sizeof(yColor));
    if(result == NULL) return NULL;

    switch(color) {
    case BLACK:
        r=0; g=0; b=0; break;
    case WHITE:
        r=255; g=255; b=255; break;
    case RED:
        r=255; g=0; b=0; break;
    case GREEN:
        r=0; g=255; b=0; break;
    case BLUE:
        r=0; g=0; b=255; break;
    case ORANGE_:
        r=255; g=160; b=0; break;
    case YELLOW:
        r=0; g=255; b=255; break;
    case CYAN_:
        r=255; g=0; b=255; break;
    case MAGENTA_:
        r=255; g=255; b=0; break;
    case MARRON_:
        r=112; g=80; b=0;
    }

    y_set_color(result, r, g, b, 255);
    return result;
}



void y_set_color(yColor *color, unsigned char r, unsigned char g, unsigned char b, unsigned char a){
    color->r=r;
    color->g=g;
    color->b=b;
    color->alpha=a;
}



void y_init_color(yColor *color, unsigned int rgba){
    color->r=rgba/(256*256*256);
    color->g=(rgba/(256*256))%256;
    color->b=(rgba/256)%(256*256);
    color->alpha=rgba%(256*256*256);
}



/**
 *
 * \return 0 if the colors are identical
 */
int compare_colors(yColor *c1, yColor *c2) {

    int comp=0;

    if(c1->r != c2->r) comp++;
    if(c1->g != c2->g) comp++;
    if(c1->b != c2->b) comp++;
    if(c1->alpha != c2->alpha) comp++;

    return comp;
}

