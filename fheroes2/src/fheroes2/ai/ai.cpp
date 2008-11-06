/***************************************************************************
 *   Copyright (C) 2008 by Andrey Afletdinov                               *
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

#include "rand.h"
#include "monster.h"
#include "heroes.h"
#include "ai.h"

Skill::Secondary::skill_t AI::SelectSkill(const Heroes & hero, const Skill::Secondary & sec1, const Skill::Secondary & sec2)
{
    return Rand::Get(0, 1) ? sec1.Skill() : sec2.Skill();
}

bool AI::SelectGoldOrExp(const Heroes & hero, const u16 gold, const u16 expr)
{
    // FIXME: AI::SelectGoldOrExp: always select gold
    return true;
}

// see Heroes::Action
void AI::HeroesAction(Heroes & hero, const u16 dst_index)
{
}
