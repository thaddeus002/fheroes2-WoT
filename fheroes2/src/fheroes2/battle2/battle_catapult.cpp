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

#include "castle.h"
#include "artifact.h"
#include "skill.h"
#include "settings.h"
#include "heroes_base.h"
#include "battle_arena.h"
#include "battle_cell.h"
#include "battle_tower.h"
#include "battle_catapult.h"

Battle2::Catapult::Catapult(const HeroBase & hero, bool fortification, Arena & a) : arena(a), cat_shots(1), cat_first(20), cat_miss(true), cat_fort(fortification)
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

    if(hero.HasArtifact(Artifact::BALLISTA)) cat_shots += 1;
}

u8 Battle2::Catapult::GetShots(void) const
{
    return cat_shots;
}

u8 Battle2::Catapult::GetDamage(u8 target)
{
    u8 value = arena.GetCastleTargetValue(target);

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
		    DEBUG(DBG_BATTLE, DBG_TRACE, "Battle2::Catapult::GetDamage: " << "from one blow capability");
		}
		else
		    value = 1;
	    }
	    break;

	case CAT_MISS: DEBUG(DBG_BATTLE, DBG_TRACE, "Battle2::Catapult::GetDamage: " << " miss!"); break;

	default: break;
    }

    return value;
}

Point Battle2::Catapult::GetTargetPosition(u8 target) const
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
	case CAT_MOAT:	res = Point(400, 195); break;
	case CAT_MISS:	res = Point(610, 320); break;

	default: break;
    }

    return res;
}

u8 Battle2::Catapult::GetTarget(void) const
{
    std::vector<u8> targets;
    targets.reserve(8);

    // check walls
    if(0 != arena.board[8].object)  targets.push_back(CAT_WALL1);
    if(0 != arena.board[29].object) targets.push_back(CAT_WALL2);
    if(0 != arena.board[73].object) targets.push_back(CAT_WALL3);
    if(0 != arena.board[96].object) targets.push_back(CAT_WALL4);

    // check right/left towers
    if(arena.towers[0] && arena.towers[0]->isValid()) targets.push_back(CAT_TOWER1);
    if(arena.towers[2] && arena.towers[2]->isValid()) targets.push_back(CAT_TOWER2);

    // check moat
    if(0 != arena.board[50].object) targets.push_back(CAT_MOAT);

    // check general tower
    if(arena.towers[1] && arena.towers[1]->isValid()) targets.push_back(CAT_TOWER3);

    if(targets.size())
    {
	if(targets.size() > 3) targets.resize(3);
	// miss for 30%
	return cat_miss && 7 > Rand::Get(1, 20) ? CAT_MISS : *Rand::Get(targets);
    }

    DEBUG(DBG_BATTLE, DBG_TRACE, "Battle2::Catapult::ApplyDamage: " << "target not found..");

    return 0;
}

void Battle2::Catapult::Action(void)
{
    Battle2::Action action;
    action.SetID(MSG_BATTLE_CATAPULT);

    u8 shots = GetShots();
    action.Push(shots);
    while(shots--)
    {
	u8 target = GetTarget();
	action.Push(target);
	action.Push(GetDamage(target));
    }
    arena.ApplyAction(action);
}
