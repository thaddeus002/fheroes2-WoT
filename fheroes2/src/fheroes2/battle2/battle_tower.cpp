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
#include "settings.h"
#include "battle_cell.h"
#include "battle_stats.h"
#include "battle_tower.h"

Battle2::Tower::Tower(const Castle & castle, u8 twr, Arena & a) : Army::Troop(Monster::ARCHER), arena(a), type(twr), valid(true)
{
    if(castle.isBuild(BUILD_THIEVESGUILD)) ++count;
    if(castle.isBuild(BUILD_TAVERN)) ++count;
    if(castle.isBuild(BUILD_SHIPYARD)) ++count;
    if(castle.isBuild(BUILD_WELL)) ++count;
    if(castle.isBuild(BUILD_STATUE)) ++count;
    if(castle.isBuild(BUILD_LEFTTURRET)) ++count;
    if(castle.isBuild(BUILD_RIGHTTURRET)) ++count;
    if(castle.isBuild(BUILD_MARKETPLACE)) ++count;
    if(castle.isBuild(BUILD_WEL2)) ++count;
    if(castle.isBuild(BUILD_MOAT)) ++count;
    if(castle.isBuild(BUILD_SPEC)) ++count;
    if(castle.isBuild(BUILD_CASTLE)) ++count;
    if(castle.isBuild(BUILD_CAPTAIN)) ++count;

    count += castle.GetLevelMageGuild();

    if(castle.isBuild(DWELLING_MONSTER1)) ++count;
    if(castle.isBuild(DWELLING_MONSTER2)) ++count;
    if(castle.isBuild(DWELLING_MONSTER3)) ++count;
    if(castle.isBuild(DWELLING_MONSTER4)) ++count;
    if(castle.isBuild(DWELLING_MONSTER5)) ++count;
    if(castle.isBuild(DWELLING_MONSTER6)) ++count;
    
    if(count > 20) count = 20;
    if(TWR_CENTER != type) count /= 2;
    if(count == 0) count = 1;

    BattleInit();
    Stats* b = GetBattleStats();
    b->SetModes(CAP_TOWER);
}

bool Battle2::Tower::isValid(void) const
{
    return valid;
}

u8 Battle2::Tower::GetColor(void) const
{
    return arena.castle ? arena.castle->GetColor() : 0;
}

Point Battle2::Tower::GetPortPosition(void) const
{
    Point res;

    switch(type)
    {
	case TWR_LEFT:	res = Point(410, 70); break;
	case TWR_RIGHT:	res = Point(410, 320); break;
	case TWR_CENTER:res = Point(560, 170); break;
	default: break;
    }

    if(Settings::Get().PocketPC())
    {
	res.x /= 2;
	res.y /= 2;
    }

    return res;
}

void Battle2::Tower::Action(void) const
{
    const Stats* b = GetBattleStats();

    arena.ResetBoard();
    arena.board.SetEnemyQuality(*b);

    const Stats* enemy = arena.GetEnemyMaxQuality(GetColor());

    if(enemy)
    {
	Battle2::Action action;
	action.SetID(MSG_BATTLE_TOWER);
	action.Push(type);
	action.Push(enemy->id);
	arena.ApplyAction(action);
    }
}

void Battle2::Tower::SetDestroy(void)
{
    switch(type)
    {
	case TWR_LEFT:	arena.board[19].object = 1; break;
	case TWR_RIGHT:	arena.board[85].object = 1; break;
	default: break;
    }
    valid = false;
}
