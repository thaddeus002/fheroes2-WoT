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

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <endian.h>
#include <stdlib.h>
#include <libgen.h> //basename()
#include <string.h> // strlen()

#include "yImage.h"
#include "ySaveImage.h"
#include "palette.h"

int main(int argc, char **argv)
{
    /* Verify number of arguments */
    if(argc != 3)
    {
        std::cout << argv[0] << " infile.til extract_to_dir" << std::endl;
        return -1;
    }

    /* Open til file */
    std::fstream fd_data(argv[1], std::ios::in | std::ios::binary);

    if(fd_data.fail())
    {
        std::cout << "error open file: " << argv[1] << std::endl;
        return -2;
    }

    /* prepare dir to extract */
    std::string prefix(argv[2]); /* directory to create */

    char * base = basename(argv[1]);
    base[strlen(base)-4]='\0';
    std::string shortname(base); /* name of til file without extention */


    std::string extractDir = prefix + '/' + shortname;

    if((mkdir(extractDir.c_str(), 0777)!=0)&&(errno != EEXIST))
    {
        std::cout << "error mkdir: " << extractDir << std::endl;
        return -3;
    }

    fd_data.seekg(0, std::ios_base::end);
    uint32_t size = fd_data.tellg(); /* til file size */
    fd_data.seekg(0, std::ios_base::beg);

    /* read the header */
    uint16_t count, width, height; /* til header infos */

    fd_data.read(reinterpret_cast<char *>(&count), sizeof(uint16_t));
    count = le16toh(count);

    fd_data.read(reinterpret_cast<char *>(&width), sizeof(uint16_t));
    width=le16toh(width);

    fd_data.read(reinterpret_cast<char *>(&height), sizeof(uint16_t));
    height =le16toh(height);

    fprintf(stdout, "TIL file (%d bytes): %d tiles of %d x %d\n", size, count, width, height);

    /* read data */
    char *body = new char[size];

    fd_data.read(body, size-6);
    fd_data.close();

    /* create "count" files */
    for(uint16_t cur = 0; cur < count; ++cur)
    {
        std::ostringstream stream;
        stream << std::setw(3) << std::setfill('0') << cur;
        /* file to create */
        std::string dstfile = extractDir + '/' + stream.str();

        #ifndef WITH_IMAGE
        dstfile += ".bmp";
        #else
        dstfile += ".png";
        #endif

        yImage *im; /* the tile number "cur" */
        int err;
        unsigned char *rgb_data;
        rgb_data=create_bitmap((unsigned char *) &body[width * height * cur], width * height);
        im=create_yImage(&err, rgb_data, width, height);
        free(rgb_data);
        if(err) {
            fprintf(stderr, "Image creation failed : %d\n", err);
        } else {
            err = sauve_png(im, dstfile.c_str());
            fprintf(stdout, "File %s created : %d\n", dstfile.c_str(), err);
            destroy_yImage(im);
        }
    }

    delete [] body;

    std::cout << "expand to: " << prefix << std::endl;

    return EXIT_SUCCESS;
}
