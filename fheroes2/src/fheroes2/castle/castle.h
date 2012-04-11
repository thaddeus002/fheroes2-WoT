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
#ifndef H2CASTLE_H
#define H2CASTLE_H

#include <vector>
#include <bitset>
#include <string>
#include "mageguild.h"
#include "color.h"
#include "captain.h"
#include "dialog.h"
#include "army.h"
#include "bitmodes.h"
#include "heroes.h"
#include "game_io.h"
#include "sprite.h"
#include "castle_heroes.h"
#include "position.h"

class Heroes;
namespace Maps { class Tiles; }

enum building_t
{
    BUILD_NOTHING           = 0x00000000,
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
    BUILD_SHRINE            = 0x00002000,
    BUILD_MAGEGUILD1        = 0x00004000,
    BUILD_MAGEGUILD2        = 0x00008000,
    BUILD_MAGEGUILD3        = 0x00010000,
    BUILD_MAGEGUILD4        = 0x00020000,
    BUILD_MAGEGUILD5        = 0x00040000,
    BUILD_MAGEGUILD         = BUILD_MAGEGUILD1 | BUILD_MAGEGUILD2 | BUILD_MAGEGUILD3 | BUILD_MAGEGUILD4 | BUILD_MAGEGUILD5,
    BUILD_TENT              = 0x00080000,	// deprecated
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
};

enum buildcond_t
{
    NOT_TODAY		= -1,
    ALREADY_BUILT	= -2,
    NEED_CASTLE		= -3,
    CASTLE_DISABLE	= -4,
    SHIPYARD_DISABLE	= -5,
    UNKNOWN_UPGRADE	= -6,
    REQUIRES_BUILD	= -7,
    LACK_RESOURCES	= -8,
    ALLOW_BUILD		= 1
};

class Castle : public Maps::Position, public BitModes
{
public:
    enum flags_t
    {
	ALLOWCASTLE		= 0x0002,
	CUSTOMARMY		= 0x0004,
	ALLOWBUILD		= 0x0008,
	DISABLEHIRES		= 0x0010,
	CAPITAL			= 0x0020
    };

    Castle();
    Castle(s16 cx, s16 cy, const u8 rs);
    void LoadFromMP2(const void *ptr);

    Captain &GetCaptain() { return captain; }

    bool isCastle(void) const{ return building & BUILD_CASTLE; }
    bool isCapital(void) const{ return Modes(CAPITAL); }
    bool HaveNearlySea(void) const;
    bool PresentBoat(void) const;
    bool AllowBuyHero(const Heroes &, std::string* = NULL);
    bool isPosition(const Point &) const;
    bool isNecromancyShrineBuild(void) const;

    u32 CountBuildings(void) const;

    Heroes* RecruitHero(Heroes*);
    CastleHeroes GetHeroes(void) const;

    u8 GetRace(void) const{ return race; }
    Color::color_t GetColor(void) const{ return color; }
    const std::string & GetName(void) const{ return name; }
    u8 GetControl(void) const;

    u8 GetLevelMageGuild(void) const;
    const MageGuild & GetMageGuild(void) const;
    bool HaveLibraryCapability(void) const;
    bool isLibraryBuild(void) const;
    void MageGuildEducateHero(HeroBase &) const;

    const Army & GetArmy(void) const;
    Army & GetArmy(void);
    const Army & GetActualArmy(void) const;
    Army & GetActualArmy(void);
    u16 GetDwellingLivedCount(u32) const;
    u32 GetActualDwelling(u32) const;

    bool RecruitMonster(u32 dw, u16 count);
    u16  RecruitMaxMonster(u32 dw);
    void RecruitAllMonster(void);

    void ChangeColor(Color::color_t cl);

    void ActionNewDay(void);
    void ActionNewWeek(void);
    void ActionNewMonth(void);

    void ActionPreBattle(void);
    void ActionAfterBattle(bool attacker_wins);

    void DrawImageCastle(const Point & pt);

    Dialog::answer_t OpenDialog(bool readonly = false, bool fade = false);

    s8 GetAttackModificator(std::string *strs) const;
    s8 GetDefenseModificator(std::string *strs) const;
    s8 GetPowerModificator(std::string *strs) const;
    s8 GetKnowledgeModificator(std::string *strs) const;
    s8 GetMoraleModificator(std::string *strs) const;
    s8 GetLuckModificator(std::string *strs) const;

    bool AllowBuild(void) const{ return Modes(ALLOWBUILD); }
    bool AllowBuyBuilding(u32) const;
    buildcond_t CheckBuyBuilding(u32) const;
    bool isBuild(u32 bd) const{ return building & bd; }
    bool BuyBuilding(u32);
    bool AllowBuyBoat(void) const;
    bool BuyBoat(void);
    u32 GetBuildingRequires(u32) const;

    void Scoute(void) const;

    static const char* GetStringBuilding(u32, u8 race = 0);
    static const char* GetDescriptionBuilding(u32, u8 race = 0);
    static ICN::icn_t GetICNBuilding(u32, u8);
    static ICN::icn_t GetICNBoat(const u8 & race);
    u32 GetUpgradeBuilding(u32) const;

    static bool PredicateIsCastle(const Castle*);
    static bool PredicateIsTown(const Castle*);
    static bool PredicateIsBuildMarketplace(const Castle*);
    static bool PredicateIsCapital(const Castle*);

    static u8 GetGrownWell(void);
    static u8 GetGrownWel2(void);
    static u8 GetGrownWeekOf(const Monster &);
    static u8 GetGrownMonthOf(void);

    std::string String(void) const;

    Dialog::answer_t DialogBuyHero(const Heroes*);
    Dialog::answer_t DialogBuyCaptain(bool fixed = true) const;
    Dialog::answer_t DialogBuyCastle(bool fixed = true) const;

    void SwapCastleHeroes(CastleHeroes &);

private:
    u16* GetDwelling(u32 dw);
    void EducateHeroes(void);
    void RedrawResourcePanel(const Point &);
    u32  OpenTown(void);
    void OpenTavern(void);
    void OpenThievesGuild(void);
    void OpenWell(void);
    void OpenMageGuild(void);
    void WellRedrawInfoArea(const Point & cur_pt);
    void JoinRNDArmy(void);

private:
    friend class Game::IOld;
    friend StreamBase & operator<< (StreamBase &, const Castle &);
    friend StreamBase & operator>> (StreamBase &, Castle &);

    u8			race;
    u32			building;
    Captain		captain;

    Color::color_t	color;
    std::string		name;

    MageGuild		mageguild;
    u16			dwelling[CASTLEMAXMONSTER];
    Army        	army;
};

namespace CastleDialog
{
    struct builds_t
    {
	builds_t(building_t b, const Rect & r) : id(b), coord(r) {};

	bool operator== (u32 b) const { return b == id; };

	building_t  id;
	Rect        coord;
	Sprite      contour;
    };

    struct CacheBuildings : std::vector<builds_t>
    {
	CacheBuildings(const Castle &, const Point &);
	const Rect & GetRect(building_t) const;
    };

    void RedrawAllBuilding(const Castle &, const Point &, const CacheBuildings &, u32 flash = BUILD_NOTHING);
    void RedrawAnimationBuilding(const Castle &, const Point &, const CacheBuildings &, u32 build);
    void RedrawBuildingSpriteToArea(const Sprite &, s16, s16, const Rect &);
}

struct VecCastles : public std::vector<Castle*>
{
    Castle* Get(s32) const;
    Castle* Get(const Point &) const;
    Castle* GetFirstCastle(void) const;

    void ChangeColors(Color::color_t, Color::color_t);
};

struct AllCastles : public VecCastles
{
    AllCastles();
    ~AllCastles();

    void Init(void);
    void clear(void);

    void Scoute(u8) const;
};

StreamBase & operator<< (StreamBase &, const VecCastles &);
StreamBase & operator>> (StreamBase &, VecCastles &);

StreamBase & operator<< (StreamBase &, const AllCastles &);
StreamBase & operator>> (StreamBase &, AllCastles &);

StreamBase & operator<< (StreamBase &, const Castle &);
StreamBase & operator>> (StreamBase &, Castle &);

#endif
