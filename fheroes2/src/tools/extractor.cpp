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
#include <fstream>
#include <vector>
#include <cctype>
#include "SDL.h"
#include "types.h"

#define AGGSIZENAME	15

typedef struct
{
    std::string name;
    u32  offset;
    u32  size;

} aggfat_t;
            
int main(int argc, char **argv)
{
    if(argc != 3)
    {
	std::cout << argv[0] << " path_heroes2.agg extract_to_dir" << std::endl;

	return EXIT_SUCCESS;
    }

    std::fstream fd_data(argv[1], std::ios::in | std::ios::binary);

    if(fd_data.fail())
    {
	std::cout << "error open file: " << argv[1] << std::endl;

	return EXIT_SUCCESS;
    }

    MKDIR(argv[2]);

    u16 count;
    u16 total = 0;
    fd_data.read(reinterpret_cast<char *>(&count), sizeof(u16));
    SwapLE16(count);

    char buf[AGGSIZENAME];
    std::vector<aggfat_t> vector(count);

    for(u16 cur = 0; cur < count; ++cur)
    {
	aggfat_t & fat = vector[cur];

	fd_data.seekg(-AGGSIZENAME * (count - cur), std::ios_base::end);
        fd_data.read(buf, AGGSIZENAME);

        for(u8 ii = 0; ii < AGGSIZENAME; ++ii) buf[ii] = tolower(buf[ii]);

        fat.name = buf;

	fd_data.seekg(sizeof(u16) + cur * (3 * sizeof(u32)), std::ios_base::beg);
	fd_data.seekg(sizeof(u32), std::ios_base::cur);

        fd_data.read(reinterpret_cast<char *>(&fat.offset), sizeof(u32));
	SwapLE32(fat.offset);

        fd_data.read(reinterpret_cast<char *>(&fat.size), sizeof(u32));
        SwapLE32(fat.size);

	fd_data.seekg(fat.offset, std::ios_base::beg);

	char *body = new char[fat.size];
        fd_data.read(body, fat.size);

	std::string full_name(argv[2]);
	full_name += SEPARATOR;
	full_name += fat.name;

	std::fstream fd_body(full_name.c_str(), std::ios::out | std::ios::binary);

	if(! fd_body.fail())
	{
    	    fd_body.write(body, fat.size);
	    fd_body.close();
	    
	    ++total;

	    std::cout << "extract: " << full_name << std::endl;
	}

	delete [] body;
    }

    fd_data.close();

    std::cout << "total: " << total << std::endl;

    return EXIT_SUCCESS;
}
