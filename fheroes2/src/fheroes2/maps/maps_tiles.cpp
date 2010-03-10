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

#include <list>
#include <iostream>
#include <algorithm>
#include "agg.h"
#include "world.h"
#include "race.h"
#include "settings.h"
#include "heroes.h"
#include "castle.h"
#include "maps.h"
#include "interface_gamearea.h"
#include "game_focus.h"
#include "object.h"
#include "objxloc.h"
#include "monster.h"
#include "resource.h"
#include "maps_tiles.h"

Maps::TilesAddon::TilesAddon() : level(0), uniq(0), object(0), index(0)
{
}

Maps::TilesAddon::TilesAddon(u8 lv, u32 gid, u8 obj, u8 ii) : level(lv), uniq(gid), object(obj), index(ii)
{
}

Maps::TilesAddon & Maps::TilesAddon::operator= (const Maps::TilesAddon & ta)
{
    level = ta.level;

    object = ta.object;

    index = ta.index;

    uniq = ta.uniq;
    
    return *this;
}

bool Maps::TilesAddon::PredicateSortRules1(const Maps::TilesAddon & ta1, const Maps::TilesAddon & ta2)
{
    return ((ta1.level % 4) > (ta2.level % 4));
}

bool Maps::TilesAddon::PredicateSortRules2(const Maps::TilesAddon & ta1, const Maps::TilesAddon & ta2)
{
    return ((ta1.level % 4) < (ta2.level % 4));
}

u16 Maps::TilesAddon::isRoad(const TilesAddon & ta)
{
    switch(MP2::GetICNObject(ta.object))
    {
	// castle and tower (gate)
	case ICN::OBJNTOWN:
	    return (13 == ta.index ||
		    29 == ta.index ||
	    	    45 == ta.index ||
	    	    61 == ta.index ||
	    	    77 == ta.index ||
	    	    93 == ta.index ||
	    	    109 == ta.index ||
	    	    125 == ta.index ||
	    	    141 == ta.index ||
	    	    157 == ta.index ||
	    	    173 == ta.index ||
	    	    189 == ta.index ? Direction::TOP | Direction::BOTTOM : 0);

	// castle lands (gate)
        case ICN::OBJNTWBA:
	    return ( 7 == ta.index ||
		    17 == ta.index ||
		    27 == ta.index ||
		    37 == ta.index ||
		    47 == ta.index ||
		    57 == ta.index ||
		    67 == ta.index ||
		    77 == ta.index ? Direction::TOP | Direction::BOTTOM : 0);

	// from sprite road
	case ICN::ROAD:
	    if(0  == ta.index ||
	       4  == ta.index ||
	       5  == ta.index ||
	       7  == ta.index ||
	       9  == ta.index ||
	       12 == ta.index ||
	       13 == ta.index ||
	       16 == ta.index ||
	       19 == ta.index ||
	       20 == ta.index ||
	       26 == ta.index)	return Direction::TOP | Direction::BOTTOM;
	    else
	    if(2  == ta.index ||
	       21 == ta.index ||
	       28 == ta.index)	return Direction::RIGHT | Direction::LEFT;
	    else
	    if(3  == ta.index ||
	       6  == ta.index ||
	       14 == ta.index)	return Direction::TOP | Direction::BOTTOM | Direction::RIGHT | Direction::LEFT;
	    else
	    if(17 == ta.index ||
	       29 == ta.index)	return Direction::TOP_LEFT | Direction::BOTTOM_RIGHT;
	    else
	    if(18 == ta.index ||
	       30 == ta.index)	return Direction::TOP_RIGHT | Direction::BOTTOM_LEFT;
	
	default:
	    break;
    }

    return 0;
}

bool Maps::TilesAddon::isStream(const TilesAddon & ta)
{
    return ICN::STREAM == MP2::GetICNObject(ta.object);
}

Maps::Tiles::Tiles(u16 index) : maps_index(index), tile_index(0),
    shape(0), general(0), quantity1(0), quantity2(0),
    fogs(0xFF), flags(REDRAW)
{
}

Maps::Tiles::Tiles(u16 mi, const MP2::mp2tile_t & mp2tile) : maps_index(mi), tile_index(mp2tile.tileIndex),
    shape(mp2tile.shape), general(mp2tile.generalObject), quantity1(mp2tile.quantity1), quantity2(mp2tile.quantity2),
    quantity3(0), quantity4(0), fogs(0xFF), flags(REDRAW)
{
    AddonsPushLevel1(mp2tile);
    AddonsPushLevel2(mp2tile);

    if(IS_DEVEL()) ClearFog(Settings::Get().MyColor());
}

void Maps::Tiles::SetTile(const u16 index, const u8 sh)
{
    tile_index = index;
    shape = sh;
}

const Surface & Maps::Tiles::GetTileSurface(void) const
{
    return AGG::GetTIL(TIL::GROUND32, tile_index, shape);
}

void Maps::Tiles::AddonsPushLevel1(const MP2::mp2tile_t & mt)
{
    if(mt.objectName1 && mt.indexName1 < 0xFF)
	addons_level1.push_back(TilesAddon(0, mt.uniqNumber1, mt.objectName1, mt.indexName1));
}

void Maps::Tiles::AddonsPushLevel1(const MP2::mp2addon_t & ma)
{
    if(ma.objectNameN1 && ma.indexNameN1 < 0xFF)
	addons_level1.push_back(TilesAddon(ma.quantityN, ma.uniqNumberN1, ma.objectNameN1, ma.indexNameN1));
}

void Maps::Tiles::AddonsPushLevel1(const TilesAddon & ta)
{
    addons_level1.push_back(ta);
}

void Maps::Tiles::AddonsPushLevel2(const MP2::mp2tile_t & mt)
{
    if(mt.objectName2 && mt.indexName2 < 0xFF)
	addons_level2.push_back(TilesAddon(0, mt.uniqNumber2, mt.objectName2, mt.indexName2));
}

void Maps::Tiles::AddonsPushLevel2(const MP2::mp2addon_t & ma)
{
    if(ma.objectNameN2 && ma.indexNameN2 < 0xFF)
	addons_level2.push_back(TilesAddon(ma.quantityN, ma.uniqNumberN2, ma.objectNameN2, ma.indexNameN2));
}

void Maps::Tiles::AddonsPushLevel2(const TilesAddon & ta)
{
    addons_level2.push_back(ta);
}

void Maps::Tiles::AddonsSort(void)
{
    if(addons_level1.size()) addons_level1.sort(Maps::TilesAddon::PredicateSortRules1);

    if(addons_level2.size()) addons_level2.sort(Maps::TilesAddon::PredicateSortRules2);
}

Maps::Ground::ground_t Maps::Tiles::GetGround(void) const
{
    // list grounds from GROUND32.TIL
    if(30 > tile_index)
        return Maps::Ground::WATER;
        
    else if(92 > tile_index)
        return Maps::Ground::GRASS;

    else if(146 > tile_index)
        return Maps::Ground::SNOW;

    else if(208 > tile_index)
        return Maps::Ground::SWAMP;

    else if(262 > tile_index)
        return Maps::Ground::LAVA;

    else if(321 > tile_index)
        return Maps::Ground::DESERT;

    else if(361 > tile_index)
        return Maps::Ground::DIRT;

    else if(415 > tile_index)
        return Maps::Ground::WASTELAND;

    //else if(432 > tile_index)

	return Maps::Ground::BEACH;
}

void Maps::Tiles::Remove(u32 uniq)
{
    if(addons_level1.size())
    {
	std::list<TilesAddon>::iterator       it1 = addons_level1.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level1.end();

	while(it1 != it2)
	    if(uniq == (*it1).uniq)
		it1 = addons_level1.erase(it1);
	    else
		++it1;
    }

    if(addons_level2.size())
    {
	std::list<TilesAddon>::iterator       it1 = addons_level2.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level2.end();

	while(it1 != it2)
	    if(uniq == (*it1).uniq)
		it1 = addons_level2.erase(it1);
	    else
		++it1;
    }
}

void Maps::Tiles::RedrawTile(void) const
{
    const Interface::GameArea & area = Interface::GameArea::Get();
    const Point mp(maps_index % world.w(), maps_index / world.w());

    if(area.GetRectMaps() & mp)
    {
	const s16 & dstx = area.GetArea().x + TILEWIDTH * (mp.x - area.GetRectMaps().x);
	const s16 & dsty = area.GetArea().y + TILEWIDTH * (mp.y - area.GetRectMaps().y);

	RedrawTile(Display::Get(), dstx, dsty);
    }
}

void Maps::Tiles::RedrawTile(Surface & dst, s16 dstx, s16 dsty) const
{
    dst.Blit(GetTileSurface(), dstx, dsty);
}

void Maps::Tiles::RedrawBottom(void) const
{
    RedrawBottom(Display::Get(), Interface::GameArea::Get());
}

void Maps::Tiles::RedrawBottom(Surface & dst, const Interface::GameArea & area) const
{
    const Point mp(maps_index % world.w(), maps_index / world.w());

    if(area.GetRectMaps() & mp)
    {
	const s16 & dstx = area.GetArea().x + TILEWIDTH * (mp.x - area.GetRectMaps().x);
	const s16 & dsty = area.GetArea().y + TILEWIDTH * (mp.y - area.GetRectMaps().y);

	RedrawBottom(dst, dstx, dsty);
    }
}

void Maps::Tiles::RedrawBottom(Surface & dst, s16 dstx, s16 dsty, const TilesAddon* skip) const
{
    if(addons_level1.size())
    {
	std::list<TilesAddon>::const_iterator it1 = addons_level1.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level1.end();

	for(; it1 != it2; ++it1)
	{
	    if(skip && skip == &(*it1)) continue;

	    const u8 & object = (*it1).object;
	    const u8 & index  = (*it1).index;
	    const ICN::icn_t icn = MP2::GetICNObject(object);

	    if(ICN::UNKNOWN != icn && ICN::MINIHERO != icn && ICN::MONS32 != icn)
	    {
		const Sprite & sprite = AGG::GetICN(icn, index);
		dst.Blit(sprite, dstx + sprite.x(), dsty + sprite.y());

		// possible anime
		if(const u16 anime_index = ICN::AnimationFrame(icn, index, Maps::AnimationTicket(), quantity2))
		{
		    const Sprite & anime_sprite = AGG::GetICN(icn, anime_index);
		    dst.Blit(anime_sprite, dstx + anime_sprite.x(), dsty + anime_sprite.y());
		}
	    }
	}
    }
}

void Maps::Tiles::RedrawObjects(void) const
{
    const Point mp(maps_index % world.w(), maps_index / world.w());
    const Interface::GameArea & area = Interface::GameArea::Get();

    if(area.GetRectMaps() & mp)
    {
	const s16 & dstx = area.GetArea().x + TILEWIDTH * (mp.x - area.GetRectMaps().x);
	const s16 & dsty = area.GetArea().y + TILEWIDTH * (mp.y - area.GetRectMaps().y);

	RedrawObjects(Display::Get(), dstx, dsty, area);
    }
}

void Maps::Tiles::RedrawObjects(Surface & dst, s16 px, s16 py, const Interface::GameArea & area) const
{
    switch(GetObject())
    {
        // boat
        case MP2::OBJ_BOAT:	RedrawBoat(dst, px, py, area); break;
        // monster
        case MP2::OBJ_MONSTER:	RedrawMonster(dst, px, py, area); break;
	//
	default: break;
    }
}

void Maps::Tiles::RedrawMonster(Surface & dst, s16 px, s16 py, const Interface::GameArea & area) const
{
    const Monster monster(*this);
    Point dst_pt;
    Rect src_rt;
    u16 dst_index = MAXU16;

    // draw attack sprite
    if(Maps::ScanAroundObject(maps_index, MP2::OBJ_HEROES, Settings::Get().OriginalVersion(), &dst_index))
    {
	bool revert = false;

	switch(Direction::Get(maps_index, dst_index))
	{
	    case Direction::TOP_LEFT:
	    case Direction::LEFT:
	    case Direction::BOTTOM_LEFT:	revert = true;
	    default: break;
	}

	const Sprite & sprite_first = AGG::GetICN(ICN::MINIMON, monster.GetSpriteIndex() * 9 + (revert ? 8 : 7));

	dst_pt.x = px + sprite_first.x() + 16;
	dst_pt.y = py + TILEWIDTH + sprite_first.y();

	area.SrcRectFixed(src_rt, dst_pt, sprite_first.w(), sprite_first.h());
	dst.Blit(sprite_first, src_rt, dst_pt);
    }
    else
    {
	// draw first sprite
	const Sprite & sprite_first = AGG::GetICN(ICN::MINIMON, monster.GetSpriteIndex() * 9);

	dst_pt.x = px + sprite_first.x() + 16;
	dst_pt.y = py + TILEWIDTH + sprite_first.y();

	area.SrcRectFixed(src_rt, dst_pt, sprite_first.w(), sprite_first.h());
	dst.Blit(sprite_first, src_rt, dst_pt);

	// draw second sprite
	const Sprite & sprite_next = AGG::GetICN(ICN::MINIMON, monster.GetSpriteIndex() * 9 + 1 + (Maps::AnimationTicket() % 6));

	dst_pt.x = px + sprite_next.x() + 16;
	dst_pt.y = py + TILEWIDTH + sprite_next.y();

	area.SrcRectFixed(src_rt, dst_pt, sprite_next.w(), sprite_next.h());
	dst.Blit(sprite_next, src_rt, dst_pt);

	//if(Maps::isValidDirection(tile.GetIndex(), Direction::BOTTOM))
    	//    world.GetTiles(Maps::GetDirectionIndex(tile.GetIndex(), Direction::BOTTOM)).RedrawTop();
    }
}

void Maps::Tiles::RedrawBoat(Surface & dst, s16 px, s16 py, const Interface::GameArea & area) const
{
    if(Settings::Get().Editor())
        dst.Blit(AGG::GetICN(ICN::OBJNWAT2, 23), px, py);
    else
    {
        // FIXME: restore direction from Maps::Tiles
        const Sprite & sprite = AGG::GetICN(ICN::BOAT32, 18);
        Point dst_pt(px + sprite.x(), py + sprite.y() + TILEWIDTH);
        Rect src_rt;
        area.SrcRectFixed(src_rt, dst_pt, sprite.w(), sprite.h());
        dst.Blit(sprite, src_rt, dst_pt);
    }
}

void Maps::Tiles::RedrawBottom4Hero(Surface & dst, const Interface::GameArea & area) const
{
    const Point mp(maps_index % world.w(), maps_index / world.w());

    if(!(area.GetRectMaps() & mp)) return;

    const s16 & dstx = area.GetArea().x + TILEWIDTH * (mp.x - area.GetRectMaps().x);
    const s16 & dsty = area.GetArea().y + TILEWIDTH * (mp.y - area.GetRectMaps().y);

    if(addons_level1.size())
    {
	std::list<TilesAddon>::const_iterator it1 = addons_level1.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level1.end();

	for(; it1 != it2; ++it1)
	{
	    const u8 & object = (*it1).object;
	    const u8 & index  = (*it1).index;
	    const ICN::icn_t icn = MP2::GetICNObject(object);

	    if(ICN::SkipBottomForRedrawHeroes(icn, index)) continue;

	    if(ICN::UNKNOWN != icn && ICN::MINIHERO != icn && ICN::MONS32 != icn)
	    {
		const Sprite & sprite = AGG::GetICN(icn, index);
		dst.Blit(sprite, dstx + sprite.x(), dsty + sprite.y());

		// possible anime
		if(const u16 anime_index = ICN::AnimationFrame(icn, index, Maps::AnimationTicket(), quantity2))
		{
		    const Sprite & anime_sprite = AGG::GetICN(icn, anime_index);
		    dst.Blit(anime_sprite, dstx + anime_sprite.x(), dsty + anime_sprite.y());
		}
	    }
	}
    }
}

void Maps::Tiles::RedrawTop(void) const
{
    RedrawTop(Display::Get(), Interface::GameArea::Get());
}

void Maps::Tiles::RedrawTop(Surface & dst, const Interface::GameArea & area) const
{
    const Point mp(maps_index % world.w(), maps_index / world.w());

    if(area.GetRectMaps() & mp)
    {
	const s16 & dstx = area.GetArea().x + TILEWIDTH * (mp.x - area.GetRectMaps().x);
	const s16 & dsty = area.GetArea().y + TILEWIDTH * (mp.y - area.GetRectMaps().y);

	RedrawTop(dst, dstx, dsty, area);
    }
}

void Maps::Tiles::RedrawTop(Surface & dst, s16 dstx, s16 dsty, const Interface::GameArea & gamearea, const TilesAddon* skip) const
{
    // fix for abandone mine
    if(MP2::OBJ_ABANDONEDMINE == general)
    {
	const Sprite & anime_sprite = AGG::GetICN(ICN::OBJNHAUN,  Maps::AnimationTicket() % 15);
	Rect rt;
	Point pt(dstx + anime_sprite.x(), dsty + anime_sprite.y());
	gamearea.SrcRectFixed(rt, pt, anime_sprite.w(), anime_sprite.h());
	dst.Blit(anime_sprite, rt, pt);
    }

    if(addons_level2.size())
    {
	std::list<TilesAddon>::const_iterator it1 = addons_level2.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level2.end();

	for(; it1 != it2; ++it1)
	{
	    if(skip && skip == &(*it1)) continue;

	    const u8 & object = (*it1).object;
	    const u8 & index  = (*it1).index;
	    const ICN::icn_t icn = MP2::GetICNObject(object);

	    if(ICN::UNKNOWN != icn && ICN::MINIHERO != icn && ICN::MONS32 != icn)
	    {
		const Sprite & sprite = AGG::GetICN(icn, index);
		    
		// fix flags redraw
		if(ICN::FLAG32 == icn)
		{
		    Rect rt;
		    Point pt(dstx + sprite.x(), dsty + sprite.y());
		    gamearea.SrcRectFixed(rt, pt, sprite.w(), sprite.h());
		    dst.Blit(sprite, rt, pt);
		}
		else
		    dst.Blit(sprite, dstx + sprite.x(), dsty + sprite.y());

		// possible anime
		if(const u16 anime_index = ICN::AnimationFrame(icn, index, Maps::AnimationTicket()))
		{
		    const Sprite & anime_sprite = AGG::GetICN(icn, anime_index);
		    dst.Blit(anime_sprite, dstx + anime_sprite.x(), dsty + anime_sprite.y());
		}
	    }
	}
    }
}

void Maps::Tiles::RedrawTop4Hero(Surface & dst, const Interface::GameArea & area, bool skip_ground) const
{
    const Point mp(maps_index % world.w(), maps_index / world.w());

    if(!(area.GetRectMaps() & mp)) return;

    const s16 & dstx = area.GetArea().x + TILEWIDTH * (mp.x - area.GetRectMaps().x);
    const s16 & dsty = area.GetArea().y + TILEWIDTH * (mp.y - area.GetRectMaps().y);

    if(addons_level2.size())
    {
	std::list<TilesAddon>::const_iterator it1 = addons_level2.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level2.end();

	for(; it1 != it2; ++it1)
	{
	    if(skip_ground && MP2::isGroundObject((*it1).object)) continue;

	    const u8 & object = (*it1).object;
	    const u8 & index  = (*it1).index;
	    const ICN::icn_t icn = MP2::GetICNObject(object);

	    if(ICN::UNKNOWN != icn)
	    {
		const Sprite & sprite = AGG::GetICN(icn, index);
		dst.Blit(sprite, dstx + sprite.x(), dsty + sprite.y());

		// possible anime
		if(const u16 anime_index = ICN::AnimationFrame(icn, index, Maps::AnimationTicket()))
		{
		    const Sprite & anime_sprite = AGG::GetICN(icn, anime_index);
		    dst.Blit(anime_sprite, dstx + anime_sprite.x(), dsty + anime_sprite.y());
		}
	    }
	}
    }
}

Maps::TilesAddon * Maps::Tiles::FindAddonICN1(u16 icn1)
{
    if(addons_level1.size())
    {
	std::list<TilesAddon>::iterator it1 = addons_level1.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level1.end();

	for(; it1 != it2; ++it1)
	{
	    TilesAddon & addon = *it1;

	    if(icn1 == MP2::GetICNObject(addon.object)) return &addon;
	}
    }

    return NULL;
}

Maps::TilesAddon * Maps::Tiles::FindAddonICN2(u16 icn2)
{
    if(addons_level2.size())
    {
	std::list<TilesAddon>::iterator it1 = addons_level2.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level2.end();

	for(; it1 != it2; ++it1)
	{
	    TilesAddon & addon = *it1;

	    if(icn2 == MP2::GetICNObject(addon.object)) return &addon;
	}
    }

    return NULL;
}

Maps::TilesAddon * Maps::Tiles::FindAddonLevel1(u32 uniq1)
{
    if(addons_level1.size())
    {
	std::list<TilesAddon>::iterator it1 = addons_level1.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level1.end();

	for(; it1 != it2; ++it1)
	{
	    TilesAddon & addon = *it1;

	    if(uniq1 == addon.uniq) return &addon;
	}
    }

    return NULL;
}

Maps::TilesAddon * Maps::Tiles::FindAddonLevel2(u32 uniq2)
{
    if(addons_level2.size())
    {
	std::list<TilesAddon>::iterator it1 = addons_level2.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level2.end();

	for(; it1 != it2; ++it1)
	{
	    TilesAddon & addon = *it1;

	    if(uniq2 == addon.uniq) return &addon;
	}
    }

    return NULL;
}

void Maps::Tiles::DebugInfo(void) const
{
    u16 res = 0;
    std::list<TilesAddon>::const_iterator it1;
    std::list<TilesAddon>::const_iterator it2;

    std::cout << std::endl << "----------------:--------" << std::endl;

    std::string value;

    String::AddInt(value, maps_index);

    std::cout << "maps index      : " << value << std::endl;
    
    value.clear();
    
    String::AddInt(value, tile_index);

    std::cout << "tile            : " << value << std::endl;
    
    value.clear();

    value = Ground::String(GetGround());
    if(isRoad()) value += ", (road)";
    std::cout << "ground          : " << value << std::endl;

    value.clear();

    value = isPassable() ? "true" : "false";
    std::cout << "passable        : " << value << std::endl;

    value.clear();
    
    String::AddInt(value, general);
    value += ", (" + std::string(MP2::StringObject(general)) + ")";
    std::cout << "general object  : " << value << std::endl;

    value.clear();
    
    String::AddInt(value, quantity1);
    std::cout << "quantity 1      : " << value << std::endl;

    value.clear();
    
    String::AddInt(value, quantity2);
    std::cout << "quantity 2      : " << value << std::endl;

    value.clear();

    String::AddInt(value, quantity3);
    std::cout << "quantity 3      : " << value << std::endl;

    value.clear();

    String::AddInt(value, quantity4);
    std::cout << "quantity 4      : " << value << std::endl;

    value.clear();

    if(addons_level1.size())
    {
	it1 = addons_level1.begin();
	it2 = addons_level1.end();
	
	for(; it1 != it2; ++it1)
	{
	    const TilesAddon & addon = *it1;

	    std::cout << "----------------1--------" << std::endl;

	    value.clear();
    
	    String::AddInt(value, addon.object);
    
	    std::cout << "object          : " << value << " (" << ICN::GetString(MP2::GetICNObject(addon.object)) << ")" << std::endl;

	    value.clear();
    
	    String::AddInt(value, addon.index);
    
	    std::cout << "index           : " << value << std::endl;

	    value.clear();
    
	    String::AddInt(value, addon.uniq);
    
	    std::cout << "uniq            : " << value << std::endl;

	    value.clear();
    
	    String::AddInt(value, addon.level);
    
	    std::cout << "level           : " << value << std::endl;
	}
    }

    if(addons_level2.size())
    {
	it1 = addons_level2.begin();
	it2 = addons_level2.end();
	
	for(; it1 != it2; ++it1)
	{
	    const TilesAddon & addon = *it1;

	    std::cout << "----------------2--------" << std::endl;

	    value.clear();
    
	    String::AddInt(value, addon.object);
    
	    std::cout << "object          : " << value << " (" << ICN::GetString(MP2::GetICNObject(addon.object)) << ")" << std::endl;

	    value.clear();
    
	    String::AddInt(value, addon.index);
    
	    std::cout << "index           : " << value << std::endl;

	    value.clear();
    
	    String::AddInt(value, addon.uniq);
    
	    std::cout << "uniq            : " << value << std::endl;

	    value.clear();
    
	    String::AddInt(value, addon.level);
    
	    std::cout << "level           : " << value << std::endl;
	}
    }

    std::cout << "----------------I--------" << std::endl;

    // extra obj info
    switch(general)
    {
	// dwelling
        case MP2::OBJ_RUINS:
        case MP2::OBJ_TREECITY:
        case MP2::OBJ_WAGONCAMP:
        case MP2::OBJ_DESERTTENT:
	case MP2::OBJ_TROLLBRIDGE:
        case MP2::OBJ_DRAGONCITY:
        case MP2::OBJ_CITYDEAD:
	//
        case MP2::OBJ_WATCHTOWER:
        case MP2::OBJ_EXCAVATION:
        case MP2::OBJ_CAVE:
        case MP2::OBJ_TREEHOUSE:
        case MP2::OBJ_ARCHERHOUSE:
        case MP2::OBJ_GOBLINHUT:
        case MP2::OBJ_DWARFCOTT:
        case MP2::OBJ_HALFLINGHOLE:
        case MP2::OBJ_PEASANTHUT:
        case MP2::OBJ_THATCHEDHUT:
	//
	case MP2::OBJ_MONSTER:
	    std::cout << "count           : " << GetCountMonster() << std::endl;
	    break;

	case MP2::OBJ_HEROES:
	    {
		const Heroes *hero = world.GetHeroes(maps_index);
		if(hero) hero->Dump();
	    }
	    break;

	case MP2::OBJN_CASTLE:
	case MP2::OBJ_CASTLE:
	    {
		const Castle *castle = world.GetCastle(maps_index);
		if(castle) castle->Dump();
	    }
	    break;

	default:
	    if(Maps::TileUnderProtection(maps_index, &res))
		std::cout << "protection      : " << res << std::endl;
	    break;
    }

    std::cout << "----------------:--------" << std::endl << std::endl;
}

MP2::object_t Maps::Tiles::GetObject(void) const
{
    return static_cast<MP2::object_t>(general);
}

/* accept move */
bool Maps::Tiles::isPassable(const Heroes *hero, bool skipfog) const
{
    if(!skipfog && isFog(Settings::Get().CurrentColor())) return false;

    if(hero)
    {
	if(hero->isShipMaster())
	{
    	    if(Ground::WATER != Maps::Tiles::GetGround()) return false;

    	    switch(general)
	    {
		case MP2::OBJ_BOAT:
        	case MP2::OBJ_HEROES:	return false;

		default: break;
	    }
	}
	else
	{
	    if(Ground::WATER == Maps::Tiles::GetGround()) return false;

    	    switch(general)
	    {
        	case MP2::OBJ_HEROES:	return false;

		default: break;
	    }

	}
    }

    return Object::isPassable(addons_level1);
}

/* check road */
bool Maps::Tiles::isRoad(const Direction::vector_t & direct) const
{
    switch(direct)
    {
	case Direction::UNKNOWN:
	case Direction::CENTER:	return addons_level1.end() != find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isRoad);

	default: break;
    }

    if(addons_level1.size())
    {
	std::list<TilesAddon>::const_iterator it1 = addons_level1.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level1.end();

	for(; it1 != it2; ++it1) if(direct & TilesAddon::isRoad(*it1)) return true;
    }

    return false;
}

bool Maps::Tiles::isStream(void) const
{
    return addons_level1.end() != std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isStream);
}

Maps::TilesAddon * Maps::Tiles::FindWaterResource(void)
{
    if(addons_level1.size())
    {
	std::list<TilesAddon>::iterator it1 = addons_level1.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level1.end();

	for(; it1 != it2; ++it1)
	{
	    TilesAddon & addon = *it1;

	    // OBJNWATR
	    if(ICN::OBJNWATR == MP2::GetICNObject(addon.object) && 
		(0 == addon.index ||	// buttle
		19 == addon.index ||	// chest
		45 == addon.index ||	// flotsam
		111 == addon.index))	// surviror
				return &addon;
	}
    }

    return NULL;
}

Maps::TilesAddon * Maps::Tiles::FindResource(void)
{
    if(addons_level1.size())
    {
	std::list<TilesAddon>::iterator it1 = addons_level1.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level1.end();

	for(; it1 != it2; ++it1)
	{
	    TilesAddon & addon = *it1;

	    // OBJNRSRC
	    if(ICN::OBJNRSRC == MP2::GetICNObject(addon.object) && (addon.index % 2)) return &addon;
	    else
	    // TREASURE
	    if(ICN::TREASURE == MP2::GetICNObject(addon.object)) return &addon;
	}
    }

    return NULL;
}

Maps::TilesAddon * Maps::Tiles::FindRNDResource(void)
{
    if(addons_level1.size())
    {
	std::list<TilesAddon>::iterator it1 = addons_level1.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level1.end();

	for(; it1 != it2; ++it1)
	{
	    TilesAddon & addon = *it1;

	    // OBJNRSRC
	    if(ICN::OBJNRSRC == MP2::GetICNObject(addon.object) && 17 == addon.index) return &addon;
	}
    }

    return NULL;
}

Maps::TilesAddon * Maps::Tiles::FindArtifact(void)
{
    if(addons_level1.size())
    {
	std::list<TilesAddon>::iterator it1 = addons_level1.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level1.end();

	for(; it1 != it2; ++it1)
	{
	    TilesAddon & addon = *it1;

	    // OBJNARTI
	    if(ICN::OBJNARTI == MP2::GetICNObject(addon.object) && (addon.index % 2)) return &addon;
	}
    }

    return NULL;
}

Maps::TilesAddon * Maps::Tiles::FindRNDArtifact(const u8 level)
{
    u8 index = 0xA3;

    switch(level)
    {
	case MP2::OBJ_RNDARTIFACT1: index = 0xA7; break;
	case MP2::OBJ_RNDARTIFACT2: index = 0xA9; break;
	case MP2::OBJ_RNDARTIFACT3: index = 0xAB; break;
	default: break;
    }

    if(addons_level1.size())
    {
	std::list<TilesAddon>::iterator it1 = addons_level1.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level1.end();

	for(; it1 != it2; ++it1)
	{
	    TilesAddon & addon = *it1;

	    // OBJNARTI
	    if(ICN::OBJNARTI == MP2::GetICNObject(addon.object) && index == addon.index) return &addon;
	}
    }

    return NULL;
}

Maps::TilesAddon * Maps::Tiles::FindUltimateArtifact(void)
{
    if(addons_level1.size())
    {
	std::list<TilesAddon>::iterator it1 = addons_level1.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level1.end();

	for(; it1 != it2; ++it1)
	{
	    TilesAddon & addon = *it1;

	    // OBJNARTI
	    if(ICN::OBJNARTI == MP2::GetICNObject(addon.object) && 0xA4 == addon.index) return &addon;
	}
    }

    return NULL;
}

Maps::TilesAddon * Maps::Tiles::FindMiniHero(void)
{
    if(addons_level1.size())
    {
	std::list<TilesAddon>::iterator it1 = addons_level1.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level1.end();

	for(; it1 != it2; ++it1)
	{
	    TilesAddon & addon = *it1;

	    // MINIHERO
	    if(ICN::MINIHERO == MP2::GetICNObject(addon.object)) return &addon;
	}
    }

    return NULL;
}

Maps::TilesAddon * Maps::Tiles::FindEvent(void)
{
    if(addons_level1.size())
    {
	std::list<TilesAddon>::iterator it1 = addons_level1.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level1.end();

	for(; it1 != it2; ++it1)
	{
	    TilesAddon & addon = *it1;

	    // OBJNMUL2
            if(ICN::OBJNMUL2 == MP2::GetICNObject(addon.object) && 0xA3 == addon.index) return &addon;
	}
    }

    return NULL;
}

Maps::TilesAddon * Maps::Tiles::FindBoat(void)
{
    if(addons_level1.size())
    {
	std::list<TilesAddon>::iterator it1 = addons_level1.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level1.end();

	for(; it1 != it2; ++it1)
	{
	    TilesAddon & addon = *it1;

	    // OBJNWAT2
            if(ICN::OBJNWAT2 == MP2::GetICNObject(addon.object) && 0x17 == addon.index) return &addon;
	}
    }

    return NULL;
}

Maps::TilesAddon * Maps::Tiles::FindCastle(void)
{
    if(addons_level1.size())
    {
	std::list<TilesAddon>::iterator it1 = addons_level1.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level1.end();

	for(; it1 != it2; ++it1)
	{
	    TilesAddon & addon = *it1;

	    // OBJNTOWN
            if(ICN::OBJNTOWN == MP2::GetICNObject(addon.object)) return &addon;
	}
    }

    if(addons_level2.size())
    {
	std::list<TilesAddon>::iterator it1 = addons_level2.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level2.end();

	for(; it1 != it2; ++it1)
	{
	    TilesAddon & addon = *it1;

	    // OBJNTOWN
            if(ICN::OBJNTOWN == MP2::GetICNObject(addon.object)) return &addon;
	}
    }

    return NULL;
}

Maps::TilesAddon * Maps::Tiles::FindRNDCastle(void)
{
    if(addons_level1.size())
    {
	std::list<TilesAddon>::iterator it1 = addons_level1.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level1.end();

	for(; it1 != it2; ++it1)
	{
	    TilesAddon & addon = *it1;

	    // OBJNTWRD
            if(ICN::OBJNTWRD == MP2::GetICNObject(addon.object) && 32 > addon.index) return &addon;
	}
    }

    if(addons_level2.size())
    {
	std::list<TilesAddon>::iterator it1 = addons_level2.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level2.end();

	for(; it1 != it2; ++it1)
	{
	    TilesAddon & addon = *it1;

	    // OBJNTWRD
            if(ICN::OBJNTWRD == MP2::GetICNObject(addon.object)  && 32 > addon.index) return &addon;
	}
    }

    return NULL;
}

Maps::TilesAddon * Maps::Tiles::FindFlags(void)
{
    if(addons_level1.size())
    {
	std::list<TilesAddon>::iterator it1 = addons_level1.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level1.end();

	for(; it1 != it2; ++it1)
	{
	    TilesAddon & addon = *it1;

	    // FLAG32
            if(ICN::FLAG32 == MP2::GetICNObject(addon.object)) return &addon;
	}
    }

    return NULL;
}

Maps::TilesAddon * Maps::Tiles::FindJail(void)
{
    if(addons_level1.size())
    {
	std::list<TilesAddon>::iterator it1 = addons_level1.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level1.end();

	for(; it1 != it2; ++it1)
	{
	    TilesAddon & addon = *it1;

	    if(ICN::X_LOC2 == MP2::GetICNObject(addon.object) && 0x09 == addon.index) return &addon;
	}
    }

    return NULL;
}

Maps::TilesAddon * Maps::Tiles::FindBarrier(void)
{
    if(addons_level1.size())
    {
	std::list<TilesAddon>::iterator it1 = addons_level1.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level1.end();

	for(; it1 != it2; ++it1)
	{
	    TilesAddon & addon = *it1;

	    if(ICN::X_LOC3 == MP2::GetICNObject(addon.object) && 
		(60 == addon.index ||
		66 == addon.index ||
		72 == addon.index ||
		78 == addon.index ||
		84 == addon.index ||
		90 == addon.index ||
		96 == addon.index ||
		102 == addon.index)) return &addon;
	}
    }

    return NULL;
}

Maps::TilesAddon * Maps::Tiles::FindRNDMonster(void)
{
    if(addons_level1.size())
    {
	std::list<TilesAddon>::iterator it1 = addons_level1.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level1.end();

	for(; it1 != it2; ++it1)
	{
	    TilesAddon & addon = *it1;

	    // MONS32
	    if(ICN::MONS32 == MP2::GetICNObject(addon.object) &&
	    (0x41 < addon.index && 0x47 > addon.index)) return &addon;
	}
    }

    return NULL;
}

Maps::TilesAddon * Maps::Tiles::FindMonster(void)
{
    if(addons_level1.size())
    {
	std::list<TilesAddon>::iterator it1 = addons_level1.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level1.end();

	for(; it1 != it2; ++it1)
	{
	    TilesAddon & addon = *it1;

	    // MONS32
	    if(ICN::MONS32 == MP2::GetICNObject(addon.object)) return &addon;
	}
    }

    return NULL;
}

Maps::TilesAddon * Maps::Tiles::FindCampFire(void)
{
    if(addons_level1.size())
    {
	std::list<TilesAddon>::iterator it1 = addons_level1.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level1.end();

	for(; it1 != it2; ++it1)
	{
	    TilesAddon & addon = *it1;

	    // MTNDSRT
            if(ICN::OBJNDSRT == MP2::GetICNObject(addon.object) && 61 == addon.index) return &addon;
	    else
	    // OBJNMULT
            if(ICN::OBJNMULT == MP2::GetICNObject(addon.object) && 131 == addon.index) return &addon;
	    else
	    // OBJNSNOW
            if(ICN::OBJNSNOW == MP2::GetICNObject(addon.object) && 4 == addon.index) return &addon;
	}
    }

    return NULL;
}

const Maps::TilesAddon * Maps::Tiles::FindMines(void) const
{
    if(addons_level1.size())
    {
	std::list<TilesAddon>::const_iterator it1 = addons_level1.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level1.end();

	for(; it1 != it2; ++it1)
	{
	    const TilesAddon & addon = *it1;

	    // EXTRAOVR
	    if(ICN::EXTRAOVR == MP2::GetICNObject(addon.object)) return &addon;
	}
    }

    return NULL;
}

/* ICN::FLAGS32 version */
void Maps::Tiles::CaptureFlags32(const MP2::object_t obj, const Color::color_t col)
{
    u8 index = 0;

    switch(col)
    {
	case Color::BLUE:	index = 0; break;
	case Color::GREEN:	index = 1; break;
	case Color::RED:	index = 2; break;
	case Color::YELLOW:	index = 3; break;
	case Color::ORANGE:	index = 4; break;
	case Color::PURPLE:	index = 5; break;
	case Color::GRAY:	index = 6; break;

	default: break;
    }

    switch(obj)
    {
	case MP2::OBJ_MINES:		index += 14; CorrectFlags32(index); break;
	//case MP2::OBJ_DRAGONCITY:	index += 35; CorrectFlags32(index); break; unused
        case MP2::OBJ_LIGHTHOUSE:	index += 42; CorrectFlags32(index); break;

	case MP2::OBJ_ALCHEMYLAB:
	{
	    index += 21;
	    if(Maps::isValidDirection(maps_index, Direction::TOP))
	    {
		Maps::Tiles & tile = world.GetTiles(Maps::GetDirectionIndex(maps_index, Direction::TOP));
		tile.CorrectFlags32(index);
	    }
	}
	break;

	case MP2::OBJ_SAWMILL:
	{
	    index += 28;
	    if(Maps::isValidDirection(maps_index, Direction::TOP_RIGHT))
    	    {
    		Maps::Tiles & tile = world.GetTiles(Maps::GetDirectionIndex(maps_index, Direction::TOP_RIGHT));
    		tile.CorrectFlags32(index);
	    }
	}
	break;

	case MP2::OBJ_CASTLE:
	{
	    index *= 2;
	    if(Maps::isValidDirection(maps_index, Direction::LEFT))
	    {
		Maps::Tiles & tile = world.GetTiles(Maps::GetDirectionIndex(maps_index, Direction::LEFT));
    		tile.CorrectFlags32(index);
	    }

	    index += 1;
	    if(Maps::isValidDirection(maps_index, Direction::RIGHT))
	    {
		Maps::Tiles & tile = world.GetTiles(Maps::GetDirectionIndex(maps_index, Direction::RIGHT));
    		tile.CorrectFlags32(index);
	    }
	}
	break;

	default: return;
    }
}

/* correct flags, ICN::FLAGS32 vesion */
void Maps::Tiles::CorrectFlags32(const u8 index)
{
    TilesAddon * taddon = FindFlags();

    // replace flag
    if(NULL != taddon)
	taddon->index = index;
    else
    // or new flag
	addons_level2.push_back(TilesAddon(TilesAddon::UPPER, world.GetUniq(), 0x38, index));
}

void Maps::Tiles::FixLoyaltyVersion(void)
{
    switch(general)
    {
	case MP2::OBJ_UNKNW_79:
        case MP2::OBJ_UNKNW_7A:
        case MP2::OBJ_UNKNW_F9:
        case MP2::OBJ_UNKNW_FA:
	{
	    std::list<TilesAddon>::iterator it1 = addons_level1.begin();
	    std::list<TilesAddon>::const_iterator it2 = addons_level1.end();

	    // check level1
	    for(; it1 != it2; ++it1)
	    {
		TilesAddon & addon = *it1;

		if(ICN::X_LOC1 == MP2::GetICNObject(addon.object) ||
		    ICN::X_LOC2 == MP2::GetICNObject(addon.object) ||
		    ICN::X_LOC3 == MP2::GetICNObject(addon.object))
		{
		    const u8 newobj = ObjLoyalty::LearnObject(addon);
		    if(MP2::OBJ_ZERO != newobj) general = newobj;
		    return;
		}
	    }

	    it1 = addons_level2.begin();
	    it2 = addons_level2.end();

	    // check level2
	    for(; it1 != it2; ++it1)
	    {
		TilesAddon & addon = *it1;

		if(ICN::X_LOC1 == MP2::GetICNObject(addon.object) ||
		    ICN::X_LOC2 == MP2::GetICNObject(addon.object) ||
		    ICN::X_LOC3 == MP2::GetICNObject(addon.object))
		{
		    const u8 newobj = ObjLoyalty::LearnObject(addon);
		    if(MP2::OBJ_ZERO != newobj) general = newobj;
		    return;
		}
	    }

	    DEBUG(DBG_GAME , DBG_WARN, "Maps::Tiles::FixLoyaltyVersion: index: " << maps_index);

	} break;

	default: break;
    }
}

/* for few object need many resource */
void Maps::Tiles::UpdateQuantity(void)
{
    const TilesAddon * addon = NULL;

    switch(general)
    {
        case MP2::OBJ_WITCHSHUT:
                quantity1 = Skill::Secondary::RandForWitchsHut();
	break;

	case MP2::OBJ_SHRINE1:
                quantity1 = Rand::Get(10) % 2 ? Spell::RandCombat(1) : Spell::RandAdventure(1);
            break;

        case MP2::OBJ_SHRINE2:
                quantity1 = Rand::Get(10) % 2 ? Spell::RandCombat(2) : Spell::RandAdventure(2);
            break;

        case MP2::OBJ_SHRINE3:
                quantity1 = Rand::Get(10) % 2 ? Spell::RandCombat(3) : Spell::RandAdventure(3);
            break;

	case MP2::OBJ_SKELETON:
	{
	    Rand::Queue percents(2);
	    // 80%: empty
	    percents.Push(0, 80);
	    // 20%: artifact 1 or 2 or 3
	    percents.Push(1, 20);
	    
	    if(percents.Get())
	    {
		switch(Rand::Get(1, 3))
		{
		    case 1: quantity1 = Artifact::Rand1(); break;
		    case 2: quantity1 = Artifact::Rand2();  break;
		    case 3: quantity1 = Artifact::Rand3(); break;
		    default: break;
		}
	    }
	}
	break;

	case MP2::OBJ_WAGON:
	{
	    Rand::Queue percents(3);
	    // 20%: empty
	    percents.Push(0, 20);
	    // 10%: artifact 1 or 2
	    percents.Push(1, 10);
	    // 50%: resource
	    percents.Push(2, 50);

	    switch(percents.Get())
	    {
		case 1:
            	    quantity1 = (1 == Rand::Get(1, 2) ? Artifact::Rand1() : Artifact::Rand2());
		    break;
		case 2:
		    quantity1 = Resource::Rand();
		    quantity2 = Rand::Get(2, 5);
		    break;
		default: break;
	    }
	}
	break;

	case MP2::OBJ_ARTIFACT:
	    // quantity1 - art, quantity2 - conditions for pickup: 
	    // 1,2,3 - 2000g, 2500g+3res, 3000g+5res,
	    // 4,5 - need have skill wisard or leadership,
	    // 6 - 50 rogues, 7 - 1 gin, 8,9,10,11,12,13 - 1 monster level4, other - none
	    addon = FindArtifact();
	    if(addon)
	    {
		Artifact::artifact_t art = Artifact::FromIndexSprite(addon->index);
		if(Artifact::SPELL_SCROLL == art)
		{
		    // spell from origin mp2
		    Spell::spell_t spell = Spell::FromInt(1 + (quantity2 * 256 + quantity1) / 8);
		    quantity3 = spell;
		}
		quantity1 = art;
		// conditions: 70% empty
		quantity2 = Rand::Get(1, 10) < 4 ? Rand::Get(1, 13) : 0;
		//  added resource
		if(quantity2 == 2 || quantity2 == 3)
		    quantity4 = Resource::Rand();
	    }
	break;

	case MP2::OBJ_RESOURCE:
	    addon = FindResource();
	    if(addon)
	    {
		quantity1 = Resource::FromIndexSprite(addon->index);
		switch(quantity1)
		{
		    case Resource::WOOD:
		    case Resource::ORE:
		    case Resource::GOLD:
			quantity2 = Rand::Get(5, 10);
			break;
		    default:
			quantity2 = Rand::Get(3, 6);
			break;
		}
	    }
	break;

	case MP2::OBJ_MAGICGARDEN:
	    // 5 gems or 500 gold
	    quantity1 = (Rand::Get(1) ? Resource::GEMS : Resource::GOLD);
	    quantity2 = 5;
	break;

	case MP2::OBJ_WATERWHEEL:
	    // first week 500 gold, next week 100 gold
	    quantity1 = Resource::GOLD;
	    quantity2 = (world.CountDay() == 1 ? 5 : 10);
	break;

	case MP2::OBJ_WINDMILL:
	    // 2 rnd resource
	    quantity1 = Resource::Rand();
	    quantity2 = 2;
	break;

	case MP2::OBJ_LEANTO:
	    // 1-4 rnd resource
	    quantity1 = Resource::Rand();
	    quantity2 = Rand::Get(1, 4);
	break;

	case MP2::OBJ_CAMPFIRE:
	    // 4-6 rnd resource and + 400-600 gold
	    quantity1 = Resource::Rand();
	    quantity2 = Rand::Get(4, 6);
	break;

	case MP2::OBJ_FLOTSAM:
	{
	    Rand::Queue percents(3);
	    // 25%: empty
	    percents.Push(0, 25);
	    // 25%: 500 gold + 10 wood
	    percents.Push(1, 25);
	    // 25%: 200 gold + 5 wood
	    percents.Push(2, 25);
	    // 25%: 5 wood
	    percents.Push(3, 25);
	    
	    // variant
	    switch(percents.Get())
	    {
		case 1: quantity1 = 5; quantity2 = 10; break;
		case 2: quantity1 = 2; quantity2 = 5;  break;
		case 3: quantity2 = 5; break;
		default: break;
	    }
	}
	break;

	case MP2::OBJ_SHIPWRECKSURVIROR:
	{
	    Rand::Queue percents(3);
	    // 55%: artifact 1
	    percents.Push(1, 55);
	    // 30%: artifact 2
	    percents.Push(1, 30);
	    // 15%: artifact 3
	    percents.Push(1, 15);
	    
	    // variant
	    switch(percents.Get())
	    {
		case 1: quantity1 = Artifact::Rand1(); break;
		case 2: quantity1 = Artifact::Rand2();  break;
		case 3: quantity1 = Artifact::Rand3(); break;
		default: break;
	    }
	}
	break;

	case MP2::OBJ_WATERCHEST:
	{
	    Rand::Queue percents(3);
            // 20% - empty
	    percents.Push(0, 20);
            // 70% - 1500 gold
	    percents.Push(1, 70);
            // 10% - 1000 gold + art
	    percents.Push(2, 10);

	    // variant
	    switch(percents.Get())
	    {
		case 1: quantity2 = 15; break;
		case 2: quantity2 = 10; quantity1 = Artifact::Rand1(); break;
		default: break;
	    }
	}
	break;

	case MP2::OBJ_TREASURECHEST:
	{
	    Rand::Queue percents(4);
	    // 31% - 2000 gold or 1500 exp
	    percents.Push(1, 31);
	    // 32% - 1500 gold or 1000 exp
	    percents.Push(2, 32);
	    // 32% - 1000 gold or 500 exp
	    percents.Push(3, 32);
	    // 5% - art
	    percents.Push(4,  5);
	    
	    // variant
	    switch(percents.Get())
	    {
		case 1: quantity2 = 20; break;
		case 2: quantity2 = 15; break;
		case 3: quantity2 = 10; break;
		case 4: quantity1 = Artifact::Rand1(); break;
		default: break;
	    }
	}
	break;

	case MP2::OBJ_DERELICTSHIP:
	    // 5000 gold
	    quantity2 = 50;
	break;

	case MP2::OBJ_SHIPWRECK:
	    // variant: 10ghost(1000g), 15 ghost(2000g), 25ghost(5000g) or 50ghost(2000g+art)
	    switch(Rand::Get(1, 4))
	    {
		case 1: quantity2 = 10; break;
		case 2: quantity2 = 15; break;
		case 3: quantity2 = 25; break;
		case 4: quantity1 = Artifact::Rand(); quantity2 = 50; break;
		default: break;
	    }
	break;

	case MP2::OBJ_GRAVEYARD:
	    // 1000 gold + art
		quantity1 = Artifact::Rand();
		quantity2 = 10;
	break;

	case MP2::OBJ_PYRAMID:
	    // random spell level 5
	    quantity1 = (Rand::Get(1) ? Spell::RandCombat(5) : Spell::RandAdventure(5));
	    quantity2 = 0;
	break;

	case MP2::OBJ_DAEMONCAVE:
	    // 1000 exp or 1000 exp + 2500 gold or 1000 exp + art or (-2500 or remove hero)
	    quantity2 = Rand::Get(1, 4);
	    quantity1 = Artifact::Rand();
	break;

	// rand monster
	case MP2::OBJ_ABANDONEDMINE:
	    quantity1 = 0;
	    quantity2 = Rand::Get(39, 45);	// I checked in Heroes II: min 3 x 13, and max 3 x 15
	break;

	case MP2::OBJ_TREEKNOWLEDGE:
	    // variant: 10 gems, 2000 gold or free
	    switch(Rand::Get(1, 3))
	    {
		case 1:
		    quantity2 = 10;
		break;
		case 2:
		    quantity2 = 20;
		break;
		default: break;
	    }
	break;

        case MP2::OBJ_BARRIER:
        case MP2::OBJ_TRAVELLERTENT:
	    quantity1 = Barrier::FromMP2(quantity1);
	break;

	default: break;
    }
}

bool Maps::Tiles::ValidQuantity(void) const
{
    if(MP2::isQuantityObject(general))
	    return quantity1 || quantity2;

    return false;
}

void Maps::Tiles::RemoveObjectSprite(void)
{
    const Maps::TilesAddon *addon = NULL;

    switch(general)
    {
	case MP2::OBJ_ARTIFACT:		addon = FindArtifact(); break;
	case MP2::OBJ_CAMPFIRE:		addon = FindCampFire(); break;

	case MP2::OBJ_WATERCHEST:
	case MP2::OBJ_BOTTLE:
	case MP2::OBJ_FLOTSAM:
	case MP2::OBJ_SHIPWRECKSURVIROR:addon = FindWaterResource(); break;

	case MP2::OBJ_TREASURECHEST:
	case MP2::OBJ_ANCIENTLAMP:
	case MP2::OBJ_RESOURCE:		addon = FindResource(); break;

	case MP2::OBJ_JAIL:		RemoveJailSprite(); return;

	case MP2::OBJ_BARRIER:		RemoveBarrierSprite(); return;

	default: return;
    }
    
    if(addon)
    {
        // remove shadow sprite from left cell
        if(Maps::isValidDirection(maps_index, Direction::LEFT))
    	    world.GetTiles(Maps::GetDirectionIndex(maps_index, Direction::LEFT)).Remove(addon->uniq);

	Remove(addon->uniq);
    }
}

void Maps::Tiles::RemoveBarrierSprite(void)
{
    const Maps::TilesAddon *addon = FindBarrier();

    if(addon)
    {
        // remove left sprite
        if(Maps::isValidDirection(maps_index, Direction::LEFT))
    	{
	    const u16 left = Maps::GetDirectionIndex(maps_index, Direction::LEFT);
	    world.GetTiles(left).Remove(addon->uniq);
	}

	Remove(addon->uniq);
    }
}

void Maps::Tiles::RemoveJailSprite(void)
{
    const Maps::TilesAddon *addon = FindJail();

    if(addon)
    {
        // remove left sprite
        if(Maps::isValidDirection(maps_index, Direction::LEFT))
    	{
	    const u16 left = Maps::GetDirectionIndex(maps_index, Direction::LEFT);
	    world.GetTiles(left).Remove(addon->uniq);

    	    // remove left left sprite
    	    if(Maps::isValidDirection(left, Direction::LEFT))
    		world.GetTiles(Maps::GetDirectionIndex(left, Direction::LEFT)).Remove(addon->uniq);
	}

        // remove top sprite
        if(Maps::isValidDirection(maps_index, Direction::TOP))
    	{
	    const u16 top = Maps::GetDirectionIndex(maps_index, Direction::TOP);
	    world.GetTiles(top).Remove(addon->uniq);
	    world.GetTiles(top).SetObject(MP2::OBJ_ZERO);

    	    // remove top left sprite
    	    if(Maps::isValidDirection(top, Direction::LEFT))
    	    {
		world.GetTiles(Maps::GetDirectionIndex(top, Direction::LEFT)).Remove(addon->uniq);
		world.GetTiles(Maps::GetDirectionIndex(top, Direction::LEFT)).SetObject(MP2::OBJ_ZERO);
	    }
	}

	Remove(addon->uniq);
    }
}

u16 Maps::Tiles::GetCountMonster(void) const
{
    return quantity2 * 0xFF + quantity1;
}

void Maps::Tiles::SetCountMonster(const u16 count)
{
    quantity1 = count % 0xFF;
    quantity2 = count / 0xFF;
}

void Maps::Tiles::UpdateMonsterInfo(void)
{
    switch(general)
    {
	case MP2::OBJ_RNDMONSTER:
        case MP2::OBJ_RNDMONSTER1:
        case MP2::OBJ_RNDMONSTER2:
        case MP2::OBJ_RNDMONSTER3:
        case MP2::OBJ_RNDMONSTER4:
	    UpdateRNDMonsterSprite(); break;
	default: break;
    }

    const Monster m = Monster(*this);
    bool fixed = false;

    // update random count
    if(0 == quantity1 && 0 == quantity2)
        SetCountMonster(4 * m.GetRNDSize(false));
    // update fixed count (mp2 format)
    else
    {
	u16 count = quantity2;
	    count <<= 8;
	    count |= quantity1;
	    count >>= 3;

        SetCountMonster(count);
	fixed = true;
    }

    // skip join
    if(m() == Monster::GHOST || m.isElemental())
	quantity4 = 0;
    else
    if(fixed)
	// for money
	quantity4 = 1;
    else
	// 20% chance of joining
        quantity4 = (3 > Rand::Get(1, 10) ? 2 : 1);
}

void Maps::Tiles::UpdateRNDMonsterSprite(void)
{
    Maps::TilesAddon *addon = FindRNDMonster();

    if(addon)
    {
	switch(general)
	{
    	    case MP2::OBJ_RNDMONSTER:       addon->index = Monster::Rand(); break;
    	    case MP2::OBJ_RNDMONSTER1:      addon->index = Monster::Rand(Monster::LEVEL1); break;
    	    case MP2::OBJ_RNDMONSTER2:      addon->index = Monster::Rand(Monster::LEVEL2); break;
    	    case MP2::OBJ_RNDMONSTER3:      addon->index = Monster::Rand(Monster::LEVEL3); break;
    	    case MP2::OBJ_RNDMONSTER4:      addon->index = Monster::Rand(Monster::LEVEL4); break;

	    default: DEBUG(DBG_GAME , DBG_WARN, "Maps::Tiles::UpdateRNDMonsterSprite: unknown object, index: " << maps_index); return;
	}

	// ICN::MONS32 start from PEASANT
        addon->index = addon->index - 1;

	general = MP2::OBJ_MONSTER;
    }
    else
        DEBUG(DBG_GAME , DBG_WARN, "Maps::Tiles::UpdateRNDMonsterSprite: FindRNDMonster return is NULL, index: " << maps_index);
}

void Maps::Tiles::UpdateAbandoneMineSprite(void)
{
    if(addons_level1.size())
    {
	std::list<TilesAddon>::iterator it1 = addons_level1.begin();
	std::list<TilesAddon>::iterator it2 = addons_level1.end();

	for(; it1 != it2; ++it1)
	{
	    TilesAddon & addon = *it1;

	    if(ICN::EXTRAOVR == MP2::GetICNObject(addon.object) && 5 == addon.index)
	    {
		addon.index = 4;
		break;
	    }
	}
    }
}

void Maps::Tiles::UpdateStoneLightsSprite(void)
{
    if(addons_level1.size())
    {
	std::list<TilesAddon>::iterator it1 = addons_level1.begin();
	std::list<TilesAddon>::iterator it2 = addons_level1.end();

	for(; it1 != it2; ++it1)
	{
	    TilesAddon & addon = *it1;

	    if(ICN::OBJNMUL2 == MP2::GetICNObject(addon.object))
	    switch(addon.index)
	    {
		case 116:	addon.object = 0x11; addon.index = 0; quantity1 = 1; break;
		case 119:	addon.object = 0x12; addon.index = 0; quantity1 = 2; break;
		case 122:	addon.object = 0x13; addon.index = 0; quantity1 = 3; break;
		default: 	break;
	    }
	}
    }
}

void Maps::Tiles::UpdateRNDArtifactSprite(void)
{
    TilesAddon *addon = NULL;
    u8 index = 0;

    switch(general)
    {
        case MP2::OBJ_RNDARTIFACT:
            addon = FindRNDArtifact(MP2::OBJ_RNDARTIFACT);
            index = Artifact::IndexSprite(Artifact::Rand());
            break;
        case MP2::OBJ_RNDARTIFACT1:
            addon = FindRNDArtifact(MP2::OBJ_RNDARTIFACT1);
            index = Artifact::IndexSprite(Artifact::Rand1());
            break;
        case MP2::OBJ_RNDARTIFACT2:
            addon = FindRNDArtifact(MP2::OBJ_RNDARTIFACT2);
            index = Artifact::IndexSprite(Artifact::Rand2());
            break;
        case MP2::OBJ_RNDARTIFACT3:
            addon = FindRNDArtifact(MP2::OBJ_RNDARTIFACT3);
            index = Artifact::IndexSprite(Artifact::Rand3());
            break;
        default: return;
    }

    if(addon)
    {
        addon->index = index;
        general = MP2::OBJ_ARTIFACT;

        // replace shadow artifact
        if(Maps::isValidDirection(maps_index, Direction::LEFT))
        {
            Maps::Tiles & left_tile = world.GetTiles(Maps::GetDirectionIndex(maps_index, Direction::LEFT));
            Maps::TilesAddon *shadow = left_tile.FindAddonLevel1(addon->uniq);

            if(shadow) shadow->index = index - 1;
        }
    }
}

void Maps::Tiles::UpdateRNDResourceSprite(void)
{
    TilesAddon *addon = FindRNDResource();

    if(addon)
    {
        addon->index = Resource::GetIndexSprite(Resource::Rand());
        general = MP2::OBJ_RESOURCE;

        // replace shadow artifact
        if(Maps::isValidDirection(maps_index, Direction::LEFT))
        {
            Maps::Tiles & left_tile = world.GetTiles(Maps::GetDirectionIndex(maps_index, Direction::LEFT));
            Maps::TilesAddon *shadow = left_tile.FindAddonLevel1(addon->uniq);

            if(shadow) shadow->index = addon->index - 1;
        }
    }
}

void Maps::Tiles::UpdateFountainSprite(void)
{
    if(addons_level1.size())
    {
	std::list<TilesAddon>::iterator it1 = addons_level1.begin();
	std::list<TilesAddon>::iterator it2 = addons_level1.end();

	for(; it1 != it2; ++it1)
	{
	    TilesAddon & addon = *it1;

	    if(ICN::OBJNMUL2 == MP2::GetICNObject(addon.object) && 15 == addon.index)
	    {
		addon.object = 0x14;
		addon.index = 0;
	    }
	}
    }
}

void Maps::Tiles::UpdateTreasureChestSprite(void)
{
    if(addons_level1.size())
    {
	std::list<TilesAddon>::iterator it1 = addons_level1.begin();
	std::list<TilesAddon>::iterator it2 = addons_level1.end();

	for(; it1 != it2; ++it1)
	{
	    TilesAddon & addon = *it1;

	    if(ICN::OBJNRSRC == MP2::GetICNObject(addon.object) && 19 == addon.index)
	    {
		addon.object = 0x15;
		addon.index = 0;
	    }
	}
    }
}

bool Maps::Tiles::isFog(u8 color) const
{
    return fogs & color;
}

void Maps::Tiles::SetFog(u8 color)
{
    fogs |= color;
}

void Maps::Tiles::ClearFog(u8 color)
{
    if(fogs & color) fogs &= ~color;
}

bool Maps::Tiles::NeedRedraw(void) const
{
    return flags & REDRAW;
}

void Maps::Tiles::SetRedraw(bool f)
{
    f ? flags |= REDRAW : flags &= ~REDRAW;
}

void Maps::Tiles::ResetQuantity(void)
{
    quantity1 = 0;
    quantity2 = 0;
    quantity3 = 0;
    quantity4 = 0;
}
