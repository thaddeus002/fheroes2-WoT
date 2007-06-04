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

#include "gamedefs.h"
#include "skill.h"

void Skill::SetSkill(u8 skill)
{
    ESTATES >= skill ? pair.first = static_cast<skill_t>(skill) : pair.first = NONE;
}

void Skill::SetLevel(u8 level)
{
    pair.second = static_cast<level_t>((level - 1) % 3);
}

const std::string & Skill::LevelString(level_t level)
{
    static const std::string str_level[] = { "Basic", "Advanced", "Expert" };

    switch(level)
    {
	case BASIC:	return str_level[0];
	case ADVANCED:	return str_level[1];
	case EXPERT:	return str_level[2];
    }
    
    return str_level[0];
}

const std::string & Skill::String(skill_t skill)
{
    static const std::string str_skill[] = { "Pathfinding", "Archery", "Logistics", "Scouting", "Diplomacy", "Navigation", 
	"Leadership", "Wisdom", "Mysticism", "Luck", "Ballistics", "Eagle Eye", "Necromancy", "Estates" };

    switch(skill)
    {
	case PATHFINDING:	return str_skill[0];
        case ARCHERY:		return str_skill[1];
        case LOGISTICS:		return str_skill[2];
        case SCOUTING:		return str_skill[3];
        case DIPLOMACY:		return str_skill[4];
        case NAVIGATION:	return str_skill[5];
        case LEADERSHIP:	return str_skill[6];
        case WISDOM:		return str_skill[7];
        case MYSTICISM:		return str_skill[8];
        case LUCK:		return str_skill[9];
        case BALLISTICS:	return str_skill[10];
        case EAGLEEYE:		return str_skill[11];
        case NECROMANCY:	return str_skill[12];
	case ESTATES:		return str_skill[13];
	
	default: break;
    }

    return str_skill[0];
}

const std::string & Skill::Description(skill_t skill, level_t level)
{
    static const std::string description_skill[] = {
	"Basic Pathfinding reduces the movement penalty for rough terrain by 25 percent.",
	"Advanced Pathfinding reduces the movement penalty for rough terrain by 50 percent.",
	"Expert Pathfinding eliminates the movement penalty for rough terrain.",

	"Basic Archery increases the damage done by range attacking creatures by 10 percent.",
	"Advanced Archery increases the damage done by range attacking creatures by 25 percent.",
	"Expert Archery increases the damage done by range attacking creatures by 50 percent.",

	"Basic Logistics increases your hero's movement points by 10 percent.",
	"Advanced Logistics increases your hero's movement points by 20 percent.",
	"Expert Logistics increases your hero's movement points by 30 percent.",

	"Basic Scouting increases your hero's viewable area by 1 square.",
	"Advanced Scouting increases your hero's viewable area by 2 squares.",
	"Expert Scouting increases your hero's viewable area by 3 squares.",

	"Basic Diplomacy allows you to negotiate with monsters who are weaker than your group.  Approximately 1/4 of the creatures may offer to join you.",
	"Advanced Diplomacy allows you to negotiate with monsters who are weaker than your group.  Approximately 1/2 of the creatures may offer to join you.",
	"Expert Diplomacy allows you to negotiate with monsters who are weaker than your group.  All of the creatures may offer to join you.",

	"Basic Navigation increases your hero's movement points over water by 1/3.",
	"Advanced Navigation increases your hero's movement points over water by 2/3.",
	"Expert Navigation doubles your hero's movement points over water.",

	"Basic Leadership increases your hero's troops' morale by 1.",
	"Advanced Leadership increases your hero's troops' morale by 2.",
	"Expert Leadership increases your hero's troops' morale by 3.",

	"Basic Wisdom allows your hero to learn third level spells.",
	"Advanced Wisdom allows your hero to learn fourth level spells.",
	"Expert Wisdom allows your hero to learn fifth level spells.",

	"Basic Mysticism regenerates two of your hero's spell points per day.",
	"Advanced Mysticism regenerates three of your hero's spell points per day.",
	"Expert Mysticism regenerates four of your hero's spell points per day.",

	"Basic Luck increases your hero's luck by 1.",
	"Advanced Luck increases your hero's luck by 2.",
	"Expert Luck increases your hero's luck by 3.",

	"Basic Ballistics gives your hero's catapult shots a greater chance to hit and do damage to castle walls.",
	"Advanced Ballistics gives your hero's catapult an extra shot, and each shot has a greater chance to hit and do damage to castle walls.",
	"Expert Ballistics gives your hero's catapult an extra shot, and each shot automatically destroys any wall, except a fortified wall in a Knight town.",

	"Basic Eagle Eye gives your hero a 20 percent chance to learn any given 1st or 2nd level enemy spell used against him in a combat.",
	"Advanced Eagle Eye gives your hero a 30 percent chance to learn any given 3rd level spell (or below) used against him in combat.",
	"Expert Eagle Eye gives your hero a 40 percent chance to learn any given 4th level spell (or below) used against him in combat.",

	"Basic Necromancy allows 10 percent of the creatures killed in combat to be brought back from the dead as Skeletons.",
	"Advanced Necromancy allows 20 percent of the creatures killed in combat to be brought back from the dead as Skeletons.",
	"Expert Necromancy allows 30 percent of the creatures killed in combat to be brought back from the dead as Skeletons.",

	"Your hero produces 100 gold pieces per turn as tax revenue from estates.",
	"Your hero produces 250 gold pieces per turn as tax revenue from estates.",
	"Your hero produces 500 gold pieces per turn as tax revenue from estates." };

    u8 index = 0;

    switch(level)
    {
	case BASIC:	index = 0; break;
	case ADVANCED:	index = 1; break;
	case EXPERT:	index = 2; break;
    }
    switch(skill)
    {
	case PATHFINDING:			break;
        case ARCHERY:		index +=  3;	break;
        case LOGISTICS:		index +=  6;	break;
        case SCOUTING:		index +=  9;	break;
        case DIPLOMACY:		index += 12;	break;
        case NAVIGATION:	index += 15;	break;
        case LEADERSHIP:	index += 18;	break;
        case WISDOM:		index += 21;	break;
        case MYSTICISM:		index += 24;	break;
        case LUCK:		index += 27;	break;
        case BALLISTICS:	index += 30;	break;
        case EAGLEEYE:		index += 33;	break;
        case NECROMANCY:	index += 36;	break;
        case ESTATES:		index += 39;	break;
	
	default: break;
    }
    
    return description_skill[index];
}
