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
#include <cstring>
#include <iostream>
#include "h2bmp.h"
#include "yImage.h"



/****************************************************/
/* Main Program                                     */
/****************************************************/

int main(int argc, char **argv)
{
    int err; /* error code */
    bool debug = false; /* print or not debug info */
    char** ptr = argv; /* to parcourt arguments */

    if(argc < 3)
    {
        std::cout << argv[0] << " [-d (debug on)] infile.bmp extract_to_dir" << std::endl;
        return EXIT_FAILURE;
    }

    ++ptr;
    if(!strcmp("-d", *ptr)){
        debug = true;
        ++ptr;
    }
    std::string infile(*ptr);
    ++ptr;
    std::string extractDir(*ptr);

    /* reading file */
    H2bmp *bmpData = new H2bmp(infile);
    if(!bmpData->is_valid()){
        return -1;
    }

    if(debug)
    {
        std::cout << "BMP file: " << infile << std::endl;
        std::cout << "\twidth : " << bmpData->get_width() << std::endl;
        std::cout << "\theight : " << bmpData->get_height() << std::endl;
        std::cout << std::endl;
    }

    if((mkdir(extractDir.c_str(), 0777)!=0)&&(errno != EEXIST))
    {
        std::cout << "error mkdir: " << extractDir << std::endl;
        return EXIT_FAILURE;
    }

    yImage *im;
    im=bmpData->convert_to_yImage();

    delete bmpData;

    std::string outfile = extractDir+"/"+infile;
    outfile.replace(outfile.find(".bmp"), 4, ".png");
    sauve_png(im, outfile.c_str());
    std::cout << "expanded to: " << outfile << std::endl;

    destroy_yImage(im);

    return EXIT_SUCCESS;
}
