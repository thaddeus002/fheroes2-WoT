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
/** the description of an image */
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

    /** write the content of the header on standart output */
    void present(int number=-1);
};


/* CLASS ICNSPRITE */
/** this is one image of the sprite */
class icnsprite : public icnheader {

    private:
        int data_size;
        unsigned char *data;

    public:
        /** create the sprite by assemble all the data */
        icnsprite(icnheader header, int dataSize, unsigned char *dataContent);
        ~icnsprite();

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

        icnheader *headers; /* array of "count_sprite" icnheader */

        unsigned char *icndata; /* the data of sprites */

        // version of data format : 1 or 2
        int version;

        /* initialize the objet with the containt of a file */
        void read_icnfile(std::string file);

        int sprite_size(int numOfSprite);
        unsigned char *sprite_data(int numOfSprite);
        icnheader get_icnheader(int numOfSprite);

    public:

        icnfile(std::string filename, int version);

        ~icnfile() {
            int i;
            if(headers!=NULL){
                delete [] headers;
                if(icndata!=NULL) free(icndata);
            }
        }

        /**
         * \brief Create the files (images + spec.xml) in the specified directory.
         */
        int create_files(std::string dir);
};


#endif

