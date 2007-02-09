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
#include "gamearea.h"
#include "maps.h"
#include "mp2.h"
#include "text.h"
#include "mapsdata.h"

u16 MapsData::width = 0;
u16 MapsData::height = 0;

MapsData::MapsData(const std::string &filename)
{
    std::fstream fd(filename.c_str(), std::ios::in | std::ios::binary);

    if(! fd || fd.fail()) Error::Except("LoadMP2: " + filename + ", file not found.");
    AGG::PreloadObject("GROUND32.TIL");

    u32 w, h;
    // offset data
    fd.seekg(MP2OFFSETDATA - 2 * sizeof(u32), std::ios_base::beg);
    // width
    fd.read(reinterpret_cast<char *>(&w), sizeof(u32));
    // height
    fd.read(reinterpret_cast<char *>(&h), sizeof(u32));

    width = w;
    height = h;

    char byte8;
    std::vector<MP2::tile_t> mp2tile;
    MP2::tile_t tile;

    for(u16 ii = 0; ii < width * height; ++ii){

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

    tiles = new Surface(width * TILEWIDTH, height * TILEWIDTH, 8, SDL_SWSURFACE);
    tiles->LoadPalette(AGG::GetPalette());

    // loading info
    display.Fill(0, 0, 0);
    TextBox(Rect(0, display.h()/2, display.w(), display.h()/2), "Maps Loading...", Font::BIG, true);
    display.Flip();

    // fill Maps::Data
    Point pt;
    u32 ii = 0;
    u32 size = mp2tile.size();

    mapstiles_t tl;
    while(ii < size){

        const MP2::tile_t cell = mp2tile[ii]; 

        pt.x = ii % width;
        pt.y = ii / height;

	pt.x *= TILEWIDTH;
	pt.y *= TILEWIDTH;

	Surface til(TILEWIDTH, TILEWIDTH, 8, SDL_SWSURFACE);
	til.LoadPalette(AGG::GetPalette());
	AGG::GetTIL("GROUND32.TIL", cell.tileIndex, cell.shape, til);

	tiles->Blit(til, pt);

	// static level 1

	//mp2tile[ii]
	tl.area		= Point();
	tl.center	= Point();
	tl.ground	= Maps::GetTypeGrounds(mp2tile[ii], mp2addons);
	tl.object	= mp2tile[ii].generalObject;

	vec_tiles.push_back(MapsTiles(tl));
	++ii;
    }

    AGG::FreeObject("GROUND32.TIL");

    tiles->SetDisplayFormat();

    // save maps to big sprite
    //if(H2Config::Debug() && tiles->SaveBMP("maps.bmp")) Error::Verbose("debug maps: save sprite: maps.bmp");
}

void MapsData::Redraw(const Rect &rt, const Point &pt) const
{
    if(pt.x < 0 || pt.y < 0 || pt.x >= GameArea::GetRect().w || pt.y >= GameArea::GetRect().h){ Error::Warning("MapsData::Redraw: out of range"); return; }

    // static level 0
    Rect  srcrt(rt.x * TILEWIDTH, rt.y * TILEWIDTH, rt.w * TILEWIDTH, rt.h * TILEWIDTH);
    Point dstpt(BORDERWIDTH + pt.x * TILEWIDTH, BORDERWIDTH + pt.y * TILEWIDTH);

    display.Blit(*tiles, srcrt, dstpt);

    // static level 1
    
    // dinamic object
    
    // animation level 1
    
    // heroes
    
    // static level 2

    // animation level 2

}
