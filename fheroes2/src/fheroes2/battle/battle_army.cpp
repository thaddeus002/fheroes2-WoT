/***************************************************************************
 *   Copyright (C) 2012 by Andrey Afletdinov <fheroes2@gmail.com>          *
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

#include "world.h"
#include "speed.h"
#include "settings.h"
#include "heroes_base.h"
#include "battle_troop.h"
#include "battle_army.h"

#define CAPACITY 16

namespace Battle
{
    bool AllowPart1(const Unit* b)
    {
        return ! b->Modes(TR_SKIPMOVE) && Speed::STANDING < b->GetSpeed();
    }

    bool AllowPart2(const Unit* b)
    {
        return b->Modes(TR_SKIPMOVE) && Speed::STANDING < b->GetSpeed();
    }
}

Battle::Units::Units()
{
    reserve(CAPACITY);
}

Battle::Units::Units(const Units & units)
{
    reserve(CAPACITY < units.size() ? units.size() : CAPACITY);
    assign(units.begin(), units.end());
}

Battle::Units::Units(const Units & units1, const Units & units2)
{
    const size_t capacity = units1.size() + units2.size();
    reserve(CAPACITY < capacity ? capacity : CAPACITY);
    insert(end(), units1.begin(), units1.end());
    insert(end(), units2.begin(), units2.end());
}

Battle::Units::~Units()
{
}

Battle::Units & Battle::Units::operator= (const Units & units)
{
    reserve(CAPACITY < units.size() ? units.size() : CAPACITY);
    assign(units.begin(), units.end());

    return *this;
}

void Battle::Units::SortSlowest(void)
{
    std::sort(begin(), end(), Army::SlowestTroop);
}

void Battle::Units::SortFastest(void)
{
    std::sort(begin(), end(), Army::FastestTroop);
}

void Battle::Units::SortStrongest(void)
{
    std::sort(begin(), end(), Army::StrongestTroop);
}

void Battle::Units::SortWeakest(void)
{
    std::sort(begin(), end(), Army::WeakestTroop);
}

Battle::Unit* Battle::Units::FindUID(u32 pid)
{
    iterator it = std::find_if(begin(), end(),
                        std::bind2nd(std::mem_fun(&Unit::isUID), pid));

    return it == end() ? NULL : *it;
}

Battle::Unit* Battle::Units::FindMode(u32 mod)
{
    iterator it = std::find_if(begin(), end(),
                            std::bind2nd(std::mem_fun(&Unit::Modes), mod));

    return it == end() ? NULL : *it;
}







Battle::Force::Force(Army & parent, bool opposite) : army(parent)
{
    for(u8 index = 0; index < army.Size(); ++index)
    {
	const Troop* troop = army.GetTroop(index);
	const u16 position = army.isSpreadFormat() ? index * 22 : 22 + index * 11;

	if(troop && troop->isValid())
	{
	    push_back(new Unit(*troop, World::GetUniq(), (opposite ? position + 10 : position), opposite));
	    back()->SetArmy(army);
	}
    }
}

Battle::Force::~Force()
{
    for(iterator it = begin(); it != end(); ++it) delete *it;
}

const HeroBase* Battle::Force::GetCommander(void) const
{
    return army.GetCommander();
}

HeroBase* Battle::Force::GetCommander(void)
{
    return army.GetCommander();
}

u8 Battle::Force::GetColor(void) const
{
    return army.GetColor();
}

u8 Battle::Force::GetControl(void) const
{
    return army.GetControl();
}

bool Battle::Force::isValid(void) const
{
    return end() != std::find_if(begin(), end(), std::mem_fun(&Unit::isValid));
}

u32 Battle::Force::GetSurrenderCost(void) const
{
    float res = 0;

    for(const_iterator it = begin(); it != end(); ++it)
        if((*it)->isValid())
    {
        // FIXME: orig: 3 titan = 7500
        payment_t payment = (*it)->GetCost();
        res += payment.gold;
    }

    const HeroBase* commander = GetCommander();

    if(commander)
    {
        switch(commander->GetLevelSkill(Skill::Secondary::DIPLOMACY))
        {
            // 40%
            case Skill::Level::BASIC: res = res * 40 / 100; break;
            // 30%
            case Skill::Level::ADVANCED: res = res * 30 / 100; break;
            // 20%
            case Skill::Level::EXPERT: res = res * 20 / 100; break;
            // 50%
            default: res = res * 50 / 100; break;
        }

        Artifact art(Artifact::STATESMAN_QUILL);

        if(commander->HasArtifact(art))
            res -= res * art.ExtraValue() / 100;
    }

    // limit
    if(res < 100) res = 100.0;

    return static_cast<u32>(res);
}

void Battle::Force::NewTurn(void)
{
    if(GetCommander())
        GetCommander()->ResetModes(Heroes::SPELLCASTED);

    std::for_each(begin(), end(), std::mem_fun(&Unit::NewTurn));
}

Battle::Unit* Battle::Force::GetCurrentUnit(const Force & army1, const Force & army2, Unit* last, Units* all, bool part1)
{
    Units units1(army1);
    Units units2(army2);

    if(all)
	*all = Units(army1, army2);

    if(part1 || Settings::Get().ExtBattleReverseWaitOrder())
    {
        units1.SortFastest();
        units2.SortFastest();
	if(all) all->SortFastest();
    }
    else
    {
        units1.SortSlowest();
        units2.SortSlowest();
	if(all) all->SortSlowest();
    }

    Unit* result = NULL;

    Units::iterator it1 = part1 ? std::find_if(units1.begin(), units1.end(), AllowPart1) :
                                    std::find_if(units1.begin(), units1.end(), AllowPart2);

    Units::iterator it2 = part1 ? std::find_if(units2.begin(), units2.end(), AllowPart1) :
                                    std::find_if(units2.begin(), units2.end(), AllowPart2);

    if(it1 != units1.end() &&
        it2 != units2.end())
    {
        // attacker first
        if((*it1)->GetSpeed() > (*it2)->GetSpeed())
        {
            result = *it1;
        }
        else
        if((*it2)->GetSpeed() > (*it1)->GetSpeed())
        {
            result = *it2;
        }
        else
        {
            // attacker first
            if(!last ||
                army2.GetColor() == last->GetColor())
            {
                result = *it1;
            }
            else
            {
                result = *it2;
            }
        }
    }
    else
    if(it1 != units1.end())
        result = *it1;
    else
    if(it2 != units2.end())
        result = *it2;

    return result &&
        result->isValid() &&
        result->GetSpeed() > Speed::STANDING ? result : NULL;
}

StreamBase & Battle::operator<< (StreamBase & msg, const Force & f)
{
    msg << static_cast<u32>(f.size());

    for(Force::const_iterator
	it = f.begin(); it != f.end(); ++it)
        msg << (*it)->GetUID() << **it;

    return msg;
}

StreamBase & Battle::operator>> (StreamBase & msg, Force & f)
{
    u32 size = 0;
    u32 uid  = 0;

    msg >> size;

    for(u32 ii = 0; ii < size; ++ii)
    {
        msg >> uid;
        Unit* b = f.FindUID(uid);
        if(b) msg >> *b;
    }

    return msg;
}

Troops Battle::Force::GetKilledTroops(void) const
{
    Troops killed;

    for(const_iterator
        it = begin(); it != end(); ++it)
    {
        const Unit & b = (**it);
        killed.PushBack(b, b.GetDead());
    }

    return killed;
}

bool Battle::Force::SetIdleAnimation(void)
{
    bool res = false;

    for(iterator it = begin(); it != end(); ++it)
    {
        Unit & unit = **it;

        if(unit.isValid())
        {
            if(unit.isFinishAnimFrame())
                unit.ResetAnimFrame(AS_IDLE);
            else
            if(unit.isStartAnimFrame() && 3 > Rand::Get(1, 10))
            {
                unit.IncreaseAnimFrame();
                res = true;
            }
        }
    }

    return res;
}

bool Battle::Force::NextIdleAnimation(void)
{
    bool res = false;

    for(iterator it = begin(); it != end(); ++it)
    {
        Unit & unit = **it;

        if(unit.isValid() && !unit.isStartAnimFrame())
        {
            unit.IncreaseAnimFrame(false);
            res = true;
        }
    }

    return res;
}

bool Battle::Force::HasMonster(const Monster & mons) const
{
    return end() != std::find_if(begin(), end(),
	std::bind2nd(std::mem_fun(&Troop::isMonster), mons()));
}

u32 Battle::Force::GetDeadCounts(void) const
{
    u32 res = 0;

    for(const_iterator it = begin(); it != end(); ++it)
        res += (*it)->GetDead();

    return res;
}

u32 Battle::Force::GetDeadHitPoints(void) const
{
    u32 res = 0;

    for(const_iterator it = begin(); it != end(); ++it)
    {
        res += static_cast<Monster*>(*it)->GetHitPoints() * (*it)->GetDead();
    }

    return res;
}

void Battle::Force::SyncArmyCount(void)
{
    for(u8 index = 0; index < army.Size(); ++index)
    {
	Troop* troop = army.GetTroop(index);

	if(troop && troop->isValid() && index < size())
	    troop->SetCount(at(index)->GetCount());
    }
}
