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

#define BUILD_CASTLE_GOLD		5000
#define BUILD_CASTLE_WOOD		20
#define BUILD_CASTLE_ORE		20
#define BUILD_MAGEGUILD1_GOLD		2000
#define BUILD_MAGEGUILD1_ORE		5
#define BUILD_MAGEGUILD1_WOOD		5
#define BUILD_MAGEGUILD2_GOLD		1000
#define BUILD_MAGEGUILD2_ORE		5
#define BUILD_MAGEGUILD2_WOOD		5
#define BUILD_MAGEGUILD2_CRYSTAL	4
#define BUILD_MAGEGUILD2_GEMS		4
#define BUILD_MAGEGUILD2_MERCURY	4
#define BUILD_MAGEGUILD2_SULFUR		4
#define BUILD_MAGEGUILD3_GOLD		1000
#define BUILD_MAGEGUILD3_ORE		5
#define BUILD_MAGEGUILD3_WOOD		5
#define BUILD_MAGEGUILD3_CRYSTAL	6
#define BUILD_MAGEGUILD3_GEMS		6
#define BUILD_MAGEGUILD3_MERCURY	6
#define BUILD_MAGEGUILD3_SULFUR		6
#define BUILD_MAGEGUILD4_GOLD		1000
#define BUILD_MAGEGUILD4_ORE		5
#define BUILD_MAGEGUILD4_WOOD		5
#define BUILD_MAGEGUILD4_CRYSTAL	8
#define BUILD_MAGEGUILD4_GEMS		8
#define BUILD_MAGEGUILD4_MERCURY	8
#define BUILD_MAGEGUILD4_SULFUR		8
#define BUILD_MAGEGUILD5_GOLD		1000
#define BUILD_MAGEGUILD5_ORE		5
#define BUILD_MAGEGUILD5_WOOD		5
#define BUILD_MAGEGUILD5_CRYSTAL	10
#define BUILD_MAGEGUILD5_GEMS		10
#define BUILD_MAGEGUILD5_MERCURY	10
#define BUILD_MAGEGUILD5_SULFUR		10
#define BUILD_TAVERN_GOLD		500
#define BUILD_TAVERN_WOOD		5
#define BUILD_THIEVESGUILD_GOLD		750
#define BUILD_THIEVESGUILD_WOOD		5
#define BUILD_SHIPYARD_GOLD		2000
#define BUILD_SHIPYARD_WOOD		20
#define BUILD_STATUE_GOLD		1250
#define BUILD_STATUE_ORE		5
#define BUILD_MARKETPLACE_GOLD		500
#define BUILD_MARKETPLACE_WOOD		5
#define BUILD_WELL_GOLD			500
#define BUILD_MOAT_GOLD			750
#define BUILD_LEFTTURRET_GOLD		1500
#define BUILD_LEFTTURRET_ORE		5
#define BUILD_RIGHTTURRET_GOLD		1500
#define BUILD_RIGHTTURRET_ORE		5
#define BUILD_CAPTAIN_GOLD		500
#define BUILD_BOAT_GOLD			1000
#define BUILD_BOAT_WOOD			10
#define BUILD_WEL2_GOLD			1000
// spec
#define BUILD_FORTIFICATION_GOLD	1500
#define BUILD_FORTIFICATION_ORE		15
#define BUILD_FORTIFICATION_WOOD	5
#define BUILD_COLISEUM_GOLD		2000
#define BUILD_COLISEUM_WOOD		10
#define BUILD_COLISEUM_ORE		10
#define BUILD_STORM_GOLD		1000
#define BUILD_STORM_MERCURY		10
#define BUILD_STORM_SULFUR		10
#define BUILD_RAINBOW_GOLD		1500
#define BUILD_RAINBOW_CRYSTAL		10
#define BUILD_DUNGEON_GOLD		3000
#define BUILD_DUNGEON_ORE		10
#define BUILD_DUNGEON_WOOD		5
#define BUIILD_LIBRARY_GOLD		1500
#define BUIILD_LIBRARY_WOOD		5
#define BUIILD_LIBRARY_ORE		5
#define BUIILD_LIBRARY_CRYSTAL		5
#define BUIILD_LIBRARY_GEMS		5
#define BUIILD_LIBRARY_MERCURY		5
#define BUIILD_LIBRARY_SULFUR		5
// dwelling level 1 
#define BUILD_THATCHEDHUT_GOLD          200 
#define BUILD_EXCAVATION_GOLD           400 
#define BUILD_HUT_GOLD                  300 
#define BUILD_TREEHOUSE_GOLD            500 
#define BUILD_TREEHOUSE_WOOD            5 
#define BUILD_HABITAT_GOLD              400 
#define BUILD_CAVE_GOLD                 500 
// dwelling level 2
#define BUILD_ARCHERYRANGE_GOLD		1000
#define BUILD_STICKHUT_GOLD		800
#define BUILD_STICKHUT_WOOD		5
#define BUILD_GRAVEYARD_GOLD		1000
#define BUILD_COTTAGE_GOLD		1000
#define BUILD_COTTAGE_WOOD		5
#define BUILD_PEN_GOLD			800
#define BUILD_CRYPT_GOLD		1000
#define BUILD_CRYPT_ORE			10
// dwelling level 3
#define BUILD_BLACKSMITH_GOLD		1000
#define BUILD_BLACKSMITH_ORE		5
#define BUILD_DEN_GOLD			1000
#define BUILD_NECPYRAMID_GOLD		1500
#define BUILD_NECPYRAMID_ORE		10
#define BUILD_ELVESRANGE_GOLD		1500
#define BUILD_FOUNDRY_GOLD		1500
#define BUILD_FOUNDRY_WOOD		5
#define BUILD_FOUNDRY_ORE		5
#define BUILD_NEST_GOLD			2000
// dwelling level 4
#define BUILD_ARMORY_GOLD		2000
#define BUILD_ARMORY_WOOD		10
#define BUILD_ARMORY_ORE		10
#define BUILD_ADOBE_GOLD		2000
#define BUILD_ADOBE_WOOD		10
#define BUILD_ADOBE_ORE			10
#define BUILD_MANSION_GOLD		3000
#define BUILD_MANSION_WOOD		10
#define BUILD_STONEHENGE_GOLD		1500
#define BUILD_STONEHENGE_ORE		10
#define BUILD_CLIFFNEST_GOLD		3000
#define BUILD_CLIFFNEST_WOOD		5
#define BUILD_MAZE_GOLD			3000
#define BUILD_MAZE_GEMS			10
// dwelling level 5
#define BUILD_ARENA_GOLD		3000
#define BUILD_ARENA_WOOD		20
#define BUILD_BRIDGE_GOLD		4000
#define BUILD_BRIDGE_ORE		20
#define BUILD_MAUSOLEUM_GOLD		4000
#define BUILD_MAUSOLEUM_WOOD		10
#define BUILD_MAUSOLEUM_SULFUR		10
#define BUILD_FENCED_GOLD		3000
#define BUILD_FENCED_WOOD		10
#define BUILD_FENCED_GEMS		10
#define BUILD_IVORYTOWER_GOLD		3500
#define BUILD_IVORYTOWER_WOOD		5
#define BUILD_IVORYTOWER_ORE		5
#define BUILD_IVORYTOWER_CRYSTAL	5
#define BUILD_IVORYTOWER_MERCURY	5
#define BUILD_IVORYTOWER_SULFUR		5
#define BUILD_IVORYTOWER_GEMS		5
#define BUILD_SWAMP_GOLD		4000
#define BUILD_SWAMP_SULFUR		10
// dwelling level 6
#define BUILD_CATHEDRAL_GOLD		5000
#define BUILD_CATHEDRAL_WOOD		20
#define BUILD_CATHEDRAL_CRYSTAL		20
#define BUILD_PYRAMID_GOLD		6000
#define BUILD_PYRAMID_ORE		20
#define BUILD_PYRAMID_CRYSTAL		20
#define BUILD_LABORATORY_GOLD		10000
#define BUILD_LABORATORY_WOOD		10
#define BUILD_LABORATORY_ORE		10
#define BUILD_LABORATORY_CRYSTAL	5
#define BUILD_LABORATORY_SULFUR		5
#define BUILD_LABORATORY_MERCURY	5
#define BUILD_LABORATORY_GEMS		5
#define BUILD_REDTOWER_GOLD		10000
#define BUILD_REDTOWER_ORE		30
#define BUILD_REDTOWER_MERCURY		20
#define BUILD_CLOUDCASTLE_GOLD		12500
#define BUILD_CLOUDCASTLE_ORE		5
#define BUILD_CLOUDCASTLE_WOOD		5
#define BUILD_CLOUDCASTLE_GEMS		20
#define BUILD_GREENDRAGON_GOLD		15000
#define BUILD_GREENDRAGON_ORE		30
#define BUILD_GREENDRAGON_SULFUR	20
// upgrade level 2
#define UPGRADE_ARCHERYRANGE_GOLD	1500
#define UPGRADE_ARCHERYRANGE_WOOD	5
#define UPGRADE_STICKHUT_GOLD		1200
#define UPGRADE_STICKHUT_WOOD		5
#define UPGRADE_GRAVEYARD_GOLD		1000
#define UPGRADE_COTTAGE_GOLD		1500
#define UPGRADE_COTTAGE_WOOD		5
// upgrade level 3
#define UPGRADE_BLACKSMITH_GOLD		1500
#define UPGRADE_BLACKSMITH_ORE		5
#define UPGRADE_NECPYRAMID_GOLD		1500
#define UPGRADE_NECPYRAMID_ORE		5
#define UPGRADE_ELVESRANGE_GOLD		1500
#define UPGRADE_ELVESRANGE_WOOD		5
#define UPGRADE_FOUNDRY_GOLD		1500
#define UPGRADE_FOUNDRY_MERCURY		5
// upgrade level 4
#define UPGRADE_ARMORY_GOLD		2000
#define UPGRADE_ARMORY_WOOD		5
#define UPGRADE_ARMORY_ORE		5
#define UPGRADE_ADOBE_GOLD		3000
#define UPGRADE_ADOBE_WOOD		5
#define UPGRADE_ADOBE_ORE		5
#define UPGRADE_MANSION_GOLD		4000
#define UPGRADE_MANSION_WOOD		5
#define UPGRADE_MANSION_CRYSTAL		10
#define UPGRADE_MANSION_GEMS		10
#define UPGRADE_STONEHENGE_GOLD		1500
#define UPGRADE_STONEHENGE_MERCURY	5
#define UPGRADE_MAZE_GOLD		2000
#define UPGRADE_MAZE_GEMS		5
// upgrade level 5
#define UPGRADE_ARENA_GOLD		3000
#define UPGRADE_ARENA_WOOD		10
#define UPGRADE_BRIDGE_GOLD		2000
#define UPGRADE_BRIDGE_ORE		10
#define UPGRADE_MAUSOLEUM_GOLD		3000
#define UPGRADE_MAUSOLEUM_ORE		5
#define UPGRADE_MAUSOLEUM_CRYSTAL	5
#define UPGRADE_IVORYTOWER_GOLD		4000
#define UPGRADE_IVORYTOWER_WOOD		5
#define UPGRADE_IVORYTOWER_ORE		5
// upgrade level 6
#define UPGRADE_CATHEDRAL_GOLD		5000
#define UPGRADE_CATHEDRAL_WOOD		10
#define UPGRADE_CATHEDRAL_CRYSTAL	10
#define UPGRADE_CLOUDCASTLE_GOLD	12500
#define UPGRADE_CLOUDCASTLE_ORE		5
#define UPGRADE_CLOUDCASTLE_WOOD	5
#define UPGRADE_CLOUDCASTLE_GEMS	20
#define UPGRADE_REDDRAGON_GOLD		5000
#define UPGRADE_REDDRAGON_ORE		5
#define UPGRADE_REDDRAGON_SULFUR	10
#define UPGRADE_BLACKDRAGON_GOLD	5000
#define UPGRADE_BLACKDRAGON_ORE		5
#define UPGRADE_BLACKDRAGON_SULFUR	10
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
                E_MAGICLEVEL level;
                E_SPELL level5[CASTLESPELL_LEVEL5];
                E_SPELL level4[CASTLESPELL_LEVEL4];
                E_SPELL level3[CASTLESPELL_LEVEL3];
                E_SPELL level2[CASTLESPELL_LEVEL2];
                E_SPELL level1[CASTLESPELL_LEVEL1];

                } S_MAGEGUILD;
                                                                                                                
typedef enum {
		BUILD_THIEVEGUILD	= 0x0002,
		BUILD_TAVERN		= 0x0004,
		BUILD_SHIPYARD		= 0x0008,
		BUILD_WELL		= 0x0010,
		BUILD_STATUE		= 0x0080,
		BUILD_LEFTTURRET	= 0x0100,
		BUILD_RIGHTTURRET	= 0x0200,
		BUILD_MARKETPLACE	= 0x0400,
		BUILD_WEL2		= 0x0800,	// Farm, Garbage He, Crystal Gar, Waterfall, Orchard, Skull Pile
		BUILD_MOAT		= 0x1000,
		BUILD_SPEC		= 0x2000,	// Fortification, Coliseum, Rainbow, Dungeon, Library, Storm
		BUILD_CAPTAIN		= 0x4000

	    } E_BUILDINGCASTLE;

typedef enum {
                DWELLING_MONSTER1       = 0x0008,
                DWELLING_MONSTER2       = 0x0010,
                DWELLING_MONSTER3       = 0x0020,
                DWELLING_MONSTER4       = 0x0040,
                DWELLING_MONSTER5       = 0x0080,
                DWELLING_MONSTER6       = 0x0100,
                DWELLING_UPGRADE2       = 0x0200,
        	DWELLING_UPGRADE3       = 0x0400,
                DWELLING_UPGRADE4       = 0x0800,
                DWELLING_UPGRADE5       = 0x1000,
                DWELLING_UPGRADE6       = 0x2000,
                DWELLING_UPGRADE7       = 0x4000        // black dragon

            } E_DWELLINGCASTLE;

typedef struct {

    E_COLORS	color;
    E_RACE	race;
    char	name[13];
    Uint8	ax;
    Uint8	ay;
    SDL_Rect	pos;
    Uint16	building;
    Uint16	dwelling;
    S_MAGEGUILD	mageGuild;
    Uint16	monster[CASTLEMAXMONSTER];
    S_ARMY	army[CASTLEMAXARMY];
    BOOL	capitan;
    E_ARMYFORMAT    af;                             // army format
    BOOL	castle;
    BOOL	allowCastle;
    BOOL	boat;
    void	*next;

} S_CASTLE;

BOOL		AddCastle(FILE *, Uint8, Uint8, Uint8);
void		FreeCastle(void);
E_RACE		GetRaceRNDCastle(Uint8, Uint8);
S_CASTLE	*GetStatCastlePos(Uint8, Uint8);
S_CASTLE	*GetStatCastle(Uint8);
const S_CASTLE	*GetCurrentCastle(void);

void		EnterCastle(Uint8, Uint8, E_NAMEHEROES);

S_CASTLE	*GetFirstCastle(E_COLORS);
S_CASTLE 	*GetNextCastle(E_COLORS);
S_CASTLE 	*GetPrevCastle(E_COLORS);
S_CASTLE 	*GetEndCastle(E_COLORS);
Uint8     	GetCountCastle(E_COLORS);
E_MONSTER	GetMonsterFromCastle(const S_CASTLE *, Uint8);
BOOL		BuyMonsterFromCastle(const S_CASTLE *, Uint8, Uint16);
BOOL		CheckBuyMonsterFromCastle(const S_CASTLE *, Uint8, Uint16);
BOOL		CastleDwellingUpgradable(const S_CASTLE *, E_DWELLINGCASTLE);
BOOL		CastleNearOcean(const S_CASTLE *);
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

void		EnableCastleUpdateBuilding(void);

Uint8           GetMonsterGrown(const S_CASTLE *, E_MONSTER);

#endif

