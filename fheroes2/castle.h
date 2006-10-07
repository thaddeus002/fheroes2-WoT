/*
    freeHeroes2 engine
    turn-based game engine (clone of Heroes Of the Might and Magic II)
    Copyright (C) 2006

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    3DO, New World Computing, Heroes of Might and Magic, and the related
    3DO logos are trademarks and or registered trademarks of 3DO Company.
    All other trademarks belong to their respective owners.

    Web: http://sourceforge.net/projects/fheroes2

    Description:
*/

#ifndef _CASTLE_H
#define _CASTLE_H

#include "SDL.h"
#include "gamedefs.h"
#include "heroes.h"
#include "actionevent.h"
#include "spell.h"
#include "monster.h"

#define CASTLEMAXARMY		5
#define CASTLEMAXMONSTER	6

typedef enum { BUILD_OK, CANNOT_BUILD, ALREADY_BUILD, END_TUR } BUILDACTION;

// growth week
#define GROWN_WEEK_PEASANT		12
#define GROWN_WEEK_ARCHER		8
#define GROWN_WEEK_RANGER		8
#define GROWN_WEEK_PIKEMAN		5
#define GROWN_WEEK_VETERAN_PIKEMAN	5
#define GROWN_WEEK_SWORDSMAN		4
#define GROWN_WEEK_MASTER_SWORDSMAN	4
#define GROWN_WEEK_CAVALIRY		3
#define GROWN_WEEK_CHAMPION		3
#define GROWN_WEEK_PALADIN		2
#define GROWN_WEEK_CRUSADER		2
#define GROWN_WEEK_GOBLIN		10
#define GROWN_WEEK_ORC			8
#define GROWN_WEEK_CHIEF_ORC		8
#define GROWN_WEEK_WOLF			5
#define GROWN_WEEK_OGRE			4
#define GROWN_WEEK_LORD_OGRE		4
#define GROWN_WEEK_TROLL		3
#define GROWN_WEEK_WAR_TROLL		3
#define GROWN_WEEK_CYCLOPS		2
#define GROWN_WEEK_SPRITE		8
#define GROWN_WEEK_DWARF		6
#define GROWN_WEEK_BATTLE_DWARF		6
#define GROWN_WEEK_ELF			4
#define GROWN_WEEK_GRAND_ELF		4
#define GROWN_WEEK_DRUID		3
#define GROWN_WEEK_GREATER_DRUID	3
#define GROWN_WEEK_UNICORN		2
#define GROWN_WEEK_PHOENIX		1
#define GROWN_WEEK_CENTAUR		8
#define GROWN_WEEK_GARGOYLE		6
#define GROWN_WEEK_GRIFFIN		4
#define GROWN_WEEK_MINOTAUR		3
#define GROWN_WEEK_KNIGHT_MINOTAUR	3
#define GROWN_WEEK_HIDRA		2
#define GROWN_WEEK_GREEN_DRAGON		1
#define GROWN_WEEK_RED_DRAGON		1
#define GROWN_WEEK_BLACK_DRAGON		1
#define GROWN_WEEK_HALFLING		8
#define GROWN_WEEK_BOAR			6
#define GROWN_WEEK_IRON_GOLEM		4
#define GROWN_WEEK_STEEL_GOLEM		4
#define GROWN_WEEK_ROC			3
#define GROWN_WEEK_MAGE			2
#define GROWN_WEEK_ARCHMAGE		2
#define GROWN_WEEK_GIANT		1
#define GROWN_WEEK_TITAN		1
#define GROWN_WEEK_SKELETON		8
#define GROWN_WEEK_ZOMBIE		6
#define GROWN_WEEK_MUTANT_ZOMBIE	6
#define GROWN_WEEK_MUMMY		4
#define GROWN_WEEK_ROYAL_MUMMY		4
#define GROWN_WEEK_VAMPIRE		3
#define GROWN_WEEK_LORD_VAMPIRE		3
#define GROWN_WEEK_LICH			2
#define GROWN_WEEK_POWER_LICH		2
#define GROWN_WEEK_BONE_DRAGON		1
#define GROWN_WEEK_ROGUE		5
#define GROWN_WEEK_NOMAD		5
#define GROWN_WEEK_GHOST		5
#define GROWN_WEEK_GENIE		5
#define GROWN_WEEK_MEDUSA		5
#define GROWN_WEEK_EARTH_ELEMENT	5
#define GROWN_WEEK_AIR_ELEMENT		5
#define GROWN_WEEK_FIRE_ELEMENT		5
#define GROWN_WEEK_WATER_ELEMENT	5


#define CASTLESPELL_LEVEL5      2 
#define CASTLESPELL_LEVEL4      3 
#define CASTLESPELL_LEVEL3      3 
#define CASTLESPELL_LEVEL2      4 
#define CASTLESPELL_LEVEL1      4 

typedef struct {
                E_SPELL level5[CASTLESPELL_LEVEL5];
                E_SPELL level4[CASTLESPELL_LEVEL4];
                E_SPELL level3[CASTLESPELL_LEVEL3];
                E_SPELL level2[CASTLESPELL_LEVEL2];
                E_SPELL level1[CASTLESPELL_LEVEL1];
                } S_MAGEGUILD;
                                                                                                                
typedef enum {
		BUILD_THIEVESGUILD	= 0x00000001,
		BUILD_TAVERN		= 0x00000002,
		BUILD_SHIPYARD		= 0x00000004,
		BUILD_WELL		= 0x00000008,
		BUILD_STATUE		= 0x00000010,
		BUILD_LEFTTURRET	= 0x00000020,
		BUILD_RIGHTTURRET	= 0x00000040,
		BUILD_MARKETPLACE	= 0x00000080,
		BUILD_WEL2		= 0x00000100,	// Farm, Garbage He, Crystal Gar, Waterfall, Orchard, Skull Pile
		BUILD_MOAT		= 0x00000200,
		BUILD_SPEC		= 0x00000400,	// Fortification, Coliseum, Rainbow, Dungeon, Library, Storm
		BUILD_CASTLE		= 0x00000800,
		BUILD_CAPTAIN		= 0x00001000,
		BUILD_BOAT		= 0x00002000,
		BUILD_MAGEGUILD1	= 0x00004000,
		BUILD_MAGEGUILD2	= 0x00008000,
		BUILD_MAGEGUILD3	= 0x00010000,
		BUILD_MAGEGUILD4	= 0x00020000,
		BUILD_MAGEGUILD5	= 0x00040000,
		BUILD_TENT		= 0x00080000,
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

	    } E_BUILDINGCASTLE;

typedef struct {

    E_COLORS	color;
    E_RACE	race;
    char	name[13];
    Uint8	ax;
    Uint8	ay;
    SDL_Rect	pos;
    Uint32	building;
    S_MAGEGUILD	mageGuild;
    Uint16	monster[CASTLEMAXMONSTER];
    S_ARMY	army[CASTLEMAXARMY];
    E_ARMYFORMAT af;                             // army format
    BOOL	allowCastle;
    BOOL	boat;
    E_NAMEHEROES hero;
    void	*next;

} S_CASTLE;

BOOL		AddCastle(FILE *, Uint8, Uint8, Uint8);
void		FreeCastle(void);
E_RACE		GetRaceRNDCastle(Uint8, Uint8);
S_CASTLE	*GetStatCastlePos(Uint8, Uint8);
S_CASTLE	*GetStatCastle(Uint8);
const S_CASTLE	*GetCurrentCastle(void);

ACTION		EnterCastle(Uint8, Uint8, E_NAMEHEROES);

S_CASTLE	*GetFirstCastle(E_COLORS);
S_CASTLE 	*GetNextCastle(E_COLORS);
S_CASTLE 	*GetPrevCastle(E_COLORS);
S_CASTLE 	*GetEndCastle(E_COLORS);

E_MONSTER	GetMonsterFromCastle(const S_CASTLE *, Uint8);
BOOL		BuyMonsterFromCastle(const S_CASTLE *, Uint8, Uint16);

BOOL		CheckBuyMonsterFromCastle(const S_CASTLE *, Uint8, Uint16);
BOOL		CastleDwellingUpgradable(const S_CASTLE *, E_BUILDINGCASTLE);
BOOL		CastleNearOcean(const S_CASTLE *);
BOOL		CastleNearBoat();
BOOL		CastlePresentHeroes(const S_CASTLE *);

void		CastleIncreaseArmy(const S_CASTLE *, E_BUILDINGCASTLE, Uint8);
void		AllCastleIncreaseRecrut(void);

Uint8           GetMonsterGrownCastle(const S_CASTLE *, E_MONSTER);
E_MAGICLEVEL	GetMageGuildLevel(const S_CASTLE *);

E_NAMEHEROES	GetHeroesFromCastle(const S_CASTLE *);

void		RedrawCastleMonster(void);
void		RedrawCastleInfoResource(void);

BUILDACTION	AllowBuildCastle(const S_CASTLE *);
BUILDACTION	AllowBuildMageGuild(const S_CASTLE *);
BUILDACTION	AllowBuildTavern(const S_CASTLE *);
BUILDACTION	AllowBuildThievesGuild(const S_CASTLE *);
BUILDACTION	AllowBuildShipyard(const S_CASTLE *);
BUILDACTION	AllowBuildStatue(const S_CASTLE *);
BUILDACTION	AllowBuildMarketplace(const S_CASTLE *);
BUILDACTION	AllowBuildWell(const S_CASTLE *);
BUILDACTION	AllowBuildMoat(const S_CASTLE *);
BUILDACTION	AllowBuildLeftTurret(const S_CASTLE *);
BUILDACTION	AllowBuildRightTurret(const S_CASTLE *);
BUILDACTION	AllowBuildCaptain(const S_CASTLE *);
BUILDACTION	AllowBuildBoat(const S_CASTLE *);
BUILDACTION	AllowBuildWel2(const S_CASTLE *);
BUILDACTION	AllowBuildSpec(const S_CASTLE *);
BUILDACTION	AllowBuildDwelling1(const S_CASTLE *);
BUILDACTION	AllowBuildDwelling2(const S_CASTLE *);
BUILDACTION	AllowBuildDwelling3(const S_CASTLE *);
BUILDACTION	AllowBuildDwelling4(const S_CASTLE *);
BUILDACTION	AllowBuildDwelling5(const S_CASTLE *);
BUILDACTION	AllowBuildDwelling6(const S_CASTLE *);
BUILDACTION	AllowRecrutHeroes(const S_CASTLE *);

BOOL		BuildWel2(const S_CASTLE *);
BOOL		BuildSpec(const S_CASTLE *);
BOOL		BuildShipyard(const S_CASTLE *);
BOOL		BuildWell(const S_CASTLE *);
BOOL		BuildMoat(const S_CASTLE *);
BOOL		BuildTavern(const S_CASTLE *);
BOOL		BuildStatue(const S_CASTLE *);
BOOL		BuildMarketplace(const S_CASTLE *);
BOOL		BuildThievesGuild(const S_CASTLE *);
BOOL		BuildRightTurret(const S_CASTLE *);
BOOL		BuildLeftTurret(const S_CASTLE *);
BOOL		BuildMageGuild(const S_CASTLE *);
BOOL		BuildCaptain(const S_CASTLE *);
BOOL		BuildCastle(const S_CASTLE *);

BOOL		BuildDwelling1(const S_CASTLE *);
BOOL		BuildDwelling2(const S_CASTLE *);
BOOL		BuildDwelling3(const S_CASTLE *);
BOOL		BuildDwelling4(const S_CASTLE *);
BOOL		BuildDwelling5(const S_CASTLE *);
BOOL		BuildDwelling6(const S_CASTLE *);
BOOL		BuildUpgrade2(const S_CASTLE *);
BOOL		BuildUpgrade3(const S_CASTLE *);
BOOL		BuildUpgrade4(const S_CASTLE *);
BOOL		BuildUpgrade5(const S_CASTLE *);
BOOL		BuildUpgrade6(const S_CASTLE *);
BOOL		BuildUpgrade7(const S_CASTLE *);

BOOL		BuildingIsDwelling(E_BUILDINGCASTLE);

void		EnableCastleUpdateBuilding(void);
E_BUILDINGCASTLE GetFlagUpdateBuilding(void);

ACTION          UpgradableArmy(const S_ARMY *, E_COLORS, const S_CASTLE *);
void		UpgradeArmy(S_ARMY *army, E_COLORS color);

void		DrawAmountedCastle(SDL_Surface *, SDL_Rect *, E_RACE, E_GROUND, BOOL);
void		ResetCastleSelector(void);
const S_CASTLE *HeroesInCastle(const S_HEROES *);

Uint8		GetCastleAlphaBlending(void);

#endif
