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
#include "SDL.h"
#include "types.h"

class wavhead
{
    public:
    wavhead(u32 sz) : riff(0x46464952), size(sz + 0x24),
	wave(0x45564157), fmt(0x20746D66), size_t(0x10), format(0x01),
	channels(0x01), samples(22050), byteper(channels * samples * 0x08 / 8),
	align(channels * 0x08 / 8), bitsper(0x08), data(0x61746164), size_d(sz)
	{
	    // "RIFF"
	    SwapLE32(riff);
	    SwapLE32(size);
	    // "WAVE"
	    SwapLE32(wave);
	    // " fmt"
	    SwapLE32(fmt);
	    SwapLE32(size_t);
	    SwapLE16(format);
	    SwapLE16(channels);
	    SwapLE32(samples);
	    SwapLE32(byteper);
	    SwapLE16(align);
	    SwapLE16(bitsper);
	    // "data"
	    SwapLE32(data);
	    SwapLE32(size_d);
	};

    void write(std::fstream & fd)
    {
	if(fd.fail()) return;
	
	fd.write(reinterpret_cast<char *>(& riff), sizeof(u32));
	fd.write(reinterpret_cast<char *>(& size), sizeof(u32));
	fd.write(reinterpret_cast<char *>(& wave), sizeof(u32));
	fd.write(reinterpret_cast<char *>(& fmt), sizeof(u32));
	fd.write(reinterpret_cast<char *>(& size_t), sizeof(u32));
	fd.write(reinterpret_cast<char *>(& format), sizeof(u16));
	fd.write(reinterpret_cast<char *>(& channels), sizeof(u16));
	fd.write(reinterpret_cast<char *>(& samples), sizeof(u32));
	fd.write(reinterpret_cast<char *>(& byteper), sizeof(u32));
	fd.write(reinterpret_cast<char *>(& align), sizeof(u16));
	fd.write(reinterpret_cast<char *>(& bitsper), sizeof(u16));
	fd.write(reinterpret_cast<char *>(& data), sizeof(u32));
	fd.write(reinterpret_cast<char *>(& size_d), sizeof(u32));
    };

    private:
    u32 riff;
    u32 size;
    u32 wave;
    u32 fmt;
    u32 size_t;
    u16 format;
    u16 channels;
    u32 samples;
    u32 byteper;
    u16 align;
    u16 bitsper;
    u32 data;
    u32 size_d;
};
            
int main(int argc, char **argv)
{
    if(argc != 3)
    {
	std::cout << argv[0] << " infile.82m outfile.wav" << std::endl;

	return EXIT_SUCCESS;
    }

    std::fstream fd_data(argv[1], std::ios::in | std::ios::binary);

    if(fd_data.fail())
    {
	std::cout << "error open file: " << argv[1] << std::endl;

	return EXIT_SUCCESS;
    }

    fd_data.seekg(0, std::ios_base::end);
    u32 size = fd_data.tellg();
    fd_data.seekg(0, std::ios_base::beg);

    char *body = new char[size];

    fd_data.read(body, size);
    fd_data.close();

    wavhead head(size);

    std::fstream fd_body(argv[2], std::ios::out | std::ios::binary);

    if(! fd_body.fail())
    {
	head.write(fd_body);

    	fd_body.write(body, size);
	fd_body.close();
    }

    delete [] body;

    return EXIT_SUCCESS;
}
