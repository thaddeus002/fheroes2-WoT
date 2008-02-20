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
#ifndef H2TILES_H
#define H2TILES_H

#include <vector>
#include "ground.h"
#include "mp2.h"
#include "direction.h"
#include "surface.h"
#include "gamedefs.h"

class Sprite;

namespace Maps
{
    class TilesAddon
    {
    public:
	typedef enum { GROUND = 0, DOWN = 1, SHADOW = 2, UPPER = 3 } level_t;

	TilesAddon(u8 lv, u32 gid, u8 obj, u8 ii);

	TilesAddon & operator= (const TilesAddon & ta);

	static u8 isRoad(const TilesAddon & ta, u8 direct = 0xFF);
	static bool RulesCompare(const TilesAddon & ta1, const TilesAddon & ta2){ return ta1.GetLevel() > ta2.GetLevel(); };

	u8 GetObject(void) const{ return object; };
	u8 GetIndex(void) const{ return index; };
	u32 GetUniq(void) const{ return uniq; };
	level_t GetLevel(void) const{ return level; };

	void SetUniq(u32 gid){ uniq = gid; };
	void SetObject(u8 obj){ object = obj; };
	void SetIndex(u8 ii){ index = ii; };

    private:
        level_t level;
        u32	uniq;
        u8	object;
        u8	index;
    };

    class Tiles
    {
    public:
	Tiles(u16 mi, const MP2::mp2tile_t & mp2tile);

	u16 GetIndex(void) const{ return maps_index; };
	MP2::object_t GetObject(void) const;
	u8 GetQuantity1(void) const{ return quantity1; };
	u8 GetQuantity2(void) const{ return quantity2; };
	Ground::ground_t GetGround(void) const;
	const Surface & GetSurface(void) const{ return tile_sprite; };
	u32 GetUniq1(void) const{ return addons_level1.size() ? addons_level1[0].GetUniq() : 0; };
	u32 GetUniq2(void) const{ return addons_level2.size() ? addons_level2[0].GetUniq() : 0; };

	bool isAnimation(u16 dstx, u16 dsty) const;
	bool isPassable(void) const;
	bool isRoad(const Direction::vector_t & direct = Direction::CENTER) const;

	const TilesAddon * FindAddon(u8 object, u8 index_min, u8 index_max) const;
	const TilesAddon * FindAddon(u8 object, u8 index = 0xFF) const;
	const TilesAddon * FindAddonLevel1(u32 uniq1) const;
	const TilesAddon * FindAddonLevel2(u32 uniq2) const;

	void SetTile(const u16 index, const u8 shape);

	void SetQuantity1(u8 val){ quantity1 = val; };
	void SetQuantity2(u8 val){ quantity2 = val; };
	void SetObject(MP2::object_t object){ general = object; };

	void AddPathSprite(const Sprite * sprite){ path_sprite = sprite; };
	void DelPathSprite(void){ path_sprite = NULL; };

	void Blit(u16 dstx, u16 dsty, u32 anime_frame = 0) const;
	void Redraw(void) const;

	void AddonsPushLevel1(const MP2::mp2tile_t & mt);
	void AddonsPushLevel1(const MP2::mp2addon_t & ma);
	void AddonsPushLevel2(const MP2::mp2tile_t & mt);
	void AddonsPushLevel2(const MP2::mp2addon_t & ma);

	void AddonsSort(void);
	void Remove(u32 uniq);

	void DebugInfo(u16 index = 0xFFFF) const;

    private:
	void RedrawHeroes(u16 dx, u16 dy) const;
	void RedrawMonster(u16 dx, u16 dy, u32 anime_sprite) const;

    private:
	const u16 maps_index;
	Surface tile_sprite;
	u16	tile_index;
        u8	shape;
        u8      general;
        u8      quantity1;
        u8      quantity2;
        std::vector<TilesAddon> addons_level1;
        std::vector<TilesAddon> addons_level2;
        
        const Sprite *path_sprite;
    };
};

#endif
