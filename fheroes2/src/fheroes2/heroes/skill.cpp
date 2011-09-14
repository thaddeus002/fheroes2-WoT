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

#include <cstring>
#include <algorithm>
#include "gamedefs.h"
#include "race.h"
#include "agg.h"
#include "cursor.h"
#include "dialog.h"
#include "editor_dialogs.h"
#include "heroes.h"
#include "settings.h"
#include "skill.h"

#ifdef WITH_XML
#include "xmlccwrap.h"
#endif

namespace Skill
{
    u8 SecondaryGetWeightSkillFromRace(u8 race, u8 skill);
    u8 SecondaryPriorityFromRace(u8, const std::vector<u8> &);
    std::vector<u8> SecondarySkills(void);
}

struct level_t
{
    u16 basic;
    u16 advanced;
    u16 expert;
};

struct primary_t
{
    u8 attack;
    u8 defense;
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

struct skillvalues_t
{
    const char *id;
    level_t values;
};

skillstats_t _skillstats[] = {
    { "knight",      { 1, 1, 1, 1 }, { 2, 2, 1, 1 }, 0, 0, { 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 }, 10, {35,45,10,10 }, {25,25,25,25 }, { 2, 4, 3, 1, 3, 5, 3, 1, 1, 2, 0, 3, 2, 2 } },
    { "barbarian",   { 1, 1, 1, 1 }, { 3, 1, 1, 1 }, 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0 }, 10, {55,35, 5, 5 }, {30,30,20,20 }, { 3, 3, 2, 1, 2, 3, 3, 2, 1, 3, 0, 4, 4, 1 } },
    { "sorceress",   { 0, 0, 2, 2 }, { 0, 0, 2, 3 }, 1,15, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 1 }, 10, {10,10,30,50 }, {20,20,30,30 }, { 3, 3, 2, 2, 2, 1, 2, 3, 3, 4, 0, 2, 1, 4 } },
    { "warlock",     { 0, 0, 2, 2 }, { 0, 0, 3, 2 }, 1,19, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1 }, 10, {10,10,50,30 }, {20,20,30,30 }, { 1, 3, 2, 3, 2, 1, 2, 1, 3, 2, 1, 2, 4, 5 } },
    { "wizard",      { 0, 0, 2, 2 }, { 0, 1, 2, 2 }, 1,17, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2 }, 10, {10,10,40,40 }, {20,20,30,30 }, { 1, 3, 2, 3, 2, 2, 2, 2, 4, 2, 0, 2, 2, 5 } },
    { "necromancer", { 0, 0, 2, 2 }, { 1, 0, 2, 2 }, 1,10, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 }, 10, {15,15,35,35 }, {25,25,25,25 }, { 1, 3, 2, 3, 2, 0, 2, 1, 3, 2, 5, 3, 1, 4 } },
    { NULL,          { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, 10, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
};

skillvalues_t _skillvalues[] = {
    { "pathfinding", { 25, 50,100} },
    { "archery",     { 10, 25, 50} },
    { "logistics",   { 10, 20, 30} },
    { "scouting",    {  1,  2,  3} },
    { "diplomacy",   { 25, 50,100} },
    { "navigation",  { 33, 66,100} },
    { "leadership",  {  1,  2,  3} },
    { "wisdom",      {  3,  4,  5} },
    { "mysticism",   {  2,  3,  4} },
    { "luck",        {  1,  2,  3} },
    { "ballistics",  {  0,  0,  0} },
    { "eagleeye",    { 20, 30, 40} },
    { "necromancy",  { 10, 20, 30} },
    { "estates",     {100,250,500} },
    { NULL,          {  0,  0,  0} },
};

secondary_t _from_witchs_hut = {
    /* archery */   1, /* ballistics */ 1, /* diplomacy */ 1, /* eagleeye */    1,
    /* estates */   1, /* leadership */ 0, /* logistics */ 1, /* luck */        1,
    /* mysticism */ 1, /* navigation */ 1, /* necromancy*/ 0, /* pathfinding */ 1,
    /* scouting */  1, /* wisdom */ 1
};

struct defaulthero_t
{
    const char*	name;
    primary_t	primary;
    secondary_t	sec;
    // default army
    // default art
    // default spells
    Surface	port30x22;
    Surface	port50x46;
    Surface	port101x93;

};
/*
 _defaultheroes[] = {
};
*/

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
	default:
	    DEBUG(DBG_GAME, DBG_WARN, "is NULL"); break;
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
    xml->Attribute("defense", &value);   skill.defense = value;
    xml->Attribute("power", &value);     skill.power = value;
    xml->Attribute("knowledge", &value); skill.knowledge = value;
}

void LoadSecondarySection(const TiXmlElement* xml, secondary_t & sec)
{
    if(!xml) return;
    int value;
    xml->Attribute("archery", &value);     sec.archery = value;
    xml->Attribute("ballistics", &value);  sec.ballistics = value;
    xml->Attribute("diplomacy", &value);   sec.diplomacy = value;
    xml->Attribute("eagleeye", &value);    sec.eagleeye = value;
    xml->Attribute("estates", &value);     sec.estates = value;
    xml->Attribute("leadership", &value);  sec.leadership = value;
    xml->Attribute("logistics", &value);   sec.logistics = value;
    xml->Attribute("luck", &value);        sec.luck = value;
    xml->Attribute("mysticism", &value);   sec.mysticism = value;
    xml->Attribute("navigation", &value);  sec.navigation = value;
    xml->Attribute("necromancy", &value);  sec.necromancy = value;
    xml->Attribute("pathfinding", &value); sec.pathfinding = value;
    xml->Attribute("scouting", &value);    sec.scouting = value;
    xml->Attribute("wisdom", &value);      sec.wisdom = value;
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
	const TiXmlElement* xml_captain = xml_skills->FirstChildElement("captain");
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

	    const TiXmlElement* captain_race = xml_captain ? xml_captain->FirstChildElement(ptr->id) : NULL;
	    if(captain_race) LoadPrimarySection(captain_race, ptr->captain_primary);

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

	xml_secondary = xml_skills->FirstChildElement("secondary");
	if(xml_secondary)
	{
	    skillvalues_t* ptr2 = &_skillvalues[0];

	    while(ptr2->id)
	    {
		const TiXmlElement* xml_sec = xml_secondary->FirstChildElement(ptr2->id);

		if(xml_sec)
		{
		    xml_sec->Attribute("basic", &value); ptr2->values.basic = value;
		    xml_sec->Attribute("advanced", &value); ptr2->values.advanced = value;
		    xml_sec->Attribute("expert", &value); ptr2->values.expert = value;
		}

		++ptr2;
	    }
	}

	const TiXmlElement* xml_witchs_hut = xml_skills->FirstChildElement("witchs_hut");
	if(xml_witchs_hut)
	    LoadSecondarySection(xml_witchs_hut, _from_witchs_hut);
    }
    else
    VERBOSE(spec << ": " << doc.ErrorDesc());
#endif
}

u16 Skill::Secondary::GetValues(void) const
{
    switch(Level())
    {
	case Level::BASIC:	return _skillvalues[Skill() - 1].values.basic;
	case Level::ADVANCED:	return _skillvalues[Skill() - 1].values.advanced;
	case Level::EXPERT:	return _skillvalues[Skill() - 1].values.expert;
	default: break;
    }
    return 0;
}

Skill::Primary::Primary() : attack(0), defense(0), power(0), knowledge(0)
{
}

void Skill::Primary::LoadDefaults(u8 type, u8 race, Primary & skill)
{
    const skillstats_t* ptr = GetSkillStats(race);

    if(ptr)
    switch(type)
    {
	case CAPTAIN:
	    skill.attack = ptr->captain_primary.attack;
	    skill.defense = ptr->captain_primary.defense;
	    skill.power = ptr->captain_primary.power;
	    skill.knowledge = ptr->captain_primary.knowledge;
	    break;

	case HEROES:
	    skill.attack = ptr->initial_primary.attack;
	    skill.defense = ptr->initial_primary.defense;
	    skill.power = ptr->initial_primary.power;
	    skill.knowledge = ptr->initial_primary.knowledge;
	    break;

	default:
	    break;
    }
}

u8 Skill::Primary::GetInitialSpell(u8 race)
{
    const skillstats_t* ptr = GetSkillStats(race);
    return ptr ? ptr->initial_spell : 0;
}

u8 Skill::Primary::LevelUp(u8 race, u8 level)
{
    Rand::Queue percents(MAXPRIMARYSKILL);

    const skillstats_t* ptr = GetSkillStats(race);
    if(ptr)
    {
	if(ptr->over_level > level)
	{
	    percents.Push(ATTACK, ptr->mature_primary_under.attack);
	    percents.Push(DEFENSE, ptr->mature_primary_under.defense);
	    percents.Push(POWER, ptr->mature_primary_under.power);
	    percents.Push(KNOWLEDGE, ptr->mature_primary_under.knowledge);
	}
	else
	{
	    percents.Push(ATTACK, ptr->mature_primary_over.attack);
	    percents.Push(DEFENSE, ptr->mature_primary_over.defense);
	    percents.Push(POWER, ptr->mature_primary_over.power);
	    percents.Push(KNOWLEDGE, ptr->mature_primary_over.knowledge);
	}
    }

    u8 result = percents.Size() ? percents.Get() : UNKNOWN;

    switch(result)
    {
	case ATTACK:	++attack; break;
	case DEFENSE:	++defense; break;
	case POWER:	++power; break;
	case KNOWLEDGE:	++knowledge; break;
	default: break;
    }

    return result;
}

const char* Skill::Primary::String(u8 skill)
{
    const char* str_skill[] = { _("Attack"), _("Defense"), _("Power"), _("Knowledge"), "Unknown" };

    switch(skill)
    {
	case ATTACK:	return str_skill[0];
        case DEFENSE:	return str_skill[1];
        case POWER:	return str_skill[2];
        case KNOWLEDGE:	return str_skill[3];
        default:	break;
    }

    return str_skill[4];
}

const char* Skill::Level::String(u8 level)
{
    const char* str_level[] = { "None", _("skill|Basic"), _("skill|Advanced"), _("skill|Expert") };

    switch(level)
    {
	case BASIC:	return str_level[1];
	case ADVANCED:	return str_level[2];
	case EXPERT:	return str_level[3];
	default: break;
    }
    
    return str_level[0];
}

Skill::Secondary::Secondary() : std::pair<u8, u8>(UNKNOWN, Level::NONE)
{
}

Skill::Secondary::Secondary(u8 skill, u8 level) : std::pair<u8, u8>(UNKNOWN, Level::NONE)
{
    SetSkill(skill);
    SetLevel(level);
}

void Skill::Secondary::Reset(void)
{
    first = UNKNOWN;
    second = Level::NONE;
}

void Skill::Secondary::Set(const Secondary & skill)
{
    first = skill.first;
    second = skill.second;
}

void Skill::Secondary::SetSkill(u8 skill)
{
    first = skill <= ESTATES ? skill : UNKNOWN;
}

void Skill::Secondary::SetLevel(u8 level)
{
    second = level <= Level::EXPERT ? level : Level::NONE;
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

u8 Skill::Secondary::Skill(void) const
{
    return first;
}

u8 Skill::Secondary::Level(void) const
{
    return second;
}

bool Skill::Secondary::isLevel(u8 level) const
{
    return level == second;
}

bool Skill::Secondary::isSkill(u8 skill) const
{
    return skill == first;
}

bool Skill::Secondary::isValid(void) const
{
    return Skill() != UNKNOWN && Level() != Level::NONE;
}

u8 Skill::Secondary::RandForWitchsHut(void)
{
    std::vector<u8> v;
    v.reserve(14);

    if(_from_witchs_hut.archery)	v.push_back(ARCHERY);
    if(_from_witchs_hut.ballistics)	v.push_back(BALLISTICS);
    if(_from_witchs_hut.diplomacy)	v.push_back(DIPLOMACY);
    if(_from_witchs_hut.eagleeye)	v.push_back(EAGLEEYE);
    if(_from_witchs_hut.estates)	v.push_back(ESTATES);
    if(_from_witchs_hut.leadership)	v.push_back(LEADERSHIP);
    if(_from_witchs_hut.logistics)	v.push_back(LOGISTICS);
    if(_from_witchs_hut.luck)		v.push_back(LUCK);
    if(_from_witchs_hut.mysticism)	v.push_back(MYSTICISM);
    if(_from_witchs_hut.navigation)	v.push_back(NAVIGATION);
    if(_from_witchs_hut.necromancy)	v.push_back(NECROMANCY);
    if(_from_witchs_hut.pathfinding)	v.push_back(PATHFINDING);
    if(_from_witchs_hut.scouting)	v.push_back(SCOUTING);
    if(_from_witchs_hut.wisdom)		v.push_back(WISDOM);

    return v.empty() ? UNKNOWN : *Rand::Get(v);
}

/* index sprite from SECSKILL */
u8 Skill::Secondary::GetIndexSprite1(void) const
{
    return Skill() <= ESTATES ? Skill() : 0;
}

/* index sprite from MINISS */
u8 Skill::Secondary::GetIndexSprite2(void) const
{
    return Skill() <= ESTATES ? Skill() - 1 : 0xFF;
}

const char* Skill::Secondary::String(u8 skill)
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

const char* Skill::Secondary::GetName(void) const
{
    const char* name_skill[] =
    {
        _("Basic Pathfinding"), _("Advanced Pathfinding"), _("Expert Pathfinding"),
        _("Basic Archery"), _("Advanced Archery"), _("Expert Archery"),
        _("Basic Logistics"), _("Advanced Logistics"), _("Expert Logistics"),
        _("Basic Scouting"), _("Advanced Scouting"), _("Expert Scouting"),
        _("Basic Diplomacy"), _("Advanced Diplomacy"), _("Expert Diplomacy"),
        _("Basic Navigation"),_("Advanced Navigation"), _("Expert Navigation"),
        _("Basic Leadership"), _("Advanced Leadership"), _("Expert Leadership"),
        _("Basic Wisdom"), _("Advanced Wisdom"), _("Expert Wisdom"),
        _("Basic Mysticism"), _("Advanced Mysticism"), _("Expert Mysticism"),
        _("Basic Luck"), _("Advanced Luck"), _("Expert Luck"),
        _("Basic Ballistics"), _("Advanced Ballistics"), _("Expert Ballistics"),
        _("Basic Eagle Eye"), _("Advanced Eagle Eye"), _("Expert Eagle Eye"),
        _("Basic Necromancy"), _("Advanced Necromancy"), _("Expert Necromancy"),
        _("Basic Estates"), _("Advanced Estates"), _("Expert Estates")
    };

    return isValid() ? name_skill[(Level() - 1) + (Skill() - 1) * 3] : "unknown";
}

std::string Skill::Secondary::GetDescription(void) const
{
    const u16 count = GetValues();
    std::string str = "unknown";

    switch(Skill())
    {
	case PATHFINDING:
	    str = ngettext("Reduces the movement penalty for rough terrain by %{count} percent.", 
			"Reduces the movement penalty for rough terrain by %{count} percent.", count); break;
        case ARCHERY:	
	    str = ngettext("Increases the damage done by range attacking creatures by %{count} percent.",
			"Increases the damage done by range attacking creatures by %{count} percent.", count); break;
        case LOGISTICS:	
	    str = ngettext("Increases your hero's movement points by %{count} percent.",
			"Increases your hero's movement points by %{count} percent.", count); break;
        case SCOUTING:	
	    str = ngettext("Increases your hero's viewable area by %{count} square.",
			"Increases your hero's viewable area by %{count} squares.", count); break;
        case DIPLOMACY:	
	    str = _("Allows you to negotiate with monsters who are weaker than your group.");
	    str.append(" ");
	    str.append(ngettext("Approximately %{count} percent of the creatures may offer to join you.",
		    "Approximately %{count} percent of the creatures may offer to join you.", count)); break;
        case NAVIGATION:
	    str = ngettext("Increases your hero's movement points over water by %{count} percent.",
		    "Increases your hero's movement points over water by %{count} percent.", count); break;
        case LEADERSHIP:
	    str = ngettext("Increases your hero's troops' morale by %{count}.",
		    "Increases your hero's troops' morale by %{count}.", count); break;
        case WISDOM:	
	{
	    switch(Level())
	    {
		case Level::BASIC:
		    str = _("Allows your hero to learn third level spells."); break;
		case Level::ADVANCED:
		    str = _("Allows your hero to learn fourth level spells."); break;
		case Level::EXPERT:
		    str = _("Allows your hero to learn fifth level spells."); break;
		default: break;
	    }
	    break;
	}
        case MYSTICISM:	
	    str = ngettext("Regenerates %{count} of your hero's spell point per day.",
		    "Regenerates %{count} of your hero's spell points per day.", count); break;
        case LUCK:	
	    str = ngettext("Increases your hero's luck by %{count}.",
		    "Increases your hero's luck by %{count}.", count); break;
        case BALLISTICS:
	    switch(Level())
	    {
		case Level::BASIC:
		    str = _("Gives your hero's catapult shots a greater chance to hit and do damage to castle walls."); break;
		case Level::ADVANCED:
		    str = _("Gives your hero's catapult an extra shot, and each shot has a greater chance to hit and do damage to castle walls."); break;
		case Level::EXPERT:
		    str = _("Gives your hero's catapult an extra shot, and each shot automatically destroys any wall, except a fortified wall in a Knight town."); break;
		default: break;
	    }
	    break;
        case EAGLEEYE:	
	    switch(Level())
	    {
		case Level::BASIC:
		    str = ngettext("Gives your hero a %{count} percent chance to learn any given 1st or 2nd level enemy spell used against him in a combat.",
		    "Gives your hero a %{count} percent chance to learn any given 1st or 2nd level enemy spell used against him in a combat.", count); break;
		case Level::ADVANCED:
		    str = ngettext("Gives your hero a %{count} percent chance to learn any given 3rd level spell (or below) used against him in combat.",
		    "Gives your hero a %{count} percent chance to learn any given 3rd level spell (or below) used against him in combat.", count); break;
		case Level::EXPERT:
		    str = ngettext("Gives your hero a %{count} percent chance to learn any given 4th level spell (or below) used against him in combat.",
		    "Gives your hero a %{count} percent chance to learn any given 4th level spell (or below) used against him in combat.", count); break;
		default: break;
	    }
	    break;
        case NECROMANCY:
	    str = ngettext("Allows %{count} percent of the creatures killed in combat to be brought back from the dead as Skeletons.",
		    "Allows %{count} percent of the creatures killed in combat to be brought back from the dead as Skeletons.", count); break;
	case ESTATES:	
	    str = ngettext("Your hero produce %{count} gold pieces per turn as tax revenue from estates.",
		    "Your hero produces %{count} gold pieces per turn as tax revenue from estates.", count); break;
	default: break;
    }

    String::Replace(str, "%{count}", count);

    return str;
}

Skill::SecSkills::SecSkills()
{
    reserve(HEROESMAXSKILL);
}

Skill::SecSkills::SecSkills(u8 race)
{
    reserve(HEROESMAXSKILL);

    if(race & Race::ALL)
    {
	const skillstats_t* ptr = GetSkillStats(race);

	if(ptr)
	{
	    if(ptr->initial_secondary.archery)		push_back(Secondary(Secondary::ARCHERY, ptr->initial_secondary.archery));
	    if(ptr->initial_secondary.ballistics)	push_back(Secondary(Secondary::BALLISTICS, ptr->initial_secondary.ballistics));
	    if(ptr->initial_secondary.diplomacy)	push_back(Secondary(Secondary::DIPLOMACY, ptr->initial_secondary.diplomacy));
	    if(ptr->initial_secondary.eagleeye)		push_back(Secondary(Secondary::EAGLEEYE, ptr->initial_secondary.eagleeye));
	    if(ptr->initial_secondary.estates)		push_back(Secondary(Secondary::ESTATES, ptr->initial_secondary.estates));
	    if(ptr->initial_secondary.leadership)	push_back(Secondary(Secondary::LEADERSHIP, ptr->initial_secondary.leadership));
	    if(ptr->initial_secondary.logistics)	push_back(Secondary(Secondary::LOGISTICS, ptr->initial_secondary.logistics));
	    if(ptr->initial_secondary.luck)		push_back(Secondary(Secondary::LUCK, ptr->initial_secondary.luck));
	    if(ptr->initial_secondary.mysticism)	push_back(Secondary(Secondary::MYSTICISM, ptr->initial_secondary.mysticism));
	    if(ptr->initial_secondary.navigation)	push_back(Secondary(Secondary::NAVIGATION, ptr->initial_secondary.navigation));
	    if(ptr->initial_secondary.necromancy)	push_back(Secondary(Secondary::NECROMANCY, ptr->initial_secondary.necromancy));
	    if(ptr->initial_secondary.pathfinding)	push_back(Secondary(Secondary::PATHFINDING, ptr->initial_secondary.pathfinding));
	    if(ptr->initial_secondary.scouting)		push_back(Secondary(Secondary::SCOUTING, ptr->initial_secondary.scouting));
	    if(ptr->initial_secondary.wisdom)		push_back(Secondary(Secondary::WISDOM, ptr->initial_secondary.wisdom));
	}
    }
}

void Skill::SecSkills::ReadFromMP2(const u8* ptr)
{
    clear();

    for(u8 ii = 0; ii < 8; ++ii)
    {
	Skill::Secondary skill(*(ptr + ii) + 1, *(ptr + ii + 8));
        if(skill.isValid()) push_back(skill);
    }
}

u8 Skill::SecSkills::GetLevel(u8 skill) const
{
    const_iterator it = std::find_if(begin(), end(),
                        std::bind2nd(std::mem_fun_ref(&Secondary::isSkill), skill));

    return it == end() ? Level::NONE : (*it).Level();
}

u16 Skill::SecSkills::GetValues(u8 skill) const
{
    const_iterator it = std::find_if(begin(), end(),
                        std::bind2nd(std::mem_fun_ref(&Secondary::isSkill), skill));

    return it == end() ? 0 : (*it).GetValues();
}

void Skill::SecSkills::AddSkill(const Skill::Secondary & skill)
{
    iterator it = std::find_if(begin(), end(),
                        std::bind2nd(std::mem_fun_ref(&Secondary::isSkill), skill.Skill()));
    if(it != end())
        (*it).SetLevel(skill.Level());
    else
    {
	it = std::find_if(begin(), end(),
                	std::not1(std::mem_fun_ref(&Secondary::isValid)));
	if(it != end())
    	    (*it).Set(skill);
        else
	    push_back(skill);
    }
}

std::string Skill::SecSkills::String(void) const
{
    std::ostringstream os;

    for(const_iterator it = begin(); it != end(); ++it)
        os << (*it).GetName() << ", ";

    return os.str();
}

u8 Skill::SecondaryGetWeightSkillFromRace(u8 race, u8 skill)
{
    const skillstats_t* ptr = GetSkillStats(race);

    if(ptr)
    {
	if(skill == Secondary::PATHFINDING)	return ptr->mature_secondary.pathfinding;
	else
	if(skill == Secondary::ARCHERY)		return ptr->mature_secondary.archery;
	else
	if(skill == Secondary::LOGISTICS)	return ptr->mature_secondary.logistics;
	else
	if(skill == Secondary::SCOUTING)	return ptr->mature_secondary.scouting;
	else
	if(skill == Secondary::DIPLOMACY)	return ptr->mature_secondary.diplomacy;
	else
	if(skill == Secondary::NAVIGATION)	return ptr->mature_secondary.navigation;
	else
	if(skill == Secondary::LEADERSHIP)	return ptr->mature_secondary.leadership;
	else
	if(skill == Secondary::WISDOM)		return ptr->mature_secondary.wisdom;
	else
	if(skill == Secondary::MYSTICISM)	return ptr->mature_secondary.mysticism;
	else
	if(skill == Secondary::LUCK)		return ptr->mature_secondary.luck;
	else
	if(skill == Secondary::BALLISTICS)	return ptr->mature_secondary.ballistics;
	else
	if(skill == Secondary::EAGLEEYE)	return ptr->mature_secondary.eagleeye;
	else
	if(skill == Secondary::NECROMANCY)	return ptr->mature_secondary.necromancy;
	else
	if(skill == Secondary::ESTATES)		return ptr->mature_secondary.estates;
    }

    return 0;
}

std::vector<u8> Skill::SecondarySkills(void)
{
    const u8 vals[] = { Secondary::PATHFINDING, Secondary::ARCHERY, Secondary::LOGISTICS, Secondary::SCOUTING,
			Secondary::DIPLOMACY, Secondary::NAVIGATION, Secondary::LEADERSHIP, Secondary::WISDOM, Secondary::MYSTICISM,
			Secondary::LUCK, Secondary::BALLISTICS, Secondary::EAGLEEYE, Secondary::NECROMANCY, Secondary::ESTATES };

    return std::vector<u8>(vals, vals + ARRAY_COUNT(vals));
}

u8 Skill::SecondaryPriorityFromRace(u8 race, const std::vector<u8> & exclude)
{
    Rand::Queue parts(MAXSECONDARYSKILL);

    std::vector<u8> skills = SecondarySkills();

    for(std::vector<u8>::const_iterator
	it = skills.begin(); it != skills.end(); ++it)
	if(exclude.empty() ||
	    exclude.end() == std::find(exclude.begin(), exclude.end(), *it))
	    parts.Push(*it, SecondaryGetWeightSkillFromRace(race, *it));

    return parts.Size() ? parts.Get() : Secondary::UNKNOWN;
}

/* select secondary skills for level up */
void Skill::SecSkills::FindSkillsForLevelUp(u8 race, Secondary & sec1, Secondary & sec2) const
{
    std::vector<u8> exclude_skills;
    exclude_skills.reserve(MAXSECONDARYSKILL + HEROESMAXSKILL);

    // exclude for expert
    {
	for(const_iterator
	    it = begin(); it != end(); ++it)
	    if((*it).Level() == Level::EXPERT) exclude_skills.push_back((*it).Skill());
    }

    // exclude is full, add other.
    if(HEROESMAXSKILL <= size())
    {
	std::vector<u8> skills = SecondarySkills();

	for(std::vector<u8>::const_iterator
	    it = skills.begin(); it != skills.end(); ++it)
	    if(Level::NONE == GetLevel(*it)) exclude_skills.push_back(*it);
    }

    sec1.SetSkill(SecondaryPriorityFromRace(race, exclude_skills));
    exclude_skills.push_back(sec1.Skill());
    sec2.SetSkill(SecondaryPriorityFromRace(race, exclude_skills));

    sec1.SetLevel(GetLevel(sec1.Skill()));
    sec2.SetLevel(GetLevel(sec2.Skill()));

    sec1.NextLevel();
    sec2.NextLevel();
}







SecondarySkillBar::SecondarySkillBar() : skills(NULL), use_mini_sprite(false), can_change(false)
{
}

const Rect & SecondarySkillBar::GetArea(void) const
{
    return pos;
}

void SecondarySkillBar::SetSkills(Skill::SecSkills & v)
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

void SecondarySkillBar::SetChangeMode(void)
{
    can_change = true;
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
        const Skill::Secondary & skill = ii < skills->size() ? skills->at(ii) : Skill::Secondary();

        if(skill.isValid())
        {
            const Sprite & sprite_skill = AGG::GetICN((use_mini_sprite ? ICN::MINISS : ICN::SECSKILL), (use_mini_sprite ? skill.GetIndexSprite2() : skill.GetIndexSprite1()));
            display.Blit(sprite_skill, dst_pt);

            if(use_mini_sprite)
	    {
		message.clear();
		String::AddInt(message, skill.Level());
        	text.Set(message);
        	text.Blit(dst_pt.x + (sprite_skill.w() - text.w()) - 3, dst_pt.y + sprite_skill.h() - 12);
	    }
	    else
	    {
        	text.Set(Skill::Secondary::String(skill.Skill()));
        	text.Blit(dst_pt.x + (sprite_skill.w() - text.w()) / 2, dst_pt.y + 3);

        	text.Set(Skill::Level::String(skill.Level()));
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

bool SecondarySkillBar::QueueEventProcessing(void)
{
    Display & display = Display::Get();
    Cursor & cursor = Cursor::Get();
    LocalEvent & le = LocalEvent::Get();
    const Point & cu = le.GetMouseCursor();
    bool modify = false;

    if(!(pos & cu) || !skills) return false;

    u8 ii = GetIndexFromCoord(cu);
    const Sprite & sprite_skill = AGG::GetICN((use_mini_sprite ? ICN::MINISS : ICN::SECSKILL), 0);
    const Rect tile(pos.x + (ii * (sprite_skill.w() + interval)), pos.y, sprite_skill.w(), sprite_skill.h());

    if(ii < skills->size())
    {
	Skill::Secondary & skill = skills->at(ii);

	if(skill.isValid())
	{
	    if(le.MouseClickLeft(tile))
    	    {
        	cursor.Hide();
        	Dialog::SecondarySkillInfo(skill, true);
        	cursor.Show();
        	display.Flip();
	    }
	    else
	    if(le.MousePressRight(tile))
	    {
		if(can_change)
		{
		    skill.Reset();
		    modify = true;
		}
		else
		{
		    cursor.Hide();
		    Dialog::SecondarySkillInfo(skill, false);
		    cursor.Show();
		    display.Flip();
		}
	    }
	}
    }
    else
    if(ii < MAXSECONDARYSKILL)
    {
	if(can_change && le.MouseClickLeft(tile))
	{
	    Skill::Secondary alt = Dialog::SelectSecondarySkill();
	    if(alt.isValid() && skills)
	    {
		skills->AddSkill(alt);
		modify = true;
	    }
	}
    }

    return modify;
}

void StringAppendModifiers(std::string & str, s8 value)
{
    if(value < 0) str.append(" "); // '-' present
    else
    if(value > 0) str.append(" +");

    String::AddInt(str, value);
}

s8 Skill::GetLeadershipModifiers(u8 level, std::string* strs = NULL)
{
    Secondary skill(Secondary::LEADERSHIP, level);

    if(skill.GetValues() && strs)
    {
        strs->append(skill.GetName());
        StringAppendModifiers(*strs, skill.GetValues());
        strs->append("\n");
    }

    return skill.GetValues();
}

s8 Skill::GetLuckModifiers(u8 level, std::string* strs = NULL)
{
    Secondary skill(Secondary::LUCK, level);

    if(skill.GetValues() && strs)
    {
        strs->append(skill.GetName());
        StringAppendModifiers(*strs, skill.GetValues());
        strs->append("\n");
    }

    return skill.GetValues();
}
