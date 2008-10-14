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

#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

/*
#if defined(_WINDOWS) || defined(_WIN32) || defined(__WIN32__)
#include <io.h>
#define MKDIR(X)    mkdir(X)
#else
#define MKDIR(X)    mkdir(X, S_IRWXU)
#endif
*/

#include <iostream>
#include <fstream>
#include <list>
#include <cstring>
#include <cctype>

#include "types.h"

#define AGGSIZENAME	15

struct aggfat_t
{
    aggfat_t() : size(0), ptr(NULL){};

    u32 size;
    char *ptr;
};
            
int main(int argc, char **argv)
{
    if(argc != 3)
    {
	std::cout << argv[0] << " source_dir new_source.agg" << std::endl;

	return EXIT_SUCCESS;
    }

    // read directory
    DIR *dp;
    struct dirent *ep;
    struct stat fs;
    const std::string sourcedir(argv[1]);
    dp = opendir(argv[1]);

    u32 size = 0;
    //std::list<char *> names;
    std::list<aggfat_t> fats;

    if(dp)
    {
        while(NULL != (ep = readdir(dp)))
        {
            const std::string fullname(sourcedir + SEPARATOR + ep->d_name);

            // if not regular file
            if(stat(fullname.c_str(), &fs) || !S_ISREG(fs.st_mode)) continue;

            fats.push_back(aggfat_t());

            aggfat_t & fat = fats.back();
	    fat.size = fs.st_size;
	    fat.ptr = new char[AGGSIZENAME];

            size += fs.st_size;

            std::memset(fat.ptr, '\0', AGGSIZENAME);
            std::strlen(ep->d_name) < AGGSIZENAME ? std::strcpy(fat.ptr, ep->d_name) : std::memcpy(fat.ptr, ep->d_name, AGGSIZENAME - 1);
	}
	closedir(dp);
    }

    std::fstream fd(argv[2], std::ios::out | std::ios::binary);

    if(fd.fail())
    {
	std::cout << "error open file: " << argv[2] << std::endl;

	return EXIT_FAILURE;
    }

    // write count
    const u16 count = fats.size();
    fd.write(reinterpret_cast<const char *>(&count), 2);

    // write fat1
    u16 offset = 2 + 6 * count;
    std::list<aggfat_t>::const_iterator it1 = fats.begin();
    std::list<aggfat_t>::const_iterator it2 = fats.end();
    for(; it1 != it2; ++it1)
    {
        const aggfat_t & fat = *it1;
        u32 crc = 0; // TODO: fix crc or ?
	fd.write(reinterpret_cast<const char *>(&crc), 4);
	fd.write(reinterpret_cast<const char *>(&offset), 4);
	fd.write(reinterpret_cast<const char *>(&fat.size), 4);
	offset += fat.size;
    }

    // write data
    it1 = fats.begin();
    it2 = fats.end();
    for(; it1 != it2; ++it1)
    {
	const std::string fullname(sourcedir + SEPARATOR + (*it1).ptr);
        const aggfat_t & fat = *it1;

	std::fstream fdin(fullname.c_str(), std::ios::in | std::ios::binary);

	char *buf = new char [fat.size];
	std::memset(buf, '\0', fat.size);

	if(fdin.fail())
	    std::cout << "error read file: " << fullname << ", skipping..." << std::endl;
	else
	{
	    fdin.read(buf, fat.size);
	    fdin.close();
	}
	fd.write(buf, fat.size);
	delete [] buf;
    }

    // write fat2
    it1 = fats.begin();
    it2 = fats.end();
    for(; it1 != it2; ++it1) fd.write((*it1).ptr, AGGSIZENAME);

    fd.close();

    it1 = fats.begin();
    it2 = fats.end();

    for(; it1 != it2; ++it1) delete [] (*it1).ptr;

    return EXIT_SUCCESS;
}
