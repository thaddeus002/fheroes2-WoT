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

#include <algorithm>
#include "gamedefs.h"
#include "race.h"
#include "agg.h"
#include "cursor.h"
#include "dialog.h"
#include "heroes.h"
#include "settings.h"
#include "skill.h"

#ifdef WITH_XML
#include "xmlccwrap.h"
#endif

struct primary_t
{
    u8 attack;
    u8 defence;
    u8 power;
    u8 knowledge;
};

struct secondary_t
{
    u8 archery;
    u8 ballistics;
    u8 diplomacy;
    u8 eagleeye;
    u8 estates;
    u8 leadership;
    u8 logistics;
    u8 luck;
    u8 mysticism;
    u8 navigation;
    u8 necromancy;
    u8 pathfinding;
    u8 scouting;
    u8 wisdom;
};

struct skillstats_t
{
    const char* id;
    primary_t   captain_primary;
    primary_t   initial_primary;
    u8          initial_book;
    u8          initial_spell;
    secondary_t initial_secondary;
    u8		over_level;
    primary_t   mature_primary_under;
    primary_t   mature_primary_over;
    secondary_t mature_secondary;
};

static skillstats_t _skillstats[] = {
    { "knight",      { 1, 1, 1, 1 }, { 2, 2, 1, 1 }, 0, 0, { 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 }, 10, {35,45,10,10 }, {25,25,25,25 }, { 2, 4, 3, 1, 3, 5, 3, 1, 1, 2, 0, 3, 2, 2 } },
    { "barbarian",   { 1, 1, 1, 1 }, { 3, 1, 1, 1 }, 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0 }, 10, {55,35, 5, 5 }, {30,30,20,20 }, { 3, 3, 2, 1, 2, 3, 3, 2, 1, 3, 0, 4, 4, 1 } },
    { "sorceress",   { 0, 0, 2, 2 }, { 0, 0, 2, 3 }, 1,15, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 1 }, 10, {10,10,30,50 }, {20,20,30,30 }, { 3, 3, 2, 2, 2, 1, 2, 3, 3, 4, 0, 2, 1, 4 } },
    { "warlock",     { 0, 0, 2, 2 }, { 0, 0, 3, 2 }, 1,19, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1 }, 10, {10,10,50,30 }, {20,20,30,30 }, { 1, 3, 2, 3, 2, 1, 2, 1, 3, 2, 1, 2, 4, 5 } },
    { "wizard",      { 0, 0, 2, 2 }, { 0, 1, 2, 2 }, 1,17, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2 }, 10, {10,10,40,40 }, {20,20,30,30 }, { 1, 3, 2, 3, 2, 2, 2, 2, 4, 2, 0, 2, 2, 5 } },
    { "necromancer", { 0, 0, 2, 2 }, { 1, 0, 2, 2 }, 1,10, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 }, 10, {15,15,35,35 }, {25,25,25,25 }, { 1, 3, 2, 3, 2, 0, 2, 1, 3, 2, 5, 3, 1, 4 } },
    { NULL,          { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, 10, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
};

const skillstats_t* GetSkillStats(u8 race)
{
    const char* id = NULL;

    switch(race)
    {
	case Race::KNGT:	id = "knight"; break;
	case Race::BARB:	id = "barbarian"; break;
	case Race::SORC:	id = "sorceress"; break;
	case Race::WRLK:	id = "warlock"; break;
	case Race::WZRD:	id = "wizard"; break;
	case Race::NECR:	id = "necromancer"; break;
	default: break;
    }

    const skillstats_t* ptr = &_skillstats[0];
    while(ptr->id && id && std::strcmp(id, ptr->id)) ++ptr;

    return id ? ptr : NULL;
}

#ifdef WITH_XML
void LoadPrimarySection(const TiXmlElement* xml, primary_t & skill)
{
    if(!xml) return;
    int value;
    xml->Attribute("attack", &value);    skill.attack = value;
    xml->Attribute("defence", &value);   skill.defence = value;
    xml->Attribute("power", &value);     skill.power = value;
    xml->Attribute("knowledge", &value); skill.knowledge = value;
}

void LoadSecondarySection(const TiXmlElement* xml, secondary_t & sec)
{
    if(!xml) return;
    int value;
    xml->Attribute("archery", &value);     sec.archery = Skill::Level::NONE > value || Skill::Level::EXPERT < value ? Skill::Level::NONE : value;
    xml->Attribute("ballistics", &value);  sec.ballistics = Skill::Level::NONE > value || Skill::Level::EXPERT < value ? Skill::Level::NONE : value;
    xml->Attribute("diplomacy", &value);   sec.diplomacy = Skill::Level::NONE > value || Skill::Level::EXPERT < value ? Skill::Level::NONE : value;
    xml->Attribute("eagleeye", &value);    sec.eagleeye = Skill::Level::NONE > value || Skill::Level::EXPERT < value ? Skill::Level::NONE : value;
    xml->Attribute("estates", &value);     sec.estates = Skill::Level::NONE > value || Skill::Level::EXPERT < value ? Skill::Level::NONE : value;
    xml->Attribute("leadership", &value);  sec.leadership = Skill::Level::NONE > value || Skill::Level::EXPERT < value ? Skill::Level::NONE : value;
    xml->Attribute("logistics", &value);   sec.logistics = Skill::Level::NONE > value || Skill::Level::EXPERT < value ? Skill::Level::NONE : value;
    xml->Attribute("luck", &value);        sec.luck = Skill::Level::NONE > value || Skill::Level::EXPERT < value ? Skill::Level::NONE : value;
    xml->Attribute("mysticism", &value);   sec.mysticism = Skill::Level::NONE > value || Skill::Level::EXPERT < value ? Skill::Level::NONE : value;
    xml->Attribute("navigation", &value);  sec.navigation = Skill::Level::NONE > value || Skill::Level::EXPERT < value ? Skill::Level::NONE : value;
    xml->Attribute("necromancy", &value);  sec.necromancy = Skill::Level::NONE > value || Skill::Level::EXPERT < value ? Skill::Level::NONE : value;
    xml->Attribute("pathfinding", &value); sec.pathfinding = Skill::Level::NONE > value || Skill::Level::EXPERT < value ? Skill::Level::NONE : value;
    xml->Attribute("scouting", &value);    sec.scouting = Skill::Level::NONE > value || Skill::Level::EXPERT < value ? Skill::Level::NONE : value;
    xml->Attribute("wisdom", &value);      sec.wisdom = Skill::Level::NONE > value || Skill::Level::EXPERT < value ? Skill::Level::NONE : value;
}
#endif

void Skill::UpdateStats(const std::string & spec)
{
#ifdef WITH_XML
    // parse skills.xml
    TiXmlDocument doc;
    const TiXmlElement* xml_skills = NULL;
                
    if(doc.LoadFile(spec.c_str()) &&
	NULL != (xml_skills = doc.FirstChildElement("skills")))
    {
	const TiXmlElement* xml_initial = xml_skills->FirstChildElement("initial");
	const TiXmlElement* xml_maturity = xml_skills->FirstChildElement("maturity");
	const TiXmlElement* xml_secondary = xml_maturity ? xml_maturity->FirstChildElement("secondary") : NULL;
	const TiXmlElement* xml_primary = xml_maturity ? xml_maturity->FirstChildElement("primary") : NULL;
	const TiXmlElement* xml_under = xml_primary ? xml_primary->FirstChildElement("under") : NULL;
	const TiXmlElement* xml_over = xml_primary ? xml_primary->FirstChildElement("over") : NULL;
	skillstats_t *ptr = &_skillstats[0];
	int value;

	while(ptr->id)
	{
	    const TiXmlElement* initial_race = xml_initial ? xml_initial->FirstChildElement(ptr->id) : NULL;

	    if(initial_race)
	    {
		LoadPrimarySection(initial_race, ptr->initial_primary);
		LoadSecondarySection(initial_race, ptr->initial_secondary);

		initial_race->Attribute("book", &value);  ptr->initial_book = value;
		initial_race->Attribute("spell", &value); ptr->initial_spell = value;
	    }

	    const TiXmlElement* under_race = xml_under ? xml_under->FirstChildElement(ptr->id) : NULL;
	    if(under_race) LoadPrimarySection(under_race, ptr->mature_primary_under);

	    const TiXmlElement* over_race = xml_over ? xml_over->FirstChildElement(ptr->id) : NULL;
	    if(over_race)
	    {
		LoadPrimarySection(over_race, ptr->mature_primary_over);
		over_race->Attribute("level", &value);
		if(value) ptr->over_level = value;
	    }
	    
	    const TiXmlElement* secondary_race = xml_secondary ? xml_secondary->FirstChildElement(ptr->id) : NULL;
	    if(secondary_race) LoadSecondarySection(secondary_race, ptr->mature_secondary);

	    ++ptr;
	}
    }
#endif
}

Skill::Primary::Primary() : attack(0), defence(0), power(0), knowledge(0)
{
}

// primary skill from level up (dependence from race and hero level)
Skill::Primary::skill_t Skill::Primary::FromLevelUp(const u8 race, const u8 level)
{
    Rand::Queue percents(MAXPRIMARYSKILL);

    const skillstats_t* ptr = GetSkillStats(race);
    if(ptr)
    {
	if(ptr->over_level > level)
	{
	    percents.Push(ATTACK, ptr->mature_primary_under.attack);
	    percents.Push(DEFENCE, ptr->mature_primary_under.defence);
	    percents.Push(POWER, ptr->mature_primary_under.power);
	    percents.Push(KNOWLEDGE, ptr->mature_primary_under.knowledge);
	}
	else
	{
	    percents.Push(ATTACK, ptr->mature_primary_over.attack);
	    percents.Push(DEFENCE, ptr->mature_primary_over.defence);
	    percents.Push(POWER, ptr->mature_primary_over.power);
	    percents.Push(KNOWLEDGE, ptr->mature_primary_over.knowledge);
	}
    }

    if(percents.Size())
    switch(percents.Get())
    {
	case ATTACK:	return ATTACK;
	case DEFENCE:	return DEFENCE;
	case POWER:	return POWER;
	case KNOWLEDGE:	return KNOWLEDGE;
	default: break;
    }

    DEBUG(DBG_GAME , DBG_WARN, "Skill::Primary::LevelUp: unknown result.");
    return UNKNOWN;
}

const char* Skill::Primary::String(const Skill::Primary::skill_t skill)
{
    const char* str_skill[] = { _("Attack"), _("Defence"), _("Power"), _("Knowledge"), "Unknown" };

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
       case 1: return BASIC;
       case 2: return ADVANCED;
       case 3: return EXPERT;

	default: break;
    }

    return NONE;
}

const char* Skill::Level::String(const type_t level)
{
    const char* str_level[] = { "None", _("Basic"), _("Advanced"), _("Expert") };

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

Skill::Secondary::Secondary(const skill_t s, const Level::type_t t) : std::pair<u8, u8>(s, t)
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
    return second > Level::EXPERT ? Level::NONE : static_cast<Level::type_t>(second);
}

Skill::Secondary::skill_t Skill::Secondary::Skill(void) const
{
    return Skill(first);
}

bool Skill::Secondary::isLevel(u8 level) const
{
    return level == second;
}

bool Skill::Secondary::isSkill(u8 skill) const
{
    return skill == first;
}

Skill::Secondary::skill_t Skill::Secondary::Skill(const u8 index)
{
    return index > ESTATES ? UNKNOWN : static_cast<skill_t>(index);
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

    DEBUG(DBG_GAME , DBG_WARN, "Skill::Secondary::GetIndexSprite2: unknown skill, index out of range");

    return 0xff;
}

const char* Skill::Secondary::String(const skill_t skill)
{
    const char* str_skill[] = { _("Pathfinding"), _("Archery"), _("Logistics"), _("Scouting"), _("Diplomacy"), _("Navigation"), 
	_("Leadership"), _("Wisdom"), _("Mysticism"), _("Luck"), _("Ballistics"), _("Eagle Eye"), _("Necromancy"), _("Estates"), "Unknown"  };

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

const char* Skill::Secondary::Description(const skill_t skill, const Level::type_t level)
{
    const char* description_skill[] =
    {
	_("Basic Pathfinding reduces the movement penalty for rough terrain by 25 percent."),
	_("Advanced Pathfinding reduces the movement penalty for rough terrain by 50 percent."),
	_("Expert Pathfinding eliminates the movement penalty for rough terrain."),

	_("Basic Archery increases the damage done by range attacking creatures by 10 percent."),
	_("Advanced Archery increases the damage done by range attacking creatures by 25 percent."),
	_("Expert Archery increases the damage done by range attacking creatures by 50 percent."),

	_("Basic Logistics increases your hero's movement points by 10 percent."),
	_("Advanced Logistics increases your hero's movement points by 20 percent."),
	_("Expert Logistics increases your hero's movement points by 30 percent."),

	_("Basic Scouting increases your hero's viewable area by 1 square."),
	_("Advanced Scouting increases your hero's viewable area by 2 squares."),
	_("Expert Scouting increases your hero's viewable area by 3 squares."),

	_("Basic Diplomacy allows you to negotiate with monsters who are weaker than your group. Approximately 1/4 of the creatures may offer to join you."),
	_("Advanced Diplomacy allows you to negotiate with monsters who are weaker than your group. Approximately 1/2 of the creatures may offer to join you."),
	_("Expert Diplomacy allows you to negotiate with monsters who are weaker than your group. All of the creatures may offer to join you."),

	_("Basic Navigation increases your hero's movement points over water by 1/3."),
	_("Advanced Navigation increases your hero's movement points over water by 2/3."),
	_("Expert Navigation doubles your hero's movement points over water."),

	_("Basic Leadership increases your hero's troops' morale by 1."),
	_("Advanced Leadership increases your hero's troops' morale by 2."),
	_("Expert Leadership increases your hero's troops' morale by 3."),

	_("Basic Wisdom allows your hero to learn third level spells."),
	_("Advanced Wisdom allows your hero to learn fourth level spells."),
	_("Expert Wisdom allows your hero to learn fifth level spells."),

	_("Basic Mysticism regenerates two of your hero's spell points per day."),
	_("Advanced Mysticism regenerates three of your hero's spell points per day."),
	_("Expert Mysticism regenerates four of your hero's spell points per day."),

	_("Basic Luck increases your hero's luck by 1."),
	_("Advanced Luck increases your hero's luck by 2."),
	_("Expert Luck increases your hero's luck by 3."),

	_("Basic Ballistics gives your hero's catapult shots a greater chance to hit and do damage to castle walls."),
	_("Advanced Ballistics gives your hero's catapult an extra shot, and each shot has a greater chance to hit and do damage to castle walls."),
	_("Expert Ballistics gives your hero's catapult an extra shot, and each shot automatically destroys any wall, except a fortified wall in a Knight town."),

	_("Basic Eagle Eye gives your hero a 20 percent chance to learn any given 1st or 2nd level enemy spell used against him in a combat."),
	_("Advanced Eagle Eye gives your hero a 30 percent chance to learn any given 3rd level spell (or below) used against him in combat."),
	_("Expert Eagle Eye gives your hero a 40 percent chance to learn any given 4th level spell (or below) used against him in combat."),

	_("Basic Necromancy allows 10 percent of the creatures killed in combat to be brought back from the dead as Skeletons."),
	_("Advanced Necromancy allows 20 percent of the creatures killed in combat to be brought back from the dead as Skeletons."),
	_("Expert Necromancy allows 30 percent of the creatures killed in combat to be brought back from the dead as Skeletons."),

	_("Your hero produces 100 gold pieces per turn as tax revenue from estates."),
	_("Your hero produces 250 gold pieces per turn as tax revenue from estates."),
	_("Your hero produces 500 gold pieces per turn as tax revenue from estates.") };

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

void Skill::Secondary::FillStandard(std::vector<skill_t> & v)
{
    v.clear();
    v.reserve(MAXSECONDARYSKILL);
    v.push_back(PATHFINDING);
    v.push_back(ARCHERY);
    v.push_back(LOGISTICS);
    v.push_back(SCOUTING);
    v.push_back(DIPLOMACY);
    v.push_back(NAVIGATION);
    v.push_back(LEADERSHIP);
    v.push_back(WISDOM);
    v.push_back(MYSTICISM);
    v.push_back(LUCK);
    v.push_back(BALLISTICS);
    v.push_back(EAGLEEYE);
    v.push_back(NECROMANCY);
    v.push_back(ESTATES);
}

u8 Skill::Secondary::GetWeightSkillFromRace(u8 race, u8 skill)
{
    const skillstats_t* ptr = GetSkillStats(race);
    if(ptr)
    {
	
	if(skill == PATHFINDING)	return ptr->mature_secondary.pathfinding;
	if(skill == ARCHERY)		return ptr->mature_secondary.archery;
	if(skill == LOGISTICS)		return ptr->mature_secondary.logistics;
	if(skill == SCOUTING)		return ptr->mature_secondary.scouting;
	if(skill == DIPLOMACY)		return ptr->mature_secondary.diplomacy;
	if(skill == NAVIGATION)		return ptr->mature_secondary.navigation;
	if(skill == LEADERSHIP)		return ptr->mature_secondary.leadership;
	if(skill == WISDOM)		return ptr->mature_secondary.wisdom;
	if(skill == MYSTICISM)		return ptr->mature_secondary.mysticism;
	if(skill == LUCK)		return ptr->mature_secondary.luck;
	if(skill == BALLISTICS)		return ptr->mature_secondary.ballistics;
	if(skill == EAGLEEYE)		return ptr->mature_secondary.eagleeye;
	if(skill == NECROMANCY)		return ptr->mature_secondary.necromancy;
	if(skill == ESTATES)		return ptr->mature_secondary.estates;
    }

    return 0;
}

Skill::Secondary::skill_t Skill::Secondary::PriorityFromRace(u8 race, const std::vector<skill_t>& exclude)
{
    Rand::Queue parts(MAXSECONDARYSKILL);

    std::vector<skill_t> skills;
    FillStandard(skills);

    std::vector<skill_t>::const_iterator it1 = skills.begin();
    std::vector<skill_t>::const_iterator it2 = skills.end();

    for(; it1 != it2; ++it1)
	if(exclude.empty() || exclude.end() == std::find(exclude.begin(), exclude.end(), *it1))
	    parts.Push(*it1, GetWeightSkillFromRace(race, *it1));

    return parts.Size() ? Skill(parts.Get()) : UNKNOWN;
}

SecondarySkillBar::SecondarySkillBar() : skills(NULL), use_mini_sprite(false)
{
}

const Rect & SecondarySkillBar::GetArea(void) const
{
    return pos;
}

void SecondarySkillBar::SetSkills(const std::vector<Skill::Secondary> & v)
{
    skills = &v;
    CalcSize();
}

void SecondarySkillBar::SetUseMiniSprite(void)
{
    use_mini_sprite = true;
}

void SecondarySkillBar::SetInterval(u8 i)
{
    interval = i;
    CalcSize();
}

void SecondarySkillBar::SetPos(s16 sx, s16 sy)
{
    pos.x = sx;
    pos.y = sy;
    CalcSize();
}

void SecondarySkillBar::CalcSize(void)
{
    pos.w = 0;
    pos.h = 0;

    if(skills)
    {
	const Sprite & sprite = AGG::GetICN((use_mini_sprite ? ICN::MINISS : ICN::SECSKILL), 0);
	pos.h = sprite.h();
	pos.w = HEROESMAXSKILL * (sprite.w() + interval);
    }
}

void SecondarySkillBar::Redraw(void)
{
    Display & display = Display::Get();
    Point dst_pt(pos);
    std::string message;
    Text text;
    text.Set(Font::SMALL);

    for(u8 ii = 0; ii < HEROESMAXSKILL; ++ii)
    {
        const Skill::Secondary::skill_t skill = ii < skills->size() ? skills->at(ii).Skill() : Skill::Secondary::UNKNOWN;
        const Skill::Level::type_t level = ii < skills->size() ? skills->at(ii).Level() : Skill::Level::NONE;

        if(Skill::Secondary::UNKNOWN != skill && Skill::Level::NONE != level)
        {
            const Sprite & sprite_skill = AGG::GetICN((use_mini_sprite ? ICN::MINISS : ICN::SECSKILL), (use_mini_sprite ? Skill::Secondary::GetIndexSprite2(skill) : Skill::Secondary::GetIndexSprite1(skill)));
            display.Blit(sprite_skill, dst_pt);

            if(use_mini_sprite)
	    {
		message.clear();
		String::AddInt(message, level);
        	text.Set(message);
        	text.Blit(dst_pt.x + (sprite_skill.w() - text.w()) - 3, dst_pt.y + sprite_skill.h() - 12);
	    }
	    else
	    {
        	text.Set(Skill::Secondary::String(skill));
        	text.Blit(dst_pt.x + (sprite_skill.w() - text.w()) / 2, dst_pt.y + 3);

        	text.Set(Skill::Level::String(level));
        	text.Blit(dst_pt.x + (sprite_skill.w() - text.w()) / 2, dst_pt.y + 50);
	    }

    	    dst_pt.x += (use_mini_sprite ? 32 : sprite_skill.w()) + interval;
        }
	else
	{
            const Sprite & sprite_skill = AGG::GetICN((use_mini_sprite ? ICN::HSICONS : ICN::SECSKILL), 0);

	    if(use_mini_sprite)
        	display.Blit(sprite_skill, Rect((sprite_skill.w() - 32) / 2, 20, 32, 32), dst_pt);
	    else
        	display.Blit(sprite_skill, dst_pt);

    	    dst_pt.x += (use_mini_sprite ? 32 : sprite_skill.w()) + interval;
	}
    }
}

u8 SecondarySkillBar::GetIndexFromCoord(const Point & cu)
{
    const Sprite & sprite_skill = AGG::GetICN((use_mini_sprite ? ICN::MINISS : ICN::SECSKILL), 0);
    return (pos & cu) ? (cu.x - pos.x) / (sprite_skill.w() + interval) : 0;
}

void SecondarySkillBar::QueueEventProcessing(void)
{
    Display & display = Display::Get();
    Cursor & cursor = Cursor::Get();
    LocalEvent & le = LocalEvent::Get();
    const Point & cu = le.GetMouseCursor();

    if(!(pos & cu) || !skills) return;

    u8 ii = GetIndexFromCoord(cu);
    const Sprite & sprite_skill = AGG::GetICN((use_mini_sprite ? ICN::MINISS : ICN::SECSKILL), 0);
    const Rect tile(pos.x + (ii * (sprite_skill.w() + interval)), pos.y, sprite_skill.w(), sprite_skill.h());

    if(ii < skills->size() && (le.MouseClickLeft(tile) || le.MousePressRight(tile)))
    {
	const Skill::Secondary::skill_t & skill = skills->at(ii).Skill();
	const Skill::Level::type_t & level = skills->at(ii).Level();

	if(Skill::Secondary::UNKNOWN != skill && Skill::Level::NONE != level)
	{
    	    cursor.Hide();
    	    Dialog::SkillInfo(skill, level, !le.MousePressRight());
    	    cursor.Show();
    	    display.Flip();
	}
    }
}

void Skill::Primary::LoadDefaults(u8 race, Primary & skill, u8 & book, u8 & spell)
{
    const skillstats_t* ptr = GetSkillStats(race);

    if(ptr)
    {
	if(CAPTAIN == skill.GetType())
	{
	    skill.attack = ptr->captain_primary.attack;
	    skill.defence = ptr->captain_primary.defence;
	    skill.power = ptr->captain_primary.power;
	    skill.knowledge = ptr->captain_primary.knowledge;
	    book = ptr->initial_book;
	    spell = 0;
	}
	else
	if(HEROES == skill.GetType())
	{
	    skill.attack = ptr->initial_primary.attack;
	    skill.defence = ptr->initial_primary.defence;
	    skill.power = ptr->initial_primary.power;
	    skill.knowledge = ptr->initial_primary.knowledge;
	    book = ptr->initial_book;
	    spell = ptr->initial_spell;
	}
    }
}

void Skill::Secondary::LoadDefaults(u8 race, std::vector<Secondary> & skills)
{
    const skillstats_t* ptr = GetSkillStats(race);

    if(ptr)
    {
	if(ptr->initial_secondary.archery)	skills.push_back(Secondary(ARCHERY, static_cast<Skill::Level::type_t>(ptr->initial_secondary.archery)));
	if(ptr->initial_secondary.ballistics)	skills.push_back(Secondary(BALLISTICS, static_cast<Skill::Level::type_t>(ptr->initial_secondary.ballistics)));
	if(ptr->initial_secondary.diplomacy)	skills.push_back(Secondary(DIPLOMACY, static_cast<Skill::Level::type_t>(ptr->initial_secondary.diplomacy)));
	if(ptr->initial_secondary.eagleeye)	skills.push_back(Secondary(EAGLEEYE, static_cast<Skill::Level::type_t>(ptr->initial_secondary.eagleeye)));
	if(ptr->initial_secondary.estates)	skills.push_back(Secondary(ESTATES, static_cast<Skill::Level::type_t>(ptr->initial_secondary.estates)));
	if(ptr->initial_secondary.leadership)	skills.push_back(Secondary(LEADERSHIP, static_cast<Skill::Level::type_t>(ptr->initial_secondary.leadership)));
	if(ptr->initial_secondary.logistics)	skills.push_back(Secondary(LOGISTICS, static_cast<Skill::Level::type_t>(ptr->initial_secondary.logistics)));
	if(ptr->initial_secondary.luck)		skills.push_back(Secondary(LUCK, static_cast<Skill::Level::type_t>(ptr->initial_secondary.luck)));
	if(ptr->initial_secondary.mysticism)	skills.push_back(Secondary(MYSTICISM, static_cast<Skill::Level::type_t>(ptr->initial_secondary.mysticism)));
	if(ptr->initial_secondary.navigation)	skills.push_back(Secondary(NAVIGATION, static_cast<Skill::Level::type_t>(ptr->initial_secondary.navigation)));
	if(ptr->initial_secondary.necromancy)	skills.push_back(Secondary(NECROMANCY, static_cast<Skill::Level::type_t>(ptr->initial_secondary.necromancy)));
	if(ptr->initial_secondary.pathfinding)	skills.push_back(Secondary(PATHFINDING, static_cast<Skill::Level::type_t>(ptr->initial_secondary.pathfinding)));
	if(ptr->initial_secondary.scouting)	skills.push_back(Secondary(SCOUTING, static_cast<Skill::Level::type_t>(ptr->initial_secondary.scouting)));
	if(ptr->initial_secondary.wisdom)	skills.push_back(Secondary(WISDOM, static_cast<Skill::Level::type_t>(ptr->initial_secondary.wisdom)));
    }
}
