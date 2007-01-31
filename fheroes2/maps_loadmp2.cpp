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


#include <fstream>
#include <vector>
#include "error.h"
#include "config.h"
#include "mp2.h"
#include "maps.h"

void Maps::LoadMP2(const std::string &filename)
{
    std::fstream fd(filename.c_str(), std::ios::in | std::ios::binary);

    if(! fd || fd.fail()) Error::Except("LoadMP2: " + filename + ", file not found.");

    u32 width, height;
    // offset data
    fd.seekg(MP2OFFSETDATA - 2 * sizeof(u32), std::ios_base::beg);
    // width
    fd.read(reinterpret_cast<char *>(&width), sizeof(u32));
    // height
    fd.read(reinterpret_cast<char *>(&height), sizeof(u32));

    char byte8;
    std::vector<MP2::tile_t> mp2tile;
    MP2::tile_t tile;

    for(unsigned int ii = 0; ii < width * height; ++ii){

	fd.read(reinterpret_cast<char *>(&tile.tileIndex), sizeof(u16));
	fd.read(&byte8, 1);
	tile.objectName1 = byte8;
	fd.read(&byte8, 1);
	tile.indexName1 = byte8;
	fd.read(&byte8, 1);
	tile.quantity1 = byte8;
	fd.read(&byte8, 1);
	tile.quantity2 = byte8;
	fd.read(&byte8, 1);
	tile.objectName2 = byte8;
	fd.read(&byte8, 1);
	tile.indexName2 = byte8;
	fd.read(&byte8, 1);
	tile.shape = byte8;
	fd.read(&byte8, 1);
	tile.generalObject = byte8;
	fd.read(reinterpret_cast<char *>(&tile.indexAddon), sizeof(u16));
	fd.read(reinterpret_cast<char *>(&tile.uniqNumber1), sizeof(u32));
	fd.read(reinterpret_cast<char *>(&tile.uniqNumber2), sizeof(u32));

	mp2tile.push_back(tile);
    }

    u32 countAddons;
    std::vector<MP2::addons_t> mp2addons;
    MP2::addons_t addons;

    // count addons_t
    fd.read(reinterpret_cast<char *>(&countAddons), sizeof(u32));

    for(unsigned int ii = 0; ii < countAddons; ++ii){

	fd.read(reinterpret_cast<char *>(&addons.indexAddon), sizeof(u16));
	fd.read(&byte8, 1);
	addons.objectNameN1 = byte8;
	fd.read(&byte8, 1);
	addons.indexNameN1 = byte8;
	fd.read(&byte8, 1);
	addons.quantityN = byte8;
	fd.read(&byte8, 1);
	addons.objectNameN2 = byte8;
	fd.read(&byte8, 1);
	addons.indexNameN2 = byte8;
	fd.read(reinterpret_cast<char *>(&addons.uniqNumberN1), sizeof(u32));
	fd.read(reinterpret_cast<char *>(&addons.uniqNumberN2), sizeof(u32));

	mp2addons.push_back(addons);
    }

    fd.close();
}
