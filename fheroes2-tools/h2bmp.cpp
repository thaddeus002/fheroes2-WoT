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


/****************************************************/
/* Class H2bmp to read BMP H2 files and decode them */
/****************************************************/

#include <fstream>
#include <iostream>
#include <cstring>
#include "h2bmp.h"


H2bmp::H2bmp(std::string file, int color0, int color1, int color2){

    std::fstream fd_data(file.c_str(), std::ios::in | std::ios::binary);

    if(fd_data.fail())
    {
        std::cerr << "error open file: " << file << std::endl;

        magic=0;
        width=0;
        height=0;
        data=NULL;

        return;
    }

    fd_data.read(reinterpret_cast<char *>(& magic), sizeof(uint16_t));
    magic = le16toh(magic);

    if(magic!=0x0021){
        std::cerr << "bad file format (" << magic << "): " << file << std::endl;

        magic=0;
        width=0;
        height=0;
        data=NULL;

        return;
    }

    this->color0=color0;
    this->color1=color1;
    this->color2=color2;

    fd_data.read(reinterpret_cast<char *>(& width), sizeof(uint16_t));
    width = le16toh(width);

    fd_data.read(reinterpret_cast<char *>(& height), sizeof(uint16_t));
    height=le16toh(height);

    // Reading data
    int data_size=width*height;

    data = new char[data_size];
    std::memset(data, 0x00, data_size);
    fd_data.read(data, data_size);

    fd_data.close();
}

H2bmp::~H2bmp(){
    if(data!=NULL)
        delete [] data;
}


uint8_t  H2bmp::get_color_index(int x, int y) {

    if(x>=0 && x<width && y>=0 && y<height) {
        return data[y*width + x];
    }

    return -1;
}

bool H2bmp::is_valid(){
    if(data==NULL) return false;
    return true;
}

yImage *H2bmp::convert_to_yImage(){

    yImage *im; /* return value */
    int err; /* error code */

    yColor col0, col1, col2, colErr; // colors for the image
    y_init_color(&col0, color0*256+255);
    y_init_color(&col1, color1*256+255);
    y_init_color(&col2, color2*256+255);
    y_init_color(&colErr, 255);

    im=y_create_image(&err, NULL, width, height);


    for(int y=0; y<height; y++) for(int x=0; x<width; x++)
    {
        uint8_t colorIndex; // color index
        yColor *col;    // color to draw

        colorIndex = this->get_color_index(x,y);

        /* there are only 3 index : this is a try to interpret them */
        switch(colorIndex){
            case 0:
                col=&col0; break;
            case 1:
                col=&col1; break;
            case 2:
                col=&col2; break;
            default:
                fprintf(stderr, "Unwaited index: %d\n", colorIndex);
                col=&colErr;
        }

        y_set_pixel(im, col, x, y);
    }

    return(im);
}
