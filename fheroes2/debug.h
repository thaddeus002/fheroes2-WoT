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

#ifndef _DEBUG_H
#define _DEBUG_H

#include "SDL.h"
#include "object.h"
#include "mp2maps.h"
#include "monster.h"
#include "castle.h"

const char * GetStringObject(E_OBJECT);
const char * GetStringRace(E_RACE);
const char * GetStringGameLevel(E_GAMELEVEL);
const char * GetStringColor(E_COLORS);
const char * GetStringLuck(E_LUCK);
const char * GetStringDescriptionsLuck(E_LUCK);
const char * GetStringMorale(E_MORALE);
const char * GetStringDescriptionsMorale(E_MORALE);
const char * GetStringSpeed(E_MONSTERSPEED);
const char * GetStringGround(E_GROUND);
const char * GetStringDwelling(E_RACE, E_DWELLINGCASTLE);
const char * GetStringBuilding(E_RACE, E_BUILDINGCASTLE);
const char * GetStringDescriptionsBuilding(E_RACE, E_BUILDINGCASTLE);
const char * GetStringResource(E_RESOURCE);

const char * GetStringFormation(E_ARMYFORMAT);
const char * GetStringDescriptionsFormation(E_ARMYFORMAT);

const char * GetStringLevelSkill(E_LEVELSKILL);
const char * GetStringSkill(E_SKILL);
const char * GetStringDescriptionsSkill(E_SKILL, E_LEVELSKILL);

const char * GetStringLevelMageGuild(E_MAGICLEVEL level);
const char * GetStringDepenceDwelling(E_RACE, E_DWELLINGCASTLE);
const char * GetStringMonsterFromDwelling(E_RACE, E_DWELLINGCASTLE);

const char * GetStringAttackSkill(void);
const char * GetStringDefenseSkill(void);
const char * GetStringSpellPower(void);
const char * GetStringKnowledge(void);

const char * GetStringDescriptionsAttackSkill(void);
const char * GetStringDescriptionsDefenseSkill(void);
const char * GetStringDescriptionsSpellPower(void);
const char * GetStringDescriptionsKnowledge(void);

const char * GetStringSpell(E_SPELL);
const char * GetStringDescriptionsSpell(E_SPELL);

void         PrintCellInfo(Uint16);

#endif
