/**
 * \file ySaveImage.h
 * \brief load and save yImage in differents formats.
 *
 * fonctions inspirées de la bibliothèque Imlib 1.x
 * => yImage est sous licence GPL (GNU GENERAL PUBLIC LICENSE)
 */


#ifndef Y_SAVE_IMAGE_H_
#define Y_SAVE_IMAGE_H_


#include <stdint.h>
#include <stdio.h>
#include "yImage.h"


/** default quality for JPEG compression : 80% (208/255) */
#define DEFAULT_JPEG_QUALITY 208


/* postscript page's size */
#define PAGE_SIZE_EXECUTIVE    0
#define PAGE_SIZE_LETTER       1
#define PAGE_SIZE_LEGAL        2
#define PAGE_SIZE_A4           3
#define PAGE_SIZE_A3           4
#define PAGE_SIZE_A5           5
#define PAGE_SIZE_FOLIO        6


/**
 * \brief sauvegarde "im" dans "file" au format ppm ou pnm.
 * \return 1 en cas de succes
 */
int sauve_ppm(yImage *im, const char *file);


/**
 * \brief sauvegarde "im" dans "file" au format pgm.
 * \return 1 en cas de succes
 */
int sauve_pgm(yImage *im, const char *file);


/**
 * \brief sauvegarde "im" dans "file" au format ps
 * valeurs conseillées :
 *  scaling = 1024;
 *  xjustification = 512;
 *  yjustification = 512;
 *  color = 1;
 * \return 1 en cas de succes
 */
int sauve_ps(yImage *im, const char *file, int page_size, int scaling, int xjustification, int yjustification, char color);


/**
 * \brief sauvegarde "im" dans "file" au format JPEG
 * \return 1 en cas de succes
 * nécessite la bibliothèque libjpeg
 */
int sauve_jpeg(yImage *im, const char *file);


/**
 * \brief sauvegarde "im" dans "file" au format PNG
 * \return 0 en cas de succes
 * nécessite la bibliothèque libpng
 */
int sauve_png(yImage *im, const char *file);

//#ifdef HAVE_LIBPNG
/**
 * \brief Load a yImage from a png file
 * \param f file descriptor of the png image
 * \return the yImage contained in f
 */
yImage *LoadPNG(FILE * f);
//#endif




/**
 * \brief sauvegarde "im" dans "file" au format TIFF
 * \return retourne 1 en cas de succes
 * nécessite la bibliothèque libtiff
 */
int sauve_tiff(yImage *im, const char *file);






#endif
