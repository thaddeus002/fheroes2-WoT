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

#if defined(_WINDOWS) || defined(_WIN32) || defined(__WIN32__)
#include <io.h>
#define MKDIR(X)    mkdir(X)
#else
#define MKDIR(X)    mkdir(X, S_IRWXU)
#endif

#include <iostream>
#include <fstream>
#include <sstream>

#include "kbpal.h"
#include "engine.h"

void DrawICN(Surface & sf, u32 size, const u8 *vdata, const std::vector<u32> & palette, bool rledebug);

class icnheader
{
    public:
    icnheader(){};

    void read(std::fstream & fd)
    {
        if(fd.fail()) return;

        fd.read(reinterpret_cast<char *>(& offsetX), sizeof(s16));
        SWAP16(offsetX);

        fd.read(reinterpret_cast<char *>(& offsetY), sizeof(s16));
        SWAP16(offsetY);

	fd.read(reinterpret_cast<char *>(& width), sizeof(u16));
	SWAP16(width);

	fd.read(reinterpret_cast<char *>(& height), sizeof(u16));
	SWAP16(height);

	fd.ignore();

	fd.read(reinterpret_cast<char *>(& offsetData), sizeof(u32));
	SWAP32(offsetData);
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
	std::cout << argv[0] << " [-d] infile.icn extract_to_dir" << std::endl;

	return EXIT_SUCCESS;
    }

    std::string prefix(argv[2]);
    std::string shortname(argv[1]);
    
    bool debug = false;
    
    if(shortname == "-d")
    {
	shortname = prefix;
	prefix = std::string(argv[3]);
	debug = true;
    }

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


    SDL_Color *colors = new SDL_Color[SIZEPALETTE];
    std::vector<u32> palette(SIZEPALETTE);

    Surface surface(1, 1, true);
    surface.SetAlpha(255);

    char *p = kb_pal;

    for(u16 ii = 0; ii < SIZEPALETTE; ++ii)
    {
	colors[ii].r = *p++;
	colors[ii].g = *p++;
	colors[ii].b = *p++;
	
	colors[ii].r <<= 2;
	colors[ii].g <<= 2;
	colors[ii].b <<= 2;
	
	palette[ii] = surface.MapRGB(colors[ii].r, colors[ii].g, colors[ii].b);
    }

    SDL::Init();

    u16 count_sprite;
    u32 total_size;

    fd_data.read(reinterpret_cast<char *>(& count_sprite), sizeof(u16));
    SWAP16(count_sprite);

    fd_data.read(reinterpret_cast<char *>(& total_size), sizeof(u32));
    SWAP32(total_size);

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

	sf.SetColorKey();

	sf.Fill(0xff, 0xff, 0xff);

	DrawICN(sf, data_size, reinterpret_cast<const u8*>(buf), palette, debug);

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

	dstfile += ".bmp";

	sf.SaveBMP(dstfile.c_str());
    }

    delete [] colors;

    fd_data.close();

    std::cout << "expand to: " << prefix << std::endl;

    SDL::Quit();

    return EXIT_SUCCESS;
}

/* draw RLE ICN to surface */
void DrawICN(Surface & sf, u32 size, const u8 *vdata, const std::vector<u32> & palette, bool rledebug)
{
    u8 i, count;
    u16 x = 0;
    u16 y = 0;
    u32 index = 0;

    u32 shadow = sf.alpha() ? sf.MapRGB(0, 0, 0, 0x40) : DEFAULT_COLOR_KEY16;

    if(rledebug) printf("START RLE DEBUG\n");

    // lock surface
    sf.Lock();

    while(index < size)
    {
	// 0x00 - end line
	if(0 == vdata[index])
	{
	    ++y;
	    x = 0;
	    if(rledebug) printf(" M:00\n");
	    ++index;
	    continue;
	}
	else
	// range 0x01..0x7F XX
	if(0x80 > vdata[index])
	{
	    if(rledebug) printf(" M:%hhX C:%d:D", vdata[index], vdata[index]);
	    count = vdata[index];
	    ++index;
	    i = 0;
	    while(i++ < count && index < size)
	    {
		if(rledebug) printf(":%hhX", vdata[index]);
		sf.SetPixel2(x++, y, palette.at(vdata[index++]));
	    }
	    continue;
	}
	else
	// end data
	if(0x80 == vdata[index])
	{
	    if(rledebug) printf("\nM:%hhX\n", vdata[index]);
	    break;
	}
	else
	// range 0x81..0xBF 00 
	if(0x80 < vdata[index] && 0xC0 > vdata[index])
	{
	    if(rledebug) printf(" M:%hhX Z:%d", vdata[index], vdata[index] - 0x80);
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
		if(rledebug) printf(" M4:%hhX:%d A", vdata[index], vdata[index] % 4);
		count = vdata[index];
		++index;
		for(i = 0; i < vdata[index]; ++i)
		{
		    sf.SetPixel2(x++, y, shadow);
		    if(rledebug) printf(":%hhX", count);
		}
		++index;
		continue;
	    }
	    else
	    {
		if(rledebug) printf(" M4:%hhX:%d A", vdata[index], vdata[index] % 4);
		count = vdata[index];
		for(i = 0; i < vdata[index] % 4; ++i)
		{
		    sf.SetPixel2(x++, y, shadow);
		    if(rledebug) printf(":%hhX", count);
		}
		++index;
		continue;
	    }
	}
	else
	// 0xC1 N D count - data
	if(0xC1 == vdata[index])
	{
	    if(rledebug) printf(" M:%hhX", vdata[index]);
	    ++index;
	    count = vdata[index];
	    if(rledebug) printf(" C:%d:D", count);
	    ++index;
	    for(i = 0; i < count; ++i)
	    {
	    	sf.SetPixel2(x++, y, palette.at(vdata[index]));
	    	if(rledebug) printf(":%hhX", vdata[index]);
	    }
	    ++index;
	    continue;
	}
	else
	// 0xC2 more
	if(0xC1 < vdata[index])
	{
	    if(rledebug) printf(" M:%hhX", vdata[index]);
	    count = vdata[index] - 0xC0;
	    if(rledebug) printf(" C:%d:D", count);
	    ++index;
	    for(i = 0; i < count; ++i)
	    {
		sf.SetPixel2(x++, y, palette.at(vdata[index]));
		if(rledebug) printf(":%hhX", vdata[index]);
	    }
	    ++index;
	    continue;
	}
    }

    // unlock surface
    sf.Unlock();

    if(rledebug) printf("END RLE DEBUG\n");
}
