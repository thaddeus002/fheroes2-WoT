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

bool AI::JoinTroops(const Heroes & hero, const u16 monster, const u16 count)
{
    // FIXME: AI::JoinTroops
    switch(monster)
    {
	default: break;
    }

    return false;
}

u16  AI::RecruitTroops(const Heroes & hero, const u16 monster, const u16 count)
{
    // FIXME: AI::RecruitTroops
    switch(monster)
    {
	default: break;
    }

    return 0;
}

void AI::Marketplace(const Heroes & hero)
{
}

// pickup artifact with conditions:
// 1 - 2500g, 2,3 - 2500g+3res 3000g+5res, 4,5 - need have skill wisard or leadership,
// 6 - 50 rogues, 7 - 1 gin, 8,9,10,11,12,13 - 1 monster level4
 bool AI::PickupArtifact(const Heroes & hero, const Artifact::artifact_t art, const u8 cond)
{
    return false;
}
