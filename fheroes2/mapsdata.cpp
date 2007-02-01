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
#include "agg.h"
#include "game.h"
#include "mp2.h"
#include "error.h"
#include "mapsdata.h"

MapsData::MapsData(const std::string &filename) : Rect(0, 0, 0, 0), tiles()
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

    w = width;
    h = height;

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

    tiles = SDLmm::Surface::CreateSurface(SDL_SWSURFACE, w * TILEWIDTH, h * TILEWIDTH, DEFAULT_DEPTH);
    if(!tiles.valid()) Error::Except(SDLmm::GetError());

    std::vector<MP2::tile_t>::const_iterator it     = mp2tile.begin();
    std::vector<MP2::tile_t>::const_iterator it_end = mp2tile.end();

    // fill Maps::Data
    Point pt;
    u32 ii = 0;

    while(it != it_end){

        pt.x = ii % width;
        pt.y = ii / height;

	pt.x *= TILEWIDTH;
	pt.y *= TILEWIDTH;

	Sprite * tile = AGG::GetTIL("GROUND32.TIL", (*it).tileIndex, (*it).shape);
	tiles.Blit(*tile, pt);
	
	delete tile;

	vec_tiles.push_back(MapsTiles(pt, *it));
	++ii;
	++it;
    }
    //tiles.SetDisplayFormat();
    tiles.SaveBMP("screenshot.bmp");
}

/*
u16 Maps::GetTypeGround(MP2::tile_t info){
    u16 index = info.tileIndex;

    // сканируем дорогу ROAD
    if(0x7A == info.objectName1) return Maps::ROAD;

    u16 indexAddon = info->indexAddon;
    MP2ADDONTAIL        *ptrAddon = NULL;

    while(indexAddon){

        ptrAddon = GetADDONTAIL(indexAddon);

        if(0x7A == ptrAddon->objectNameN1 * 2) return Maps::ROAD;

        indexAddon = ptrAddon->indexAddon;
    }


    // список поверхностей по индексу из GROUND32.TIL
    if(30 > index)
        return Maps::WATER;

    else if(92 > index)
        return Maps::GRASS;

    else if(146 > index)
        return Maps::SNOW;

    else if(208 > index)
        return Maps::SWAMP;

    else if(262 > index)
        return Maps::LAVA;

    else if(321 > index)
        return Maps::DESERT;

    else if(361 > index)
        return Maps::DIRT;

    else if(415 > index)
        return Maps::WASTELAND;

    //else if(432 > index)
    return Maps::BEACH;
}
*/
