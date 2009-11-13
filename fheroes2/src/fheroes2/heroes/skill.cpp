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

#include <algorithm>
#include "gamedefs.h"
#include "race.h"
#include "agg.h"
#include "cursor.h"
#include "dialog.h"
#include "heroes.h"
#include "settings.h"
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
		percents.Push(ATTACK, 55); percents.Push(DEFENCE, 35); percents.Push(POWER, 5); percents.Push(KNOWLEDGE, 5);
	    }
	    else
	    {
		percents.Push(ATTACK, 30); percents.Push(DEFENCE, 30); percents.Push(POWER, 20); percents.Push(KNOWLEDGE, 20);
	    }
	    break;

	case Race::KNGT:
	    if(10 > level)
	    {
		percents.Push(ATTACK, 35); percents.Push(DEFENCE, 45); percents.Push(POWER, 10); percents.Push(KNOWLEDGE, 10);
	    }
	    else
	    {
		percents.Push(ATTACK, 25); percents.Push(DEFENCE, 25); percents.Push(POWER, 25); percents.Push(KNOWLEDGE, 25);
	    }
	    break;

	case Race::NECR:
	    if(10 > level)
	    {
		percents.Push(ATTACK, 15); percents.Push(DEFENCE, 15); percents.Push(POWER, 35); percents.Push(KNOWLEDGE, 35);
	    }
	    else
	    {
		percents.Push(ATTACK, 25); percents.Push(DEFENCE, 25); percents.Push(POWER, 25); percents.Push(KNOWLEDGE, 25);
	    }
	    break;

	case Race::SORC:
	    if(10 > level)
	    {
		percents.Push(ATTACK, 10); percents.Push(DEFENCE, 10); percents.Push(POWER, 30); percents.Push(KNOWLEDGE, 50);
	    }
	    else
	    {
		percents.Push(ATTACK, 20); percents.Push(DEFENCE, 20); percents.Push(POWER, 30); percents.Push(KNOWLEDGE, 30);
	    }
	    break;

	case Race::WRLK:
	    if(10 > level)
	    {
		percents.Push(ATTACK, 10); percents.Push(DEFENCE, 10); percents.Push(POWER, 50); percents.Push(KNOWLEDGE, 30);
	    }
	    else
	    {
		percents.Push(ATTACK, 20); percents.Push(DEFENCE, 20); percents.Push(POWER, 30); percents.Push(KNOWLEDGE, 30);
	    }
	    break;

	case Race::WZRD:
	    if(10 > level)
	    {
		percents.Push(ATTACK, 10); percents.Push(DEFENCE, 10); percents.Push(POWER, 40); percents.Push(KNOWLEDGE, 40);
	    }
	    else
	    {
		percents.Push(ATTACK, 20); percents.Push(DEFENCE, 20); percents.Push(POWER, 30); percents.Push(KNOWLEDGE, 30);
	    }
	    break;

	default:
	    DEBUG(DBG_GAME , DBG_WARN, "Skill::Primary::LevelUp: unknown race."); return UNKNOWN;
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
    switch(race)
    {
	case Race::BARB:
	    if(skill == PATHFINDING)	return 4;
	    if(skill == ARCHERY)	return 3;
	    if(skill == LOGISTICS)	return 3;
	    if(skill == SCOUTING)	return 4;
	    if(skill == DIPLOMACY)	return 2;
	    if(skill == NAVIGATION)	return 3;
	    if(skill == LEADERSHIP)	return 3;
	    if(skill == WISDOM)		return 1;
	    if(skill == MYSTICISM)	return 1;
	    if(skill == LUCK)		return 2;
	    if(skill == BALLISTICS)	return 3;
	    if(skill == EAGLEEYE)	return 1;
	    if(skill == NECROMANCY)	return 0;
	    if(skill == ESTATES)	return 2;
    	break;

	case Race::KNGT:
	    if(skill == PATHFINDING)	return 3;
	    if(skill == ARCHERY)	return 2;
	    if(skill == LOGISTICS)	return 3;
	    if(skill == SCOUTING)	return 2;
	    if(skill == DIPLOMACY)	return 3;
	    if(skill == NAVIGATION)	return 2;
	    if(skill == LEADERSHIP)	return 5;
	    if(skill == WISDOM)		return 2;
	    if(skill == MYSTICISM)	return 1;
	    if(skill == LUCK)		return 1;
	    if(skill == BALLISTICS)	return 4;
	    if(skill == EAGLEEYE)	return 1;
	    if(skill == NECROMANCY)	return 0;
	    if(skill == ESTATES)	return 3;
	    break;

	case Race::NECR:
	    if(skill == PATHFINDING)	return 3;
	    if(skill == ARCHERY)	return 1;
	    if(skill == LOGISTICS)	return 2;
	    if(skill == SCOUTING)	return 1;
	    if(skill == DIPLOMACY)	return 2;
	    if(skill == NAVIGATION)	return 2;
	    if(skill == LEADERSHIP)	return 0;
	    if(skill == WISDOM)		return 4;
	    if(skill == MYSTICISM)	return 3;
	    if(skill == LUCK)		return 1;
	    if(skill == BALLISTICS)	return 3;
	    if(skill == EAGLEEYE)	return 3;
	    if(skill == NECROMANCY)	return 5;
	    if(skill == ESTATES)	return 2;
	    break;

	case Race::SORC:
	    if(skill == PATHFINDING)	return 2;
	    if(skill == ARCHERY)	return 3;
	    if(skill == LOGISTICS)	return 2;
	    if(skill == SCOUTING)	return 1;
	    if(skill == DIPLOMACY)	return 2;
	    if(skill == NAVIGATION)	return 4;
	    if(skill == LEADERSHIP)	return 1;
	    if(skill == WISDOM)		return 4;
	    if(skill == MYSTICISM)	return 3;
	    if(skill == LUCK)		return 3;
	    if(skill == BALLISTICS)	return 3;
	    if(skill == EAGLEEYE)	return 3;
	    if(skill == NECROMANCY)	return 0;
	    if(skill == ESTATES)	return 2;
	    break;

	case Race::WRLK:
	    if(skill == PATHFINDING)	return 4;
	    if(skill == ARCHERY)	return 1;
	    if(skill == LOGISTICS)	return 2;
	    if(skill == SCOUTING)	return 4;
	    if(skill == DIPLOMACY)	return 2;
	    if(skill == NAVIGATION)	return 2;
	    if(skill == LEADERSHIP)	return 1;
	    if(skill == WISDOM)		return 5;
	    if(skill == MYSTICISM)	return 3;
	    if(skill == LUCK)		return 1;
	    if(skill == BALLISTICS)	return 3;
	    if(skill == EAGLEEYE)	return 3;
	    if(skill == NECROMANCY)	return 1;
	    if(skill == ESTATES)	return 2;
	    break;

	case Race::WZRD:
	    if(skill == PATHFINDING)	return 2;
	    if(skill == ARCHERY)	return 1;
	    if(skill == LOGISTICS)	return 2;
	    if(skill == SCOUTING)	return 2;
	    if(skill == DIPLOMACY)	return 2;
	    if(skill == NAVIGATION)	return 2;
	    if(skill == LEADERSHIP)	return 2;
	    if(skill == WISDOM)		return 5;
	    if(skill == MYSTICISM)	return 4;
	    if(skill == LUCK)		return 2;
	    if(skill == BALLISTICS)	return 3;
	    if(skill == EAGLEEYE)	return 3;
	    if(skill == NECROMANCY)	return 0;
	    if(skill == ESTATES)	return 2;
	    break;

	default: break;
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
            const Sprite & sprite_skill = AGG::GetICN(ICN::SECSKILL, 0);

	    if(use_mini_sprite)
        	display.Blit(sprite_skill, Rect((sprite_skill.w() - 32) / 2, 0, 32, 32), dst_pt);
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
