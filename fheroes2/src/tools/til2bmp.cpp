/***************************************************************************
 *   Copyright (C) 2006 by Andrey Afletdinov                               *
 *   afletdinov@mail.dc.baikal.ru                                          *
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
#include <fstream>
#include <sstream>

#include "palette.h"
#include "gamedefs.h"
#include "surface.h"
#include "display.h"

int main(int argc, char **argv)
{
    if(argc != 3)
    {
	std::cout << argv[0] << " infile.til extract_to_dir" << std::endl;

	return EXIT_SUCCESS;
    }

    std::fstream fd_data(argv[1], std::ios::in | std::ios::binary);

    if(fd_data.fail())
    {
	std::cout << "error open file: " << argv[1] << std::endl;

	return EXIT_SUCCESS;
    }

    std::string prefix(argv[2]);
    std::string shortname(argv[1]);
    shortname.replace(shortname.find("."), 4, "");
    
    prefix += SEPARATOR + shortname;

    if(0 != mkdir(prefix.c_str(), S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IXGRP|S_IROTH|S_IXOTH))
    {
	std::cout << "error mkdir: " << prefix << std::endl;

	return EXIT_SUCCESS;
    }

    fd_data.seekg(0, std::ios_base::end);
    u32 size = fd_data.tellg();
    fd_data.seekg(0, std::ios_base::beg);

    u16 count, width, height;
    
    fd_data.read(reinterpret_cast<char *>(&count), sizeof(u16));
    SWAP16(count);

    fd_data.read(reinterpret_cast<char *>(&width), sizeof(u16));
    SWAP16(width);

    fd_data.read(reinterpret_cast<char *>(&height), sizeof(u16));
    SWAP16(height);

    char *body = new char[size];

    fd_data.read(body, size);


    SDL_Color *colors = new SDL_Color[SIZEPALETTE];

    char *p = kb_pal;

    for(u16 ii = 0; ii < SIZEPALETTE; ++ii)
    {
	colors[ii].r = *p++;
	colors[ii].g = *p++;
	colors[ii].b = *p++;
	
	colors[ii].r <<= 2;
	colors[ii].g <<= 2;
	colors[ii].b <<= 2;
    }

    Display::Initialize();

    for(u16 cur = 0; cur < count; ++cur)
    {
	const char *vdata = &body[width * height * cur];

        Surface sf(width, height, 8, SDL_SWSURFACE);

        sf.LoadPalette(colors, SIZEPALETTE);

	sf.Lock();

	memcpy(const_cast<void *>(sf.pixels()), vdata, width * height);

	sf.Unlock();

	std::string dstfile(prefix);

	dstfile += SEPARATOR;

	std::ostringstream stream;
        stream << cur;

        switch(stream.str().size())
        {
    	    case 1:
    		dstfile += "00" + stream.str();
    		break;

    	    case 2:
    		dstfile += "0" + stream.str();
    		break;

    	    default:
    		dstfile += stream.str();
    		break;
        }

	dstfile += ".bmp";

	sf.SaveBMP(dstfile.c_str());
    }

    delete [] colors;
    delete [] body;

    fd_data.close();

    std::cout << "expand to: " << prefix << std::endl;

    return EXIT_SUCCESS;
}
