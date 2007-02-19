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
#include "mp2.h"
#include "text.h"
#include "rand.h"
#include "mapsdata.h"

u16 MapsData::width = 0;
u16 MapsData::height = 0;

MapsData::MapsData(const std::string &filename)
{
    std::fstream fd(filename.c_str(), std::ios::in | std::ios::binary);

    if(! fd || fd.fail()) Error::Except("LoadMP2: " + filename + ", file not found.");

    // loading info
    display.Fill(0, 0, 0);
    TextBox(Rect(0, display.h()/2, display.w(), display.h()/2), "Maps Loading...", Font::BIG, true);
    display.Flip();

    AGG::PreloadObject("GROUND32.TIL");

    char byte8;
    u16  byte16;
    u32  byte32;
    
    // offset data
    fd.seekg(MP2OFFSETDATA - 2 * sizeof(u32), std::ios_base::beg);

    // width
    fd.read(reinterpret_cast<char *>(&byte32), sizeof(u32));
    SWAP32(byte32);
    width = byte32;

    // height
    fd.read(reinterpret_cast<char *>(&byte32), sizeof(u32));
    SWAP32(byte32);
    height = byte32;

    // seek to ADDONS block
    fd.ignore(width * height * SIZEOFMP2TILE);

    // count mp2addon_t
    fd.read(reinterpret_cast<char *>(&byte32), sizeof(u32));
    SWAP32(byte32);

    // read all addons
    std::vector<MP2::mp2addon_t> vec_mp2addons;

    for(unsigned int ii = 0; ii < byte32; ++ii){

	MP2::mp2addon_t mp2addon;

	fd.read(reinterpret_cast<char *>(&mp2addon.indexAddon), sizeof(u16));
	SWAP16(mp2addon.indexAddon);
	fd.read(&byte8, 1);
	mp2addon.objectNameN1 = byte8;
	fd.read(&byte8, 1);
	mp2addon.indexNameN1 = byte8;
	fd.read(&byte8, 1);
	mp2addon.quantityN = byte8;
	fd.read(&byte8, 1);
	mp2addon.objectNameN2 = byte8;
	fd.read(&byte8, 1);
	mp2addon.indexNameN2 = byte8;
	fd.read(reinterpret_cast<char *>(&mp2addon.uniqNumberN1), sizeof(u32));
	SWAP32(mp2addon.uniqNumberN1);
	fd.read(reinterpret_cast<char *>(&mp2addon.uniqNumberN2), sizeof(u32));
	SWAP32(mp2addon.uniqNumberN2);

	vec_mp2addons.push_back(mp2addon);
    }

    // offset data
    fd.seekg(MP2OFFSETDATA, std::ios_base::beg);

    // read all tiles
    std::vector<MP2::tile_t> vec_tiles;

    for(u16 ii = 0; ii < width * height; ++ii){

	MP2::tile_t stile;
	MP2::addon_t saddon;

	fd.read(reinterpret_cast<char *>(&stile.tile), sizeof(u16));
	SWAP16(stile.tile);

	fd.read(&byte8, 1);
	saddon.object1   = byte8;

	fd.read(&byte8, 1);
	saddon.index1    = byte8;

	fd.read(&byte8, 1);
	saddon.quantity1 = byte8;

	fd.read(&byte8, 1);
	stile.quantity2  = byte8;

	fd.read(&byte8, 1);
	saddon.object2   = byte8;

	fd.read(&byte8, 1);
	saddon.index2    = byte8;

	fd.read(&byte8, 1);
	stile.shape      = byte8;

	fd.read(&byte8, 1);
	stile.general    = byte8;

	stile.addons.push_back(saddon);

	// offset first addon
	fd.read(reinterpret_cast<char *>(&byte16), sizeof(u16));
	SWAP16(byte16);

	// load all addon for current tils
	while(byte16){

	    //--byte16;
	    if(vec_mp2addons.size() <= byte16){ Error::Warning("MapsData::MapsData: index addons out of range!"); break; }

	    saddon.object1	= vec_mp2addons[byte16].objectNameN1 * 2;
	    saddon.index1	= vec_mp2addons[byte16].indexNameN1;
	    saddon.quantity1	= vec_mp2addons[byte16].quantityN;
	    saddon.object2	= vec_mp2addons[byte16].objectNameN2;
	    saddon.index2	= vec_mp2addons[byte16].indexNameN2;

	    stile.addons.push_back(saddon);

	    byte16 = vec_mp2addons[byte16].indexAddon;
	}

	std::sort(stile.addons.begin(), stile.addons.end(), MP2::VectorAddonSort);

	fd.ignore(sizeof(u32));
	fd.ignore(sizeof(u32));

	vec_tiles.push_back(stile);
    }

    fd.close();

    //

    tiles = new Surface(width * TILEWIDTH, height * TILEWIDTH, 8, SDL_SWSURFACE);
    tiles->LoadPalette(AGG::GetPalette());

    // MapsData
    Point pt;
    u32 ii = 0;
    u32 size = vec_tiles.size();

    Maps::tiles_t tl;
    for(; ii < size; ++ii){

        pt.x = ii % width;
        pt.y = ii / height;

	tl.coord	= pt;
	//tl.center	= Point();
	tl.ground	= MP2::GetTypeGrounds(vec_tiles[ii]);
	tl.object	= vec_tiles[ii].general;
	tl.level1	= NULL;
	tl.level2	= NULL;

	switch(tl.object){

	    case MP2::OBJ_RNDRESOURCE:
		tl.object = MP2::OBJ_RESOURCE;
	    case MP2::OBJ_RESOURCE:
		//ii
		//vec_tiles[ii].index1;
		//count
		break;

	    default:
		break;
	}

	// static sprite level1 and level2
	std::vector<MP2::addon_t>::const_iterator itaddon     = vec_tiles[ii].addons.begin();
	std::vector<MP2::addon_t>::const_iterator itaddon_end = vec_tiles[ii].addons.end();

	for(; itaddon != itaddon_end; ++itaddon){

	    if((*itaddon).object1 && MP2::StaticObject((*itaddon).object1, (*itaddon).index1)){

		if(NULL == tl.level1) tl.level1 = new std::vector<const Sprite *>;

		tl.level1->push_back(&AGG::GetICN(MP2::GetICNObject((*itaddon).object1), (*itaddon).index1));
	    }

	    if((*itaddon).object2 && MP2::StaticObject((*itaddon).object2, (*itaddon).index2)){

		if(NULL == tl.level2) tl.level2 = new std::vector<const Sprite *>;

		tl.level2->push_back(&AGG::GetICN(MP2::GetICNObject((*itaddon).object2), (*itaddon).index2));
	    }
	}

	vec_mapstiles.push_back(tl);

	
	// blit tiles to global tiles
	pt.x *= TILEWIDTH;
	pt.y *= TILEWIDTH;

	Surface til(TILEWIDTH, TILEWIDTH, 8, SDL_SWSURFACE);
	til.LoadPalette(AGG::GetPalette());
	AGG::GetTIL("GROUND32.TIL", vec_tiles[ii].tile, vec_tiles[ii].shape, til);

	tiles->Blit(til, pt);
    }

    AGG::FreeObject("GROUND32.TIL");

    tiles->SetDisplayFormat();

    // save maps to big sprite
    //if(H2Config::Debug() && tiles->SaveBMP("maps.bmp")) Error::Verbose("debug maps: save sprite: maps.bmp");
}

MapsData::~MapsData()
{
    delete tiles;

    std::vector<Maps::tiles_t>::const_iterator itl = vec_mapstiles.begin();
    for(; itl != vec_mapstiles.end(); ++itl){
	if((*itl).level1) delete (*itl).level1;
	if((*itl).level2) delete (*itl).level2;
    }

}

void MapsData::Redraw(const Rect &rt, const Point &pt) const
{
    if(pt.x < 0 || pt.y < 0 || pt.x >= GameArea::GetRect().w || pt.y >= GameArea::GetRect().h){ Error::Warning("MapsData::Redraw: out of range"); return; }

    // static level 0
    Rect  srcrt(rt.x * TILEWIDTH, rt.y * TILEWIDTH, rt.w * TILEWIDTH, rt.h * TILEWIDTH);
    Point dstpt(BORDERWIDTH + pt.x * TILEWIDTH, BORDERWIDTH + pt.y * TILEWIDTH);

    display.Blit(*tiles, srcrt, dstpt);

    // static level 1
    std::vector<Maps::tiles_t>::const_iterator itm = vec_mapstiles.begin();
    std::vector<Maps::tiles_t>::const_iterator itm_end = vec_mapstiles.end();

    for(; itm != itm_end; ++itm) if((rt & (*itm).coord) && (*itm).level1 && (*itm).level1->size()){

	std::vector<const Sprite *>::const_iterator its     = (*itm).level1->begin();
	std::vector<const Sprite *>::const_iterator its_end = (*itm).level1->end();

	for(; its != its_end; ++its)
	    display.Blit(**its,
		BORDERWIDTH + ((*itm).coord.x - rt.x) * TILEWIDTH + (*its)->x(),
		BORDERWIDTH + ((*itm).coord.y - rt.y) * TILEWIDTH + (*its)->y());
    }

    // dinamic object
    
    // animation level 1
    
    // heroes
    
    // static level 2
    itm = vec_mapstiles.begin();

    for(; itm != itm_end; ++itm) if((rt & (*itm).coord) && (*itm).level2 && (*itm).level2->size()){

	std::vector<const Sprite *>::const_iterator its     = (*itm).level2->begin();
	std::vector<const Sprite *>::const_iterator its_end = (*itm).level2->end();

	for(; its != its_end; ++its)
	    display.Blit(**its,
		BORDERWIDTH + ((*itm).coord.x - rt.x) * TILEWIDTH + (*its)->x(),
		BORDERWIDTH + ((*itm).coord.y - rt.y) * TILEWIDTH + (*its)->y());
    }

    // animation level 2

}

/* movement on maps */
bool MapsData::Movement(u16 index) const
{
    if(index >= vec_mapstiles.size()) return false; 

    switch(vec_mapstiles[index].object){

        case MP2::OBJ_STONES:
        case MP2::OBJ_OILLAKE:
        case MP2::OBJ_BIGCRACK:
        case MP2::OBJ_MOUNTS:
        case MP2::OBJ_TREES:
        case MP2::OBJ_FIRTREES:
        case MP2::OBJN_WAGONCAMP:
        case MP2::OBJN_SAWMILL:
        case MP2::OBJN_MINES:
        case MP2::OBJ_WATERLAKE:
        case MP2::OBJN_ALCHEMYTOWER:
        case MP2::OBJN_EXCAVATION:
        case MP2::OBJN_FORT:
        case MP2::OBJN_DRAGONCITY:
	    return false;
	
	default:
	    break;
    }
    
    return true;
}

