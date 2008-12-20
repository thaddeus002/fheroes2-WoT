/***************************************************************************
 *   Copyright (C) 2008 by Andrey Afletdinov                               *
 *   <afletdinov@mail.dc.baikal.ru>                                        *
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

#include "world.h"
#include "game.h"
#include "kingdom.h"
#include "settings.h"
#include "arena_tiles.h"
#include "battle_arena.h"

#define ARENASIZE 99

BattleArena::BattleArena(const u16 index, Army::army_t & a1, Army::army_t & a2) : 
	army1(a1), army2(a2),
	color1(a1.GetColor()), color2(a2.GetColor()),
	control1(world.GetKingdom(color1).Control()), control2(world.GetKingdom(color2).Control()),
	graphics(control1 == Game::LOCAL || control2 == Game::LOCAL || 3 < Settings::Get().Debug()),
	result(0)
{
    // build maps
    maps.reserve(ARENASIZE);
    for(u8 ii = 0; ii < ARENASIZE; ++ii) maps.push_back(new ArenaTiles(*this, ii));

    // place army1 troops
    if(army1.At(0).isValid()) maps[0]->SetModes(ArenaTiles::A1TROOP1);
    if(army1.At(1).isValid()) maps[22]->SetModes(ArenaTiles::A1TROOP2);
    if(army1.At(2).isValid()) maps[44]->SetModes(ArenaTiles::A1TROOP3);
    if(army1.At(3).isValid()) maps[66]->SetModes(ArenaTiles::A1TROOP4);
    if(army1.At(4).isValid()) maps[88]->SetModes(ArenaTiles::A1TROOP5);

    // place army2 troops
    if(army2.At(0).isValid()) maps[10]->SetModes(ArenaTiles::A2TROOP1);
    if(army2.At(1).isValid()) maps[32]->SetModes(ArenaTiles::A2TROOP2);
    if(army2.At(2).isValid()) maps[54]->SetModes(ArenaTiles::A2TROOP3);
    if(army2.At(3).isValid()) maps[76]->SetModes(ArenaTiles::A2TROOP4);
    if(army2.At(4).isValid()) maps[98]->SetModes(ArenaTiles::A2TROOP5);

    // TODO: place maps objects

    // build interface
    if(graphics)
    {
	arena.Set(640, 444);
    }
}

BattleArena::~BattleArena()
{
    std::vector<ArenaTiles *>::iterator it1 = maps.begin();
    std::vector<ArenaTiles *>::iterator it2 = maps.end();

    for(; it1 != it2; ++it1) if(*it1) delete *it1;
}

bool BattleArena::GameOver(void) const
{
    return !army1.isValid() || !army2.isValid();
}

void BattleArena::Turns(const u16 turn)
{
    army1.BattleNewTurn();
    army2.BattleNewTurn();

    bool check_skip1 = false;
    bool check_skip2 = false;

    // basic loop turns
    while(1)
    {
	const u8 speed1 = army1.GetFastestTroop().GetSpeed();
	const u8 speed2 = army2.GetFastestTroop().GetSpeed();

	if(0 == speed1 && 0 == speed2)
	{
	    // end turn
	    if(check_skip1 && check_skip2) break;
	    
	    if(0 == speed1 && false == check_skip1){ army1.ResetModes(Army::SKIPMOVE); check_skip1 = true; }
	    if(0 == speed2 && false == check_skip2){ army2.ResetModes(Army::SKIPMOVE); check_skip2 = true; }
	}

	// move army1
	if(speed1 >= speed2)
	{
	    switch(control1)
	    {
		case Game::AI:		AITurn(1);	break;
		case Game::LOCAL:	HumanTurn(1);	break;
		default: break;
	    }
	}
	else
	// move army2
	{
	    switch(control2)
	    {
		case Game::AI:		AITurn(2);	break;
		case Game::LOCAL:	HumanTurn(2);	break;
		default: break;
	    }
	}
    }
}

u8 BattleArena::GetResult(void)
{
    return result;
}
