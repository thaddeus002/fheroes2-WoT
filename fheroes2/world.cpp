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
#include "agg.h"
#include "game.h"
#include "gamearea.h"
#include "mp2.h"
#include "text.h"
#include "rand.h"
#include "world.h"

World::World(const std::string &filename) : sprite_maps(NULL), day(1), week(1), month(1), begin_week(true), begin_month(true)
{
    std::fstream fd(filename.c_str(), std::ios::in | std::ios::binary);

    if(! fd || fd.fail()) Error::Except("LoadMP2: " + filename + ", file not found.");

    // playing kingdom
    kingdom[Color::BLUE]   = Kingdom(Color::BLUE & H2Config::GetKingdomColors() ? true : false);
    kingdom[Color::GREEN]  = Kingdom(Color::GREEN & H2Config::GetKingdomColors() ? true : false);
    kingdom[Color::RED]    = Kingdom(Color::RED & H2Config::GetKingdomColors() ? true : false);
    kingdom[Color::YELLOW] = Kingdom(Color::YELLOW & H2Config::GetKingdomColors() ? true : false);
    kingdom[Color::ORANGE] = Kingdom(Color::ORANGE & H2Config::GetKingdomColors() ? true : false);
    kingdom[Color::PURPLE] = Kingdom(Color::PURPLE & H2Config::GetKingdomColors() ? true : false);
    kingdom[Color::GRAY]   = Kingdom(false);

    // loading info
    display.Fill(0, 0, 0);
    TextBox(Rect(0, display.h()/2, display.w(), display.h()/2), "Maps Loading...", Font::BIG, true);
    display.Flip();

    AGG::PreloadObject("GROUND32.TIL");

    char byte8;
    u16  byte16;
    u32  byte32;
    std::vector<u16> vec_object; // index maps for OBJ_CASTLE, OBJ_HEROES, OBJ_SIGN, OBJ_BOTTLE, OBJ_EVENT
    
    // endof
    fd.seekg(0, std::ios_base::end);
    const u32 endof = fd.tellg();

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

	switch(stile.general){
	    case MP2::OBJ_RNDTOWN:
	    case MP2::OBJ_RNDCASTLE:
	    case MP2::OBJ_CASTLE:
	    case MP2::OBJ_HEROES:
	    case MP2::OBJ_SIGN:
	    case MP2::OBJ_BOTTLE:
	    case MP2::OBJ_EVENT:
	    case MP2::OBJ_SPHINX:
		vec_object.push_back(ii);
		break;
	    default:
		break;
	}

	stile.addons.push_back(saddon);

	// offset first addon
	fd.read(reinterpret_cast<char *>(&byte16), sizeof(u16));
	SWAP16(byte16);

	// load all addon for current tils
	while(byte16){

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
	stile.quantity1 = stile.addons[0].quantity1;

	std::sort(stile.addons.begin(), stile.addons.end(), MP2::VectorAddonSort);

	fd.ignore(sizeof(u32));
	fd.ignore(sizeof(u32));

	vec_mp2tiles.push_back(stile);
    }

    // count addon
    fd.read(reinterpret_cast<char *>(&byte32), sizeof(u32));
    SWAP32(byte32);
    
    // skip addons
    fd.ignore(byte32 * SIZEOFMP2ADDON);

    // cood castles
    // 72 x 3 byte (cx, cy, id)
    for(u8 ii = 0; ii < 72; ++ii){

	char cx, cy, id;

	fd.read(&cx, 1);
	fd.read(&cy, 1);
	fd.read(&id, 1);
	
	// empty block
	if(-1 == cx && -1 == cy) continue;

	switch(id){
	    case 0x00: break; // tower: knight
	    case 0x01: break; // tower: barbarian
	    case 0x02: break; // tower: sorceress
	    case 0x03: break; // tower: warlock
	    case 0x04: break; // tower: wizard
	    case 0x05: break; // tower: necromancer
	    case 0x06: break; // tower: random
	    case 0x80: break; // castle: knight
	    case 0x81: break; // castle: barbarian
	    case 0x82: break; // castle: sorceress
	    case 0x83: break; // castle: warlock
	    case 0x84: break; // castle: wizard
	    case 0x85: break; // castle: necromancer
	    case 0x86: break; // castle: random
	    default: Error::Warning("World::World: castle block, unknown id: ", id); Error::Verbose(Point(cx, cy)); break;
	}
    }

    // cood resource kingdoms
    // 144 x 3 byte (cx, cy, id)
    for(u8 ii = 0; ii < 144; ++ii){

	char cx, cy, id;

	fd.read(&cx, 1);
	fd.read(&cy, 1);
	fd.read(&id, 1);
	
	// empty block
	if(-1 == cx && -1 == cy) continue;

	switch(id){
	    case 0x00: break; // mines: wood
	    case 0x01: break; // mines: mercury
 	    case 0x02: break; // mines: ore
	    case 0x03: break; // mines: sulfur
	    case 0x04: break; // mines: crystal
	    case 0x05: break; // mines: gems
	    case 0x06: break; // mines: gold
	    case 0x64: break; // lighthouse
	    case 0x65: break; // dragon city
	    case 0x67: break; // abandoned mines
	    default: Error::Warning("World::World: kingdom block, unknown id: ", id); Error::Verbose(Point(cx, cy)); break;	
	}
    }
    
    // unknown byte
    fd.ignore();

    // count final mp2 blocks
    u16 countblock = 0;
    byte16 = 0xffff;
    while(byte16){

	// debug
	if(endof < fd.tellg()) Error::Except("World::World: read maps: out of range.");

	fd.read(reinterpret_cast<char *>(&byte16), sizeof(u16));
	SWAP16(byte16);
	if(byte16) countblock = byte16 - 1;
    }
    
    // castle or heroes or (events, rumors, etc)
    for(u16 ii = 0; ii < countblock; ++ii){

	// debug
	if(endof < fd.tellg()) Error::Except("World::World: read maps: out of range.");

	// size block
	u16 sizeblock;
	fd.read(reinterpret_cast<char *>(&sizeblock), sizeof(u16));
	SWAP16(sizeblock);

	char *pblock = new char[sizeblock + 1];
	// end string '\0'
	pblock[sizeblock] = 0;

	// read block
	fd.read(pblock, sizeblock);

	std::vector<u16>::const_iterator it_index = vec_object.begin();
	bool findobject = false;

	while(it_index != vec_object.end()){

	    // orders(quantity2, quantity1)
	    u16 orders = (vec_mp2tiles[*it_index].quantity2 ? vec_mp2tiles[*it_index].quantity2 : 0);
	    orders <<= 8;
	    orders |= static_cast<u16>(vec_mp2tiles[*it_index].quantity1);
	    
	    if(orders && !(orders % 0x08) && (ii + 1 == orders / 0x08)){ findobject = true; break; }

	    ++it_index;
	}
	
	if(findobject){
	    switch(vec_mp2tiles[*it_index].general){
		case MP2::OBJ_CASTLE:
		case MP2::OBJ_RNDTOWN:
		case MP2::OBJ_RNDCASTLE:
		    // add castle
		    //if(SIZEOFMP2CASTLE == sizeblock)
		    Error::Verbose("add castle");
		    vec_mp2tiles[*it_index].general = MP2::OBJ_CASTLE;
		    break;
		case MP2::OBJ_HEROES:
		    // add heroes
		    //if(SIZEOFMP2HEROES == sizeblock)
		    Error::Verbose("add heroes");
		    break;
		case MP2::OBJ_SIGN:
		case MP2::OBJ_BOTTLE:
		    // add sign or buttle
		    if(SIZEOFMP2SIGN - 1 < sizeblock && 0x0001 == *reinterpret_cast<u16 *>(pblock))
				    sign.push_back(GameEvent::Sign(*it_index, &pblock[9]));
		    break;
		case MP2::OBJ_EVENT:
		    // add event maps
		    if(SIZEOFMP2EVENT - 1 < sizeblock && 0x0001 == *reinterpret_cast<u16 *>(pblock))
				    event_coord.push_back(GameEvent::Coord(*it_index, pblock));
		    break;
		case MP2::OBJ_SPHINX:
		    // add riddle sphinx
		    if(SIZEOFMP2RIDDLE - 1 < sizeblock && 0x0000 == *reinterpret_cast<u16 *>(pblock))
				    riddle.push_back(GameEvent::Riddle(*it_index, pblock));
		    break;
		default:
		    break;
	    }
	}
	// other events
	else if(0x0000 == *reinterpret_cast<u16 *>(pblock)){

	    // add event day
	    if(SIZEOFMP2EVENT - 1 < sizeblock && (0 == pblock[32] || 1 == pblock[32]))
		event_day.push_back(GameEvent::Day(pblock));

	    // add rumors
	    else if(SIZEOFMP2RUMOR - 1 < sizeblock)
	    {
		std::string message(&pblock[8]);
		rumors.push_back(message);
		Error::Verbose("add Rumors: " + message);
	    }
	}
	// debug
	else Error::Warning("World::World: read maps: unknown block addons.");

	delete [] pblock;
    }

    // last rumors
    rumors.push_back("This game is now in alpha development version.");

    // close mp2
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

	    // resource
	    case MP2::OBJ_RNDRESOURCE:
		tl.object = MP2::OBJ_RESOURCE;
	    case MP2::OBJ_RESOURCE:
		tl.resource = Resource::FromMP2(vec_mp2tiles[ii].index1);
		break;

	    // ultimate artifact
    	    case MP2::OBJ_RNDULTIMATEARTIFACT:
		//tl.artifact = Artifact::FromMP2(vec_mp2tiles[ii].index1);
		break;

	    // artifact
    	    case MP2::OBJ_RNDARTIFACT:
    	    case MP2::OBJ_RNDARTIFACT1:
    	    case MP2::OBJ_RNDARTIFACT2:
    	    case MP2::OBJ_RNDARTIFACT3:
		tl.object = MP2::OBJ_ARTIFACT;
    	    case MP2::OBJ_ARTIFACT:
		tl.artifact = Artifact::FromMP2(vec_mp2tiles[ii].index1);
		break;


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
    ++day;

    if(!(day % DAYOFWEEK)) ++week;
    if(!(week % WEEKOFMONTH)) ++month;

    begin_week = (day % DAYOFWEEK ? false : true);
    begin_month = (week % WEEKOFMONTH ? false : true);
}

void World::Redraw(const Rect &rt, const Point &pt) const
{
    if(pt.x < 0 || pt.y < 0 || pt.x >= GameArea::GetRect().w || pt.y >= GameArea::GetRect().h){ Error::Warning("World::Redraw: out of range"); return; }

    // static level 0 (tiles)
    //
    Rect  srcrt(rt.x * TILEWIDTH, rt.y * TILEWIDTH, rt.w * TILEWIDTH, rt.h * TILEWIDTH);
    Point dstpt(BORDERWIDTH + pt.x * TILEWIDTH, BORDERWIDTH + pt.y * TILEWIDTH);

    display.Blit(*sprite_maps, srcrt, dstpt);

    // static level 1 (relief and other static builds)
    //
    std::vector<Maps::tiles_t>::const_iterator itm = vec_tiles.begin();
    const std::vector<Maps::tiles_t>::const_iterator itm_end = vec_tiles.end();

    for(; itm != itm_end; ++itm) if((rt & (*itm).coord) && (*itm).level1 && (*itm).level1->size()){

	std::vector<const Sprite *>::const_iterator its     = (*itm).level1->begin();
	std::vector<const Sprite *>::const_iterator its_end = (*itm).level1->end();

	u16 offset_x = BORDERWIDTH + ((*itm).coord.x - rt.x) * TILEWIDTH;
	u16 offset_y = BORDERWIDTH + ((*itm).coord.y - rt.y) * TILEWIDTH;

	for(; its != its_end; ++its) display.Blit(**its, offset_x + (*its)->x(), offset_y + (*its)->y());
    }

    // animation level 1
    //

    // dinamic object level 1
    //
    itm = vec_tiles.begin();

    for(; itm != itm_end; ++itm) if((rt & (*itm).coord)){

	u16 offset_x = BORDERWIDTH + ((*itm).coord.x - rt.x) * TILEWIDTH;
	u16 offset_y = BORDERWIDTH + ((*itm).coord.y - rt.y) * TILEWIDTH;

	switch((*itm).object){

	    // draw resource
	    case MP2::OBJ_RESOURCE:
		{
		    const Sprite & sprite = Resource::GetForMapsSprite((*itm).resource);
		    display.Blit(sprite, offset_x + sprite.x(), offset_y + sprite.y());
		}
		break;

	    // draw artifact
    	    case MP2::OBJ_ARTIFACT:
		{
		    const Sprite & sprite = Artifact::GetForMapsSprite((*itm).artifact);
		    display.Blit(sprite, offset_x + sprite.x(), offset_y + sprite.y());
		}
		break;

	    // draw other resource (lamp, campfire, sea resource e.t.c.)
	    case MP2::OBJ_TREASURECHEST:
	    case MP2::OBJ_ANCIENTLAMP:
	    case MP2::OBJ_CAMPFIRE:
	    case MP2::OBJ_SHIPWRECKSURVIROR:
	    case MP2::OBJ_FLOTSAM:
	    case MP2::OBJ_BOTTLE:
		break;
	    
	    // draw army monster
    	    case MP2::OBJ_MONSTER:
		break;
	    
	    default:
		break;
	}
    }
    
    // heroes, castle and other.....
    //
    
    // all flags
    //

    // static level 2
    //
    itm = vec_tiles.begin();

    for(; itm != itm_end; ++itm) if((rt & (*itm).coord) && (*itm).level2 && (*itm).level2->size()){

	std::vector<const Sprite *>::const_iterator its     = (*itm).level2->begin();
	std::vector<const Sprite *>::const_iterator its_end = (*itm).level2->end();

	u16 offset_x = BORDERWIDTH + ((*itm).coord.x - rt.x) * TILEWIDTH;
	u16 offset_y = BORDERWIDTH + ((*itm).coord.y - rt.y) * TILEWIDTH;

	for(; its != its_end; ++its) display.Blit(**its, offset_x + (*its)->x(), offset_y + (*its)->y());
    }

    // animation level 2
    //

}

/* movement on maps */
bool World::Movement(u16 index) const
{
    if(index >= vec_tiles.size()) return false; 

    switch(vec_tiles[index].object){

        case MP2::OBJ_STONES:
        case MP2::OBJ_OILLAKE:
        case MP2::OBJ_CRATER:
        case MP2::OBJ_MOUNTS:
        case MP2::OBJ_TREES:
        case MP2::OBJN_WAGONCAMP:
        case MP2::OBJN_SAWMILL:
        case MP2::OBJN_MINES:
        case MP2::OBJ_WATERLAKE:
	case MP2::OBJN_TROLLBRIDGE:
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

const Kingdom & World::GetKingdom(Color::color_t color) const
{
    std::map<Color::color_t, Kingdom>::const_iterator it = kingdom.find(color);
    
    if(it == kingdom.end()) Error::Except("World::GetKingdom: unknown.");
    
    return it->second;
}
