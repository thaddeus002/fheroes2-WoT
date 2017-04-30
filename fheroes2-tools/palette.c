/**
 * This is a 256 colors palette tool, for coloring HOMM images files.
 *
 */

#include <stdio.h>
#include <stdlib.h> //malloc()
#include "palette.h"


/* Heroes of Migth and Magic II palette. to use by default. */
static unsigned char default_pal[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x3f,
    0x3f, 0x3c, 0x3c, 0x3c, 0x3a, 0x3a, 0x3a, 0x37, 0x37, 0x37, 0x35, 0x35, 0x35, 0x32, 0x32, 0x32,
    0x30, 0x30, 0x30, 0x2d, 0x2d, 0x2d, 0x2b, 0x2b, 0x2b, 0x29, 0x29, 0x29, 0x26, 0x26, 0x26, 0x24,
    0x24, 0x24, 0x21, 0x21, 0x21, 0x1f, 0x1f, 0x1f, 0x1c, 0x1c, 0x1c, 0x1a, 0x1a, 0x1a, 0x17, 0x17,
    0x17, 0x15, 0x15, 0x15, 0x12, 0x12, 0x12, 0x10, 0x10, 0x10, 0x0e, 0x0e, 0x0e, 0x0b, 0x0b, 0x0b,
    0x09, 0x09, 0x09, 0x06, 0x06, 0x06, 0x04, 0x04, 0x04, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x3f,
    0x3b, 0x37, 0x3c, 0x37, 0x32, 0x3a, 0x34, 0x2e, 0x38, 0x31, 0x2a, 0x36, 0x2e, 0x26, 0x34, 0x2a,
    0x22, 0x32, 0x28, 0x1e, 0x30, 0x25, 0x1b, 0x2e, 0x22, 0x18, 0x2b, 0x1f, 0x15, 0x29, 0x1c, 0x12,
    0x27, 0x1a, 0x0f, 0x25, 0x18, 0x0d, 0x23, 0x15, 0x0b, 0x21, 0x13, 0x08, 0x1f, 0x11, 0x07, 0x1d,
    0x0f, 0x05, 0x1a, 0x0d, 0x04, 0x18, 0x0c, 0x03, 0x16, 0x0a, 0x02, 0x14, 0x09, 0x01, 0x12, 0x07,
    0x01, 0x0f, 0x06, 0x00, 0x0d, 0x05, 0x00, 0x0b, 0x04, 0x00, 0x09, 0x03, 0x00, 0x30, 0x33, 0x3f,
    0x2b, 0x2e, 0x3c, 0x26, 0x2a, 0x3a, 0x22, 0x26, 0x38, 0x1e, 0x22, 0x36, 0x1a, 0x1e, 0x34, 0x16,
    0x1a, 0x31, 0x13, 0x16, 0x2f, 0x10, 0x13, 0x2d, 0x0d, 0x10, 0x2b, 0x0a, 0x0d, 0x29, 0x08, 0x0c,
    0x26, 0x07, 0x0a, 0x24, 0x05, 0x09, 0x22, 0x04, 0x08, 0x20, 0x03, 0x07, 0x1e, 0x02, 0x06, 0x1c,
    0x01, 0x05, 0x19, 0x01, 0x05, 0x17, 0x00, 0x04, 0x15, 0x00, 0x03, 0x13, 0x00, 0x03, 0x11, 0x2b,
    0x38, 0x27, 0x27, 0x35, 0x23, 0x24, 0x33, 0x20, 0x20, 0x30, 0x1c, 0x1d, 0x2e, 0x19, 0x1a, 0x2c,
    0x17, 0x17, 0x29, 0x14, 0x14, 0x27, 0x11, 0x12, 0x24, 0x0f, 0x0f, 0x22, 0x0c, 0x0d, 0x1f, 0x0a,
    0x0b, 0x1d, 0x09, 0x09, 0x1b, 0x07, 0x08, 0x19, 0x06, 0x06, 0x17, 0x05, 0x05, 0x15, 0x03, 0x03,
    0x13, 0x02, 0x02, 0x10, 0x01, 0x01, 0x0e, 0x01, 0x01, 0x0c, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x08,
    0x00, 0x00, 0x06, 0x00, 0x3f, 0x3d, 0x34, 0x3e, 0x3a, 0x2b, 0x3d, 0x38, 0x23, 0x3c, 0x37, 0x1b,
    0x3b, 0x35, 0x14, 0x3a, 0x33, 0x0d, 0x39, 0x32, 0x05, 0x38, 0x31, 0x00, 0x36, 0x2f, 0x08, 0x34,
    0x2c, 0x07, 0x32, 0x28, 0x06, 0x2f, 0x26, 0x06, 0x2d, 0x23, 0x06, 0x2a, 0x1f, 0x05, 0x27, 0x1c,
    0x04, 0x25, 0x19, 0x03, 0x22, 0x16, 0x03, 0x1f, 0x13, 0x02, 0x1d, 0x11, 0x02, 0x1a, 0x0f, 0x00,
    0x18, 0x0c, 0x00, 0x15, 0x0a, 0x00, 0x13, 0x08, 0x00, 0x39, 0x33, 0x3e, 0x36, 0x2f, 0x3b, 0x32,
    0x2a, 0x39, 0x30, 0x27, 0x36, 0x2d, 0x23, 0x34, 0x2a, 0x1f, 0x31, 0x27, 0x1c, 0x2f, 0x24, 0x19,
    0x2d, 0x21, 0x16, 0x2a, 0x1e, 0x13, 0x28, 0x1c, 0x11, 0x25, 0x19, 0x0e, 0x23, 0x17, 0x0c, 0x20,
    0x14, 0x0a, 0x1e, 0x12, 0x08, 0x1b, 0x10, 0x06, 0x19, 0x0e, 0x05, 0x17, 0x0b, 0x02, 0x14, 0x08,
    0x01, 0x11, 0x06, 0x00, 0x0e, 0x04, 0x00, 0x0b, 0x2d, 0x3d, 0x3f, 0x2a, 0x3a, 0x3c, 0x28, 0x38,
    0x3a, 0x25, 0x36, 0x38, 0x22, 0x33, 0x35, 0x20, 0x31, 0x33, 0x1e, 0x2e, 0x31, 0x1c, 0x2c, 0x2f,
    0x19, 0x2a, 0x2c, 0x17, 0x27, 0x2a, 0x16, 0x25, 0x28, 0x14, 0x23, 0x25, 0x12, 0x20, 0x23, 0x10,
    0x1d, 0x20, 0x0e, 0x1a, 0x1d, 0x0c, 0x18, 0x1b, 0x0a, 0x15, 0x18, 0x08, 0x13, 0x16, 0x07, 0x10,
    0x13, 0x05, 0x0e, 0x10, 0x04, 0x0b, 0x0e, 0x03, 0x09, 0x0b, 0x02, 0x07, 0x09, 0x3f, 0x39, 0x39,
    0x3d, 0x34, 0x34, 0x3c, 0x2f, 0x2f, 0x3a, 0x2b, 0x2b, 0x39, 0x27, 0x27, 0x37, 0x23, 0x23, 0x36,
    0x1f, 0x1f, 0x34, 0x1b, 0x1b, 0x33, 0x17, 0x17, 0x31, 0x14, 0x14, 0x30, 0x11, 0x11, 0x2f, 0x0e,
    0x0e, 0x2e, 0x0b, 0x0b, 0x2d, 0x09, 0x09, 0x2a, 0x08, 0x08, 0x27, 0x06, 0x06, 0x24, 0x04, 0x04,
    0x21, 0x03, 0x03, 0x1e, 0x02, 0x02, 0x1b, 0x01, 0x01, 0x18, 0x00, 0x00, 0x15, 0x00, 0x00, 0x12,
    0x00, 0x00, 0x3f, 0x39, 0x27, 0x3e, 0x36, 0x23, 0x3d, 0x34, 0x1f, 0x3c, 0x31, 0x1c, 0x3b, 0x2e,
    0x18, 0x3a, 0x2b, 0x14, 0x39, 0x28, 0x11, 0x38, 0x24, 0x0e, 0x38, 0x21, 0x0b, 0x33, 0x1d, 0x08,
    0x2e, 0x19, 0x06, 0x29, 0x16, 0x04, 0x25, 0x12, 0x02, 0x20, 0x0f, 0x01, 0x1b, 0x0c, 0x00, 0x17,
    0x0a, 0x00, 0x3f, 0x16, 0x03, 0x37, 0x0d, 0x01, 0x30, 0x05, 0x00, 0x29, 0x00, 0x00, 0x3f, 0x3f,
    0x00, 0x3f, 0x33, 0x00, 0x30, 0x23, 0x00, 0x23, 0x12, 0x00, 0x29, 0x34, 0x00, 0x25, 0x2f, 0x00,
    0x21, 0x2b, 0x00, 0x1e, 0x27, 0x01, 0x1a, 0x23, 0x01, 0x17, 0x1e, 0x01, 0x13, 0x1a, 0x01, 0x10,
    0x16, 0x01, 0x0d, 0x12, 0x01, 0x0a, 0x1e, 0x34, 0x06, 0x1a, 0x31, 0x01, 0x12, 0x2d, 0x00, 0x0e,
    0x2b, 0x03, 0x15, 0x2f, 0x00, 0x0e, 0x2b, 0x00, 0x10, 0x2d, 0x21, 0x38, 0x3f, 0x00, 0x26, 0x3f,
    0x00, 0x14, 0x39, 0x00, 0x00, 0x29, 0x23, 0x23, 0x2f, 0x1c, 0x1c, 0x27, 0x15, 0x15, 0x1f, 0x0f,
    0x0f, 0x17, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

unsigned char pal[256*3];

unsigned char *palette = default_pal;

/* there is a spécific traitement for version 1 */
int version = 2;


/**
 * Read palette data from a file.
 * \param filename The name of a pal file.
 * \return 0 in case of success. An error code otherwise
 */
int homm_init_palette(const char *filename){
    return homm_init_palette(filename, 2);
}

/**
 * Read palette data from a file.
 * \param filename The name of a pal file.
 * \param version If 1, take in account a translation function for the palette when getting a color
 * \return 0 in case of success. An error code otherwise
 */
int homm_init_palette(const char *filename, int homm_version){

    FILE *fd;
    int i;
    int success;

    version = homm_version;

    success=1; // we are optimistic ;)

    fd = fopen(filename, "r");



    if(fd == NULL){
        fprintf(stderr, "Impossible de lire le fichier %s\n", filename);
        return -1;
    }

    for(i=0; i<3*256; i++){

        if(fread(pal+i, 1, 1, fd)!=1){
            success=0;
            break;
        }

        if(pal[i]>=64){
            fprintf(stderr, "Unexpected value :%d\n", pal[i]);
            success=0;
            break;
        }

        //pal[i]=pal[i]*4;
    }

    fclose(fd);

    if(!success) {
        fprintf(stderr, "Echec de création de la palette, utilisation de la palette par défaut\n", filename);
        return -1;
    }
    palette = pal;
    return 0;
}

int translate(int index) {

    int i = index;

    if( (index>=32) && (index<54)){
        i+=4+((index-32)/4);
    }
    if( (index>=54) && (index<=256)){
        i+=10;
        i=i%256;
    }



    return i;
}

/**
 * Find a color in a palette. Default palette is used if none had been setted by homeinit_palette() function.
 * \param index The number of the color in palette. Must be >= 0 and <256
 * \return the corresponding color (transparent if number is out of range)
 */
yColor getColor(int index){

    yColor retour;

    if((index<0)||(index>255)) {

        retour.r=0;
        retour.g=0;
        retour.b=0;
        retour.alpha=0;

    } else {

        int i = index;
        if(version == 1) {
            i = translate(index);
        }

        retour.r=palette[3*i]*4;
        retour.g=palette[3*i+1]*4;
        retour.b=palette[3*i+2]*4;
        retour.alpha=255;
    }
    return retour;
}


/**
 * Allocate memory to transform a table of pixels' number in a bitmap of RGB colors.
 * \param data A table of uint8_t which represent an image (palette indexes)
 * \param the size of data table
 * \return a table of RGB data, that need to be freed
 */
unsigned char *create_bitmap(unsigned char *data, int size){

    int i; // counter
    unsigned char *rgb = (unsigned char *) malloc(3*size);

    for(i=0; i<size; i++){

        yColor color = getColor(data[i]);

        rgb[3*i]=color.r;
        rgb[3*i+1]=color.g;
        rgb[3*i+2]=color.b;
    }

    return rgb;
}



/**
 * Create a picture representing the palette.
 * \param filename The name of file to create
 * \return 0 in case of success
 */
int draw_palette(const char *filename){

    yImage *im;
    int err=0;
    int i;
    yColor color;

    if(filename == NULL) {
        return -1;
    }

    fprintf(stdout, "Dessin de la palette dans le fichier %s\n", filename);

    im=create_yImage(&err, NULL, 16*16, 16*16);

    if(err) {
        return err;
    }

    for(i=0; i<256*256; i++){
        int x, y;
        int k, l;
        int index;

        x=i%256;
        y=i/256;

        k=x/16; l=y/16;

        index = l*16 + k;

        //fprintf(stdout, " %d", index);

        color = getColor(index);
        yImage_set_pixel(im, &color, x, y);
    }

    err = sauve_png(im, filename);

    destroy_yImage(im);
    return err;
}

