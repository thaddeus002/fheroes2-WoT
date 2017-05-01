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

#include <sys/stat.h>
#include <sys/types.h>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cstring>

#include "yImage.h"
#include "palette.h"
#include <vector>

#include "h2icn.h"
#include <endian.h>


/* CLASS ICNHEADER */

void icnheader::read(std::fstream & fd)
{
        if(fd.fail()) return;

        fd.read(reinterpret_cast<char *>(& offsetX), sizeof(int16_t));
        offsetX=le16toh(offsetX);

        fd.read(reinterpret_cast<char *>(& offsetY), sizeof(int16_t));
        offsetY=le16toh(offsetY);

        fd.read(reinterpret_cast<char *>(& width), sizeof(uint16_t));
        width=le16toh(width);

        fd.read(reinterpret_cast<char *>(& height), sizeof(uint16_t));
        height=le16toh(height);

        if(version == 2){
            fd.read(reinterpret_cast<char *>(& type), sizeof(uint8_t));
        }

        fd.read(reinterpret_cast<char *>(& offsetData), sizeof(uint32_t));
        offsetData=le32toh(offsetData);
}

void icnheader::present(int number)
{
        std::cout << std::endl;
        if(number<0) {
            std::cout << "Content of sprite header: " << std::endl;
        } else {
            std::cout << "Content of sprite "<< number << " header: " << std::endl;
        }
        std::cout << "Offset X: " << offsetX << std::endl;
        std::cout << "Offset Y: " << offsetY << std::endl;
        std::cout << "Width: " << width << std::endl;
        std::cout << "Height: " << height << std::endl;
        if(version == 2) {
            std::cout << "Type of sprite: " << (int) type << std::endl;
        }
        std::cout << "Offset of data in file: " << offsetData << std::endl;
}


/* CLASS ICNSPRITE */

icnsprite::icnsprite(icnheader header, int dataSize, unsigned char *dataContent){

    data=(unsigned char *) malloc(dataSize);
    if(data==NULL) return;

    memcpy(data, dataContent, dataSize);
    data_size=dataSize;

    width=header.width;
    height=header.height;
    type=header.type;
    version=header.version;
}



icnsprite::~icnsprite(){
    if(data!=NULL) free(data);
}


yImage *icnsprite::converti_en_yImage(){

    yImage *im;
    int err;

    bool debug=true;
    std::cout << "Decoding image (" << data_size << " bytes)" << std::endl;

    if(data == NULL || data_size == 0) return NULL;


    im=create_yImage(&err, NULL, width, height);
    if(im==NULL) return NULL;
    transp(im);

    const uint8_t *max = data + data_size; // position of end of data for sprite
    uint8_t *cur = data;

    uint8_t  c = 0; // number of pixel coming
    // position on the surface
    uint16_t x = 0;
    uint16_t y = 0;

    /* color for the shadows */
    yColor shadow;
    y_set_color(&shadow, 0, 0, 0, 0x40);

    if(version == 1){
        homm_init_palette("extractions/HEROES.AGG/kb.pal", 1);
    } else {
        homm_init_palette("extractions/HEROES2.AGG/kb.pal");
    }

    yColor color; /* for coloring current pixel */

    while(1)
    {
        // 0x00 - end line
        if(0 == *cur)
        {
            if(debug) std::cout << "Found : end of line" << std::endl;
            ++y;
            x = 0;
            ++cur;
        }

        else
        // 0x7F - count data
        if(0x80 > *cur)
        {
            if(!type)
            {
                c = *cur;
                if(debug) std::cout << "Reading " << (int) c << " pixels" << std::endl;
                ++cur;
                while(c-- && cur < max)
                {
                    if(debug)
                    {
                        std::cout << "  c(" << x << "," << y << ") = " << (int) *cur;
                    }

                    color = getColor(*cur);
                    yImage_set_pixel(im, &color, x, y);

                    ++x;
                    ++cur;
                }
                if(debug) std::cout << std::endl;
            }
            else
            {
                if(debug) std::cout << "writing " << (int) *cur << " pixels" << std::endl;
                c = *cur;
                while(c--)
                {
                    color = getColor(1);
                    yImage_set_pixel(im, &color, x, y);
                    ++x;
                }
                ++cur;
            }

        }

        else
        // 0x80 - end data
        if(0x80 == *cur)
        {
            if(debug) std::cout << "end of sprite" << std::endl;
            break;
        }

        else
        // 0xBF - skip data
        if(0xC0 > *cur)
        {
            if(debug) std::cout << (int) *cur << " : skipping " <<  (int) (*cur - 0x80) << " pixels" << std::endl;
            x += *cur - 0x80;
            ++cur;
        }

        else
        // 0xC0 - shadow
        if((!type)&&(0xC0 == *cur))
        {
            ++cur;
            c = *cur % 4 ? *cur % 4 : *(++cur);

            if(debug) std::cout << (int) c << " pixels of shadow" << std::endl;

            while(c--){
                yImage_set_pixel(im, &shadow, x, y);

                ++x;
            }

            ++cur;
        }

        else
        // 0xC1
        if((!type)&&(0xC1 == *cur))
        {
            ++cur;
            c = *cur;
            ++cur;

            color = getColor(*cur);
            if(debug) std::cout << "writing " << (int) c << " pixels of color " << *cur << std::endl;
            while(c--){
                yImage_set_pixel(im, &color, x, y);
                ++x;
            }
            ++cur;
        }

        else
        {
            if(!type)
            {
                c = *cur - 0xC0;
                if(debug) std::cout << "found " << (int) *cur << " : reading " << (int) c << " pixels" << std::endl;
                ++cur;
                while(c--){
                    color = getColor(*cur);
                    yImage_set_pixel(im, &color, x, y);
                    ++x;
                }
            }
            else // for type 32 - skipping data
            {
                if(debug) std::cout << (int) *cur << " : skipping " <<  (int) (*cur - 0x80) << " pixels" << std::endl;
                x += *cur - 0x80;
            }
            ++cur;
        }

        if(cur >= max)
        {
            std::cerr << "out of range (over max position for the sprite : " << cur-data << " >= " << data_size << ")" << std::endl;
            break;
        }
    }

    return(im);
}


/* CLASS ICNFILE */


icnfile::icnfile(std::string filename, int version){
    headers=NULL;
    icndata=NULL;
    this->version=version;
    this->read_icnfile(filename);
}


void icnfile::read_icnfile(std::string file){

    int i; /* counter */

    int header_size=13;
    if(version==1){
        header_size=12;
    }

    std::fstream fd(file.c_str(), std::ios::in | std::ios::binary);
    if(fd.fail()) return;

    name=file;

    fd.read(reinterpret_cast<char *>(& count_sprite), sizeof(uint16_t));
    count_sprite=le16toh(count_sprite);

    fd.read(reinterpret_cast<char *>(& total_size), sizeof(uint32_t));
    total_size=le32toh(total_size);

    std::cout << count_sprite << " sprite(s) on " << total_size << " bytes" << std::endl;

    headers = new icnheader[count_sprite];

    icndata=(unsigned char *) malloc(total_size-(count_sprite*header_size));
    if(icndata==NULL){
        std::cerr << "Allocate error for icn data" << std::endl;
        return;
    }

    for(i=0; i<count_sprite; i++){
        headers[i].version = version;
        headers[i].read(fd);
        headers[i].present();
    }

    fd.read(reinterpret_cast<char *>(icndata), total_size-(count_sprite*header_size));
    fd.close();
}


int icnfile::sprite_size(int numOfSprite){
    if(numOfSprite>count_sprite) return(0);

    if(numOfSprite==count_sprite)
        return(total_size-headers[count_sprite-1].offsetData);

    return(headers[numOfSprite].offsetData-headers[numOfSprite-1].offsetData);
}


unsigned char *icnfile::sprite_data(int numOfSprite){
    int size;
    int header_size=13;
    if(version==1){
        header_size=12;
    }

    size=sprite_size(numOfSprite);

    if(!size) return(NULL);

    return(icndata+headers[numOfSprite-1].offsetData-(count_sprite*header_size));
}


icnheader icnfile::get_icnheader(int numOfSprite){

    return(headers[numOfSprite-1]);
}



int icnfile::create_files(std::string dir){

    int i; /* counter */
    std::string name_spec_file = dir+'/'+"spec.xml";

    if((mkdir(dir.c_str(), 0777)!=0)&&(errno != EEXIST))
    {
        std::cout << "error mkdir: " << dir << std::endl;
        return EXIT_FAILURE;
    }

    /* create spec file */
    std::fstream fd_spec(name_spec_file.c_str(), std::ios::out);
    if(fd_spec.fail())
    {
        std::cout << "error write file: " << name_spec_file << std::endl;
        return -1;
    }
    fd_spec << "<?xml version=\"1.0\" ?>" << std::endl;

    fd_spec << "<icn name=\"" << name << "\" count=\"" << count_sprite << "\">" << std::endl;


    for (i=0; i<count_sprite; i++){
        yImage *image;

        icnsprite sprite(headers[i], this->sprite_size(i+1), this->sprite_data(i+1));

        std::ostringstream stream;
        stream << std::setw(3) << std::setfill('0') << i;

        std::string dstfile = dir+'/'+stream.str();
        std::string shortdstfile(stream.str()); // the name of destfile without the path


        image=sprite.converti_en_yImage();

        #ifndef HAVE_LIBPNG
            dstfile += ".ppm";
            shortdstfile += ".ppm";
            sauve_ppm(image, dstfile.c_str());
        #else
            dstfile += ".png";
            shortdstfile += ".png";
            sauve_png(image, dstfile.c_str());
        #endif

        destroy_yImage(image);

        fd_spec << " <sprite index=\"" << i+1 << "\" name=\"" << shortdstfile.c_str() << "\" ox=\"" << headers[i].offsetX << "\" oy=\"" << headers[i].offsetY << "\"/>" << std::endl;
    }

    fd_spec << "</icn>" << std::endl;
    fd_spec.close();

    std::cout << "expanded to: " << dir << std::endl;

    return(0);
}
