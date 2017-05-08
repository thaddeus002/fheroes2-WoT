/**
 * \file ySaveImage.c : sauvegarder des images dans différents formats
 * fonctions inspirées de la bibliothèque Imlib 1.x
 *      => yImage est sous licence GPL (GNU GENERAL PUBLIC LICENSE)
 */


#include "ySaveImage.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h> //memset()

#define HAVE_LIBPNG

#ifdef HAVE_LIBPNG
#include "png.h"
#endif
#ifdef HAVE_LIBJPEG
#include "jpeglib.h"
#endif
#ifdef HAVE_LIBTIFF
#include "tiffio.h"
#endif



/* sauvegarde "im" dans "file" au format ppm ou pnm */
/* retourne 1 en cas de succes */
int sauve_ppm(yImage *im, const char *file){
    FILE *f; /* descripteur du fichier à créer */

    f = fopen(file, "wb");
    if (f)
    {
        if (!fprintf(f, "P6\n# Created by yImage\n%i %i\n255\n", im->rgbWidth, im->rgbHeight))
        {
            fclose(f);
            return 0;
        }
        if (!fwrite(im->rgbData, 1, (im->rgbWidth * im->rgbHeight * 3), f))
        {
            fclose(f);
            return 0;
        }
        fclose(f);
        return 1;
    }
    return(0);
}


/* sauvegarde "im" dans "file" au format pgm */
/* retourne 1 en cas de succes */
int sauve_pgm(yImage *im, const char *file)
{
    int                 x, y;
    unsigned char      *ptr, val;
    int                 v;
    FILE *f; /* descripteur du fichier à créer */

    f = fopen(file, "wb");
    if (f)
    {
        if (!fprintf(f, "P5\n# Created by yImage\n%i %i\n255\n", im->rgbWidth, im->rgbHeight))
        {
            fclose(f);
            return 0;
        }
        ptr = im->rgbData;
        for (y = 0; y < im->rgbHeight; y++)
        {
            for (x = 0; x < im->rgbWidth; x++)
            {
                v = (int)(*ptr++);
                v += (int)(*ptr++);
                v += (int)(*ptr++);
                val = (unsigned char)(v / 3);
                if (!fwrite(&val, 1, 1, f))
                {
                    fclose(f);
                    return 0;
                }
            }
        }
        fclose(f);
        return 1;
    }
    return(0);
}


/* sauvegarde "im" dans "file" au format ps */
/* retourne 1 en cas de succes */
/* valeurs conseillées :
   scaling = 1024;
   xjustification = 512;
   yjustification = 512;
   color=1; */
int sauve_ps(yImage *im, const char *file, int page_size, int scaling, int xjustification, int yjustification, char color)
{
      int                 bx, by, bxx, byy;
      int                 w, h;
      int                 sx, sy;
      int                 tx = 35, ty = 35;
      int                 x, y;
      unsigned char      *ptr;
      int                 v;
      FILE *f; /* descripteur du fichier à créer */

      sx = 0;
      sy = 0;
      f = fopen(file, "wb");

      if (f == NULL)
    return 0;

      w = im->rgbWidth;
      h = im->rgbHeight;

      switch (page_size)
    {
    case PAGE_SIZE_EXECUTIVE:
      sx = 540;
      sy = 720;
      break;
    case PAGE_SIZE_LETTER:
      sx = 612;
      sy = 792;
      break;
    case PAGE_SIZE_LEGAL:
      sx = 612;
      sy = 1008;
      break;
    case PAGE_SIZE_A4:
      sx = 595;
      sy = 842;
      break;
    case PAGE_SIZE_A3:
      sx = 842;
      sy = 1190;
      break;
    case PAGE_SIZE_A5:
      sx = 420;
      sy = 595;
      break;
    case PAGE_SIZE_FOLIO:
      sx = 612;
      sy = 936;
      break;
    }
      bxx = ((sx - (tx * 2)) * scaling) >> 10;
      byy = (int)(((float)h / (float)w) * (float)bxx);
      if ((((sy - (ty * 2)) * scaling) >> 10) < byy)
    {
      byy = ((sy - (ty * 2)) * scaling) >> 10;
      bxx = (int)(((float)w / (float)h) * (float)byy);
    }
      bx = tx + ((((sx - (tx * 2)) - bxx) * xjustification) >> 10);
      by = ty + ((((sy - (ty * 2)) - byy) * yjustification) >> 10);
      if (f)
    {
      fprintf(f, "%%!PS-Adobe-2.0 EPSF-2.0\n");
      fprintf(f, "%%%%Title: %s\n", file);
      fprintf(f, "%%%%Creator: Imlib by The Rasterman\n");
      fprintf(f, "%%%%BoundingBox: %i %i %i %i\n", bx, by, bxx, byy);
      fprintf(f, "%%%%Pages: 1\n");
      fprintf(f, "%%%%DocumentFonts:\n");
      fprintf(f, "%%%%EndComments\n");
      fprintf(f, "%%%%EndProlog\n");
      fprintf(f, "%%%%Page: 1 1\n");
      fprintf(f, "/origstate save def\n");
      fprintf(f, "20 dict begin\n");
      if (color)
        {
          fprintf(f, "/pix %i string def\n", w * 3);
          fprintf(f, "/grays %i string def\n", w);
          fprintf(f, "/npixls 0 def\n");
          fprintf(f, "/rgbindx 0 def\n");
          fprintf(f, "%i %i translate\n", bx, by);
          fprintf(f, "%i %i scale\n", bxx, byy);
          fprintf(f,
              "/colorimage where\n"
              "{ pop }\n"
              "{\n"
              "/colortogray {\n"
              "/rgbdata exch store\n"
              "rgbdata length 3 idiv\n"
              "/npixls exch store\n"
              "/rgbindx 0 store\n"
              "0 1 npixls 1 sub {\n"
              "grays exch\n"
              "rgbdata rgbindx       get 20 mul\n"
              "rgbdata rgbindx 1 add get 32 mul\n"
              "rgbdata rgbindx 2 add get 12 mul\n"
              "add add 64 idiv\n"
              "put\n"
              "/rgbindx rgbindx 3 add store\n"
              "} for\n"
              "grays 0 npixls getinterval\n"
              "} bind def\n"
              "/mergeprocs {\n"
              "dup length\n"
              "3 -1 roll\n"
              "dup\n"
              "length\n"
              "dup\n"
              "5 1 roll\n"
              "3 -1 roll\n"
              "add\n"
              "array cvx\n"
              "dup\n"
              "3 -1 roll\n"
              "0 exch\n"
              "putinterval\n"
              "dup\n"
              "4 2 roll\n"
              "putinterval\n"
              "} bind def\n"
              "/colorimage {\n"
              "pop pop\n"
              "{colortogray} mergeprocs\n"
              "image\n"
              "} bind def\n"
              "} ifelse\n");
          fprintf(f, "%i %i 8\n", w, h);
          fprintf(f, "[%i 0 0 -%i 0 %i]\n", w, h, h);
          fprintf(f, "{currentfile pix readhexstring pop}\n");
          fprintf(f, "false 3 colorimage\n");
          fprintf(f, "\n");
          ptr = im->rgbData;
          for (y = 0; y < h; y++)
        {
          for (x = 0; x < w; x++)
            {
              v = (int)(*ptr++);
              if (v < 0x10)
            fprintf(f, "0%x", v);
              else
            fprintf(f, "%x", v);
              v = (int)(*ptr++);
              if (v < 0x10)
            fprintf(f, "0%x", v);
              else
            fprintf(f, "%x", v);
              v = (int)(*ptr++);
              if (v < 0x10)
            fprintf(f, "0%x", v);
              else
            fprintf(f, "%x", v);
            }
          fprintf(f, "\n");
        }
        }
      else
        {
          fprintf(f, "/pix %i string def\n", w);
          fprintf(f, "/grays %i string def\n", w);
          fprintf(f, "/npixls 0 def\n");
          fprintf(f, "/rgbindx 0 def\n");
          fprintf(f, "%i %i translate\n", bx, by);
          fprintf(f, "%i %i scale\n", bxx, byy);
          fprintf(f, "%i %i 8\n", w, h);
          fprintf(f, "[%i 0 0 -%i 0 %i]\n", w, h, h);
          fprintf(f, "{currentfile pix readhexstring pop}\n");
          fprintf(f, "image\n");
          fprintf(f, "\n");
          ptr = im->rgbData;
          for (y = 0; y < h; y++)
        {
          for (x = 0; x < w; x++)
            {
              v = (int)(*ptr++);
              v += (int)(*ptr++);
              v += (int)(*ptr++);
              v /= 3;
              if (v < 0x10)
            fprintf(f, "0%x", v);
              else
            fprintf(f, "%x", v);
            }
          fprintf(f, "\n");
        }
        }
      fprintf(f, "\n");
      fprintf(f, "showpage\n");
      fprintf(f, "end\n");
      fprintf(f, "origstate restore\n");
      fprintf(f, "%%%%Trailer\n");
      fclose(f);
      return 1;
    }
    return(0);
}



/* sauvegarde "im" dans "file" au format JPEG */
/* retourne 1 en cas de succes */
/* nécessite la bibliothèque libjpeg */
int sauve_jpeg(yImage *im, const char *file)
{
    #ifdef HAVE_LIBJPEG
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPROW row_pointer[1];
    int row_stride;
    FILE *f; /* descripteur du fichier à créer */

    f = fopen(file, "wb");
    if(f)
    {
        cinfo.err = jpeg_std_error(&jerr);
        jpeg_create_compress(&cinfo);
        jpeg_stdio_dest(&cinfo, f);
        cinfo.image_width = im->rgbWidth;
        cinfo.image_height = im->rgbHeight;
        cinfo.input_components = 3;
        cinfo.in_color_space = JCS_RGB;
        jpeg_set_defaults(&cinfo);
        jpeg_set_quality(&cinfo, (100 * DEFAULT_JPEG_QUALITY) >> 8, TRUE);
        jpeg_start_compress(&cinfo, TRUE);
        row_stride = cinfo.image_width * 3;
        while (cinfo.next_scanline < cinfo.image_height)
        {
            row_pointer[0] = im->rgbData + (cinfo.next_scanline * row_stride);
            jpeg_write_scanlines(&cinfo, row_pointer, 1);
        }
        jpeg_finish_compress(&cinfo);
        fclose(f);
        return(1);
    }
    #endif
    return(0);
}



/* sauvegarde "im" dans "file" au format PNG */
/* retourne 0 en cas de succes */
/* nécessite la bibliothèque libpng */
int sauve_png(yImage *im, const char *file)
{
    #ifdef HAVE_LIBPNG
    png_structp png_ptr;
    png_infop info_ptr;
    unsigned char *data, *ptr;
    int x, y;
    png_bytep row_ptr;
    png_color_8 sig_bit;
    FILE *f; /* descripteur du fichier à créer */

    f = fopen(file, "wb");
    if (f)
    {
        png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        if (!png_ptr)
        {
            fclose(f);
            fprintf(stderr, "Fail create png file %s\n", file);
            return 1;
        }
        info_ptr = png_create_info_struct(png_ptr);
        if (info_ptr == NULL)
        {
            fclose(f);
            png_destroy_write_struct(&png_ptr, (png_infopp) NULL);
            fprintf(stderr, "Fail create png file %s\n", file);
            return 2;
        }
        //if (setjmp(png_ptr->jmpbuf))
        if (setjmp(png_jmpbuf(png_ptr)))
        {
            fclose(f);
            png_destroy_write_struct(&png_ptr, (png_infopp) NULL);
            fprintf(stderr, "Fail create png file %s\n", file);
            return 3;
        }
        png_init_io(png_ptr, f);
        png_set_IHDR(png_ptr, info_ptr, im->rgbWidth, im->rgbHeight, 8,
            PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
            PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
        sig_bit.red = 8;
        sig_bit.green = 8;
        sig_bit.blue = 8;
        sig_bit.alpha = 8;
        png_set_sBIT(png_ptr, info_ptr, &sig_bit);
        png_write_info(png_ptr, info_ptr);
        png_set_shift(png_ptr, &sig_bit);
        png_set_packing(png_ptr);
        data = (unsigned char *) malloc(im->rgbWidth * 4);
        if (!data)
        {
            fclose(f);
            png_destroy_write_struct(&png_ptr, (png_infopp) NULL);
            fprintf(stderr, "Fail create png file %s : no data\n", file);
            return 4;
        }
        for (y = 0; y < im->rgbHeight; y++)
        {
            ptr = im->rgbData + (y * im->rgbWidth * 3);
            for (x = 0; x < im->rgbWidth; x++)
            {
                data[(x << 2) + 0] = *ptr++;
                data[(x << 2) + 1] = *ptr++;
                data[(x << 2) + 2] = *ptr++;
                if(im->alphaChanel!=NULL)
                    data[(x << 2) + 3] = im->alphaChanel[ x + y*im->rgbWidth ];
                else if(im->presShapeColor)
                    if ((data[(x << 2) + 0] == im->shapeColor.r) &&
                        (data[(x << 2) + 1] == im->shapeColor.g) &&
                        (data[(x << 2) + 2] == im->shapeColor.b))
                         data[(x << 2) + 3] = 0; /* transparent */
                    else
                        data[(x << 2) + 3] = 255; /* opaque */
                else
                    data[(x << 2) + 3] = 255;
            }
            row_ptr = data;
            png_write_rows(png_ptr, &row_ptr, 1);
        }
        free(data);
        png_write_end(png_ptr, info_ptr);
        png_destroy_write_struct(&png_ptr, (png_infopp) NULL);

        fclose(f);
        return 0;
    }
    #endif
    return 5;
}



/* sauvegarde "im" dans "file" au format TIFF */
/* retourne 1 en cas de succes */
/* nécessite la bibliothèque libtiff */
int sauve_tiff(yImage *im, const char *file)
{
    #ifdef HAVE_LIBTIFF
    TIFF               *tif;
    unsigned char      *data;
    int                 y;
    //int                 w;

    tif = TIFFOpen(file, "w");
    if (tif)
    {
        TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, im->rgbWidth);
        TIFFSetField(tif, TIFFTAG_IMAGELENGTH, im->rgbHeight);
        TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
        TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);
        TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
        TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_LZW);
        {
            TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 3);
            TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
            /*w = */TIFFScanlineSize(tif);
            TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP,
            TIFFDefaultStripSize(tif, -1));
            for (y = 0; y < im->rgbHeight; y++)
            {
                data = im->rgbData + (y * im->rgbWidth * 3);
                TIFFWriteScanline(tif, data, y, 0);
            }
        }
        TIFFClose(tif);
        return 1;
    }
    #endif
    return(0);
}



/* LOADING FILES */

unsigned char *_LoadPPM(FILE * f, int *w, int *h)
{
    int                 done;
    unsigned char      *ptr;
    unsigned char       chr;
    char s[256]; /* read line */
    int                 a, b, i, j;
    int                 color /* is pixmap */, scale, ascii /* is ascii */, bw /* is bitmap */;

    a = b = scale = ascii = bw = color = 0;


    fgets(s, 256, f);
    s[2] = 0;
    if (!strcmp(s, "P6")) // pixmap binary
        color = 1;
    else if (!strcmp(s, "P5")) // gray binary
        color = 0;
    else if (!strcmp(s, "P4")) // bitmap binary
        bw = 1;
    else if (!strcmp(s, "P3")) // pixmap ascii
    {
        color = 1;
        ascii = 1;
    }
    else if (!strcmp(s, "P2")) // gray ascii
    {
        ascii = 1;
    }
    else if (!strcmp(s, "P1")) // bitmap ascii
    {
        ascii = 1;
        bw = 1;
    }
    else
        return NULL;



  done = 1;
  ptr = NULL;
  while (done)
    {
      if (fgets(s, 256, f) == NULL)
    break;

      if (s[0] != '#')
    {
      done = 0;
      sscanf(s, "%i %i", w, h);
      a = *w;
      b = *h;
      if (a > 32767)
        {
          fprintf(stderr, "IMLIB ERROR: Image width > 32767 pixels for file\n");
          return NULL;
        }
      if (b > 32767)
        {
          fprintf(stderr, "IMLIB ERROR: Image height > 32767 pixels for file\n");
          return NULL;
        }
      if (!bw)
        {
          fgets(s, 256, f);
          sscanf(s, "%i", &scale);
        }
      else
        scale = 99999;
      ptr = (unsigned char *)malloc(a * b * 3);
      if (!ptr)
        {
          fprintf(stderr, "IMLIB ERROR: Cannot allocate RAM for RGB data in file");
          return ptr;
        }
      if ((color) && (!ascii) && (!bw))
        {
          fread(ptr, a * b * 3, 1, f);
        }
      else if ((!color) && (!ascii) && (!bw))
        {
          b = (a * b * 3);
          a = 0;
          while ((fread(&chr, 1, 1, f)) && (a < b))
        {
          ptr[a++] = chr;
          ptr[a++] = chr;
          ptr[a++] = chr;
        }
        }
      else if ((!color) && (!ascii) && (bw))
        {
          b = (a * b * 3);
          a = 0;
          j = 0;
          while ((fread(&chr, 1, 1, f)) && (a < b))
        {
          for (i = 7; i >= 0; i--)
            {
              j++;
              if (j <= *w)
            {
              if (chr & (1 << i))
                {
                  ptr[a++] = 0;
                  ptr[a++] = 0;
                  ptr[a++] = 0;
                }
              else
                {
                  ptr[a++] = 255;
                  ptr[a++] = 255;
                  ptr[a++] = 255;
                }
            }
            }
          if (j >= *w)
            j = 0;
        }
        }
      else if ((color) && (ascii) && (!bw))
        {
          b = (a * b * 3);
          a = 0;
          i = 0;
          if (scale != 255)
        {
          while ((fread(&chr, 1, 1, f)) && (a < b))
            {
              s[i++] = chr;
              if (!isdigit(chr))
            {
              s[i - 1] = 0;
              if ((i > 1) && (isdigit(s[i - 2])))
                {
                  ptr[a++] = ((atoi(s)) * 255) / scale;
                }
              i = 0;
            }
            }
        }
          else
        {
          while ((fread(&chr, 1, 1, f)) && (a < b))
            {
              s[i++] = chr;
              if (!isdigit(chr))
            {
              s[i - 1] = 0;
              if ((i > 1) && (isdigit(s[i - 2])))
                {
                  ptr[a++] = atoi(s);
                }
              i = 0;
            }
            }
        }

        }
      else if ((!color) && (ascii) && (!bw))
        {
          b = (a * b * 3);
          a = 0;
          i = 0;
          if (scale != 255)
        {
          while ((fread(&chr, 1, 1, f)) && (a < b))
            {
              s[i++] = chr;
              if (!isdigit(chr))
            {
              s[i - 1] = 0;
              if ((i > 1) && (isdigit(s[i - 2])))
                {
                  ptr[a++] = ((atoi(s)) * 255) / scale;
                  ptr[a++] = ptr[a - 1];
                  ptr[a++] = ptr[a - 1];
                }
              i = 0;
            }
            }
        }
          else
        {
          while ((fread(&chr, 1, 1, f)) && (a < b))
            {
              s[i++] = chr;
              if (!isdigit(chr))
            {
              s[i - 1] = 0;
              if ((i > 1) && (isdigit(s[i - 2])))
                {
                  ptr[a++] = atoi(s);
                  ptr[a++] = ptr[a - 1];
                  ptr[a++] = ptr[a - 1];
                }
              i = 0;
            }
            }
        }
        }
      else if ((!color) && (ascii) && (bw))
        {
        }
    }
    }
  if (!ptr)
    return NULL;
  if (scale == 0)
    {
      free(ptr);
      return NULL;
    }
  if ((scale < 255) && (!ascii))
    {
      int                 rot;
      unsigned char      *po;

      if (scale <= 1)
    rot = 7;
      else if (scale <= 3)
    rot = 6;
      else if (scale <= 7)
    rot = 5;
      else if (scale <= 15)
    rot = 4;
      else if (scale <= 31)
            rot = 3;
        else if (scale <= 63)
            rot = 2;
        else
            rot = 1;

        if (rot > 0)
        {
            po = ptr;
            while (po < (ptr + (*w ** h * 3)))
            {
                *po++ <<= rot;
                *po++ <<= rot;
                *po++ <<= rot;
            }
        }
    }
    return ptr;
}





//#ifdef HAVE_LIBPNG
yImage *LoadPNG(FILE *f)
{
    png_structp png_ptr;
    png_infop info_ptr;

    unsigned char *ptr, *ptr2; /* parcourt of data arrays */
    unsigned char **lines; /* PNG data */
    unsigned char r, g, b, a; /* RGBA of a pixel */
    int i, x, y/*, interlace_type*/;


    unsigned char *ptrAlpha;
    int width, height;
    png_byte color_type;
    //png_byte bit_depth;
    char header[8];    // 8 is the maximum size that can be checked
    int err; /* error code */

    yImage *im=NULL;




    /* Init PNG Reader */

    /*Raj*/
    fread((void *) header, 1, 8, f);
    if (png_sig_cmp((png_const_bytep)header, 0, 8))
    {
        return NULL;
    }


    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) return NULL;
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return NULL;
    }

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return NULL;
    }


    png_init_io(png_ptr, f);

    /*Raj*/
    png_set_sig_bytes(png_ptr, 8);

    png_read_info(png_ptr, info_ptr);

    width = png_get_image_width(png_ptr, info_ptr);
    height = png_get_image_height(png_ptr, info_ptr);
    color_type = png_get_color_type(png_ptr, info_ptr);
    //bit_depth = png_get_bit_depth(png_ptr, info_ptr);



  //png_get_IHDR(png_ptr, info_ptr, &ww, &hh, &bit_depth, &color_type, &interlace_type, NULL, NULL);





    /* Setup Translators */
    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_expand(png_ptr);
    png_set_strip_16(png_ptr);
    png_set_packing(png_ptr);
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
        png_set_expand(png_ptr);
    png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);





    /* Allocate memory for read png data*/
    lines = (unsigned char **)malloc(height * sizeof(unsigned char *));

    if (lines == NULL)
    {
        destroy_yImage(im);
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return NULL;
    }

    for (i = 0; i < height; i++)
    {
        if ((lines[i] = (unsigned char*) malloc(width * (sizeof(unsigned char) * 4))) == NULL)
        {
            int n;

            destroy_yImage(im);

            for (n = 0; n < i; n++)
                free(lines[n]);
            free(lines);
            png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
            return NULL;
        }
    }

    /* Reading */
    png_read_image(png_ptr, lines);
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);


    /* allocate memory for yImage structure */
    im=create_yImage(&err, NULL, width, height);
    if(im==NULL)
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return NULL;
    }

    im->rgbData = (unsigned char*) malloc(im->rgbWidth * im->rgbHeight * 3);
    if (!im->rgbData)
    {
      png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
      return NULL;
    }

    im->alphaChanel = (unsigned char*) malloc(im->rgbWidth * im->rgbHeight);
    if (!im->alphaChanel)
    {
      png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
      return NULL;
    }


    /* Data interpretation */
    ptr = im->rgbData;
    ptrAlpha= im->alphaChanel;

    if (color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
    {
        for (y = 0; y < im->rgbHeight; y++)
        {
            ptr2 = lines[y];
            for (x = 0; x < im->rgbWidth; x++)
            {
                r = *ptr2++;
                a = *ptr2++;

                *ptr++ = r;
                *ptr++ = r;
                *ptr++ = r;
                *ptrAlpha++=a;
            }
        }
    }

    else if (color_type == PNG_COLOR_TYPE_GRAY)
    {
        for (y = 0; y < im->rgbHeight; y++)
        {
            ptr2 = lines[y];
            for (x = 0; x < im->rgbWidth; x++)
            {
                r = *ptr2++;
                *ptr++ = r;
                *ptr++ = r;
                *ptr++ = r;
            }
        }
    }

    else
    {
        for (y = 0; y < im->rgbHeight; y++)
        {
            ptr2 = lines[y];
            for (x = 0; x < im->rgbWidth; x++)
            {
                r = *ptr2++;
                g = *ptr2++;
                b = *ptr2++;
                a = *ptr2++;

                *ptr++ = r;
                *ptr++ = g;
                *ptr++ = b;
                *ptrAlpha++=a;
            }
        }
    }

    /* Freing memory */
    for (i = 0; i < im->rgbHeight; i++)
        free(lines[i]);
    free(lines);

    /* End */
    return im;
}
//#endif /* HAVE_LIBPNG */











/* POUR CHARGER UN FICHIER */

#ifdef TOTOT


#define _GNU_SOURCE
#include <config.h>
#include "Imlib.h"
#include "Imlib_private.h"
#include <setjmp.h>

static int broke_width =  189;
static int broke_height =   9;
static unsigned char broke[] =
{
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  0  ,0  ,0,255,255,255,255,255,255  ,0  ,0  ,0,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  0  ,0  ,0  ,0  ,0  ,0,255,255,255,255,255,255,255,255,255,
255,255,255  ,0  ,0  ,0,255,255,255,255,255,255,255,255,255,
  0  ,0  ,0,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255  ,0  ,0  ,0,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255  ,0  ,0  ,0  ,0  ,0  ,0,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255  ,0  ,0  ,0  ,0  ,0  ,0,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255  ,0  ,0  ,0,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  0  ,0  ,0,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  0  ,0  ,0,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255  ,0  ,0  ,0,
255,255,255,255,255,255  ,0  ,0  ,0,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255  ,0  ,0  ,0,
  0  ,0  ,0,255,255,255  ,0  ,0  ,0,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  0  ,0  ,0,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  0  ,0  ,0,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255  ,0  ,0  ,0,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255  ,0  ,0  ,0,
  0  ,0  ,0,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255  ,0  ,0  ,0,
255,255,255,255,255,255  ,0  ,0  ,0,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255  ,0  ,0  ,0,
255,255,255,255,255,255  ,0  ,0  ,0,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  0  ,0  ,0,255,255,255,255,255,255,255,255,255,255,255,255,
  0  ,0  ,0,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255  ,0  ,0  ,0,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255  ,0  ,0  ,0,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255  ,0  ,0  ,0,255,255,255,
255,255,255  ,0  ,0  ,0,255,255,255,255,255,255  ,0  ,0  ,0,
  0  ,0  ,0,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255  ,0  ,0  ,0  ,0  ,0  ,0,
255,255,255  ,0  ,0  ,0,255,255,255,255,255,255  ,0  ,0  ,0,
  0  ,0  ,0,255,255,255,255,255,255  ,0  ,0  ,0  ,0  ,0  ,0,
  0  ,0  ,0  ,0  ,0  ,0,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255  ,0  ,0  ,0,255,255,255,
255,255,255  ,0  ,0  ,0,255,255,255,255,255,255  ,0  ,0  ,0,
  0  ,0  ,0  ,0  ,0  ,0,255,255,255,255,255,255  ,0  ,0  ,0,
  0  ,0  ,0,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255  ,0  ,0  ,0,
255,255,255,255,255,255,255,255,255  ,0  ,0  ,0  ,0  ,0  ,0,
255,255,255,255,255,255,255,255,255  ,0  ,0  ,0  ,0  ,0  ,0,
  0  ,0  ,0,255,255,255,255,255,255  ,0  ,0  ,0  ,0  ,0  ,0,
  0  ,0  ,0,255,255,255,255,255,255  ,0  ,0  ,0  ,0  ,0  ,0,
  0  ,0  ,0,255,255,255,255,255,255,255,255,255  ,0  ,0  ,0,
  0  ,0  ,0  ,0  ,0  ,0,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255  ,0  ,0  ,0,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255  ,0  ,0  ,0,255,255,255,
  0  ,0  ,0  ,0  ,0  ,0,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255  ,0  ,0  ,0,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255  ,0  ,0  ,0,
255,255,255,255,255,255,255,255,255  ,0  ,0  ,0  ,0  ,0  ,0,
  0  ,0  ,0  ,0  ,0  ,0,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255  ,0  ,0  ,0  ,0  ,0  ,0,
255,255,255,255,255,255,255,255,255,255,255,255  ,0  ,0  ,0,
  0  ,0  ,0  ,0  ,0  ,0,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255  ,0  ,0  ,0  ,0  ,0  ,0,
  0  ,0  ,0,255,255,255,255,255,255  ,0  ,0  ,0,255,255,255,
  0  ,0  ,0  ,0  ,0  ,0,255,255,255,255,255,255  ,0  ,0  ,0,
  0  ,0  ,0,255,255,255,255,255,255  ,0  ,0  ,0,255,255,255,
255,255,255  ,0  ,0  ,0,255,255,255,255,255,255  ,0  ,0  ,0,
  0  ,0  ,0,255,255,255,255,255,255  ,0  ,0  ,0  ,0  ,0  ,0,
  0  ,0  ,0,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255  ,0  ,0  ,0,255,255,255,255,255,255,
  0  ,0  ,0,255,255,255  ,0  ,0  ,0,255,255,255,255,255,255,
  0  ,0  ,0,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255  ,0  ,0  ,0,255,255,255  ,0  ,0  ,0,
  0  ,0  ,0,255,255,255  ,0  ,0  ,0,255,255,255,255,255,255,
  0  ,0  ,0,255,255,255,255,255,255  ,0  ,0  ,0,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255  ,0  ,0  ,0,255,255,255,255,255,255,
  0  ,0  ,0,255,255,255  ,0  ,0  ,0,255,255,255,255,255,255,
255,255,255,255,255,255  ,0  ,0  ,0,255,255,255,255,255,255,
  0  ,0  ,0,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255  ,0  ,0  ,0,255,255,255,
255,255,255,255,255,255,255,255,255  ,0  ,0  ,0,255,255,255,
255,255,255,255,255,255  ,0  ,0  ,0,255,255,255,255,255,255,
  0  ,0  ,0,255,255,255  ,0  ,0  ,0,255,255,255,255,255,255,
  0  ,0  ,0,255,255,255  ,0  ,0  ,0,255,255,255,255,255,255,
  0  ,0  ,0,255,255,255  ,0  ,0  ,0,255,255,255,255,255,255,
  0  ,0  ,0,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255  ,0  ,0  ,0,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255  ,0  ,0  ,0  ,0  ,0  ,0,255,255,255,
  0  ,0  ,0,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255  ,0  ,0  ,0,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255  ,0  ,0  ,0,255,255,255,
255,255,255,255,255,255,255,255,255  ,0  ,0  ,0,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255  ,0  ,0  ,0,255,255,255,
255,255,255,255,255,255  ,0  ,0  ,0,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255  ,0  ,0  ,0,255,255,255,255,255,255,
  0  ,0  ,0,255,255,255  ,0  ,0  ,0  ,0  ,0  ,0,255,255,255,
255,255,255,255,255,255  ,0  ,0  ,0,255,255,255,255,255,255,
  0  ,0  ,0,255,255,255  ,0  ,0  ,0,255,255,255  ,0  ,0  ,0,
255,255,255,255,255,255  ,0  ,0  ,0,255,255,255,255,255,255,
  0  ,0  ,0,255,255,255  ,0  ,0  ,0,255,255,255,255,255,255,
  0  ,0  ,0,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255  ,0  ,0  ,0,255,255,255,255,255,255  ,0  ,0  ,0,
255,255,255  ,0  ,0  ,0,255,255,255,255,255,255  ,0  ,0  ,0,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255  ,0  ,0  ,0,255,255,255  ,0  ,0  ,0  ,0  ,0  ,0,
255,255,255  ,0  ,0  ,0,255,255,255,255,255,255  ,0  ,0  ,0,
255,255,255,255,255,255  ,0  ,0  ,0,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255  ,0  ,0  ,0,255,255,255,255,255,255  ,0  ,0  ,0,
255,255,255,255,255,255  ,0  ,0  ,0  ,0  ,0  ,0,255,255,255,
255,255,255  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255  ,0  ,0  ,0,255,255,255,255,255,255,
255,255,255,255,255,255  ,0  ,0  ,0,255,255,255,255,255,255,
255,255,255  ,0  ,0  ,0,255,255,255,255,255,255  ,0  ,0  ,0,
255,255,255  ,0  ,0  ,0,255,255,255,255,255,255  ,0  ,0  ,0,
255,255,255  ,0  ,0  ,0,255,255,255,255,255,255  ,0  ,0  ,0,
255,255,255  ,0  ,0  ,0,255,255,255,255,255,255  ,0  ,0  ,0,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255  ,0  ,0  ,0,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255  ,0  ,0  ,0,255,255,255,255,255,255  ,0  ,0  ,0,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255  ,0  ,0  ,0,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255  ,0  ,0  ,0,255,255,255,255,255,255,
255,255,255,255,255,255  ,0  ,0  ,0,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255  ,0  ,0  ,0,255,255,255,255,255,255,
255,255,255,255,255,255  ,0  ,0  ,0  ,0  ,0  ,0,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255  ,0  ,0  ,0,255,255,255,255,255,255  ,0  ,0  ,0,
255,255,255  ,0  ,0  ,0,255,255,255,255,255,255,255,255,255,
255,255,255  ,0  ,0  ,0,255,255,255,255,255,255  ,0  ,0  ,0,
255,255,255  ,0  ,0  ,0  ,0  ,0  ,0,255,255,255,255,255,255,
255,255,255  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0,
255,255,255  ,0  ,0  ,0,255,255,255,255,255,255  ,0  ,0  ,0,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  0  ,0  ,0,255,255,255,255,255,255  ,0  ,0  ,0,255,255,255,
  0  ,0  ,0,255,255,255,255,255,255  ,0  ,0  ,0,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  0  ,0  ,0,255,255,255,255,255,255  ,0  ,0  ,0,255,255,255,
  0  ,0  ,0,255,255,255,255,255,255  ,0  ,0  ,0,255,255,255,
255,255,255  ,0  ,0  ,0,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  0  ,0  ,0,255,255,255,255,255,255  ,0  ,0  ,0,255,255,255,
255,255,255,255,255,255,255,255,255  ,0  ,0  ,0,255,255,255,
  0  ,0  ,0,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255  ,0  ,0  ,0,255,255,255,255,255,255,255,255,255,
255,255,255  ,0  ,0  ,0,255,255,255,255,255,255,255,255,255,
  0  ,0  ,0,255,255,255,255,255,255  ,0  ,0  ,0,255,255,255,
  0  ,0  ,0,255,255,255,255,255,255  ,0  ,0  ,0,255,255,255,
  0  ,0  ,0,255,255,255,255,255,255  ,0  ,0  ,0,255,255,255,
255,255,255  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255  ,0  ,0  ,0,255,255,255,255,255,255,255,255,255,
255,255,255  ,0  ,0  ,0,255,255,255,255,255,255,255,255,255,
  0  ,0  ,0,255,255,255,255,255,255  ,0  ,0  ,0,255,255,255,
255,255,255  ,0  ,0  ,0,255,255,255,255,255,255,255,255,255,
  0  ,0  ,0,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255  ,0  ,0  ,0,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255  ,0  ,0  ,0,255,255,255,255,255,255,255,255,255,
255,255,255  ,0  ,0  ,0,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255  ,0  ,0  ,0,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255  ,0  ,0  ,0,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  0  ,0  ,0,255,255,255,255,255,255  ,0  ,0  ,0,255,255,255,
  0  ,0  ,0,255,255,255,255,255,255,255,255,255,255,255,255,
  0  ,0  ,0,255,255,255,255,255,255  ,0  ,0  ,0,255,255,255,
  0  ,0  ,0,255,255,255  ,0  ,0  ,0,255,255,255,255,255,255,
  0  ,0  ,0,255,255,255,255,255,255,255,255,255,255,255,255,
  0  ,0  ,0,255,255,255,255,255,255  ,0  ,0  ,0,255,255,255,
255,255,255  ,0  ,0  ,0,255,255,255,255,255,255  ,0  ,0  ,0,
  0  ,0  ,0  ,0  ,0  ,0,255,255,255,255,255,255,255,255,255,
  0  ,0  ,0  ,0  ,0  ,0,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255  ,0  ,0  ,0,
255,255,255,255,255,255  ,0  ,0  ,0,255,255,255,255,255,255,
  0  ,0  ,0  ,0  ,0  ,0,255,255,255,255,255,255,255,255,255,
255,255,255  ,0  ,0  ,0  ,0  ,0  ,0,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0,255,255,255  ,0  ,0  ,0,
  0  ,0  ,0  ,0  ,0  ,0,255,255,255,255,255,255,255,255,255,
  0  ,0  ,0  ,0  ,0  ,0,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255  ,0  ,0  ,0,
  0  ,0  ,0  ,0  ,0  ,0,255,255,255,255,255,255  ,0  ,0  ,0,
  0  ,0  ,0  ,0  ,0  ,0,255,255,255,255,255,255  ,0  ,0  ,0,
  0  ,0  ,0  ,0  ,0  ,0,255,255,255,255,255,255  ,0  ,0  ,0,
  0  ,0  ,0  ,0  ,0  ,0,255,255,255,255,255,255  ,0  ,0  ,0,
255,255,255,255,255,255  ,0  ,0  ,0,255,255,255,255,255,255,
255,255,255,255,255,255  ,0  ,0  ,0,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  0  ,0  ,0,255,255,255,255,255,255,255,255,255,255,255,255,
  0  ,0  ,0,255,255,255,255,255,255,255,255,255,255,255,255,
  0  ,0  ,0  ,0  ,0  ,0,255,255,255,255,255,255,255,255,255,
  0  ,0  ,0,255,255,255,255,255,255,255,255,255  ,0  ,0  ,0,
  0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0,255,255,255,255,255,255,
  0  ,0  ,0,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255  ,0  ,0  ,0,
  0  ,0  ,0  ,0  ,0  ,0,255,255,255,255,255,255,255,255,255,
255,255,255  ,0  ,0  ,0  ,0  ,0  ,0,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255  ,0  ,0  ,0,
  0  ,0  ,0  ,0  ,0  ,0,255,255,255,255,255,255  ,0  ,0  ,0,
  0  ,0  ,0  ,0  ,0  ,0,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255  ,0  ,0  ,0,
  0  ,0  ,0  ,0  ,0  ,0,255,255,255,255,255,255  ,0  ,0  ,0,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  0  ,0  ,0  ,0  ,0  ,0,255,255,255,255,255,255  ,0  ,0  ,0,
255,255,255,255,255,255  ,0  ,0  ,0,255,255,255,255,255,255,
  0  ,0  ,0  ,0  ,0  ,0,255,255,255,255,255,255  ,0  ,0  ,0,
255,255,255,255,255,255  ,0  ,0  ,0,255,255,255,255,255,255,
  0  ,0  ,0,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255  ,0  ,0  ,0,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255  ,0  ,0  ,0,
  0  ,0  ,0,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255};

/*      Split the ID - damages input    */

static char        *
_SplitID(char *file)
{
  char *p = strrchr(file, ':');

  if (p == NULL)
    return "";
  else
    {
      *p++ = 0;
      return p;
    }
}

/*
 * *     Doesn't damage the input
 */

char               *
_GetExtension(char *file)
{
  char               *p = strrchr(file, '.');

  if (p == NULL)
    return "";
  else
    return p + 1;
}

#ifdef HAVE_LIBJPEG

/**
 *  * This error handling is broken beyond belief, but oh well it works
 *  **/

struct ImLib_JPEG_error_mgr
{
  struct jpeg_error_mgr pub;
  sigjmp_buf setjmp_buffer;
};

typedef struct ImLib_JPEG_error_mgr *emptr;

void
_JPEGFatalErrorHandler(j_common_ptr cinfo)
{
  /*
   * FIXME:
   * We should somehow signal what error occurred to the caller so the
   * caller can handle the error message
   */
  emptr errmgr;

  errmgr = (emptr) cinfo->err;
  cinfo->err->output_message(cinfo);
  siglongjmp(errmgr->setjmp_buffer, 1);
  return;
}

unsigned char *
_LoadJPEG(ImlibData *id, FILE *f, int *w, int *h)
{
  struct jpeg_decompress_struct cinfo;
  struct ImLib_JPEG_error_mgr jerr;
  unsigned char *data, *line[16], *ptr;
  int x, y, i;

  cinfo.err = jpeg_std_error(&(jerr.pub));
  jerr.pub.error_exit = _JPEGFatalErrorHandler;

  /* error handler to longjmp to, we want to preserve signals */
  if (sigsetjmp(jerr.setjmp_buffer, 1))
    {
      /* Whoops there was a jpeg error */
      jpeg_destroy_decompress(&cinfo);
      return NULL;
    }

  jpeg_create_decompress(&cinfo);
  jpeg_stdio_src(&cinfo, f);
  jpeg_read_header(&cinfo, TRUE);
  cinfo.do_fancy_upsampling = FALSE;
  cinfo.do_block_smoothing = FALSE;
  jpeg_start_decompress(&cinfo);
  *w = cinfo.output_width;
  *h = cinfo.output_height;
  data = malloc(*w ** h * 3);
  if (!data)
    {
      jpeg_destroy_decompress(&cinfo);
      return NULL;
    }
  ptr = data;

  if (cinfo.rec_outbuf_height > 16)
    {
      fprintf(stderr, "Imlib ERROR: JPEG uses line buffers > 16. Cannot load.\n");
      return NULL;
    }
  if (cinfo.output_components == 3)
    {
      for (y = 0; y < *h; y += cinfo.rec_outbuf_height)
    {
      for (i = 0; i < cinfo.rec_outbuf_height; i++)
        {
          line[i] = ptr;
          ptr += *w * 3;
        }
      jpeg_read_scanlines(&cinfo, line, cinfo.rec_outbuf_height);
    }
    }
  else if (cinfo.output_components == 1)
    {
      for (i = 0; i < cinfo.rec_outbuf_height; i++)
    {
      if ((line[i] = malloc(*w)) == NULL)
        {
          int                 t = 0;

          for (t = 0; t < i; t++)
            free(line[t]);
          jpeg_destroy_decompress(&cinfo);
          return NULL;
        }
    }
      for (y = 0; y < *h; y += cinfo.rec_outbuf_height)
    {
      jpeg_read_scanlines(&cinfo, line, cinfo.rec_outbuf_height);
      for (i = 0; i < cinfo.rec_outbuf_height; i++)
        {
          for (x = 0; x < *w; x++)
        {
          *ptr++ = line[i][x];
          *ptr++ = line[i][x];
          *ptr++ = line[i][x];
        }
        }
    }
      for (i = 0; i < cinfo.rec_outbuf_height; i++)
    free(line[i]);
    }
  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);

  return data;
}
#endif /* HAVE_LIBJPEG */



#ifdef HAVE_LIBTIFF
unsigned char      *
_LoadTIFF(ImlibData * id, FILE *f, char *file, int *w, int *h, int *t)
{
  TIFF               *tif;
  unsigned char      *data, *ptr, r, g, b, a;
  int                 x, y;
  uint32              ww, hh, *rast, *tptr;
  size_t              npix;
  int                 istransp;
  int                 fd;

  istransp = 0;
  if (!f)
    return NULL;

  fd = fileno(f);
  /* Apparently rewind(f) isn't sufficient */
  lseek(fd, (long) 0, 0);
  /* So why does libtif need a filename here ??? */
  tif = TIFFFdOpen(fd, file, "r");

  if (!tif)
    return NULL;
  TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &ww);
  TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &hh);
  npix = ww * hh;
  *w = (int)ww;
  *h = (int)hh;
  rast = (uint32 *) _TIFFmalloc(npix * sizeof(uint32));
  if (!rast)
    {
      TIFFClose(tif);
      return NULL;
    }
  data = NULL;
  if (TIFFReadRGBAImage(tif, ww, hh, rast, 0))
    {
      data = (unsigned char *)malloc(*w ** h * 3);
      if (!data)
    {
      _TIFFfree(rast);
      TIFFClose(tif);
      return NULL;
    }
      ptr = data;
      for (y = 0; y < *h; y++)
    {
      tptr = rast;
      tptr += ((*h - y - 1) ** w);
      for (x = 0; x < *w; x++)
        {
          a = TIFFGetA(*tptr);
          b = TIFFGetB(*tptr);
          g = TIFFGetG(*tptr);
          r = TIFFGetR(*tptr);
          tptr++;
          if (a < 128)
        {
          *ptr++ = 255;
          *ptr++ = 0;
          *ptr++ = 255;
          istransp = 1;
        }
          else
        {
          if ((r == 255) && (g == 0) && (b == 255))
            r = 254;
          *ptr++ = r;
          *ptr++ = g;
          *ptr++ = b;
        }
        }
    }
    }
  _TIFFfree(rast);
  TIFFClose(tif);
  *t = istransp;
  return data;
}

#endif /* HAVE_LIBTIFF */

#ifdef HAVE_LIBGIF
unsigned char      *
_LoadGIF(ImlibData * id, FILE *f, int *w, int *h, int *t)
{
  unsigned char      *data, *ptr;
  GifFileType        *gif;
  GifRowType         *rows;
  GifRecordType       rec;
  ColorMapObject     *cmap;
  int                 i, j, done, bg, csize, r, g, b;
  int                 intoffset[] = {0, 4, 2, 1};
  int                 intjump[] = {8, 8, 4, 2};
  int                 istransp, transp;
  int                 fd;

  done = 0;
  istransp = 0;
  data = NULL;
  rows = NULL;
  transp = -1;

  fd = fileno(f);
  /* Apparently rewind(f) isn't sufficient */
  lseek(fd, (long) 0, 0);
  gif = DGifOpenFileHandle(fd);

  if (!gif)
    return NULL;
  do
    {
      if (DGifGetRecordType(gif, &rec) == GIF_ERROR)
    {
      PrintGifError();
      rec = TERMINATE_RECORD_TYPE;
    }
      if ((rec == IMAGE_DESC_RECORD_TYPE) && (!done))
    {
      if (DGifGetImageDesc(gif) == GIF_ERROR)
        {
          PrintGifError();
          rec = TERMINATE_RECORD_TYPE;
        }
      *w = gif->Image.Width;
      *h = gif->Image.Height;
      rows = malloc(*h * sizeof(GifRowType *));
      if (!rows)
        {
          DGifCloseFile(gif);
          return NULL;
        }
      data = malloc(*w ** h * 3);
      if (!data)
        {
          DGifCloseFile(gif);
          free(rows);
          return NULL;
        }
      for (i = 0; i < *h; i++)
        rows[i] = NULL;
      for (i = 0; i < *h; i++)
        {
          rows[i] = malloc(*w * sizeof(GifPixelType));
          if (!rows[i])
        {
          DGifCloseFile(gif);
          for (i = 0; i < *h; i++)
            if (rows[i])
              free(rows[i]);
          free(rows);
          free(data);
          return NULL;
        }
        }
      if (gif->Image.Interlace)
        {
          for (i = 0; i < 4; i++)
        {
          for (j = intoffset[i]; j < *h; j += intjump[i])
            DGifGetLine(gif, rows[j], *w);
        }
        }
      else
        {
          for (i = 0; i < *h; i++)
        DGifGetLine(gif, rows[i], *w);
        }
      done = 1;
    }
      else if (rec == EXTENSION_RECORD_TYPE)
    {
      int                 ext_code;
      GifByteType        *ext;

      ext = NULL;
      DGifGetExtension(gif, &ext_code, &ext);
      while (ext)
        {
          if ((ext_code == 0xf9) && (ext[1] & 1) && (transp < 0))
        {
          istransp = 1;
          transp = (int)ext[4];
        }
          ext = NULL;
          DGifGetExtensionNext(gif, &ext);
        }
    }
    }
  while (rec != TERMINATE_RECORD_TYPE);
  bg = gif->SBackGroundColor;
  cmap = (gif->Image.ColorMap ? gif->Image.ColorMap : gif->SColorMap);
  csize = cmap->ColorCount;
  ptr = data;
  if (!istransp)
    {
      for (i = 0; i < *h; i++)
    {
      for (j = 0; j < *w; j++)
        {
          r = cmap->Colors[rows[i][j]].Red;
          g = cmap->Colors[rows[i][j]].Green;
          b = cmap->Colors[rows[i][j]].Blue;
          *ptr++ = r;
          *ptr++ = g;
          *ptr++ = b;
        }
    }
    }
  else
    {
      for (i = 0; i < *h; i++)
    {
      for (j = 0; j < *w; j++)
        {
          if (rows[i][j] == transp)
        {
          *ptr++ = 255;
          *ptr++ = 0;
          *ptr++ = 255;
        }
          else
        {
          r = cmap->Colors[rows[i][j]].Red;
          g = cmap->Colors[rows[i][j]].Green;
          b = cmap->Colors[rows[i][j]].Blue;
          if ((r == 255) && (g == 0) && (b == 255))
            r = 254;
          *ptr++ = r;
          *ptr++ = g;
          *ptr++ = b;
        }
        }
    }
    }
  DGifCloseFile(gif);
  for (i = 0; i < *h; i++)
    free(rows[i]);
  free(rows);
  *t = istransp;
  return data;
}

#endif /* HAVE_LIBGIF */

unsigned char      *
_LoadBMP(ImlibData * id, FILE *file, int *w, int *h, int *t)
{
  unsigned char      *data, *ptr;
  int                 done, i, bpp, planes, comp, ncolors, line, column,
                      linesize, linepos, rshift, gshift, bshift, size;
  unsigned char       byte;
  short int           word;
  long int            dbuf[4], dword, rmask, gmask, bmask, offset;
  signed char         bbuf[4];
  struct _cmap
    {
      unsigned char       r, g, b;
    }
                     *cmap;

#define BI_RGB       0
#define BI_RLE8      1
#define BI_RLE4      2
#define BI_BITFIELDS 3

  rshift = 0;
  gshift = 0;
  bshift = 0;
  rmask = 0xff;
  gmask = 0xff;
  bmask = 0xff;
  if (!file)
    return NULL;

  done = 0;
  /*
   * reading the bmp header
   */

  fread(bbuf, 1, 2, file);

  fread(dbuf, 4, 4, file);

  size = dbuf[0];
  offset = dbuf[2];

  fread(dbuf, 4, 2, file);
  *w = (int)dbuf[0];
  *h = (int)dbuf[1];
  if (*w > 32767)
    {
      fprintf(stderr, "IMLIB ERROR: Image width > 32767 pixels for file\n");
      return NULL;
    }
  if (*h > 32767)
    {
      fprintf(stderr, "IMLIB ERROR: Image height > 32767 pixels for file\n");
      return NULL;
    }
  fread(&word, 2, 1, file);
  planes = (int)word;
  fread(&word, 2, 1, file);
  bpp = (int)word;
  if (bpp != 1 && bpp != 4 && bpp != 8 && bpp && 16 && bpp != 24 && bpp != 32)
    {
      fprintf(stderr, "IMLIB ERROR: unknown bitdepth in file\n");
      return NULL;
    }
  fread(dbuf, 4, 4, file);
  comp = (int)dbuf[0];
  if (comp != BI_RGB && comp != BI_RLE4 && comp != BI_RLE8 && comp != BI_BITFIELDS)
    {
      fprintf(stderr, "IMLIB ERROR: unknown encoding in Windows BMP file\n");
      return NULL;
    }
  fread(dbuf, 4, 2, file);
  ncolors = (int)dbuf[0];
  if (ncolors == 0)
    ncolors = 1 << bpp;
  /* some more sanity checks */
  if (((comp == BI_RLE4) && (bpp != 4)) || ((comp == BI_RLE8) && (bpp != 8)) || ((comp == BI_BITFIELDS) && (bpp != 16 && bpp != 32)))
    {
      fprintf(stderr, "IMLIB ERROR: encoding of BMP doesn't match bitdepth\n");
      return NULL;
    }
  if (bpp < 16)
    {
      cmap = (struct _cmap *)malloc(sizeof(struct _cmap) * ncolors);

      if (!cmap)
    {
      fprintf(stderr, "IMLIB ERROR: Cannot allocate RAM for color map in BMP file\n");
      return NULL;
    }
    }
  else
    cmap = NULL;
  ptr = (unsigned char *)malloc(*w * *h * 3);
  if (!ptr)
    {
      fprintf(stderr, "IMLIB ERROR: Cannot allocate RAM for RGB data in file\n");
      if (cmap)
    free(cmap);
      return NULL;
    }

  /*
   * Reading the palette, if it exists.
   */
  if (bpp < 16 && ncolors != 0)
    {
      for (i = 0; i < ncolors; i++)
    {
      fread(bbuf, 1, 4, file);
      cmap[i].b = bbuf[0];
      cmap[i].g = bbuf[1];
      cmap[i].r = bbuf[2];
    }
    }
  else if (bpp == 16 || bpp == 32)
    {
      if (comp == BI_BITFIELDS)
    {
      int                 bit = 0;

      fread(dbuf, 4, 3, file);
      bmask = dbuf[0];
      gmask = dbuf[1];
      rmask = dbuf[2];
      /* find shift amount.. ugly, but i can't think of a better way */
      for (bit = 0; bit < bpp; bit++)
        {
          if (bmask & (1 << bit))
        bshift = bit;
          if (gmask & (1 << bit))
        gshift = bit;
          if (rmask & (1 << bit))
        rshift = bit;
        }
    }
      else if (bpp == 16)
    {
      rmask = 0x7C00;
      gmask = 0x03E0;
      bmask = 0x001F;
      rshift = 10;
      gshift = 5;
      bshift = 0;
    }
      else if (bpp == 32)
    {
      rmask = 0x00FF0000;
      gmask = 0x0000FF00;
      bmask = 0x000000FF;
      rshift = 16;
      gshift = 8;
      bshift = 0;
    }
    }

  /*
   * Reading the image data
   */
  fseek(file, offset, SEEK_SET);
  data = ptr;

  /* set the whole image to the background color */
  if (bpp < 16 && (comp == BI_RLE4 || comp == BI_RLE8))
    {
      for (i = 0; i < *w * *h; i++)
    {
      *ptr++ = cmap[0].r;
      *ptr++ = cmap[0].g;
      *ptr++ = cmap[0].b;
    }
      ptr = data;
    }
  line = 0;
  column = 0;
#define poffset (line * *w * 3 + column * 3)

  /*
   * BMPs are stored upside down... hmmmmmmmmmm....
   */

  linesize = ((*w * bpp + 31) / 32) * 4;
  for (line = (*h - 1); line >= 0; line--)
    {
      linepos = 0;
      for (column = 0; column < *w;)
    {
      if (bpp < 16)
        {
          int                 index;

          linepos++;
          byte = getc(file);
          if (bpp == 1)
        {
          int                 bit = 0;

          for (bit = 0; bit < 8; bit++)
            {
              index = ((byte & (0x80 >> bit)) ? 1 : 0);
              ptr[poffset] = cmap[index].r;
              ptr[poffset + 1] = cmap[index].g;
              ptr[poffset + 2] = cmap[index].b;
              column++;
            }
        }
          else if (bpp == 4)
        {
          if (comp == BI_RLE4)
            {
              fprintf(stderr, "can't deal with 4bit encoded yet.\n");
              free(data);
              free(cmap);
              return NULL;
            }
          else
            {
              int                 nibble = 0;

              for (nibble = 0; nibble < 2; nibble++)
            {
              index = ((byte & (0xF0 >> nibble * 4)) >> (!nibble * 4));
              if (index >= 16)
                index = 15;
              ptr[poffset] = cmap[index].r;
              ptr[poffset + 1] = cmap[index].g;
              ptr[poffset + 2] = cmap[index].b;
              column++;
            }
            }
        }
          else if (bpp == 8)
        {
          if (comp == BI_RLE8)
            {
              unsigned char       first = byte;

              byte = getc(file);
              if (first == 0)
            {
              if (byte == 0)
                {
                  /*   column = *w; */
                }
              else if (byte == 1)
                {
                  column = *w;
                  line = -1;
                }
              else if (byte == 2)
                {
                  byte = getc(file);
                  column += byte;
                  linepos = column * bpp / 8;
                  byte = getc(file);
                  line += byte;
                }
              else
                {
                  int                 absolute = byte;

                  for (i = 0; i < absolute; i++)
                {
                  linepos++;
                  byte = getc(file);
                  ptr[poffset] = cmap[byte].r;
                  ptr[poffset + 1] = cmap[byte].g;
                  ptr[poffset + 2] = cmap[byte].b;
                  column++;
                }
                  if (absolute & 0x01)
                byte = getc(file);
                }
            }
              else
            {
              for (i = 0; i < first; i++)
                {
                  ptr[poffset] = cmap[byte].r;
                  ptr[poffset + 1] = cmap[byte].g;
                  ptr[poffset + 2] = cmap[byte].b;
                  column++;
                  linepos++;
                }
            }
            }
          else
            {
              ptr[poffset] = cmap[byte].r;
              ptr[poffset + 1] = cmap[byte].g;
              ptr[poffset + 2] = cmap[byte].b;
              column++;
            }
        }
        }
      else if (bpp == 24)
        {
          linepos += fread(bbuf, 1, 3, file);
          ptr[poffset] = (unsigned char)bbuf[2];
          ptr[poffset + 1] = (unsigned char)bbuf[1];
          ptr[poffset + 2] = (unsigned char)bbuf[0];
          column++;
        }
      else if (bpp == 16)
        {
          unsigned char       temp;

          linepos += fread(&word, 2, 1, file);
          temp = (word & rmask) >> rshift;
          ptr[poffset] = temp;
          temp = (word & gmask) >> gshift;
          ptr[poffset + 1] = temp;
          temp = (word & bmask) >> gshift;
          ptr[poffset + 2] = temp;
          column++;
        }
      else
        {
          unsigned char       temp;

          linepos += fread(&dword, 4, 1, file);
          temp = (dword & rmask) >> rshift;
          ptr[poffset] = temp;
          temp = (dword & gmask) >> gshift;
          ptr[poffset + 1] = temp;
          temp = (dword & bmask) >> bshift;
          ptr[poffset + 2] = temp;
          column++;
        }
    }
      while ((linepos < linesize) && (comp != 1) && (comp != 2))
    {
      int                 temp = fread(&byte, 1, 1, file);

      linepos += temp;
      if (!temp)
        break;
    }
    }
  if (cmap)
    free(cmap);
  *t = 0;
  return data;
}

unsigned char      *
_LoadXPM(ImlibData * id, FILE *file, int *w, int *h, int *t)
{
  unsigned char      *data, *ptr, *end;
  int                 pc, c, i, j, k, ncolors, cpp, comment, transp, quote,
                      context, len, done;
  char                line[65536], s[65536], tok[65536], col[65536];
  XColor              xcol;
  struct _cmap
    {
      unsigned char       str[8];
      unsigned char       transp;
      int                 r, g, b;
    }
                     *cmap;
  int                 lookup[256][256];

  transp = 0;
  done = 0;
  if (!file)
    return NULL;
  i = 0;
  j = 0;
  cmap = NULL;
  *w = 10;
  *h = 10;
  ptr = NULL;
  data = NULL;
  end = NULL;
  c = ' ';
  comment = 0;
  quote = 0;
  context = 0;
  while (!done)
    {
      pc = c;
      c = fgetc(file);
      if (c == EOF)
    break;
      if (!quote)
    {
      if ((pc == '/') && (c == '*'))
        comment = 1;
      else if ((pc == '*') && (c == '/') && (comment))
        comment = 0;
    }
      if (!comment)
    {
      if ((!quote) && (c == '"'))
        {
          quote = 1;
          i = 0;
        }
      else if ((quote) && (c == '"'))
        {
          line[i] = 0;
          quote = 0;
          if (context == 0)
        {
          /* Header */
          sscanf(line, "%i %i %i %i", w, h, &ncolors, &cpp);
          if (cpp > 7)
            {
              fprintf(stderr, "IMLIB ERROR: XPM files with characters per pixel > 7 not supported\n");
              return NULL;
            }
          if (*w > 32767)
            {
              fprintf(stderr, "IMLIB ERROR: Image width > 32767 pixels for file\n");
              return NULL;
            }
          if (*h > 32767)
            {
              fprintf(stderr, "IMLIB ERROR: Image height > 32767 pixels for file\n");
              return NULL;
            }
          cmap = malloc(sizeof(struct _cmap) * ncolors);

          if (!cmap)
            return NULL;
          data = malloc(*w ** h * 3);
          if (!data)
            {
              free(cmap);
              return NULL;
            }
          ptr = data;
          end = ptr + (*w ** h * 3);
          j = 0;
          context++;
        }
          else if (context == 1)
        {
          /* Color Table */
          if (j < ncolors)
            {
              int                 slen;
              int                 hascolor, iscolor;

              iscolor = 0;
              hascolor = 0;
              tok[0] = 0;
              col[0] = 0;
              s[0] = 0;
              len = strlen(line);
              strncpy(cmap[j].str, line, cpp);
              cmap[j].str[cpp] = 0;
              cmap[j].r = -1;
              cmap[j].transp = 0;
              for (k = cpp; k < len; k++)
            {
              if (line[k] != ' ')
                {
                  s[0] = 0;
                  sscanf(&line[k], "%65535s", s);
                  slen = strlen(s);
                  k += slen;
                  if (!strcmp(s, "c"))
                iscolor = 1;
                  if ((!strcmp(s, "m")) || (!strcmp(s, "s")) ||
                  (!strcmp(s, "g4")) || (!strcmp(s, "g")) ||
                  (!strcmp(s, "c")) || (k >= len))
                {
                  if (k >= len)
                    {
                      if (col[0])
                    strcat(col, " ");
                      strcat(col, s);
                    }
                  if (col[0])
                    {
                      if (!strcasecmp(col, "none"))
                    {
                      transp = 1;
                      cmap[j].transp = 1;
                    }
                      else
                    {
                      if ((((cmap[j].r < 0) ||
                        (!strcmp(tok, "c"))) &&
                           (!hascolor)))
                        {
                          XParseColor(id->x.disp,
                              id->x.root_cmap,
                              col, &xcol);
                          cmap[j].r = xcol.red >> 8;
                          cmap[j].g = xcol.green >> 8;
                          cmap[j].b = xcol.blue >> 8;
                          if ((cmap[j].r == 255) &&
                          (cmap[j].g == 0) &&
                          (cmap[j].b == 255))
                        cmap[j].r = 254;
                          if (iscolor)
                        hascolor = 1;
                        }
                    }
                    }
                  strcpy(tok, s);
                  col[0] = 0;
                }
                  else
                {
                  if (col[0])
                    strcat(col, " ");
                  strcat(col, s);
                }
                }
            }
            }
          j++;
          if (j >= ncolors)
            {
              if (cpp == 1)
            for (i = 0; i < ncolors; i++)
              lookup[(int)cmap[i].str[0]][(int)cmap[i].str[1]] = i;
              if (cpp == 2)
            for (i = 0; i < ncolors; i++)
              lookup[(int)cmap[i].str[0]][(int)cmap[i].str[1]] = i;
              context++;
            }
        }
          else
        {
          /* Image Data */
          i = 0;
          if (cpp == 0)
            {
              /* Chars per pixel = 0? well u never know */
            }
          if (cpp == 1)
            {
              if (transp)
            {
              for (i = 0; ((i < 65536) && (ptr < end) && (line[i])); i++)
                {
                  col[0] = line[i];
                  if (cmap[lookup[(int)col[0]][0]].transp)
                {
                  *ptr++ = 255;
                  *ptr++ = 0;
                  *ptr++ = 255;
                }
                  else
                {
                  *ptr++ = (unsigned char)cmap[lookup[(int)col[0]][0]].r;
                  *ptr++ = (unsigned char)cmap[lookup[(int)col[0]][0]].g;
                  *ptr++ = (unsigned char)cmap[lookup[(int)col[0]][0]].b;
                }
                }
            }
              else
            {
              for (i = 0; ((i < 65536) && (ptr < end) && (line[i])); i++)
                {
                  col[0] = line[i];
                  *ptr++ = (unsigned char)cmap[lookup[(int)col[0]][0]].r;
                  *ptr++ = (unsigned char)cmap[lookup[(int)col[0]][0]].g;
                  *ptr++ = (unsigned char)cmap[lookup[(int)col[0]][0]].b;
                }
            }
            }
          else if (cpp == 2)
            {
              if (transp)
            {
              for (i = 0; ((i < 65536) && (ptr < end) && (line[i])); i++)
                {
                  col[0] = line[i++];
                  col[1] = line[i];
                  if (cmap[lookup[(int)col[0]][(int)col[1]]].transp)
                {
                  *ptr++ = 255;
                  *ptr++ = 0;
                  *ptr++ = 255;
                }
                  else
                {
                  *ptr++ = (unsigned char)cmap[lookup[(int)col[0]][(int)col[1]]].r;
                  *ptr++ = (unsigned char)cmap[lookup[(int)col[0]][(int)col[1]]].g;
                  *ptr++ = (unsigned char)cmap[lookup[(int)col[0]][(int)col[1]]].b;
                }
                }
            }
              else
            {
              for (i = 0; ((i < 65536) && (ptr < end) && (line[i])); i++)
                {
                  col[0] = line[i++];
                  col[1] = line[i];
                  *ptr++ = (unsigned char)cmap[lookup[(int)col[0]][(int)col[1]]].r;
                  *ptr++ = (unsigned char)cmap[lookup[(int)col[0]][(int)col[1]]].g;
                  *ptr++ = (unsigned char)cmap[lookup[(int)col[0]][(int)col[1]]].b;
                }
            }
            }
          else
            {
              if (transp)
            {
              for (i = 0; ((i < 65536) && (ptr < end) && (line[i])); i++)
                {
                  for (j = 0; j < cpp; j++, i++)
                {
                  col[j] = line[i];
                }
                  col[j] = 0;
                  i--;
                  for (j = 0; j < ncolors; j++)
                {
                  if (!strcmp(col, cmap[j].str))
                    {
                      if (cmap[j].transp)
                    {
                      *ptr++ = 255;
                      *ptr++ = 0;
                      *ptr++ = 255;
                    }
                      else
                    {
                      *ptr++ = (unsigned char)cmap[j].r;
                      *ptr++ = (unsigned char)cmap[j].g;
                      *ptr++ = (unsigned char)cmap[j].b;
                    }
                      j = ncolors;
                    }
                }
                }
            }
              else
            {
              for (i = 0; ((i < 65536) && (ptr < end) && (line[i])); i++)
                {
                  for (j = 0; j < cpp; j++, i++)
                {
                  col[j] = line[i];
                }
                  col[j] = 0;
                  i--;
                  for (j = 0; j < ncolors; j++)
                {
                  if (!strcmp(col, cmap[j].str))
                    {
                      *ptr++ = (unsigned char)cmap[j].r;
                      *ptr++ = (unsigned char)cmap[j].g;
                      *ptr++ = (unsigned char)cmap[j].b;
                      j = ncolors;
                    }
                }
                }
            }
            }
        }
        }
    }
      /* Scan in line from XPM file (limit line length 65k) */
      if (i < 65536)
    {
      if ((!comment) && (quote) && (c != '"'))
        {
          line[i++] = c;
        }
    }
      if ((ptr) && ((ptr - data) >= *w ** h * 3))
    done = 1;
    }
  if (transp)
    *t = 1;
  else
    *t = 0;
  free(cmap);
  return data;
}




#endif
