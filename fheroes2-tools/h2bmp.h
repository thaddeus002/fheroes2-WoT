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


extern "C" {
#include "yImage.h"
}

// colors definitions
/** Default color for pixels of type 0 */
#define COLOR0 0xd79801
/** Default color for pixels of type 1 */
#define COLOR1 0x42403a
/** Default color for pixels of type 2 */
#define COLOR2 0x8f6a13


/************************************************************************//**
 * Class to read BMP H2 files and decode them.                              *
 * BMP H2 files are 3 colors images.                                        *
 ***************************************************************************/
class H2bmp {

    private :
        uint16_t magic; /* 0x21 0x00 */
        uint16_t width;
        uint16_t height;
        char *data; /* array of color indexes whose values are 0, 1 or 2 */
        int color0, color1, color2; /* convertion from data values to RGB colors */

    public:

        /** \brief read de data from a file */
        H2bmp(std::string file, int color0 = COLOR0, int color1 = COLOR1, int color2 = COLOR2);

        /** \brief free the object */
        ~H2bmp();

        /** \brief test the validity of this object */
        bool is_valid();

        /** \brief returns the image width (in pixels) */
        uint16_t get_width(){ return width; }

        /** \brief returns the image width (in pixels) */
        uint16_t get_height(){ return height; }

        /**
         * \brief Give the index color of a point.
         * \param x horizontal index of pixel
         * \param y vertical index of pixel, from top
         * \return 0, 1 or 2, or -1 in case of error
         */
        uint8_t get_color_index(int x, int y);

        /** \brief interpretation of image */
        yImage *convert_to_yImage();
};
