/***************************************************************************
 *   Copyright (C) 2006 by Andrey Afletdinov                               *
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

#include <algorithm>
#include <functional>
#include "speed.h"
#include "luck.h"
#include "morale.h"
#include "army.h"
#include "army_troop.h"

Army::Troop::Troop(monster_t m, u16 c) : Monster(m), count(c), army(NULL)
{
}

bool Army::Troop::HasMonster(monster_t m) const
{
    return id == m;
}

void Army::Troop::Set(const Monster & m, u16 c)
{
    id = m();
    count = c;
}

void Army::Troop::Set(monster_t m, u16 c)
{
    id = m;
    count = c;
}

void Army::Troop::SetMonster(const Monster & m)
{
    id = m();
}

void Army::Troop::SetMonster(monster_t m)
{
    id = m;
}

void Army::Troop::SetCount(u16 c)
{
    count = c;
}

void Army::Troop::SetModes(u32 f)
{

    // check magic
    if(((IS_MAGIC & f) || (SP_DISRUPTINGRAY == f)) && Modes(SP_ANTIMAGIC)) return;

    switch(f)
    {
	case SP_STEELSKIN:
	    if(Modes(SP_STONESKIN))
	    {
		ResetModes(SP_STONESKIN);
		return;
	    }
	    break;

	case SP_STONESKIN:
	    if(Modes(SP_STEELSKIN))
	    {
		ResetModes(SP_STEELSKIN);
		return;
	    }
	    break;

	case SP_BLESS:
	    if(Modes(SP_CURSE))
	    {
		ResetModes(SP_CURSE);
		return;
	    }
	    break;

	case SP_CURSE:
	    if(Modes(SP_BLESS))
	    {
		ResetModes(SP_BLESS);
		return;
	    }
	    break;

	case SP_HASTE:
	    if(Modes(SP_SLOW))
	    {
		ResetModes(SP_SLOW);
		return;
	    }
	    break;

	case SP_SLOW:
	    if(Modes(SP_HASTE))
	    {
		ResetModes(SP_HASTE);
		return;
	    }
	    break;

	case SP_CURE:
	{
	    u32 max = Monster::GetHitPoints() * count;
	    if(army && army->commander) hp = (max < hp + 5 * army->commander->GetPower() ? max : hp + 5 * army->commander->GetPower());
	    ResetModes(IS_MAGIC);
	    return;
	}

	case SP_DISPEL:
	    ResetModes(IS_MAGIC);
	    return;

	case SP_ANTIMAGIC:
	    ResetModes(IS_MAGIC);
	    break;

	case SP_DISRUPTINRAY:
	    // TODO: need accumulate and fix GetDefense
	    break;

	default:
	    break;
    }

    BitModes::SetModes(f);
}

void Army::Troop::BattleInit(void)
{
    hp = Monster::GetHitPoints() * count;
}

void Army::Troop::BattleUpdate(void)
{
    // update count
    if(hp < static_cast<u32>(count * Monster::GetHitPoints()))
    {
	count = hp / Monster::GetHitPoints();
	if(hp > static_cast<u32>(count * Monster::GetHitPoints())) ++count;
    }

    // update other
}

void Army::Troop::BattleNewTurn(void)
{
    ResetModes(MODES_ALL);
    SetModes(BATTLE);

    switch(id)
    {
	case Monster::TROLL:
	case Monster::WAR_TROLL:
	    hp = Monster::GetHitPoints() * count;
	    break;

	default: break;
    }
}

void Army::Troop::Reset(void)
{
    id = Monster::UNKNOWN;
    count = 0;
}

const Skill::Primary* Army::Troop::MasterSkill(void) const
{
    return army ? army->commander : NULL;
}

const Army::army_t* Army::Troop::GetArmy(void) const
{
    return army;
}

const std::string & Army::Troop::GetName(void) const
{
    return 1 < count ? Monster::GetMultiName() : Monster::GetName();
}

u16 Army::Troop::Count(void) const
{
    return count;
}

u8 Army::Troop::GetAttack(void) const
{
    u8 mod = 0;

    if(Modes(SP_BLODLUST)) mod += 3;

    return army && army->commander ? army->commander->GetAttack() + Monster::GetAttack() + mod : Monster::GetAttack() + mod;
}

u8 Army::Troop::GetDefense(void) const
{
    u8 mod = 0;

    if(Modes(SP_STONESKIN)) mod += 3;
    if(Modes(SP_STEELSKIN)) mod += 5;
    if(Modes(SP_DISRUPTINGRAY)) mod -= 3;

    return army && army->commander ? army->commander->GetDefense() + Monster::GetDefense() + mod : Monster::GetDefense() + mod;
}

Color::color_t Army::Troop::GetColor(void) const
{
    return army && army->commander ? army->commander->GetColor() : Color::GRAY;
}

u32 Army::Troop::GetHitPoints(void) const
{
    return Modes(BATTLE) ? hp : Monster::GetHitPoints() * count;
}

u16 Army::Troop::GetDamage(void) const
{
    u16 dmg = 0;

    if(Modes(SP_BLESS))
	dmg = GetDamageMax();
    else
    if(Modes(SP_CURSE))
	dmg = GetDamageMin();
    else
	dmg = Rand::Get(GetDamageMin(), GetDamageMax());

    if(isArchers())
    {
	if(Modes(HANDFIGHTING))
	{
	    switch(id)
	    {
		// skip
		case Monster::MAGE:
		case Monster::ARCHMAGE:
		case Monster::TITAN: break;

		default: dmg /= 2; break;
	    }
	}
	else
	{
	    // check skill archery +%10, +%25, +%50
	    if(army && army->commander)
	    {
		switch(army->commander->GetLevelSkill(Skill::Secondary::ARCHERY))
		{
		    case Skill::Level::BASIC:	dmg += (dmg * 10) / 100; break;
		    case Skill::Level::ADVANCED:dmg += (dmg * 25) / 100; break;
		    case Skill::Level::EXPERT:	dmg += (dmg * 50) / 100; break;
		    default: break;
		}
	    }
	}
    }

    // check luck x2
    if(Modes(LUCK_GOOD)) dmg *= 2;
    else
    if(Modes(LUCK_BAD)) dmg /= 2;

    return dmg;
}

u16 Army::Troop::GetDamageMin(void) const
{
    return Monster::GetDamageMin() * count;
}

u16 Army::Troop::GetDamageMax(void) const
{
    return Monster::GetDamageMax() * count;
}

u8 Army::Troop::GetSpeed(void) const
{
    if(Modes(MOVED | SKIPMOVE | SP_BLIND | SP_PARALYZE | SP_HYPNOTIZE | SP_STONE)) return Speed::STANDING;

    if(Modes(SP_HASTE)) return (Speed::ULTRAFAST < Monster::GetSpeed() ? Speed::INSTANT : Monster::GetSpeed() + 2);
    else
    if(Modes(SP_SLOW)) return (Speed::SLOW > Monster::GetSpeed() ? Speed::CRAWLING : Monster::GetSpeed() - 2);

    return Monster::GetSpeed();
}

s8 Army::Troop::GetMorale(void) const
{
    if(isUndead() || isElemental()) return Morale::NORMAL;
    else
    if(army)
    {
	return army->commander ? army->commander->GetMorale() : army->GetMorale();
    }

    return Morale::NORMAL;
}

s8 Army::Troop::GetLuck(void) const
{
    if(army)
    {
	return army->commander ? army->commander->GetLuck() : army->GetLuck();
    }

    return Luck::NORMAL;
}

bool Army::Troop::isValid(void) const
{
    return Monster::UNKNOWN < id && count;
}

bool Army::isValidTroop(const Troop & troop)
{
    return troop.isValid();
}

bool Army::StrongestTroop(const Troop & t1, const Troop & t2)
{
    return t1.isValid() && t2.isValid() && (t1.GetDamageMin() > t2.GetDamageMin());
}

bool Army::WeakestTroop(const Troop & t1, const Troop & t2)
{
    return t1.isValid() && t2.isValid() && (t1.GetDamageMax() < t2.GetDamageMax());
}

bool Army::SlowestTroop(const Troop & t1, const Troop & t2)
  {
    return t1.isValid() && t2.isValid() && (t1.GetSpeed() < t2.GetSpeed());
}

bool Army::FastestTroop(const Troop & t1, const Troop & t2)
{
    return t1.isValid() && t2.isValid() && (t1.GetSpeed() > t2.GetSpeed());
}

void Army::SwapTroops(Troop & t1, Troop & t2)
{
    std::swap(t1, t2);
}

