/***************************************************************************
 *   Copyright (C) 2009 by Andrey Afletdinov <fheroes2@gmail.com>          *
 *                                                                         *
 *   Part of the Free Heroes2 Engine:                                      *
 *   http://sourceforge.net/projects/fheroes2                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef ICN2IMG_H
#define ICN2IMG_H

#include "yImage.h"


/* CLASS ICNHEADER */
/**
 * \brief The description of an image.
 */
class icnheader
{
    public:

    /* relative position of the image */
    int16_t offsetX, offsetY;
    /* dimension of the image */
    uint16_t width, height;
    /** type of sprite : 0 = Normal, 32 = Monochromatic shape */
    uint8_t type;
    /** beginning of data in the global file */
    uint32_t offsetData;
    /** version of heroes from which the file is */
    int version;

    icnheader(){ width=0; height=0; version=2;};

    /** read ICN Header from stream */
    void read(std::fstream & fd);

    /** write ICN Header in a stream */
    void write(std::fstream & fd);

    /** write the content of the header on standart output */
    void present(int number=-1);
};


/* CLASS ICNSPRITE */
/**
 * \brief This is one image of the sprite.
 *
 * A sprite is a header plus the image's data.
 */
class icnsprite : public icnheader {

    private:
        int data_size;
        unsigned char *data;

    public:
        /** create the sprite by assemble all the data */
        icnsprite(icnheader *header, int dataSize, unsigned char *dataContent);
        /**
         * \brief Create the sprite from an image file.
         * \param filename the complete path of image file
         * \param ox X offset for this sprite
         * \param oy Y offset for this sprite
         */
        icnsprite(std::string filename, int ox, int oy);
        
        ~icnsprite();

        int get_data_size() { return data_size; }

        unsigned char *get_data() { return data; } 

        /** convert the data in a image */
        yImage *converti_en_yImage();
};


/* CLASS ICNFILE */
/** Info contained in a ICN file */
class icnfile {

    private:

        /** name of file */
        std::string name;
        /* General file header */
        uint16_t count_sprite; /* number of sprites */
        uint32_t total_size; /* size of icnfile without the general header */

        icnheader **headers; /* array of "count_sprite" pointers to icnheader */

        unsigned char *icndata; /* the data of sprites */

        // version of data format : 1 or 2
        int version;

        /* initialize the objet with the containt of a file */
        void read_icnfile(std::string file);

        /**
         * \param numOfSprite number of sprite : 1 to count_sprite
         */
        int sprite_size(int numOfSprite);
        unsigned char *sprite_data(int numOfSprite);
        icnheader *get_icnheader(int numOfSprite);

    public:

        /**
         * \brief Read the data from an ICN file.
         * \param filename the data filename
         * \param version HOMM version of datafile (1 or 2)
         */
        icnfile(std::string filename, int version);

        /**
         * \brief Read the data from files in a directory.
         * \param dirname the path to the directory where are extracted
         *            the data
         */
        icnfile(std::string dirname);

        ~icnfile() {
            if(headers!=NULL){
                for(int i = 0; i < count_sprite; i++)
                    if(headers[i] != NULL) delete headers[i];
                delete [] headers;
                if(icndata!=NULL) free(icndata);
            }
        }

        /**
         * \brief Create the files (images + spec.xml) in the specified
         * directory.
         * \param dir the directory name in which the extracted data
         *            will be stored
         */
        int create_files(std::string dir);

        /**
         * \brief Create an HOMM II compatible icn file.
         * \param filename the name of the new icn file (may have ".icn"
         *             extension
         * \return 0 on succes
         */
        int create_icn_file(std::string filename);

        /**
         * \brief Show infos about the content of this icnfile.
         */
        void show_infos();
};


#endif

