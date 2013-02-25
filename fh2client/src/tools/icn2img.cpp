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
#include "SDL.h"
#include "engine.h"

void SpriteDrawICN(Surface & sf, const u8* cur, const u32 size,  bool debug);

class icnheader
{
    public:
    icnheader(){};

    void read(std::fstream & fd)
    {
        if(fd.fail()) return;

        fd.read(reinterpret_cast<char *>(& offsetX), sizeof(s16));
        SwapLE16(offsetX);

        fd.read(reinterpret_cast<char *>(& offsetY), sizeof(s16));
        SwapLE16(offsetY);

	fd.read(reinterpret_cast<char *>(& width), sizeof(u16));
	SwapLE16(width);

	fd.read(reinterpret_cast<char *>(& height), sizeof(u16));
	SwapLE16(height);

	fd.ignore();

	fd.read(reinterpret_cast<char *>(& offsetData), sizeof(u32));
	SwapLE32(offsetData);
    };

    s16	offsetX;
    s16 offsetY;
    u16 width;
    u16 height;
    u32 offsetData;
};
                                                
int main(int argc, char **argv)
{
    if(argc < 3)
    {
	std::cout << argv[0] << " [-s (skip shadow)] [-d (debug on)] infile.icn extract_to_dir" << std::endl;

	return EXIT_SUCCESS;
    }

    bool debug = false;
    bool shadow = true;

    char** ptr = argv;
    ++ptr;

    while(ptr && *ptr)
    {
	if(0 == strcmp("-d", *ptr))
	    debug = true;
	else
	if(0 == strcmp("-s", *ptr))
	    shadow = false;
	else
	    break;

	++ptr;
    }
    
    std::string shortname(*ptr);
    ++ptr;
    std::string prefix(*ptr);

    std::fstream fd_data(shortname.c_str(), std::ios::in | std::ios::binary);

    if(fd_data.fail())
    {
	std::cout << "error open file: " << shortname << std::endl;
	return EXIT_SUCCESS;
    }
    
    shortname.replace(shortname.find(".icn"), 4, "");
    
    prefix += SEPARATOR + shortname;

    if(0 != MKDIR(prefix.c_str()))
    {
	std::cout << "error mkdir: " << prefix << std::endl;
	return EXIT_SUCCESS;
    }


    SDL::Init();

    u16 count_sprite;
    u32 total_size;

    fd_data.read(reinterpret_cast<char *>(& count_sprite), sizeof(u16));
    SwapLE16(count_sprite);

    fd_data.read(reinterpret_cast<char *>(& total_size), sizeof(u32));
    SwapLE32(total_size);

    u32 save_pos = fd_data.tellg();

    std::vector<icnheader> headers(count_sprite);
    for(int ii = 0; ii < count_sprite; ++ii) headers[ii].read(fd_data);

    for(int ii = 0; ii < count_sprite; ++ii)
    {
	const icnheader & head = headers[ii];

	u32 data_size = (ii + 1 != count_sprite ? headers[ii + 1].offsetData - head.offsetData : total_size - head.offsetData);
	fd_data.seekg(save_pos + head.offsetData, std::ios_base::beg);
    
        char *buf = new char[data_size + 100];
        std::memset(buf, 0x80, data_size + 100);
        fd_data.read(buf, data_size);

	Surface sf(head.width, head.height, false);
	sf.SetDefaultColorKey();
	sf.Fill(0xff, 0xff, 0xff);

	SpriteDrawICN(sf, reinterpret_cast<const u8*>(buf), data_size, debug);
        delete [] buf;

	std::string dstfile(prefix);
	dstfile += SEPARATOR;

	std::ostringstream stream;
        stream << ii;

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

#ifndef WITH_IMAGE
	dstfile += ".bmp";
#else
	dstfile += ".png";
#endif
	sf.Save(dstfile.c_str());
    }

    fd_data.close();
    std::cout << "expand to: " << prefix << std::endl;

    SDL::Quit();
    return EXIT_SUCCESS;
}

void SpriteDrawICN(Surface & sf, const u8* cur, const u32 size,  bool debug)
{
    if(NULL == cur || 0 == size) return;

    const u8 *max = cur + size;

    u8  c = 0;
    u16 x = 0;
    u16 y = 0;

    u32 shadow = sf.MapRGB(0, 0, 0, 0x40);

    // lock surface
    sf.Lock();
    while(1)
    {
	// 0x00 - end line
	if(0 == *cur)
	{
	    ++y;
	    x = 0;
	    ++cur;
	}
	else
	// 0x7F - count data
	if(0x80 > *cur)
	{
	    c = *cur;
	    ++cur;
	    while(c-- && cur < max)
	    {
		sf.SetPixel(x, y, sf.GetColorIndex(*cur));
		++x;
		++cur;
	    }
	}
	else
	// 0x80 - end data
	if(0x80 == *cur)
	{
	    break;
	}
	else
	// 0xBF - skip data
	if(0xC0 > *cur)
	{
	    x += *cur - 0x80;
	    ++cur;
	}
	else
	// 0xC0 - shadow
	if(0xC0 == *cur)
	{
	    ++cur;
	    c = *cur % 4 ? *cur % 4 : *(++cur);

	    while(c--){ sf.SetPixel(x, y, shadow); ++x; }

	    ++cur;
	}
	else
	// 0xC1
	if(0xC1 == *cur)
	{
	    ++cur;
	    c = *cur;
	    ++cur;
	    while(c--){ sf.SetPixel(x, y, sf.GetColorIndex(*cur)); ++x; }
	    ++cur;
	}
	else
	{
	    c = *cur - 0xC0;
	    ++cur;
	    while(c--){ sf.SetPixel(x, y, sf.GetColorIndex(*cur)); ++x; }
	    ++cur;
	}

	if(cur >= max)
	{
	    std::cerr << "out of range" << std::endl;
	    break;
	}
    }

    // unlock surface
    sf.Unlock();
}
