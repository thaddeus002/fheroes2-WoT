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
#include <functional>
#include "ground.h"
#include "mp2.h"
#include "direction.h"
#include "gamedefs.h"
#include "game_io.h"
#include "color.h"
#include "skill.h"
#include "artifact.h"
#include "army_troop.h"
#include "resource.h"

class Sprite;
class Heroes;
class Spell;
class Monster;

namespace Maps
{
    struct TilesAddon
    {
	enum level_t { GROUND = 0, DOWN = 1, SHADOW = 2, UPPER = 3 };

	TilesAddon();
	TilesAddon(u8 lv, u32 gid, u8 obj, u8 ii);

	TilesAddon & operator= (const TilesAddon & ta);

	bool isUniq(u32) const;
	bool isRoad(u16) const;
	bool isICN(u16) const;

	std::string String(int level) const;

	static bool isStream(const TilesAddon &);
	static bool isRoad(const TilesAddon &);

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
	static bool isShadow(const TilesAddon &);
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
	static bool isSkeleton(const TilesAddon &);
	static bool isSkeletonFix(const TilesAddon &);
	static bool isFlag32(const TilesAddon &);
	static bool isX_LOC123(const TilesAddon &);
	static bool isAbandoneMineSprite(const TilesAddon &);
	static bool isMounts(const TilesAddon &);
	static bool isRocs(const TilesAddon &);
	static bool isForests(const TilesAddon &);
	static bool isTrees(const TilesAddon &);
	static u16  GetPassable(const TilesAddon &);
	static u8   GetLoyaltyObject(const TilesAddon &);

	static bool PredicateSortRules1(const TilesAddon &, const TilesAddon &);
	static bool PredicateSortRules2(const TilesAddon &, const TilesAddon &);

	static void UpdateFountainSprite(TilesAddon &);
	static void UpdateTreasureChestSprite(TilesAddon &);
	static u8   UpdateStoneLightsSprite(TilesAddon &);
	static void UpdateAbandoneMineLeftSprite(TilesAddon &, u8 resource);
	static void UpdateAbandoneMineRightSprite(TilesAddon &);

	static bool ForceLevel1(const TilesAddon &);
	static bool ForceLevel2(const TilesAddon &);

        u32	uniq;
        u8	level;
        u8	object;
        u8	index;
	u8	tmp;
    };

    struct Addons : public std::list<TilesAddon>
    {
	void Remove(u32 uniq);
    };

    class Tiles
    {
    public:
	Tiles();

	void Init(s32, const MP2::mp2tile_t &);

	s32 GetIndex(void) const;
	MP2::object_t GetObject(bool skip_hero = true) const;
	u8 GetQuantity1(void) const{ return quantity1; }
	u8 GetQuantity2(void) const{ return quantity2; }
	Ground::ground_t GetGround(void) const;
	bool isWater(void) const;

	u16 TileSpriteIndex(void) const;
	u8  TileSpriteShape(void) const;

	const Surface & GetTileSurface(void) const;

	bool isPassable(const Heroes*, Direction::vector_t, bool skipfog) const;
	bool isRoad(u16 = DIRECTION_ALL) const;
	bool isObject(MP2::object_t obj) const { return obj == mp2_object; };
	bool isStream(void) const;
	bool GoodForUltimateArtifact(void) const;

	TilesAddon* FindAddonICN1(u16 icn1);
	TilesAddon* FindAddonICN2(u16 icn2);

	TilesAddon* FindAddonLevel1(u32 uniq1);
	TilesAddon* FindAddonLevel2(u32 uniq2);

	TilesAddon* FindObject(u8);
	const TilesAddon* FindObjectConst(u8) const;

	void SetTile(const u16 sprite_index, const u8 shape);
	void SetObject(u8 object);

	void FixObject(void);
	void FixLoadOldVersion(u16 version, u8 quantity3, u8 quantity4, u8 quantity5, u8 quantity6, u8 quantity7);

	void UpdatePassable(void);
	void CaptureFlags32(u8 obj, u8 col);

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

	std::string String(void) const;
	
	bool isFog(u8 color) const;
	void ClearFog(u8 color);

	/* monster operation */
	bool MonsterJoinConditionSkip(void) const;
	bool MonsterJoinConditionMoney(void) const;
	bool MonsterJoinConditionFree(void) const;
	bool MonsterJoinConditionForce(void) const;
	u8   MonsterJoinCondition(void) const;
	void MonsterSetJoinCondition(u8);
	void MonsterSetFixedCount(void);
	bool MonsterFixedCount(void) const;
	void MonsterSetCount(const u16 count);
	u16  MonsterCount(void) const;

	bool CaptureObjectIsProtection(void) const;

	/* object quantity operation */
	void QuantityUpdate(void);
	void QuantityReset(void);
	bool QuantityIsValid(void) const;
	void QuantitySetColor(u8);
	u8		QuantityTeleportType(void) const;
	u8   		QuantityVariant(void) const;
	u8   		QuantityExt(void) const;
	u8   		QuantityColor(void) const;
	u16   		QuantityGold(void) const;
	Spell		QuantitySpell(void) const;
	Skill::Secondary QuantitySkill(void) const;
	Artifact	QuantityArtifact(void) const;
	ResourceCount	QuantityResourceCount(void) const;
	Funds         	QuantityFunds(void) const;
	Monster		QuantityMonster(void) const;
	Army::Troop	QuantityTroop(void) const;

	void SetObjectPassable(bool);

	Heroes* GetHeroes(void) const;
	void    SetHeroes(Heroes*);

	static void PlaceMonsterOnTile(Tiles &, const Monster &, u16, u32);
	static void UpdateAbandoneMineSprite(Tiles &);
	static void FixedPreload(Tiles &);

    private:
	TilesAddon* FindFlags(void);
	void CorrectFlags32(const u8 index, bool);
	void RemoveJailSprite(void);
	void RemoveBarrierSprite(void);
	bool isLongObject(u16 direction);

	void RedrawBoat(Surface &) const;
	void RedrawMonster(Surface &) const;

	void QuantitySetVariant(u8);
	void QuantitySetExt(u8);
	void QuantitySetSkill(u8);
	void QuantitySetSpell(u8);
	void QuantitySetArtifact(u8);
	void QuantitySetResource(u8, u16);
	void QuantitySetTeleportType(u8);

	u8 GetQuantity3(void) const;
	void SetQuantity3(u8);

	static void UpdateMonsterInfo(Tiles &);
	static void UpdateDwellingPopulation(Tiles &);
	static void UpdateMonsterPopulation(Tiles &);
	static void UpdateRNDArtifactSprite(Tiles &);
	static void UpdateRNDResourceSprite(Tiles &);
	static void UpdateStoneLightsSprite(Tiles &);
	static void UpdateFountainSprite(Tiles &);
	static void UpdateTreasureChestSprite(Tiles &);

    private:
	friend class Game::IO;

        Addons addons_level1;
        Addons addons_level2; // 16

	u32	pack_maps_index;
	u16	pack_sprite_index;

	u16	tile_passable;
        u8      mp2_object;
        u8	fog_colors;

        u8      quantity1;
        u8      quantity2;
    };
}

#endif
