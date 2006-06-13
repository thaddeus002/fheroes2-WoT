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
#include "gamedefs.h"

#define HEROESMAXSKILL		8
#define HEROESMAXARTIFACT	14
#define HEROESMAXARMY		5

#define SCOUTINGBASE		4	// стартовое количество клеток обзора у героя

typedef enum {
		LNONE,
		BASIC,
		ADVANCED,
		EXPERT

	    } E_LEVELSKILL;

typedef enum {
		SNONE,
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

	    } E_SKILL;

typedef enum {
		BARBARIAN, 
		KNIGHT, 
		NECROMANCER, 
		SORCERESS, 
		WARLOCK, 
		WIZARD

	    } E_RACE;

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
		Uint8		posx;
		Uint8		posy;
	    } S_HEROES;

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

#endif
