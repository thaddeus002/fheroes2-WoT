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
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cstring>
#include <iomanip>

#include "engine.h"
#include "system.h"
#include "palette_h2.h"

class icnheader
{
    public:
    icnheader(){};

    void read(std::fstream & fd)
    {
        if(! fd.fail())
	{
	    offsetX = StreamBase::getLE16(fd);
    	    offsetY = StreamBase::getLE16(fd);
    	    width = StreamBase::getLE16(fd);
    	    height = StreamBase::getLE16(fd);
    	    type = fd.get();
    	    offsetData = StreamBase::getLE32(fd);
	}
    };

    s16	offsetX;
    s16 offsetY;
    u16 width;
    u16 height;
    u8 type; // type of sprite : 0 = Normal, 32 = Monochromatic shape
    u32 offsetData;
};

void SpriteDrawICNv1(Surface & sf, const u8* cur, const u32 size,  bool debug);
void SpriteDrawICNv2(Surface & sf, const u8* cur, const u32 size,  bool debug);

namespace H2Palette
{
    std::vector<SDL_Color> pal_colors;

    void Init(void)
    {
	// load palette
	u32 ncolors = ARRAY_COUNT(kb_pal) / 3;
	pal_colors.reserve(ncolors);

	for(u32 ii = 0; ii < ncolors; ++ii)
	{
    	    u32 index = ii * 3;
    	    SDL_Color cols;

    	    cols.r = kb_pal[index] << 2;
    	    cols.g = kb_pal[index + 1] << 2;
    	    cols.b = kb_pal[index + 2] << 2;

    	    pal_colors.push_back(cols);
	}

	Surface::SetDefaultPalette(&pal_colors[0], pal_colors.size());
    }

    RGBA GetColor(u32 index)
    {
	return index < pal_colors.size() ?
    	RGBA(pal_colors[index].r, pal_colors[index].g, pal_colors[index].b) : RGBA(0,0,0);
    }
};

int main(int argc, char **argv)
{
    if(argc < 3)
    {
	std::cout << argv[0] << " [-s (skip shadow)] [-d (debug on)] infile.icn extract_to_dir" << std::endl;

	return EXIT_SUCCESS;
    }

    bool debug = false;
    //bool shadow = true;

    char** ptr = argv;
    ++ptr;

    while(ptr && *ptr)
    {
	if(0 == strcmp("-d", *ptr))
	    debug = true;
	//else
	//if(0 == strcmp("-s", *ptr))
	//    shadow = false;
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
    
    prefix = System::ConcatePath(prefix, shortname);

    if(0 != System::MakeDirectory(prefix))
    {
	std::cout << "error mkdir: " << prefix << std::endl;
	return EXIT_SUCCESS;
    }

    // write file "spec.xml"
    std::string name_spec_file = System::ConcatePath(prefix, "spec.xml");
    
    std::fstream fd_spec(name_spec_file.c_str(), std::ios::out);
    if(fd_spec.fail())
    {
	std::cout << "error write file: " << shortname << std::endl;
	return EXIT_SUCCESS;
    }

    SDL::Init();
    H2Palette::Init();

    u16 count_sprite;
    u32 total_size;

    count_sprite = StreamBase::getLE16(fd_data);
    total_size = StreamBase::getLE32(fd_data);

    fd_spec << "<?xml version=\"1.0\" ?>" << std::endl <<
		"<icn name=\"" << shortname << ".icn\" count=\"" << count_sprite << "\">" << std::endl;

    u32 save_pos = fd_data.tellg();

    std::vector<icnheader> headers(count_sprite);
    for(int ii = 0; ii < count_sprite; ++ii) headers[ii].read(fd_data);

    for(int ii = 0; ii < count_sprite; ++ii)
    {
	const icnheader & head = headers[ii];

	u32 data_size = (ii + 1 != count_sprite ? headers[ii + 1].offsetData - head.offsetData : total_size - head.offsetData);
	fd_data.seekg(save_pos + head.offsetData, std::ios_base::beg);
    
        u8* buf = new u8[data_size + 100];
        std::memset(buf, 0x80, data_size + 100);
        fd_data.read((char*) buf, data_size);

	Surface sf(head.width, head.height, /*false*/true); // accepting transparency

        const RGBA clkey = RGBA(0xFF, 0, 0xFF);
        sf.Fill(clkey);
	sf.SetColorKey(clkey);

	//sf.Fill(0xff, 0xff, 0xff);
	sf.Fill(ColorBlack); // filling with transparent color

	if(0x20 == head.type)
	    SpriteDrawICNv2(sf, buf, data_size, debug);
	else
	    SpriteDrawICNv1(sf, buf, data_size, debug);
        delete [] buf;

	std::ostringstream stream;
        stream << std::setw(3) << std::setfill('0') << ii;

	std::string dstfile = System::ConcatePath(prefix, stream.str());
	std::string shortdstfile(stream.str()); // the name of destfile without the path

#ifndef WITH_IMAGE
	dstfile += ".bmp";
	shortdstfile += ".bmp";
#else
	dstfile += ".png";
	shortdstfile += ".png";
#endif
	sf.Save(dstfile.c_str());
	fd_spec << " <sprite index=\"" << ii+1 << "\" name=\"" << shortdstfile.c_str() << "\" ox=\"" << head.offsetX << "\" oy=\"" << head.offsetY << "\"/>" << std::endl; 
    }

    fd_data.close();
    fd_spec << "</icn>" << std::endl;
    fd_spec.close();
    std::cout << "expand to: " << prefix << std::endl;

    SDL::Quit();
    return EXIT_SUCCESS;
}

void SpriteDrawICNv1(Surface & sf, const u8* cur, const u32 size,  bool debug)
{
    const u8 *max = cur + size;

    u8  c = 0;
    u16 x = 0;
    u16 y = 0;

    u32 shadow = sf.MapRGB(RGBA(0, 0, 0, 0x40));
    u32 opaque = sf.MapRGB(RGBA(0, 0, 0, 0xff)); // non-transparent mask

    // lock surface
    sf.Lock();
    while(1)
    {
	if(debug)
	    std::cerr << "CMD:" << "0x" << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(*cur);

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
		sf.SetPixel(x, y, sf.MapRGB(H2Palette::GetColor(*cur)) | opaque);
		++x;
		++cur;
	    }
	}
	else
	// 0x80 - end data
	if(0x80 == *cur)
	{
	    if(debug)
		std::cerr << std::endl;

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
	    while(c--){ sf.SetPixel(x, y, sf.MapRGB(H2Palette::GetColor(*cur)) | opaque); ++x; }
	    ++cur;
	}
	else
	{
	    c = *cur - 0xC0;
	    ++cur;
	    while(c--){ sf.SetPixel(x, y, sf.MapRGB(H2Palette::GetColor(*cur)) | opaque); ++x; }
	    ++cur;
	}

	if(cur >= max)
	{
	    std::cerr << "out of range" << std::endl;
	    break;
	}

	if(debug)
	    std::cerr << std::endl;
    }

    // unlock surface
    sf.Unlock();
}

void SpriteDrawICNv2(Surface & sf, const u8* cur, const u32 size,  bool debug)
{
    const u8 *max = cur + size;

    u8  c = 0;
    u16 x = 0;
    u16 y = 0;

    u32 opaque = sf.MapRGB(RGBA(0, 0, 0, 0xff)); // non-transparent mask
    u32 shadow = sf.MapRGB(H2Palette::GetColor(1)) | opaque;

    // lock surface
    sf.Lock();
    while(1)
    {
	if(debug)
	    std::cerr << "CMD:" << "0x" << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(*cur);

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
	    while(c--)
	    {
		sf.SetPixel(x, y, shadow);
		++x;
	    }
	    ++cur;
	}
	else
	// 0x80 - end data
	if(0x80 == *cur)
	{
	    if(debug)
		std::cerr << std::endl;

	    break;
	}
	else
	// other - skip data
	{
	    x += *cur - 0x80;
	    ++cur;
	}

	if(cur >= max)
	{
	    std::cerr << "out of range" << std::endl;
	    break;
	}

	if(debug)
	    std::cerr << std::endl;
    }

    // unlock surface
    sf.Unlock();
}
