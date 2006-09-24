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

#ifndef _SPELL_H
#define _SPELL_H

#include "SDL.h"
#include "gamedefs.h"
#include "actionevent.h"

#define SPELLMAXCOUNT		66
#define COMBATSPELL_LEVEL1	10
#define COMBATSPELL_LEVEL2	7
#define COMBATSPELL_LEVEL3	13
#define COMBATSPELL_LEVEL4	10
#define COMBATSPELL_LEVEL5	8
#define ADVENTURESPELL_LEVEL1	2
#define ADVENTURESPELL_LEVEL2	4
#define ADVENTURESPELL_LEVEL3	3
#define ADVENTURESPELL_LEVEL4	6
#define ADVENTURESPELL_LEVEL5	2

typedef enum {
	    MAGIC_NONE = 0,
	    MAGIC_LEVEL1 = 1,
	    MAGIC_LEVEL2 = 2,
	    MAGIC_LEVEL3 = 3,
	    MAGIC_LEVEL4 = 4,
	    MAGIC_LEVEL5 = 5

	    } E_MAGICLEVEL;

typedef enum { ADVENTURE, COMBAT } E_SPELLTYPE;
	    
typedef enum {
		SPELLNONE,
		SPELLSLOW,
		MASSCURE,
		CURSE,
		LIGHTNINGBOLT,
		CHAINLIGHTNING,
		CURE,
		BLESS,
		FIREBALL,
		FIREBLAST,
		TELEPORT,
		ELEMENTALSTORM,
		RESURRECT,
		RESURRECTTRUE,
		HASTE,
		SHIELD,
		ARMAGEDDON,
		ANTIMAGIC,
		DISPELMAGIC,
		BERZERKER,
		PARALYZE,
		BLIND,
		HOLYWORD,
		HOLYSHOUT,
		METEORSHOWER,
		ANIMATEDEAD,
		MIRRORIMAGE,
		BLOODLUST,
		DEATHRIPPLE,
		DEATHWAVE,
		STEELSKIN,
		STONESKIN,
		DRAGONSLAYER,
		EARTHQUAKE,
		DISRUPTINGRAY,
		COLDRAY,
		COLDRING,
		HYPNOTIZE,
		MAGICARROW,
		VIEWMINES,
		VIEWRESOURCES,
		VIEWARTIFACTS,
		VIEWTOWNS,
		VIEWHEROES,
		VIEWALL,
		IDENTIFYHERO,
		SUMMONBOAT,
		DIMENSIONDOOR,
		TOWNGATE,
		TOWNPORTAL,
		VISIONS,
		HAUNT,
		EARTHGUARDIAN,
		AIRGUARDIAN,
		FIREGUARDIAN,
		WATERGUARDIAN,
		EARTHSUMMON,
		AIRSUMMON,
		FIRESUMMON,
		WATERSUMMON,
		
		// далее магия без оригинальных картинок
		MASSBLESS,
		MASSCURSE,
		MASSDISPEL,
		MASSHASTE,
		MASSSHIELD,
		MASSSLOW

	    } E_SPELL;

typedef struct {
	    E_SPELLTYPE		type;
	    E_MAGICLEVEL	level;
	    Uint8		cost;

	    } S_SPELL;

BOOL	     InitSpell(void);
void	     FreeSpell(void);
E_MAGICLEVEL GetLevelSpellMagic(E_SPELL);
Uint8 	     GetCostSpellMagic(E_SPELL);

void	     FillSpellLevel1(E_RACE, E_SPELL *);
void	     FillSpellLevel2(E_RACE, E_SPELL *);
void	     FillSpellLevel3(E_RACE, E_SPELL *);
void	     FillSpellLevel4(E_RACE, E_SPELL *);
void	     FillSpellLevel5(E_RACE, E_SPELL *);

ACTION	     ShowSpellInfo(E_SPELL, Uint32);

#endif
