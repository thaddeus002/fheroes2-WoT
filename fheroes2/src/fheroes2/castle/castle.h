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
#ifndef H2CASTLE_H
#define H2CASTLE_H

#include <vector>
#include <bitset>
#include <string>
#include "gamedefs.h"
#include "maps_tiles.h"
#include "mageguild.h"
#include "color.h"
#include "captain.h"
#include "dialog.h"
#include "race.h"
#include "rect.h"
#include "army.h"
#include "heroes.h"

#define CASTLEMAXARMY           5 
#define CASTLEMAXMONSTER        6 

#define GROWN_WELL		2
#define GROWN_WEL2		8

class Heroes;

class Castle
{
public:
    typedef enum {
	BUILD_NOTHING		= 0x00000000,
	BUILD_THIEVESGUILD      = 0x00000001,
        BUILD_TAVERN            = 0x00000002,
        BUILD_SHIPYARD          = 0x00000004,
        BUILD_WELL              = 0x00000008,
        BUILD_STATUE            = 0x00000010,
        BUILD_LEFTTURRET        = 0x00000020,
        BUILD_RIGHTTURRET       = 0x00000040,
        BUILD_MARKETPLACE       = 0x00000080,
        BUILD_WEL2              = 0x00000100,   // Farm, Garbage He, Crystal Gar, Waterfall, Orchard, Skull Pile
        BUILD_MOAT              = 0x00000200,
        BUILD_SPEC              = 0x00000400,   // Fortification, Coliseum, Rainbow, Dungeon, Library, Storm
        BUILD_CASTLE            = 0x00000800,
        BUILD_CAPTAIN           = 0x00001000,
        BUILD_BOAT              = 0x00002000,
        BUILD_MAGEGUILD1        = 0x00004000,
	BUILD_MAGEGUILD2        = 0x00008000,
	BUILD_MAGEGUILD3        = 0x00010000,
	BUILD_MAGEGUILD4        = 0x00020000,
	BUILD_MAGEGUILD5        = 0x00040000,
	BUILD_TENT              = 0x00080000,
	DWELLING_MONSTER1       = 0x00100000,
	DWELLING_MONSTER2       = 0x00200000,
	DWELLING_MONSTER3       = 0x00400000,
	DWELLING_MONSTER4       = 0x00800000,
	DWELLING_MONSTER5       = 0x01000000,
	DWELLING_MONSTER6       = 0x02000000,
	DWELLING_UPGRADE2       = 0x04000000,
	DWELLING_UPGRADE3       = 0x08000000,
	DWELLING_UPGRADE4       = 0x10000000,
	DWELLING_UPGRADE5       = 0x20000000,
	DWELLING_UPGRADE6       = 0x40000000,
	DWELLING_UPGRADE7       = 0x80000000        // black dragon
    } building_t;

    Castle(u32 gid, u16 mapindex, const void *ptr, bool rnd = false);

    bool isCastle(void) const{ return building & BUILD_CASTLE; };
    bool AllowBuild(void) const{ return allow_build; };
    bool isBuild(u32 bd) const{ return building & bd; };
    bool HaveNearlySea(void) const;
    bool isHeroesPresent(void);
    bool RecruitMonster(building_t dw, u16 count);
    bool AllowBuyBuilding(building_t build) const;
    bool AllowBuyHero(void);
    bool ContainCoord(const u16 ax, const u16 ay) const;

    void RecruitHero(const Heroes::heroes_t hero);
    const Heroes* GetHeroes(void){ return castle_heroes; };

    Race::race_t GetRace(void) const{ return race; };
    Color::color_t GetColor(void) const{ return color; };
    const std::string & GetName(void) const{ return name; };
    u8 GetLevelMageGuild(void);
    u32 GetUniq(void) const{ return uniq; };

    const std::vector<Army::Troops> & GetArmy(void) const{ return army; }; 
    u8 GetCountArmy(void) const;
    u16 GetDwellingLivedCount(building_t dw);

    const Point & GetCenter(void) const{ return mp; };

    void ChangeColor(Color::color_t cl);

    void ActionNewDay(void);
    void ActionNewWeek(void);
    void ActionNewMonth(void);

    void BuyBuilding(building_t build);
    void DrawImageCastle(const Point & pt);

    Dialog::answer_t OpenDialog(void);

    static const std::string & GetStringBuilding(const building_t & build, const Race::race_t & race = Race::BOMG);
    static const std::string & GetDescriptionBuilding(const building_t & build, const Race::race_t & race = Race::BOMG);
    static ICN::icn_t GetICNBuilding(const building_t & build, const Race::race_t & race);
    static u32 GetUpgradeBuilding(const u32 build, const Race::race_t & race);
    
    static bool PredicateIsCastle(const Castle *castle);
    static bool PredicateIsTown(const Castle *castle);
    static bool PredicateIsBuildMarketplace(const Castle *castle);

private:
    void RedrawResourcePanel(void);
    void CorrectAreaMaps(void);
    void TownUpgradeToCastle(void);
    void ModifyTIlesRNDSprite(Maps::Tiles & tile);
    void ModifyTilesTownToCastle(Maps::Tiles & tile);
    void MinimizeAreaMapsID(void);
    u32 GetBuildingRequires(const building_t & build) const;

    Rect GetCoordBuilding(building_t building, const Point & pt);
    void RedrawAllBuilding(const Point & dst_pt, const std::vector<building_t> & orders);
    void RedrawBuilding(const building_t build, const Point & dst_pt, const u32 ticket);
    building_t OpenTown(void);
    void OpenTavern(void);
    void OpenThievesGuild(void);
    void OpenWell(void);
    void OpenMageGuild(void);
    void RedrawNameTown(const Point & src_pt);
    void WellRedrawInfoArea(const Point & cur_pt);
    void RedrawAnimationBuilding(const Point & dst_pt, const building_t build);

    Dialog::answer_t DialogBuyHero(const Heroes::heroes_t hero);
    Dialog::answer_t DialogBuyBuilding(building_t build, bool fixed = true);

    void PressRightAction(building_t b);

private:
    Color::color_t	color;
    Race::race_t	race;
    std::string		name;
    u32			building;
    bool		allow_castle;
    bool		army_spread;
    bool		allow_build;
    bool		present_boat;
    MageGuild		mageguild;
    std::vector<u16>    dwelling;
    std::vector<Army::Troops> army;
    Heroes * 		castle_heroes;

    Captain		captain;

    const u32		uniq;

    // center point maps
    const Point		mp;
};

#endif
