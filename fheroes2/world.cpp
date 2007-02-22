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
#include "world.h"

World World::world;

void World::LoadMaps(const std::string &filename)
{
    std::fstream fd(filename.c_str(), std::ios::in | std::ios::binary);

    if(! fd || fd.fail()) Error::Except("LoadMP2: " + filename + ", file not found.");

    // playing kingdom
    kingdom[0].color = Color::BLUE;
    kingdom[0].play = (Color::BLUE & H2Config::GetKingdomColors() ? true : false);
    kingdom[1].color = Color::GREEN;
    kingdom[1].play = (Color::GREEN & H2Config::GetKingdomColors() ? true : false);
    kingdom[2].color = Color::RED;
    kingdom[2].play = (Color::RED & H2Config::GetKingdomColors() ? true : false);
    kingdom[3].color = Color::YELLOW;
    kingdom[3].play = (Color::YELLOW & H2Config::GetKingdomColors() ? true : false);
    kingdom[4].color = Color::ORANGE;
    kingdom[4].play = (Color::ORANGE & H2Config::GetKingdomColors() ? true : false);
    kingdom[5].color = Color::PURPLE;
    kingdom[5].play = (Color::PURPLE & H2Config::GetKingdomColors() ? true : false);
    kingdom[6].play = false;

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
    std::vector<MP2::tile_t> vec_mp2tiles;

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
	    if(vec_mp2addons.size() <= byte16){ Error::Warning("World::World: index addons out of range!"); break; }

	    saddon.object1	= vec_mp2addons[byte16].objectNameN1 * 2;
	    saddon.index1	= vec_mp2addons[byte16].indexNameN1;
	    saddon.quantity1	= vec_mp2addons[byte16].quantityN;
	    saddon.object2	= vec_mp2addons[byte16].objectNameN2;
	    saddon.index2	= vec_mp2addons[byte16].indexNameN2;

	    stile.addons.push_back(saddon);

	    byte16 = vec_mp2addons[byte16].indexAddon;
	}

	stile.index1 = stile.addons[0].index1;

	std::sort(stile.addons.begin(), stile.addons.end(), MP2::VectorAddonSort);

	fd.ignore(sizeof(u32));
	fd.ignore(sizeof(u32));

	vec_mp2tiles.push_back(stile);
    }

    // read castle
    
    // read heroes
    
    // read events
    
    fd.close();

    //
    sprite_maps = new Surface(width * TILEWIDTH, height * TILEWIDTH, 8, SDL_SWSURFACE);
    sprite_maps->LoadPalette(AGG::GetPalette());

    // World
    Point pt;
    u32 ii = 0;
    u32 size = vec_mp2tiles.size();

    Maps::tiles_t tl;
    for(; ii < size; ++ii){

        pt.x = ii % width;
        pt.y = ii / height;

	tl.coord	= pt;
	//tl.center	= Point();
	tl.ground	= MP2::GetTypeGrounds(vec_mp2tiles[ii]);
	tl.object	= vec_mp2tiles[ii].general;
	tl.level1	= NULL;
	tl.level2	= NULL;

	switch(tl.object){

	    case MP2::OBJ_RNDRESOURCE:
		tl.object = MP2::OBJ_RESOURCE;
	    case MP2::OBJ_RESOURCE:
/*
		switch(vec_mp2tiles[ii].index1){
            	    case 1: tl.resource = Resource::WOOD; break;
		    case 3: tl.resource = Resource::MERCURY; break;
            	    case 5: tl.resource = Resource::ORE; break;
                    case 7: tl.resource = Resource::SULFUR; break;
                    case 9: tl.resource = Resource::CRYSTAL; break;
                    case 11: tl.resource = Resource::GEMS; break;
                    case 13: tl.resource = Resource::GOLD; break;
                    case 17: tl.resource = Resource::Rand(); break;
		    default:
			Error::Warning("unknown resource: ", vec_mp2tiles[ii].index1);
		    break;
                    //case 15: tl.resource = Maps::LAMP; break;
                    //case 19: tl.resource = Maps::CHEST; break;
		}
		break;
*/
	    default:
		break;
	}

	// static sprite level1 and level2
	std::vector<MP2::addon_t>::const_iterator itaddon     = vec_mp2tiles[ii].addons.begin();
	std::vector<MP2::addon_t>::const_iterator itaddon_end = vec_mp2tiles[ii].addons.end();

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

	vec_tiles.push_back(tl);

	
	// blit tiles to global tiles
	pt.x *= TILEWIDTH;
	pt.y *= TILEWIDTH;

	Surface til(TILEWIDTH, TILEWIDTH, 8, SDL_SWSURFACE);
	til.LoadPalette(AGG::GetPalette());
	AGG::GetTIL("GROUND32.TIL", vec_mp2tiles[ii].tile, vec_mp2tiles[ii].shape, til);

	sprite_maps->Blit(til, pt);
    }

    AGG::FreeObject("GROUND32.TIL");

    sprite_maps->SetDisplayFormat();

    // save maps to big sprite
    //if(H2Config::Debug() && sprite_maps->SaveBMP("maps.bmp")) Error::Verbose("debug maps: save sprite: maps.bmp");
}

World::~World()
{
    if(sprite_maps) delete sprite_maps;

    std::vector<Maps::tiles_t>::const_iterator itl = vec_tiles.begin();
    for(; itl != vec_tiles.end(); ++itl){
	if((*itl).level1) delete (*itl).level1;
	if((*itl).level2) delete (*itl).level2;
    }

}

/* end day */
void World::NextDay(void)
{
    ++World::day;

    if(!(World::day % DAYOFWEEK)) ++World::week;
    if(!(World::week % WEEKOFMONTH)) ++World::month;

    World::begin_week = (World::day % DAYOFWEEK ? false : true);
    World::begin_month = (World::week % WEEKOFMONTH ? false : true);
}

/* set new date */
void World::ResetDate(void)
{
    World::day = 0;
    World::week = 0;
    World::month = 0;
    World::begin_week = true;
    World::begin_month = true;
}

void World::Redraw(const Rect &rt, const Point &pt) const
{
    if(pt.x < 0 || pt.y < 0 || pt.x >= GameArea::GetRect().w || pt.y >= GameArea::GetRect().h){ Error::Warning("World::Redraw: out of range"); return; }

    // static level 0
    Rect  srcrt(rt.x * TILEWIDTH, rt.y * TILEWIDTH, rt.w * TILEWIDTH, rt.h * TILEWIDTH);
    Point dstpt(BORDERWIDTH + pt.x * TILEWIDTH, BORDERWIDTH + pt.y * TILEWIDTH);

    display.Blit(*sprite_maps, srcrt, dstpt);

    // static level 1
    std::vector<Maps::tiles_t>::const_iterator itm = vec_tiles.begin();
    std::vector<Maps::tiles_t>::const_iterator itm_end = vec_tiles.end();

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
    itm = vec_tiles.begin();

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
bool World::Movement(u16 index) const
{
    if(index >= vec_tiles.size()) return false; 

    switch(vec_tiles[index].object){

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

const Kingdom & World::GetKingdom(Color::color_t color)
{
    switch(color){
    
	case Color::BLUE:
	    return kingdom[0];
	
	case Color::GREEN:
	    return kingdom[1];

	case Color::RED:
	    return kingdom[2];
	
	case Color::YELLOW:
	    return kingdom[3];
	
	case Color::ORANGE:
	    return kingdom[4];
	
	case Color::PURPLE:
	    return kingdom[5];
	
	default:
	    return kingdom[6];
    }
}
