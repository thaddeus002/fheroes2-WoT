/***************************************************************************
 *   Copyright (C) 2009 by Andrey Afletdinov <fheroes2@gmail.com>          *
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
#include "luck.h"
#include "morale.h"
#include "race.h"
#include "agg.h"
#include "settings.h"
#include "captain.h"

Captain::Captain(Castle & cstl) : HeroBase(Skill::Primary::CAPTAIN, cstl.GetRace()), home(cstl)
{
    SetCenter(home.GetCenter());
}

bool Captain::isValid(void) const
{
    return home.isBuild(BUILD_CAPTAIN);
}

u8 Captain::GetAttack(void) const
{
    return attack + GetAttackModificator(NULL);
}

u8 Captain::GetDefense(void) const
{
    return defense + GetDefenseModificator(NULL);
}

u8 Captain::GetPower(void) const
{
   return power + GetPowerModificator(NULL);
}

u8 Captain::GetKnowledge(void) const
{
    return knowledge + GetKnowledgeModificator(NULL);
}

s8 Captain::GetMorale(void) const
{
    s8 result = Morale::NORMAL;

    // global modificator
    result += GetMoraleModificator(NULL);

    // result
    if(result < Morale::AWFUL)  return Morale::TREASON;
    else
    if(result < Morale::POOR)   return Morale::AWFUL;
    else
    if(result < Morale::NORMAL) return Morale::POOR;
    else
    if(result < Morale::GOOD)   return Morale::NORMAL;
    else
    if(result < Morale::GREAT)  return Morale::GOOD;
    else
    if(result < Morale::BLOOD)  return Morale::GREAT;

    return Morale::BLOOD;
}

s8 Captain::GetLuck(void) const
{
    s8 result = Luck::NORMAL;

    // global modificator
    result += GetLuckModificator(NULL);

    // result
    if(result < Luck::AWFUL)    return Luck::CURSED;
    else
    if(result < Luck::BAD)      return Luck::AWFUL;
    else
    if(result < Luck::NORMAL)   return Luck::BAD;
    else
    if(result < Luck::GOOD)     return Luck::NORMAL;
    else
    if(result < Luck::GREAT)    return Luck::GOOD;
    else
    if(result < Luck::IRISH)    return Luck::GREAT;

    return Luck::IRISH;
}

u8 Captain::GetRace(void) const
{
    return home.GetRace();
}

Color::color_t Captain::GetColor(void) const
{
    return home.GetColor();
}

const std::string & Captain::GetName(void) const
{
    return home.GetName();
}

u8 Captain::GetType(void) const
{
    return Skill::Primary::CAPTAIN;
}

u8 Captain::GetLevelSkill(u8) const
{
    return 0;
}

u16 Captain::GetSecondaryValues(u8) const
{
    return 0;
}

const Army & Captain::GetArmy(void) const
{
    return home.GetArmy();
}

Army & Captain::GetArmy(void)
{
    return home.GetArmy();
}

u16 Captain::GetMaxSpellPoints(void) const
{
    return knowledge * 10;
}

u8 Captain::GetControl(void) const
{
    return home.GetControl();
}

s32 Captain::GetIndex(void) const
{
    return home.GetIndex();
}

void Captain::ActionAfterBattle(void)
{
    SetSpellPoints(GetMaxSpellPoints());
}

void Captain::ActionPreBattle(void)
{
    SetSpellPoints(GetMaxSpellPoints());
}

const Castle* Captain::inCastle(void) const
{
    return &home;
}

const Surface & Captain::GetPortrait(u8 type) const
{
    switch(type)
    {
        case PORT_BIG:
            switch(GetRace())
            {
                case Race::KNGT:        return AGG::GetICN(ICN::PORT0090, 0);
                case Race::BARB:        return AGG::GetICN(ICN::PORT0091, 0);
                case Race::SORC:        return AGG::GetICN(ICN::PORT0092, 0);
                case Race::WRLK:        return AGG::GetICN(ICN::PORT0093, 0);
                case Race::WZRD:        return AGG::GetICN(ICN::PORT0094, 0);
                case Race::NECR:        return AGG::GetICN(ICN::PORT0095, 0);
                default: break;
            }
            break;

        case PORT_MEDIUM:
        case PORT_SMALL:
            switch(GetRace())
            {
                case Race::KNGT:        return AGG::GetICN(ICN::MINICAPT, 0);
                case Race::BARB:        return AGG::GetICN(ICN::MINICAPT, 1);
                case Race::SORC:        return AGG::GetICN(ICN::MINICAPT, 2);
                case Race::WRLK:        return AGG::GetICN(ICN::MINICAPT, 3);
                case Race::WZRD:        return AGG::GetICN(ICN::MINICAPT, 4);
                case Race::NECR:        return AGG::GetICN(ICN::MINICAPT, 5);
                default: break;
            }
            break;
    }

    return AGG::GetICN(ICN::PORT0090, 0);
}

void Captain::PortraitRedraw(s16 px, s16 py, u8 type, Surface & dstsf) const
{
}
