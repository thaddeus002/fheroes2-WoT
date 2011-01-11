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

#include <algorithm>
#include "heroes.h"
#include "castle.h"
#include "ai_simple.h"

void AI::AddHeroes(const Heroes &)
{
}

void AI::RemoveHeroes(const Heroes &)
{
}

void AI::AddCastle(const Castle &)
{
}

void AI::RemoveCastle(const Castle & castle)
{
    AIKingdom & ai = AIKingdoms::Get(castle.GetColor());

    if(ai.capital == &castle)
    {
        ai.capital->ResetModes(Castle::CAPITAL);
        ai.capital = NULL;
    }
}

void AI::Init(void)
{
    AIKingdoms::Reset();
    AIHeroes::Reset();
}

bool Queue::isPresent(s32 index) const
{
    return end() != std::find(begin(), end(), index);
}
