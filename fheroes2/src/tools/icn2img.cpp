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

void DrawICN(Surface & sf, u32 size, const u8 *vdata, bool rledebug, bool);

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

	Surface sf(head.width, head.height, true);

	sf.SetDefaultColorKey();

	sf.Fill(0xff, 0xff, 0xff);

	DrawICN(sf, data_size, reinterpret_cast<const u8*>(buf), debug, shadow);

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

/* draw RLE ICN to surface */
void DrawICN(Surface & sf, u32 size, const u8 *vdata, bool rledebug, bool allow_shadow)
{
    u8 i, count;
    u16 x = 0;
    u16 y = 0;
    u32 index = 0;
    u32 shadow = allow_shadow ? sf.MapRGB(0, 0, 0, 0x40) : sf.GetColorKey();

    if(rledebug) std::cerr << "START RLE DEBUG" << std::hex << std::setfill('0') << std::endl;

    // lock surface
    sf.Lock();

    while(index < size)
    {
	// 0x00 - end line
	if(0 == vdata[index])
	{
	    ++y;
	    x = 0;
	    if(rledebug) std::cerr << " M:00" << std::endl;
	    ++index;
	    continue;
	}
	else
	// range 0x01..0x7F XX
	if(0x80 > vdata[index])
	{
	    if(rledebug)std::cerr << " M:0x" << std::setw(2) << static_cast<int>(vdata[index]) << " C:0x" << std::setw(2) << static_cast<int>(vdata[index]);
	    count = vdata[index];
	    ++index;
	    i = 0;
	    while(i++ < count && index < size)
	    {
		if(rledebug) std::cerr << ":0x" << std::setw(2) << static_cast<int>(vdata[index]);
		sf.SetPixel(x++, y, sf.GetColorIndex(vdata[index++]));
	    }
	    continue;
	}
	else
	// end data
	if(0x80 == vdata[index])
	{
	    if(rledebug) std::cerr << std::endl << "M:0x" << std::setw(2) << static_cast<int>(vdata[index]) << std::endl;
	    break;
	}
	else
	// range 0x81..0xBF 00 
	if(0x80 < vdata[index] && 0xC0 > vdata[index])
	{
	    if(rledebug) std::cerr << "M:0x" << std::setw(2) << static_cast<int>(vdata[index]) << " Z:0x" << std::setw(2) << static_cast<int>(vdata[index] - 0x80) <<std::endl;
	    x += (vdata[index] - 0x80);
	    ++index;
	    continue;
	}
	else
	// 0xC0 - seek
	if(0xC0 == vdata[index])
	{
	    if(rledebug) printf(" M:C0");
	    ++index;

	    if( 0 == vdata[index] % 4)
	    {
		if(rledebug) std::cerr << " M4:0x" << std::setw(2) << static_cast<int>(vdata[index]) << ":" << std::setw(1) << static_cast<int>(vdata[index] % 4) << " A";
		count = vdata[index];
		++index;
		for(i = 0; i < vdata[index]; ++i)
		{
		    if(allow_shadow)
		    {
			sf.SetPixel(x++, y, shadow);
			if(rledebug) std::cerr << ":0x" << std::setw(2) << static_cast<int>(count);
		    }
		    x++;
		}
		++index;
		continue;
	    }
	    else
	    {
		if(rledebug) std::cerr << " M4:0x" << std::setw(2) << static_cast<int>(vdata[index]) << ":" << std::setw(1) << static_cast<int>(vdata[index] % 4) << " A";
		count = vdata[index];
		for(i = 0; i < vdata[index] % 4; ++i)
		{
		    if(allow_shadow)
		    {
			sf.SetPixel(x++, y, shadow);
			if(rledebug) std::cerr << ":0x" << std::setw(2) << static_cast<int>(count);
		    }
		}
		++index;
		continue;
	    }
	}
	else
	// 0xC1 N D count - data
	if(0xC1 == vdata[index])
	{
	    if(rledebug) std::cerr << ":0x" << std::setw(2) << static_cast<int>(vdata[index]);
	    ++index;
	    count = vdata[index];
	    if(rledebug) std::cerr << " C:0x" << std::setw(2) << static_cast<int>(count) << ":D";
	    ++index;
	    for(i = 0; i < count; ++i)
	    {
	    	sf.SetPixel(x++, y, sf.GetColorIndex(vdata[index]));
		if(rledebug) std::cerr << ":0x" << std::setw(2) << static_cast<int>(vdata[index]);
	    }
	    ++index;
	    continue;
	}
	else
	// 0xC2 more
	if(0xC1 < vdata[index])
	{
	    if(rledebug) std::cerr << " M:0x" << std::setw(2) << static_cast<int>(vdata[index]);
	    count = vdata[index] - 0xC0;
	    if(rledebug) std::cerr << " C:0x" << std::setw(2) << static_cast<int>(count) << ":D";
	    ++index;
	    for(i = 0; i < count; ++i)
	    {
		sf.SetPixel(x++, y, sf.GetColorIndex(vdata[index]));
		if(rledebug) std::cerr << ":0x" << std::setw(2) << static_cast<int>(vdata[index]);
	    }
	    ++index;
	    continue;
	}
    }

    // unlock surface
    sf.Unlock();

    if(rledebug) std::cerr << "END RLE DEBUG" << std::endl;
}
