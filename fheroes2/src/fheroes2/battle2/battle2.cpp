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

#include "army.h"
#include "color.h"
#include "settings.h"
#include "battle_arena.h"
#include "battle2.h"

u8 Army::BattleLoader(Army::army_t & army1, Army::army_t & army2, const u16 mapsindex)
{
    if(2 < Settings::Get().Debug())
    {
	Error::Verbose("Army::Battle: start, army1(" + Color::String(army1.GetColor()) + "), army2(" + Color::String(army1.GetColor()) + ")");
	army1.Dump();
	army2.Dump();
    }

    BattleArena arena(mapsindex, army1, army2);

    army1.SetModes(Army::BATTLE);
    army2.SetModes(Army::BATTLE);

    u16 turn = 1;

    while(1)
    {
	arena.Turns(turn);

	if(arena.GameOver()) break;

	++turn;
    }

    army1.ResetModes(Army::BATTLE);
    army2.ResetModes(Army::BATTLE);

    if(2 < Settings::Get().Debug()) Error::Verbose("Army::Battle: end, turns:", turn);

    return arena.GetResult();
}
