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

#include "battle_arena.h"

void BattleArena::HumanTurn(const u8 who)
{
    Army::army_t & army = (who == 1 ? army1 : army2);
    Army::army_t & enmy = (who == 1 ? army2 : army1);

    // current troop
    Army::Troop & troop = army.GetFastestTroop();
    bool skip = false;

    army.Dump();
    enmy.Dump();




    // set flag
    troop.SetModes(skip ? Army::SKIPMOVE : Army::MOVED);
}

void BattleArena::AITurn(const u8 who)
{
    Army::army_t & army = (who == 1 ? army1 : army2);
    Army::army_t & enmy = (who == 1 ? army2 : army1);

    // current troop
    Army::Troop & troop = army.GetFastestTroop();
    bool skip = false;

    army.Dump();
    enmy.Dump();


    // set flag
    troop.SetModes(skip ? Army::SKIPMOVE : Army::MOVED);
}
