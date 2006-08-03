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

#ifndef _HEROES_H
#define _HEROES_H

#include "SDL.h"
#include "artifact.h"
#include "monster.h"
#include "actionevent.h"
#include "heroes.h"
#include "gamedefs.h"

#define HEROESMAXSKILL		8
#define HEROESMAXARTIFACT	14
#define HEROESMAXARMY		5
#define HEROESMAXCOUNT		60	// количество портретов

#define SCOUTINGBASE		4	// стартовое количество клеток обзора у героя

typedef enum {
		BASIC,
		ADVANCED,
		EXPERT,
		LEVELNONE

	    } E_LEVELSKILL;

typedef enum {
		ARCHERY,
		BALLISTICS,
		DIPLOMACY,
		EAGLEEYE,
		ESTATES,
		LEADERSHIP,
		LOGISTICS,
		LUCK,
		MYSTICISM,
		NAVIGATION,
		NECROMANCY,
		PATHFINDING,
		SCOUTING,
		WISDOM,
		SKILLNONE

	    } E_SKILL;

typedef enum {
		// knight
		LORDKILBURN	= 0,
		SIRGALLANTH	= 1,
		ECTOR		= 2,
		GVENNETH	= 3,
		TYRO		= 4,
		AMBROSE		= 5,
		RUBY		= 6,
		MAXIMUS		= 7,
		DIMITRY		= 8,

		// barbarian
		THUNDAX		= 9,
		FINEOUS		= 10,
		JOJOSH		= 11,
		CRAGHACK	= 12,
		JEZEBEL		= 13,
		JACLYN		= 14,
		ERGON		= 15,
		TSABU		= 16,
		ATLAS		= 17,

		// sorceress
		ASTRA		= 18,
		NATASHA		= 19,
		TROYAN		= 20,
		VATAWNA		= 21,
		REBECCA		= 22,
		GEM		= 23,
		ARIEL		= 24,
		CARLAWN		= 25,
		LUNA		= 26,

		// warlock
		ARIE		= 27,
		ALAMAR		= 28,
		VESPER		= 29,
		CRODO		= 30,
		BAROK		= 31,
		KASTORE		= 32,
		AGAR		= 33,
		FALAGAR		= 34,
		WRATHMONT	= 35,

		// wizard
		MYRA		= 36,
		FLINT		= 37,
		DAWN		= 38,
		HALON		= 39,
		MYRINI		= 40,
		WILFREY		= 41,
		SARAKIN		= 42,
		KALINDRA	= 43,
		MANDIGAL	= 44,

		// necromancer
		ZOM		= 45,
		DARLANA		= 46,
		ZAM		= 47,
		RANLOO		= 48,
		CHARITY		= 49,
		RIALDO		= 50,
		ROXANA		= 51,
		SANDRO		= 52,
		CELIA		= 53,

		// герои из компаний
		ROLAND		= 54,
		NAME_UNK2	= 55,
		NAME_UNK3	= 56,
		ARCHIBALD	= 57,
		NAME_UNK5	= 58,
		SANDYSANDY	= 59,

		HEROESNULL	= 0xFF

	    } E_NAMEHEROES;

typedef enum {
		SPREAD,
		GROUPED
	    } E_ARMYFORMAT;

typedef struct {
		E_SKILL		type;
		E_LEVELSKILL	level;
	    } S_SKILL;

typedef struct { 
		E_RACE          race;
		const char *	name;
		SDL_Surface *	face;
		E_COLORS	color;
		Uint8           attack;
		Uint8           defence;
		Uint8           power;
		Uint8           knowledge;
		E_MORALE        morale;
		E_LUCK          luck;
		Uint32          experience;
		Uint16          magicPoint;
		float           movePoint;
		S_SKILL		skill[HEROESMAXSKILL];
		E_ARTIFACT	artifact[HEROESMAXARTIFACT];
	        S_ARMY		army[HEROESMAXARMY];
		E_ARMYFORMAT	af;				// army format
		Uint8		ax;
		Uint8		ay;
	    } S_HEROES;

BOOL		InitHeroes(void);
void		FreeHeroes();

Uint8		CalculateHeroesAttack(S_HEROES *);
Uint8		CalculateHeroesDefence(S_HEROES *);
Uint8		CalculateHeroesPower(S_HEROES *);
Uint8		CalculateHeroesKnowledge(S_HEROES *);
E_MORALE	CalculateHeroesMorale(S_HEROES *);
E_LUCK		CalculateHeroesLuck(S_HEROES *);
Uint16		CalculateHeroesMagicPoint(S_HEROES *);
Uint8		CalculateHeroesMoveLandPoint(S_HEROES *);
Uint8		CalculateHeroesMoveSeaPoint(S_HEROES *);
Uint8		CalculateHeroesScouting(S_HEROES *);

BOOL		HeroesArtifactPresent(S_HEROES *, E_ARTIFACT);
E_LEVELSKILL	HeroesLevelSkill(S_HEROES *, E_SKILL);
void		HeroesDefaultValues(S_HEROES *, E_RACE);
Uint8		HeroesCountArmy(S_HEROES *);
S_HEROES       *GetStatHeroes(E_NAMEHEROES);
ACTION		ShowArmyInfo(S_ARMY *, S_HEROES *);

const char *	HeroesBigNamePortrait(E_NAMEHEROES);
const char *	CapitanBigNamePortrait(E_RACE);

#endif
