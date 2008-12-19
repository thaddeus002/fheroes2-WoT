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
#include "race.h"
#include "skill.h"

Skill::Primary::Primary() : attack(0), defence(0), power(0), knowledge(0)
{
}

// primary skill from level up (dependence from race and hero level)
Skill::Primary::skill_t Skill::Primary::FromLevelUp(const u8 race, const u8 level)
{
    Rand::Queue percents(MAXPRIMARYSKILL);

    // primary skill
    switch(race)
    {
	case Race::BARB:
	    if(10 > level)
	    {
		percents.Push(55); percents.Push(35); percents.Push(5); percents.Push(5);
	    }
	    else
	    {
		percents.Push(30); percents.Push(30); percents.Push(20); percents.Push(20);
	    }
	    break;

	case Race::KNGT:
	    if(10 > level)
	    {
		percents.Push(35); percents.Push(45); percents.Push(10); percents.Push(10);
	    }
	    else
	    {
		percents.Push(25); percents.Push(25); percents.Push(25); percents.Push(25);
	    }
	    break;

	case Race::NECR:
	    if(10 > level)
	    {
		percents.Push(15); percents.Push(15); percents.Push(35); percents.Push(35);
	    }
	    else
	    {
		percents.Push(25); percents.Push(25); percents.Push(25); percents.Push(25);
	    }
	    break;

	case Race::SORC:
	    if(10 > level)
	    {
		percents.Push(10); percents.Push(10); percents.Push(30); percents.Push(50);
	    }
	    else
	    {
		percents.Push(20); percents.Push(20); percents.Push(30); percents.Push(30);
	    }
	    break;

	case Race::WRLK:
	    if(10 > level)
	    {
		percents.Push(10); percents.Push(10); percents.Push(50); percents.Push(30);
	    }
	    else
	    {
		percents.Push(20); percents.Push(20); percents.Push(30); percents.Push(30);
	    }
	    break;

	case Race::WZRD:
	    if(10 > level)
	    {
		percents.Push(10); percents.Push(10); percents.Push(40); percents.Push(40);
	    }
	    else
	    {
		percents.Push(20); percents.Push(20); percents.Push(30); percents.Push(30);
	    }
	    break;

	default:
	    Error::Warning("Skill::Primary::LevelUp: unknown race."); return UNKNOWN;
    }

    switch(percents.Get())
    {
	case 0:	return ATTACK;
	case 1: return DEFENCE;
	case 2: return POWER;
	case 3: return KNOWLEDGE;

	default: break;
    }

    Error::Warning("Skill::Primary::LevelUp: unknown result.");
    return UNKNOWN;
}

const std::string & Skill::Primary::String(const Skill::Primary::skill_t skill)
{
    static const std::string str_skill[] = { "Attack", "Defence", "Power", "Knowledge", "Unknown" };

    switch(skill)
    {
	case ATTACK:	return str_skill[0];
        case DEFENCE:	return str_skill[1];
        case POWER:	return str_skill[2];
        case KNOWLEDGE:	return str_skill[3];
        default:	break;
    }

    return str_skill[4];
}

Skill::Level::type_t Skill::Level::FromMP2(const u8 byte)
{
    switch(byte)
    {
       case 0: return BASIC;
       case 1: return ADVANCED;
       case 2: return EXPERT;

	default: break;
    }

    return NONE;
}

const std::string & Skill::Level::String(const type_t level)
{
    static const std::string str_level[] = { "None", "Basic", "Advanced", "Expert" };

    switch(level)
    {
	case BASIC:	return str_level[1];
	case ADVANCED:	return str_level[2];
	case EXPERT:	return str_level[3];
	default: break;
    }
    
    return str_level[0];
}

Skill::Secondary::Secondary() : std::pair<u8, u8>(Skill::Secondary::UNKNOWN, Skill::Level::NONE)
{
}

Skill::Secondary::Secondary(const skill_t & s, const Level::type_t & t) : std::pair<u8, u8>(s, t)
{
}

void Skill::Secondary::SetSkill(const skill_t skill)
{
    if(UNKNOWN == skill) return;

    first = skill;
}

void Skill::Secondary::SetLevel(const u8 level)
{
    switch(level)
    {
	case Level::BASIC:
	case Level::ADVANCED:
	case Level::EXPERT:	second = level; break;
	default: break;
    }
}

void Skill::Secondary::NextLevel(void)
{
    switch(second)
    {
	case Level::NONE:	second = Level::BASIC; break;
	case Level::BASIC:	second = Level::ADVANCED; break;
	case Level::ADVANCED:	second = Level::EXPERT; break;
	default: break;
    }
}

Skill::Level::type_t Skill::Secondary::Level(void) const
{
    switch(second)
    {
	case Level::BASIC:	return Level::BASIC;
	case Level::ADVANCED:	return Level::ADVANCED;
	case Level::EXPERT:	return Level::EXPERT;
        default: break;
    }

    return Level::NONE;
}

Skill::Secondary::skill_t Skill::Secondary::Skill(void) const
{
    switch(first)
    {
	case PATHFINDING:return PATHFINDING;
        case ARCHERY:	return ARCHERY;
        case LOGISTICS:	return LOGISTICS;
        case SCOUTING:	return SCOUTING;
        case DIPLOMACY:	return DIPLOMACY;
        case NAVIGATION:return NAVIGATION;
        case LEADERSHIP:return LEADERSHIP;
        case WISDOM:	return WISDOM;
        case MYSTICISM:	return MYSTICISM;
        case LUCK:	return LUCK;
        case BALLISTICS:return BALLISTICS;
        case EAGLEEYE:	return EAGLEEYE;
        case NECROMANCY:return NECROMANCY;
        case ESTATES:	return ESTATES;
        default: break;
    }

    return UNKNOWN;
}

Skill::Secondary::skill_t Skill::Secondary::FromMP2(const u8 byte)
{
    switch(byte)
    {
	case 0:		return PATHFINDING;
        case 1:		return ARCHERY;
        case 2:		return LOGISTICS;
        case 3:		return SCOUTING;
        case 4:		return DIPLOMACY;
        case 5:		return NAVIGATION;
        case 6:		return LEADERSHIP;
        case 7:		return WISDOM;
        case 8:		return MYSTICISM;
        case 9:		return LUCK;
        case 10:	return BALLISTICS;
        case 11:	return EAGLEEYE;
        case 12:	return NECROMANCY;
        case 13:	return ESTATES;
        
        default: break;
    }
    
    return UNKNOWN;
}

Skill::Secondary::skill_t Skill::Secondary::RandForWitchsHut(void)
{
    switch(Rand::Get(11))
    {
	case 0:		return PATHFINDING;
        case 1:		return ARCHERY;
        case 2:		return LOGISTICS;
        case 3:		return SCOUTING;
        case 4:		return DIPLOMACY;
        case 5:		return NAVIGATION;
        case 6:		return WISDOM;
        case 7:		return MYSTICISM;
        case 8:		return LUCK;
        case 9:		return BALLISTICS;
        case 10:	return EAGLEEYE;
        case 11:	return ESTATES;

        default: break;
    }

    return UNKNOWN;
}

/* index sprite from SECSKILL */
u8 Skill::Secondary::GetIndexSprite1(const skill_t skill)
{
    switch(skill)
    {
    	case PATHFINDING:	return 1;
        case ARCHERY:		return 2;
        case LOGISTICS:		return 3;
        case SCOUTING:		return 4;
        case DIPLOMACY:		return 5;
        case NAVIGATION:	return 6;
        case LEADERSHIP:	return 7;
        case WISDOM:		return 8;
        case MYSTICISM:		return 9;
        case LUCK:		return 10;
        case BALLISTICS:	return 11;
        case EAGLEEYE:		return 12;
        case NECROMANCY:	return 13;
        case ESTATES:		return 14;

        default: break;
    }

    return 0;
}

/* index sprite from MINISS */
u8 Skill::Secondary::GetIndexSprite2(const skill_t skill)
{
    switch(skill)
    {
    	case PATHFINDING:	return 0;
        case ARCHERY:		return 1;
        case LOGISTICS:		return 2;
        case SCOUTING:		return 3;
        case DIPLOMACY:		return 4;
        case NAVIGATION:	return 5;
        case LEADERSHIP:	return 6;
        case WISDOM:		return 7;
        case MYSTICISM:		return 8;
        case LUCK:		return 9;
        case BALLISTICS:	return 10;
        case EAGLEEYE:		return 11;
        case NECROMANCY:	return 12;
        case ESTATES:		return 13;

        default: break;
    }

    Error::Warning("Skill::Secondary::GetIndexSprite2: unknown skill, index out of range");

    return 0xff;
}

const std::string & Skill::Secondary::String(const skill_t skill)
{
    static const std::string str_skill[] = { "Pathfinding", "Archery", "Logistics", "Scouting", "Diplomacy", "Navigation", 
	"Leadership", "Wisdom", "Mysticism", "Luck", "Ballistics", "Eagle Eye", "Necromancy", "Estates", "Unknown"  };

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

    return str_skill[14];
}

const std::string & Skill::Secondary::Description(const skill_t skill, const Level::type_t level)
{
    static const std::string description_skill[] =
    {
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
	case Level::BASIC:	index = 0; break;
	case Level::ADVANCED:	index = 1; break;
	case Level::EXPERT:	index = 2; break;
	default: break;
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

Skill::Secondary::skill_t Skill::Secondary::PriorityFromRace(const u8 race, const std::vector<skill_t> &exclude)
{
    if(exclude.empty()) return PriorityFromRace(race);
    
    std::vector<skill_t>::const_iterator it1 = exclude.begin();
    std::vector<skill_t>::const_iterator it2 = exclude.end();
    Rand::Queue parts(MAXSECONDARYSKILL);

    // primary skill
    switch(race)
    {
	case Race::BARB:
	    if(it2 == std::find(it1, it2, PATHFINDING))	parts.Push(4);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, ARCHERY))	parts.Push(3);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, LOGISTICS))	parts.Push(3);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, SCOUTING))	parts.Push(4);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, DIPLOMACY))	parts.Push(2);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, NAVIGATION))	parts.Push(3);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, LEADERSHIP))	parts.Push(3);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, WISDOM))	parts.Push(1);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, MYSTICISM))	parts.Push(1);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, LUCK))	parts.Push(2);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, BALLISTICS))	parts.Push(3);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, EAGLEEYE))	parts.Push(1);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, NECROMANCY))	parts.Push(0);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, ESTATES))	parts.Push(2);	else	parts.Push(0);
    	break;

	case Race::KNGT:
	    if(it2 == std::find(it1, it2, PATHFINDING))	parts.Push(3);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, ARCHERY))	parts.Push(2);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, LOGISTICS))	parts.Push(3);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, SCOUTING))	parts.Push(2);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, DIPLOMACY))	parts.Push(3);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, NAVIGATION))	parts.Push(2);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, LEADERSHIP))	parts.Push(5);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, WISDOM))	parts.Push(2);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, MYSTICISM))	parts.Push(1);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, LUCK))	parts.Push(1);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, BALLISTICS))	parts.Push(4);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, EAGLEEYE))	parts.Push(1);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, NECROMANCY))	parts.Push(0);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, ESTATES))	parts.Push(3);	else	parts.Push(0);
	    break;

	case Race::NECR:
	    if(it2 == std::find(it1, it2, PATHFINDING))	parts.Push(3);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, ARCHERY))	parts.Push(1);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, LOGISTICS))	parts.Push(2);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, SCOUTING))	parts.Push(1);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, DIPLOMACY))	parts.Push(2);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, NAVIGATION))	parts.Push(2);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, LEADERSHIP))	parts.Push(0);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, WISDOM))	parts.Push(4);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, MYSTICISM))	parts.Push(3);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, LUCK))	parts.Push(1);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, BALLISTICS))	parts.Push(3);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, EAGLEEYE))	parts.Push(3);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, NECROMANCY))	parts.Push(5);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, ESTATES))	parts.Push(2);	else	parts.Push(0);
	    break;

	case Race::SORC:
	    if(it2 == std::find(it1, it2, PATHFINDING))	parts.Push(2);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, ARCHERY))	parts.Push(3);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, LOGISTICS))	parts.Push(2);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, SCOUTING))	parts.Push(1);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, DIPLOMACY))	parts.Push(2);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, NAVIGATION))	parts.Push(4);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, LEADERSHIP))	parts.Push(1);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, WISDOM))	parts.Push(4);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, MYSTICISM))	parts.Push(3);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, LUCK))	parts.Push(3);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, BALLISTICS))	parts.Push(3);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, EAGLEEYE))	parts.Push(2);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, NECROMANCY))	parts.Push(0);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, ESTATES))	parts.Push(2);	else	parts.Push(0);
	    break;

	case Race::WRLK:
	    if(it2 == std::find(it1, it2, PATHFINDING))	parts.Push(4);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, ARCHERY))	parts.Push(1);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, LOGISTICS))	parts.Push(2);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, SCOUTING))	parts.Push(4);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, DIPLOMACY))	parts.Push(2);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, NAVIGATION))	parts.Push(2);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, LEADERSHIP))	parts.Push(1);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, WISDOM))	parts.Push(5);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, MYSTICISM))	parts.Push(3);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, LUCK))	parts.Push(1);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, BALLISTICS))	parts.Push(3);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, EAGLEEYE))	parts.Push(3);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, NECROMANCY))	parts.Push(1);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, ESTATES))	parts.Push(2);	else	parts.Push(0);
	    break;

	case Race::WZRD:
	    if(it2 == std::find(it1, it2, PATHFINDING))	parts.Push(2);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, ARCHERY))	parts.Push(1);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, LOGISTICS))	parts.Push(2);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, SCOUTING))	parts.Push(2);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, DIPLOMACY))	parts.Push(2);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, NAVIGATION))	parts.Push(2);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, LEADERSHIP))	parts.Push(2);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, WISDOM))	parts.Push(5);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, MYSTICISM))	parts.Push(4);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, LUCK))	parts.Push(2);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, BALLISTICS))	parts.Push(3);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, EAGLEEYE))	parts.Push(3);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, NECROMANCY))	parts.Push(0);	else	parts.Push(0);
	    if(it2 == std::find(it1, it2, ESTATES))	parts.Push(2);	else	parts.Push(0);
	    break;

	default:
	    Error::Warning("Skill::Secondary::PriorityForRace: unknown race."); return UNKNOWN;
    }

    switch(parts.Get())
    {
	case 0: return PATHFINDING;
	case 1: return ARCHERY;
	case 2: return LOGISTICS;
	case 3: return SCOUTING;
	case 4: return DIPLOMACY;
	case 5: return NAVIGATION;
	case 6: return LEADERSHIP;
	case 7: return WISDOM;
	case 8: return MYSTICISM;
	case 9: return LUCK;
	case 10: return BALLISTICS;
	case 11: return EAGLEEYE;
	case 12: return NECROMANCY;
	case 13: return ESTATES;

	default: break;
    }

    Error::Warning("Skill::Secondary::PriorityForRace: unknown result.");
    return UNKNOWN;
}

Skill::Secondary::skill_t Skill::Secondary::PriorityFromRace(const u8 race)
{
    Rand::Queue parts(MAXSECONDARYSKILL);

    // primary skill
    switch(race)
    {
	case Race::BARB:
	    // pathfinding
	    parts.Push(4);
	    // archery
	    parts.Push(3);
	    // logistics
	    parts.Push(3);
	    // scouting
	    parts.Push(4);
	    // diplomacy
	    parts.Push(2);
	    // navigation
	    parts.Push(3);
	    // leadership
	    parts.Push(3);
	    // wisdom
	    parts.Push(1);
	    // mysticism
	    parts.Push(1);
	    // luck
	    parts.Push(2);
	    // ballistics
	    parts.Push(3);
	    // eagleeye
	    parts.Push(1);
	    // necromancy
	    parts.Push(0);
	    // estates
	    parts.Push(2);
    	break;

	case Race::KNGT:
	    // pathfinding
	    parts.Push(3);
	    // archery
	    parts.Push(2);
	    // logistics
	    parts.Push(3);
	    // scouting
	    parts.Push(2);
	    // diplomacy
	    parts.Push(3);
	    // navigation
	    parts.Push(2);
	    // leadership
	    parts.Push(5);
	    // wisdom
	    parts.Push(2);
	    // mysticism
	    parts.Push(1);
	    // luck
	    parts.Push(1);
	    // ballistics
	    parts.Push(4);
	    // eagleeye
	    parts.Push(1);
	    // necromancy
	    parts.Push(0);
	    // estates
	    parts.Push(3);
	    break;

	case Race::NECR:
	    // pathfinding
	    parts.Push(3);
	    // archery
	    parts.Push(1);
	    // logistics
	    parts.Push(2);
	    // scouting
	    parts.Push(1);
	    // diplomacy
	    parts.Push(2);
	    // navigation
	    parts.Push(2);
	    // leadership
	    parts.Push(0);
	    // wisdom
	    parts.Push(4);
	    // mysticism
	    parts.Push(3);
	    // luck
	    parts.Push(1);
	    // ballistics
	    parts.Push(3);
	    // eagleeye
	    parts.Push(3);
	    // necromancy
	    parts.Push(5);
	    // estates
	    parts.Push(2);
	    break;

	case Race::SORC:
	    // pathfinding
	    parts.Push(2);
	    // archery
	    parts.Push(3);
	    // logistics
	    parts.Push(2);
	    // scouting
	    parts.Push(1);
	    // diplomacy
	    parts.Push(2);
	    // navigation
	    parts.Push(4);
	    // leadership
	    parts.Push(1);
	    // wisdom
	    parts.Push(4);
	    // mysticism
	    parts.Push(3);
	    // luck
	    parts.Push(3);
	    // ballistics
	    parts.Push(3);
	    // eagleeye
	    parts.Push(2);
	    // necromancy
	    parts.Push(0);
	    // estates
	    parts.Push(2);
	    break;

	case Race::WRLK:
	    // pathfinding
	    parts.Push(2);
	    // archery
	    parts.Push(1);
	    // logistics
	    parts.Push(2);
	    // scouting
	    parts.Push(4);
	    // diplomacy
	    parts.Push(2);
	    // navigation
	    parts.Push(2);
	    // leadership
	    parts.Push(1);
	    // wisdom
	    parts.Push(5);
	    // mysticism
	    parts.Push(3);
	    // luck
	    parts.Push(1);
	    // ballistics
	    parts.Push(3);
	    // eagleeye
	    parts.Push(3);
	    // necromancy
	    parts.Push(1);
	    // estates
	    parts.Push(2);
	    break;

	case Race::WZRD:
	    // pathfinding
	    parts.Push(2);
	    // archery
	    parts.Push(1);
	    // logistics
	    parts.Push(2);
	    // scouting
	    parts.Push(2);
	    // diplomacy
	    parts.Push(2);
	    // navigation
	    parts.Push(2);
	    // leadership
	    parts.Push(2);
	    // wisdom
	    parts.Push(5);
	    // mysticism
	    parts.Push(4);
	    // luck
	    parts.Push(2);
	    // ballistics
	    parts.Push(3);
	    // eagleeye
	    parts.Push(3);
	    // necromancy
	    parts.Push(0);
	    // estates
	    parts.Push(2);
	    break;

	default:
	    Error::Warning("Skill::Secondary::PriorityForRace: unknown race."); return UNKNOWN;
    }

    switch(parts.Get())
    {
	case 0: return PATHFINDING;
	case 1: return ARCHERY;
	case 2: return LOGISTICS;
	case 3: return SCOUTING;
	case 4: return DIPLOMACY;
	case 5: return NAVIGATION;
	case 6: return LEADERSHIP;
	case 7: return WISDOM;
	case 8: return MYSTICISM;
	case 9: return LUCK;
	case 10: return BALLISTICS;
	case 11: return EAGLEEYE;
	case 12: return NECROMANCY;
	case 13: return ESTATES;

	default: break;
    }

    Error::Warning("Skill::Secondary::PriorityForRace: unknown result.");
    return UNKNOWN;
}
