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

namespace Maps
{
    class TilesAddon
    {
    public:
	enum level_t { GROUND = 0, DOWN = 1, SHADOW = 2, UPPER = 3 };

	TilesAddon();
	TilesAddon(u8 lv, u32 gid, u8 obj, u8 ii);

	TilesAddon & operator= (const TilesAddon & ta);

	void DebugInfo(int level) const;

	static u16 isRoad(const TilesAddon &);
	static bool isStream(const TilesAddon &);

	static bool isResource(const TilesAddon &);
	static bool isWaterResource(const TilesAddon &);
	static bool isWhirlPool(const TilesAddon &);
	static bool isStandingStone(const TilesAddon &);
	static bool isArtifact(const TilesAddon &);
	static bool isCampFire(const TilesAddon &);
	static bool isMonster(const TilesAddon &);
	static bool isBarrier(const TilesAddon &);
	static bool isArtesianSpring(const TilesAddon &);
	static bool isOasis(const TilesAddon &);
	static bool isWateringHole(const TilesAddon &);
	static bool isJail(const TilesAddon &);
	static bool isMine(const TilesAddon &);
	static bool isEvent(const TilesAddon &);
	static bool isBoat(const TilesAddon &);
	static bool isMiniHero(const TilesAddon &);
	static bool isRandomResource(const TilesAddon &);
	static bool isRandomArtifact(const TilesAddon &);
	static bool isRandomArtifact1(const TilesAddon &);
	static bool isRandomArtifact2(const TilesAddon &);
	static bool isRandomArtifact3(const TilesAddon &);
	static bool isUltimateArtifact(const TilesAddon &);
	static bool isCastle(const TilesAddon &);
	static bool isRandomCastle(const TilesAddon &);
	static bool isRandomMonster(const TilesAddon &);
	static bool isFlag32(const TilesAddon &);

	static bool PredicateSortRules1(const TilesAddon & ta1, const TilesAddon & ta2);
	static bool PredicateSortRules2(const TilesAddon & ta1, const TilesAddon & ta2);

        u8	level;
        u32	uniq;
        u8	object;
        u8	index;
    };

    class Tiles
    {
    public:
	Tiles(s32);
	Tiles(s32 mi, const MP2::mp2tile_t & mp2tile);

	s32 GetIndex(void) const{ return maps_index; }
	MP2::object_t GetObject(void) const;
	u8 GetQuantity1(void) const{ return quantity1; }
	u8 GetQuantity2(void) const{ return quantity2; }
	u8 GetQuantity3(void) const{ return quantity3; }
	u8 GetQuantity4(void) const{ return quantity4; }
	Ground::ground_t GetGround(void) const;
	const Surface & GetTileSurface(void) const;

	bool isPassable(const Heroes * hero = NULL, bool skipfog = false) const;
	bool isRoad(const Direction::vector_t & direct = Direction::CENTER) const;
	bool isStream(void) const;
	bool GoodForUltimateArtifact(void) const;

	bool OtherObjectsIsProtection(void) const;
	bool CaptureObjectIsProtection(u8 color) const;

	TilesAddon* FindAddonICN1(u16 icn1);
	TilesAddon* FindAddonICN2(u16 icn2);

	TilesAddon* FindAddonLevel1(u32 uniq1);
	TilesAddon* FindAddonLevel2(u32 uniq2);

	TilesAddon* FindObject(u8);
	const TilesAddon* FindObject(u8) const;

	void SetTile(const u16 sprite_index, const u8 shape);

	void SetQuantity1(u8 val){ quantity1 = val; }
	void SetQuantity2(u8 val){ quantity2 = val; }
	void SetQuantity3(u8 val){ quantity3 = val; }
	void SetQuantity4(u8 val){ quantity4 = val; }
	void ResetQuantity(void);
	void SetObject(MP2::object_t object){ mp2_object = object; }

	void UpdateQuantity(void);
	bool ValidQuantity(void) const;
	void CaptureFlags32(const MP2::object_t obj, const Color::color_t col);

	void RedrawTile(Surface &) const;
	void RedrawBottom(Surface &, bool skip_objs = false) const;
	void RedrawBottom4Hero(Surface &) const;
	void RedrawTop(Surface &, const TilesAddon* skip = NULL) const;
	void RedrawTop4Hero(Surface &, bool skip_ground) const;
	void RedrawObjects(Surface &) const;
	void RedrawFogs(Surface &, u8) const;

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

	void FixLoyaltyVersion(void);

	u8   GetMinesType(void) const;

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
	TilesAddon* FindFlags(void);
	void CorrectFlags32(const u8 index, bool);
	void RemoveJailSprite(void);
	void RemoveBarrierSprite(void);

	void RedrawBoat(Surface &) const;
	void RedrawMonster(Surface &) const;

    private:
	friend class Game::IO;

        std::list<TilesAddon> addons_level1;
        std::list<TilesAddon> addons_level2;

	const s32 maps_index;

	u16	tile_sprite_index;
	u8	tile_sprite_shape;

        u8      mp2_object;
        u8      quantity1;
        u8      quantity2;
	u8	quantity3;
	u8	quantity4;
        u8	fogs;

	u8	unused1; /* memory align */
	u8	unused2; /* memory align */
	u8	unused3; /* memory align */
    };
}

#endif
