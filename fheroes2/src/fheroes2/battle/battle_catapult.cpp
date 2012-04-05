/***************************************************************************
 *   Copyright (C) 2010 by Andrey Afletdinov <fheroes2@gmail.com>          *
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

#include "artifact.h"
#include "skill.h"
#include "settings.h"
#include "heroes_base.h"
#include "battle_command.h"
#include "battle_catapult.h"

Battle::Catapult::Catapult(const HeroBase & hero, bool fortification) : cat_shots(1), cat_first(20), cat_miss(true), cat_fort(fortification)
{
    switch(hero.GetLevelSkill(Skill::Secondary::BALLISTICS))
    {
	case Skill::Level::BASIC:
	    cat_first = 40;
	    cat_miss = false;
	    break;

	case Skill::Level::ADVANCED:
	    cat_first = 80;
	    cat_shots += 1;
	    cat_miss = false;
	    break;

	case Skill::Level::EXPERT:
	    cat_first = 100;
	    cat_shots += 1;
	    cat_miss = false;
	    break;

	default: break;
    }

    u8 acount = hero.HasArtifact(Artifact::BALLISTA);
    if(acount) cat_shots += acount * Artifact(Artifact::BALLISTA).ExtraValue();
}

u8 Battle::Catapult::GetDamage(u8 target, u8 value) const
{
    switch(target)
    {
	case CAT_WALL1:
	case CAT_WALL2:
	case CAT_WALL3:
	case CAT_WALL4:
	    if(value)
	    {
		if(cat_first == 100 || cat_first >= Rand::Get(1, 100))
		{
		    // value = value;
		    DEBUG(DBG_BATTLE, DBG_TRACE, "from one blow capability");
		}
		else
		    value = 1;
	    }
	    break;

	case CAT_MISS: DEBUG(DBG_BATTLE, DBG_TRACE, "miss!"); break;

	default: break;
    }

    return value;
}

Point Battle::Catapult::GetTargetPosition(u8 target)
{
    Point res;

    switch(target)
    {
	case CAT_WALL1:	res = Point(475, 45); break;
	case CAT_WALL2:	res = Point(420, 115); break;
	case CAT_WALL3:	res = Point(415, 280); break;
	case CAT_WALL4:	res = Point(490, 390); break;
	case CAT_TOWER1:res = Point(430, 40); break;
	case CAT_TOWER2:res = Point(430, 300); break;
	case CAT_TOWER3:res = Point(580, 160); break;
	case CAT_BRIDGE:res = Point(400, 195); break;
	case CAT_MISS:	res = Point(610, 320); break;

	default: break;
    }

    return res;
}

u8 Battle::Catapult::GetTarget(const std::vector<u8> & values) const
{
    std::vector<u8> targets;
    targets.reserve(4);

    // check walls
    if(0 != values[CAT_WALL1]) targets.push_back(CAT_WALL1);
    if(0 != values[CAT_WALL2]) targets.push_back(CAT_WALL2);
    if(0 != values[CAT_WALL3]) targets.push_back(CAT_WALL3);
    if(0 != values[CAT_WALL4]) targets.push_back(CAT_WALL4);

    // check right/left towers
    if(targets.empty())
    {
	if(values[CAT_TOWER1]) targets.push_back(CAT_TOWER1);
	if(values[CAT_TOWER2]) targets.push_back(CAT_TOWER2);
    }

    // check bridge
    if(targets.empty())
    {
	if(values[CAT_BRIDGE]) targets.push_back(CAT_BRIDGE);
    }

    // check general tower
    if(targets.empty())
    {
	if(values[CAT_TOWER3]) targets.push_back(CAT_TOWER3);
    }

    if(targets.size())
    {
	// miss for 30%
	return cat_miss && 7 > Rand::Get(1, 20) ? CAT_MISS : (1 < targets.size() ? *Rand::Get(targets) : targets.front());
    }

    DEBUG(DBG_BATTLE, DBG_TRACE, "target not found..");

    return 0;
}

Battle::Command Battle::Catapult::GetAction(Arena & arena) const
{
    u8 shots = cat_shots;
    std::vector<u8> values(CAT_MISS + 1, 0);

    values[CAT_WALL1] = arena.GetCastleTargetValue(CAT_WALL1);
    values[CAT_WALL2] = arena.GetCastleTargetValue(CAT_WALL2);
    values[CAT_WALL3] = arena.GetCastleTargetValue(CAT_WALL3);
    values[CAT_WALL4] = arena.GetCastleTargetValue(CAT_WALL4);
    values[CAT_TOWER1] = arena.GetCastleTargetValue(CAT_TOWER1);
    values[CAT_TOWER2] = arena.GetCastleTargetValue(CAT_TOWER2);
    values[CAT_TOWER3] = arena.GetCastleTargetValue(CAT_TOWER3);
    values[CAT_BRIDGE] = arena.GetCastleTargetValue(CAT_BRIDGE);

    Command cmd(MSG_BATTLE_CATAPULT);
    cmd.GetStream() << shots;

    while(shots--)
    {
        const u8 & target = GetTarget(values);
        const u8 & damage = GetDamage(target, arena.GetCastleTargetValue(target));
        cmd.GetStream() << target << damage;
        values[target] -= damage;
    }

    return cmd;
}
