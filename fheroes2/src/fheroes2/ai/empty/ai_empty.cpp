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

#include "kingdom.h"
#include "castle.h"
#include "heroes.h"
#include "dialog.h"
#include "battle2.h"
#include "battle_arena.h"
#include "ai.h"

const char* AI::Type(void)
{
    return "empty";
}

const char* AI::License(void)
{
    return "GPL";
}

void AI::AddCastle(const Castle &)
{
}

void AI::RemoveCastle(const Castle &)
{
}

void AI::AddHeroes(const Heroes &)
{
}

void AI::RemoveHeroes(const Heroes &)
{
}

void AI::HeroesAction(Heroes &, s32)
{
}

std::string AI::HeroesString(const Heroes &)
{
    return "";
}

void AI::HeroesLevelUp(Heroes &)
{
}

void AI::KingdomTurn(Kingdom &)
{
}

void AI::BattleTurn(Battle2::Arena &, const Battle2::Stats & b, Battle2::Actions & a)
{
    a.AddedEndAction(b);
}

void AI::BattleMagicTurn(Battle2::Arena &, const Battle2::Stats &, Battle2::Actions &, const Battle2::Stats*)
{
}

void AI::Init(void)
{
}

void AI::CastlePreBattle(Castle &)
{
}

void AI::CastleAfterBattle(Castle &, bool attacker_wins)
{
}
