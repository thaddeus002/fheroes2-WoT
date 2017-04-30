/* yImage : sauvegarder des images dans différents formats */
/* fonctions inspirées de la bibliothèque Imlib */
/* => yImage est sous licence GPL (GNU GENERAL PUBLIC LICENSE) */

#include "stdint.h"
#include <stdio.h>

#ifndef YIMAGE_H_
#define YIMAGE_H_

/* qualité par défaut de la compression JPEG = 80% (208/255) */
#define DEFAULT_JPEG_QUALITY 208


/* taille de la page postscript à créer*/
#define PAGE_SIZE_EXECUTIVE    0
#define PAGE_SIZE_LETTER       1
#define PAGE_SIZE_LEGAL        2
#define PAGE_SIZE_A4           3
#define PAGE_SIZE_A3           4
#define PAGE_SIZE_A5           5
#define PAGE_SIZE_FOLIO        6

/* error code */
#define ERR_NULL_PALETTE -1
#define ERR_NULL_COLOR -2
#define ERR_BAD_INDEX -3
#define ERR_ALLOCATE_FAIL -4




/* pour représenter une couleur */
typedef struct _yColor {
    unsigned char r, g, b; /* Rouge, Vert et Bleu entre 0 et 255 */
    unsigned char alpha; /* 0 => transparence ; 255 => opacité */
} yColor;



/* for 8 bits color images */
/* palette de couleurs */
typedef uint8_t yColorPalette_t[256*3];



/* une image raster */
typedef struct {
    unsigned char *rgbData; /* tableau RVBRVGRVB... */
    unsigned char *alphaChanel; /* array of alpha (8bits) values */
    int rgbWidth, rgbHeight; /* largeur et hauteur */
    int presShapeColor; /* indicate if shape_color is use or not */
    /* available if alpha_chanel == NULL and pres_shape_color != 0 */
    yColor shapeColor; /* couleur correspondant à un pixel transparent */
} yImage;


/* Image raster avec canal alpha */
/* alpha == 0 => transparence totale */
/* alpha == 255 => opacité */
//typedef struct {
//  unsigned char *rgba_data; /* tableau RVBARVBA... */
//  int rgba_width, rgba_height; /* dimensions de l'image */
//} yAlphaImage;


/************************************************************/
/*               CREATION / DESTRUCTION DES IMAGES          */
/************************************************************/

/** Cette fonction spécifique aux tools fheroes2 est-elle utile ?? */
void init_palette(yColorPalette_t palette, const uint8_t *pal);

/* create an yImage without transparency */
/* im must be allocated */
//int create_yImage(yImage *im, const unsigned char *rgb_data, int width, int height);
/* Create an yImage */
yImage *create_yImage(int *err, const unsigned char *rgb_data, int width, int height);



/* libération de la memoire */
void destroy_yImage(yImage *im);

/************************************************************/
/*               MANIPULATION DES IMAGES                    */
/************************************************************/

/* recupere une couleur sur la palette */
int y_get_color_index(yColor *color, yColorPalette_t palette, int index);

/* create a color */
void y_set_color(yColor *color, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
void y_init_color(yColor *color, unsigned int rgba);

/* rend l'image transparente */
int transp(yImage *im);


void yImage_set_pixel(yImage *im, yColor *color, int x, int y);




#ifdef HAVE_LIBPNG
yImage *LoadPNG(FILE * f);
#endif


/************************************************************/
/*               SAUVEGARDE DES IMAGES                      */
/************************************************************/


/* sauvegarde "im" dans "file" au format ppm ou pnm */
/* retourne 1 en cas de succes */
int sauve_ppm(yImage *im, const char *file);


/* sauvegarde "im" dans "file" au format pgm */
/* retourne 1 en cas de succes */
int sauve_pgm(yImage *im, const char *file);


/* sauvegarde "im" dans "file" au format ps */
/* retourne 1 en cas de succes */
/* valeurs conseillées :
   scaling = 1024;
   xjustification = 512;
   yjustification = 512;
   color = 1; */
int sauve_ps(yImage *im, const char *file, int page_size, int scaling, int xjustification, int yjustification, char color);


/* sauvegarde "im" dans "file" au format JPEG */
/* retourne 1 en cas de succes */
/* nécessite la bibliothèque libjpeg */
int sauve_jpeg(yImage *im, const char *file);


/* sauvegarde "im" dans "file" au format PNG */
/* retourne 1 en cas de succes */
/* nécessite la bibliothèque libpng */
int sauve_png(yImage *im, const char *file);

/* sauvegarde "im" dans "file" au format PNG */
/* retourne 1 en cas de succes */
/* nécessite la bibliothèque libpng */
//int sauve_alphaImage_png(yAlphaImage *im, const char *file);


/* sauvegarde "im" dans "file" au format TIFF */
/* retourne 1 en cas de succes */
/* nécessite la bibliothèque libtiff */
int sauve_tiff(yImage *im, const char *file);

#endif
