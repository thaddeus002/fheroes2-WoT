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
#ifndef H2TILES_H
#define H2TILES_H

#include <list>
#include "ground.h"
#include "mp2.h"
#include "direction.h"
#include "gamedefs.h"
#include "game_io.h"
#include "color.h"

class Sprite;
class Heroes;
namespace Interface { class GameArea; };

namespace Maps
{
    class TilesAddon
    {
    public:
	typedef enum { GROUND = 0, DOWN = 1, SHADOW = 2, UPPER = 3 } level_t;

	TilesAddon();
	TilesAddon(u8 lv, u32 gid, u8 obj, u8 ii);

	TilesAddon & operator= (const TilesAddon & ta);

	static u16 isRoad(const TilesAddon & ta);
	static bool isStream(const TilesAddon & ta);
	static bool PredicateSortRules1(const TilesAddon & ta1, const TilesAddon & ta2);
	static bool PredicateSortRules2(const TilesAddon & ta1, const TilesAddon & ta2);

        u8	level;
        u32	uniq;
        u8	object;
        u8	index;
    };

    enum flags_t
    {
	REDRAW	= 0x01,	// reserved
	USR4	= 0x04,
	USR8	= 0x08
    };

    class Tiles
    {
    public:
	Tiles(u16);
	Tiles(u16 mi, const MP2::mp2tile_t & mp2tile);

	u16 GetIndex(void) const{ return maps_index; };
	MP2::object_t GetObject(void) const;
	u8 GetQuantity1(void) const{ return quantity1; };
	u8 GetQuantity2(void) const{ return quantity2; };
	u8 GetQuantity3(void) const{ return quantity3; };
	u8 GetQuantity4(void) const{ return quantity4; };
	Ground::ground_t GetGround(void) const;
	const Surface & GetTileSurface(void) const;
	u32 GetUniq1(void) const{ return addons_level1.size() ? addons_level1.front().uniq : 0; };
	u32 GetUniq2(void) const{ return addons_level2.size() ? addons_level2.front().uniq : 0; };
	u32 GetSize1(void) const{ return addons_level1.size(); };
	u32 GetSize2(void) const{ return addons_level2.size(); };

	bool isPassable(const Heroes * hero = NULL, bool skipfog = false) const;
	bool isRoad(const Direction::vector_t & direct = Direction::CENTER) const;
	bool isStream(void) const;

	TilesAddon * FindAddonICN1(u16 icn1);
	TilesAddon * FindAddonICN2(u16 icn2);

	TilesAddon * FindAddonLevel1(u32 uniq1);
	TilesAddon * FindAddonLevel2(u32 uniq2);

	TilesAddon * FindCampFire(void);
	TilesAddon * FindWaterResource(void);
	TilesAddon * FindResource(void);
	TilesAddon * FindRNDResource(void);
	TilesAddon * FindArtifact(void);
	TilesAddon * FindRNDArtifact(const u8 level = 0);
	TilesAddon * FindUltimateArtifact(void);
	TilesAddon * FindMiniHero(void);
	TilesAddon * FindEvent(void);
	TilesAddon * FindBoat(void);
	TilesAddon * FindCastle(void);
	TilesAddon * FindRNDCastle(void);
	TilesAddon * FindFlags(void);
	TilesAddon * FindRNDMonster(void);
	TilesAddon * FindMonster(void);
	const TilesAddon * FindMines(void) const;
	const TilesAddon * FindStandingStones(void) const;
	TilesAddon * FindJail(void);
	TilesAddon * FindBarrier(void);

	void SetTile(const u16 index, const u8 shape);

	void SetQuantity1(u8 val){ quantity1 = val; };
	void SetQuantity2(u8 val){ quantity2 = val; };
	void SetQuantity3(u8 val){ quantity3 = val; };
	void SetQuantity4(u8 val){ quantity4 = val; };
	void ResetQuantity(void);
	void SetObject(MP2::object_t object){ general = object; };

	void UpdateQuantity(void);
	bool ValidQuantity(void) const;
	void CaptureFlags32(const MP2::object_t obj, const Color::color_t col);

	void RedrawTile(void) const;
	void RedrawTile(Surface &, s16, s16) const;
	void RedrawBottom(void) const;
	void RedrawBottom(Surface &, const Interface::GameArea &) const;
	void RedrawBottom(Surface &, s16, s16, const TilesAddon* skip = NULL) const;
	void RedrawBottom4Hero(Surface &, const Interface::GameArea &) const;
	void RedrawTop(void) const;
	void RedrawTop(Surface &, const Interface::GameArea &) const;
	void RedrawTop(Surface &, s16, s16, const Interface::GameArea &, const TilesAddon* skip = NULL) const;
	void RedrawTop4Hero(Surface &, const Interface::GameArea &, bool skip_ground) const;
	void RedrawObjects(void) const;
	void RedrawObjects(Surface &, s16, s16, const Interface::GameArea &) const;

	void AddonsPushLevel1(const MP2::mp2tile_t & mt);
	void AddonsPushLevel1(const MP2::mp2addon_t & ma);
	void AddonsPushLevel1(const TilesAddon & ta);
	void AddonsPushLevel2(const MP2::mp2tile_t & mt);
	void AddonsPushLevel2(const MP2::mp2addon_t & ma);
	void AddonsPushLevel2(const TilesAddon & ta);

	void AddonsSort(void);
	void Remove(u32 uniq);
	void RemoveObjectSprite(void);

	void DebugInfo(void) const;
	
	bool isFog(u8 color) const;
	void SetFog(u8 color);
	void ClearFog(u8 color);

	bool NeedRedraw(void) const;
	void SetRedraw(bool);

	void FixLoyaltyVersion(void);

	u16 GetCountMonster(void) const;
	void SetCountMonster(const u16 count);
	void UpdateRNDMonsterSprite(void);
	void UpdateMonsterInfo(void);

	void UpdateRNDArtifactSprite(void);
	void UpdateRNDResourceSprite(void);

	void UpdateAbandoneMineSprite(void);
	void UpdateStoneLightsSprite(void);
	void UpdateFountainSprite(void);
	void UpdateTreasureChestSprite(void);

    private:
	void CorrectFlags32(const u8 index);
	void RemoveJailSprite(void);
	void RemoveBarrierSprite(void);

	void RedrawBoat(Surface &, s16, s16, const Interface::GameArea &) const;
	void RedrawMonster(Surface &, s16, s16, const Interface::GameArea &) const;

    private:
	friend class Game::IO;

        std::list<TilesAddon> addons_level1;
        std::list<TilesAddon> addons_level2;

	const u16 maps_index;
	u16	tile_index;

	u8	shape;
        u8      general;
        u8      quantity1;
        u8      quantity2;
	u8	quantity3;
	u8	quantity4;	/* memory align */
        u8	fogs;
	u8	flags;
    };
};

#endif
