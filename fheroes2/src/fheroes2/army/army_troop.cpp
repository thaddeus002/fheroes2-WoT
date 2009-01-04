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
#include "agg.h"
#include "speed.h"
#include "spell.h"
#include "luck.h"
#include "morale.h"
#include "army.h"
#include "army_troop.h"

Army::Troop::Troop(monster_t m, u16 c) : Monster(m), count(c), army(NULL), disruptingray(0)
{
}

Army::Troop::~Troop()
{
    if(contours.size())
    {
	std::vector<Surface *>::const_iterator it1 = contours.begin();
	std::vector<Surface *>::const_iterator it2 = contours.end();

	for(; it1 != it2; ++it1) if(*it1) delete *it1;
    }
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

void Army::Troop::ResetModes(u32 f)
{
    if(!isValid()) return;

    switch(f)
    {
	case BATTLE:
	    disruptingray = 0;
	    modes = 0;
	    return;

	default:
	    break;
    }

    BitModes::ResetModes(f);
}

void Army::Troop::SetModes(u32 f)
{
    if(!isValid()) return;

    // check magic
    if(((IS_MAGIC & f) || (SP_DISRUPTINGRAY == f)) && Modes(SP_ANTIMAGIC)) return;

    switch(f)
    {
	// init battle
	case BATTLE:
	    hp = Monster::GetHitPoints() * count;
	    break;

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
	case SP_DISPEL:
	    ResetModes(IS_MAGIC);
	    return;

	case SP_ANTIMAGIC:
	    ResetModes(IS_MAGIC);
	    break;

	case SP_DISRUPTINGRAY:
	    ++disruptingray;
	    break;

	default:
	    break;
    }

    BitModes::SetModes(f);
}

bool Army::Troop::BattleApplySpell(u8 spell, u8 sp)
{
    // global magic resistant
    switch(id)
    {
	case Monster::GREEN_DRAGON:
	case Monster::RED_DRAGON:
	case Monster::BLACK_DRAGON: return false;

	default: break;
    }

    if(Modes(SP_ANTIMAGIC)) return false;

    u16 dmg = Spell::GetInflictDamage(spell, sp);


    // local magic resistant
    if(isUndead())
    {
	switch(spell)
	{
	    case Spell::DEATHRIPPLE:
	    case Spell::DEATHWAVE: dmg = 0; break;

	    case Spell::CURSE:
	    case Spell::MASSCURSE:
	    case Spell::BLESS:
	    case Spell::MASSBLESS:
	    case Spell::BLIND:
	    case Spell::PARALYZE:
	    case Spell::HYPNOTIZE:
	    case Spell::BERZERKER: return false;
	    default: break;
	}
    }
    // is live
    else
    {
	switch(spell)
	{
	    case Spell::HOLYSHOUT:
	    case Spell::HOLYWORD: dmg = 0; break;
	    default: break;
	}
    }

    if(isElemental())
    {
	switch(spell)
	{
	    case Spell::BLIND:
	    case Spell::PARALYZE:
	    case Spell::HYPNOTIZE:
	    case Spell::BERZERKER: return false;
	    default: break;
	}
    }

    if(Spell::isBad(spell))
    {
	switch(id)
	{
            case Monster::EARTH_ELEMENT:
		if(Spell::LIGHTNINGBOLT == spell ||
		    Spell::CHAINLIGHTNING == spell ||
		    Spell::METEORSHOWER == spell) return false; break;

            case Monster::AIR_ELEMENT:
		if(Spell::METEORSHOWER == spell) return false;
		else
		if(Spell::LIGHTNINGBOLT == spell ||
		    Spell::CHAINLIGHTNING == spell ||
		    Spell::ELEMENTALSTORM == spell) dmg *= 2; break;

            case Monster::FIRE_ELEMENT:
		if(Spell::FIREBALL == spell ||
		    Spell::FIREBLAST) return false;
		else
		if(Spell::COLDRAY == spell ||
		    Spell::COLDRING == spell) dmg *= 2; break;

    	    case Monster::WATER_ELEMENT:
		if(Spell::COLDRAY == spell ||
		    Spell::COLDRING == spell) return false;
		else
		if(Spell::FIREBALL == spell ||
		    Spell::FIREBLAST) dmg *= 2; break;

	    case Monster::DWARF:
	    case Monster::BATTLE_DWARF:
		if(3 == Rand::Get(1,4)) return false; break;

	    case Monster::GIANT:
	    case Monster::TITAN:
		if(Spell::BLIND == spell ||
		    Spell::PARALYZE == spell ||
		    Spell::BERZERKER == spell ||
		    Spell::HYPNOTIZE == spell) return false; break;

	    case Monster::PHOENIX:
		if(Spell::COLDRAY == spell ||
		    Spell::LIGHTNINGBOLT == spell ||
		    Spell::COLDRING == spell ||
		    Spell::FIREBALL == spell ||
		    Spell::CHAINLIGHTNING == spell ||
		    Spell::ELEMENTALSTORM == spell ||
		    Spell::FIREBLAST) return false; break;

	    case Monster::CRUSADER:
		if(Spell::CURSE == spell ||
		    Spell::MASSCURSE == spell) return false; break;

            case Monster::IRON_GOLEM:
    	    case Monster::STEEL_GOLEM:
		if(Spell::COLDRAY == spell ||
		    Spell::LIGHTNINGBOLT == spell ||
		    Spell::COLDRING == spell ||
		    Spell::FIREBALL == spell ||
		    Spell::CHAINLIGHTNING == spell ||
		    Spell::ELEMENTALSTORM == spell ||
		    Spell::FIREBLAST) dmg /=2; break;

	    default: break;
	}
    }

    // apply damage
    if(dmg)
    {
	if(dmg > hp) hp = 0;
	else hp -= dmg;

	BattleUpdateHitPoints();
    }

    if(Spell::CURE == spell || Spell::MASSCURE == spell)
    {
	u32 max = Monster::GetHitPoints() * count;
	if(army && army->commander) hp = (max < hp + 5 * sp ? max : hp + 5 * sp);
    }

    // set modes
    switch(spell)
    {
	case Spell::CURE:
	case Spell::MASSCURE: SetModes(SP_CURE); break;

	case Spell::DISPEL:
	case Spell::MASSDISPEL: SetModes(SP_DISPEL); break;

	case Spell::BLESS:
	case Spell::MASSBLESS: SetModes(SP_BLESS); break;

	case Spell::BLOODLUST: SetModes(SP_BLODLUST); break;

	case Spell::CURSE:
	case Spell::MASSCURSE: SetModes(SP_CURSE); break;

    	case Spell::HASTE:
	case Spell::MASSHASTE: SetModes(SP_HASTE); break;

	case Spell::SHIELD:
	case Spell::MASSSHIELD: SetModes(SP_SHIELD); break;

	case Spell::SLOW:
	case Spell::MASSSLOW: SetModes(SP_SLOW); break;

	case Spell::STONESKIN: SetModes(SP_STONESKIN); break;
	case Spell::STEELSKIN: SetModes(SP_STEELSKIN); break;

	case Spell::ANTIMAGIC: SetModes(SP_ANTIMAGIC); break;

	case Spell::BLIND: SetModes(SP_BLIND); break;
	case Spell::PARALYZE: SetModes(SP_PARALYZE); break;
	case Spell::BERZERKER: SetModes(SP_BERZERKER); break;
	case Spell::HYPNOTIZE: SetModes(SP_HYPNOTIZE); break;

	case Spell::STONE: SetModes(SP_STONE); break;

	case Spell::DRAGONSLAYER: SetModes(SP_DRAGONSLAYER); break;
	case Spell::DISRUPTINGRAY: SetModes(SP_DISRUPTINGRAY); break;

	default: break;
    }

    return true;
}

void Army::Troop::BattleUpdateHitPoints(void)
{
    if(!isValid()) return;

    // update count
    if(hp < static_cast<u32>(count * Monster::GetHitPoints()))
    {
	count = hp / Monster::GetHitPoints();
	if(hp > static_cast<u32>(count * Monster::GetHitPoints())) ++count;
    }
}

void Army::Troop::BattleNewTurn(void)
{
    if(!isValid()) return;

    switch(id)
    {
	case Monster::TROLL:
	case Monster::WAR_TROLL:
	    hp = Monster::GetHitPoints() * count;
	    break;

	default: break;
    }
    ResetModes(Army::MOVED);
    ResetModes(Army::SKIPMOVE);
    ResetModes(Army::MORALE_BAD);
    ResetModes(Army::MORALE_GOOD);
    ResetModes(Army::LUCK_BAD);
    ResetModes(Army::LUCK_GOOD);

    // check for reset flags SP_MAGIC

    // define morale
    switch(GetMorale())
    {
	case Morale::TREASON:	if(9 > Rand::Get(1, 16)) SetModes(Army::MORALE_BAD); break;	// 50%
        case Morale::AWFUL:	if(6 > Rand::Get(1, 15)) SetModes(Army::MORALE_BAD); break;	// 30%
        case Morale::POOR:	if(2 > Rand::Get(1, 15)) SetModes(Army::MORALE_BAD); break;	// 15%
        case Morale::GOOD:	if(2 > Rand::Get(1, 15)) SetModes(Army::MORALE_GOOD); break;	// 15%
        case Morale::GREAT:	if(6 > Rand::Get(1, 15)) SetModes(Army::MORALE_GOOD); break;	// 30%
        case Morale::BLOOD:	if(9 > Rand::Get(1, 16)) SetModes(Army::MORALE_GOOD); break;	// 50%
	default: break;
    }

    // define luck
    switch(GetLuck())
    {
        case Luck::CURSED:	if(9 > Rand::Get(1, 16)) SetModes(Army::LUCK_BAD); break;	// 50%
        case Luck::AWFUL:	if(6 > Rand::Get(1, 15)) SetModes(Army::LUCK_BAD); break;	// 30%
        case Luck::BAD:		if(2 > Rand::Get(1, 15)) SetModes(Army::LUCK_BAD); break;	// 15%
        case Luck::GOOD:	if(2 > Rand::Get(1, 15)) SetModes(Army::LUCK_GOOD); break;	// 15%
        case Luck::GREAT:	if(6 > Rand::Get(1, 15)) SetModes(Army::LUCK_GOOD); break;	// 30%
        case Luck::IRISH:	if(9 > Rand::Get(1, 16)) SetModes(Army::LUCK_GOOD); break;	// 50%
	default: break;
    }
}

void Army::Troop::BattleLoadContours(bool inv)
{
    if(!isValid()) return;

    if(contours.size())
    {
	std::vector<Surface *>::const_iterator it1 = contours.begin();
	std::vector<Surface *>::const_iterator it2 = contours.end();

	for(; it1 != it2; ++it1) if(*it1) delete *it1;
    }

    u8 start = 0;
    u8 length = 0;
    GetAnimFrames(Monster::AS_IDLE, start, length);

    contours.reserve(length);

    for(u8 ii = start; ii < start + length; ++ii)
    {
	const Sprite & sprite = AGG::GetICN(ICNFile(), ii, inv);

        Surface *contour = new Surface();
        sprite.MakeContour(*contour, sprite.GetColor(0xDA));

	contours.push_back(contour);
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

    mod += army && army->commander ? army->commander->GetDefense() + Monster::GetDefense() : Monster::GetDefense();

    if(disruptingray) mod = (mod <= disruptingray * 3 ? 1 : mod - disruptingray * 3);

    return mod;
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
    if(Modes(BATTLE))
    {
	if(!isValid()) return Speed::STANDING;

	if(Modes(SKIPMOVE | SP_BLIND | SP_PARALYZE | SP_HYPNOTIZE | SP_STONE)) return Speed::STANDING;

	if(!Modes(MORALE_GOOD) && Modes(MOVED)) return Speed::STANDING;

	if(Modes(SP_HASTE)) return (Speed::ULTRAFAST < Monster::GetSpeed() ? Speed::INSTANT : Monster::GetSpeed() + 2);
	else
	if(Modes(SP_SLOW)) return (Speed::SLOW > Monster::GetSpeed() ? Speed::CRAWLING : Monster::GetSpeed() - 2);
    }

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

const Surface* Army::Troop::GetContour(u8 index) const
{
    return (contours.empty() || index >= contours.size() ? NULL : contours[index]);
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
    return t1.GetDamageMin() > t2.GetDamageMin();
}

bool Army::WeakestTroop(const Troop & t1, const Troop & t2)
{
    return t1.GetDamageMax() < t2.GetDamageMax();
}

bool Army::SlowestTroop(const Troop & t1, const Troop & t2)
{
    return t1.GetSpeed() < t2.GetSpeed();
}

bool Army::FastestTroop(const Troop & t1, const Troop & t2)
{
    return t1.GetSpeed() > t2.GetSpeed();
}

void Army::SwapTroops(Troop & t1, Troop & t2)
{
    std::swap(t1, t2);
}
