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
#include "settings.h"
#include "battle_troop.h"
#include "payment.h"
#include "world.h"
#include "kingdom.h"
#include "maps_tiles.h"
#include "text.h"
#include "luck.h"
#include "morale.h"
#include "castle.h"
#include "army.h"

std::vector<Army::Troop>::iterator MaxElement(std::vector<Army::Troop>::iterator first, std::vector<Army::Troop>::iterator last, bool (*pf)(const Army::Troop &, const Army::Troop &))
{
    while(first != last) if(Army::isValidTroop(*first)) break; else ++first;
    std::vector<Army::Troop>::iterator largest = first;
    if(first == last) return last;
    while(++first != last) if(Army::isValidTroop(*first) && pf(*first, *largest)) largest = first;
    return largest;
}

std::vector<Army::Troop>::const_iterator MaxElement(std::vector<Army::Troop>::const_iterator first, std::vector<Army::Troop>::const_iterator last, bool (*pf)(const Army::Troop &, const Army::Troop &))
{
    while(first != last) if(Army::isValidTroop(*first)) break; else ++first;
    std::vector<Army::Troop>::const_iterator largest = first;
    if(first == last) return last;
    while(++first != last) if(Army::isValidTroop(*first) && pf(*first, *largest)) largest = first;
    return largest;
}

std::vector<Army::Troop>::iterator MinElement(std::vector<Army::Troop>::iterator first, std::vector<Army::Troop>::iterator last, bool (*pf)(const Army::Troop &, const Army::Troop &))
{
    while(first != last) if(Army::isValidTroop(*first)) break; else ++first;
    std::vector<Army::Troop>::iterator lowest = first;
    if(first == last) return last;
    while(++first != last) if(Army::isValidTroop(*first) && pf(*first, *lowest)) lowest = first;
    return lowest;
}

std::vector<Army::Troop>::const_iterator MinElement(std::vector<Army::Troop>::const_iterator first, std::vector<Army::Troop>::const_iterator last, bool (*pf)(const Army::Troop &, const Army::Troop &))
{
    while(first != last) if(Army::isValidTroop(*first)) break; else ++first;
    std::vector<Army::Troop>::const_iterator lowest = first;
    if(first == last) return last;
    while(++first != last) if(Army::isValidTroop(*first) && pf(*first, *lowest)) lowest = first;
    return lowest;
}

const std::string & Army::String(Army::armysize_t size)
{
    static const std::string str_size[] = { "Few", "Several", "Pack", "Lots", "Horde", "Throng", "Swarm", "Zounds", "Legion" };

    switch(size)
    {
	case FEW:	return str_size[0];
        case SEVERAL:	return str_size[1];
        case PACK:	return str_size[2];
        case LOTS:	return str_size[3];
        case HORDE:	return str_size[4];
        case THRONG:	return str_size[5];
        case SWARM:	return str_size[6];
        case ZOUNDS:	return str_size[7];
        case LEGION:	return str_size[8];
    }

    return str_size[0];
}

Army::armysize_t Army::GetSize(u16 count)
{
    if(LEGION <= count)		return LEGION;
    else
    if(ZOUNDS <= count)		return ZOUNDS;
    else
    if(SWARM <= count)		return SWARM;
    else
    if(THRONG <= count)		return THRONG;
    else
    if(HORDE <= count)		return HORDE;
    else
    if(LOTS <= count)		return LOTS;
    else
    if(PACK <= count)		return PACK;
    else
    if(SEVERAL <= count)	return SEVERAL;

    return FEW;
}

Army::army_t::army_t(const Skill::Primary* s) : army(ARMYMAXTROOPS), commander(s)
{
    std::vector<Troop>::iterator it1 = army.begin();
    std::vector<Troop>::const_iterator it2 = army.end();
    for(; it1 != it2; ++it1) (*it1).army = this;
}

// deprecated, will be removed!
Army::army_t::army_t & Army::army_t::operator= (const army_t & a)
{
    Import(a.army);

    return *this;
}

void Army::army_t::FromGuardian(const Maps::Tiles & t)
{
    Reset();

    u8 obj = t.GetObject();

    if(MP2::OBJ_HEROES == obj)
    {
	const Heroes* hero = world.GetHeroes(t.GetIndex());
	if(hero) obj = hero->GetUnderObject();
    }

    switch(obj)
    {
	case MP2::OBJ_PYRAMID:
            army[0].Set(Monster::ROYAL_MUMMY, 10);
            army[1].Set(Monster::LORD_VAMPIRE, 10);
            army[2].Set(Monster::ROYAL_MUMMY, 10);
            army[3].Set(Monster::LORD_VAMPIRE, 10);
            army[4].Set(Monster::ROYAL_MUMMY, 10);
	    break;

	case MP2::OBJ_GRAVEYARD:
	    army[0].Set(Monster::MUTANT_ZOMBIE, 100);
	    ArrangeForBattle();
	    break;

	case MP2::OBJ_SHIPWRECK:
	    army[0].Set(Monster::GHOST, t.GetQuantity2());
	    ArrangeForBattle();
	    break;

	case MP2::OBJ_DERELICTSHIP:
	    army[0].Set(Monster::SKELETON, 200);
	    ArrangeForBattle();
	    break;

	case MP2::OBJ_ARTIFACT:
	    switch(t.GetQuantity2())
	    {
		case 6:	army[0].Set(Monster::ROGUE, 50); break;	
		case 7:	army[0].Set(Monster::GENIE, 1); break;	
		case 8:	army[0].Set(Monster::PALADIN, 1); break;	
		case 9:	army[0].Set(Monster::CYCLOPS, 1); break;	
		case 10:army[0].Set(Monster::PHOENIX, 1); break;	
		case 11:army[0].Set(Monster::GREEN_DRAGON, 1); break;	
		case 12:army[0].Set(Monster::TITAN, 1); break;	
		case 13:army[0].Set(Monster::BONE_DRAGON, 1); break;
		default: break;	
	    }
	    ArrangeForBattle();
	    break;

	case MP2::OBJ_ABANDONEDMINE:
	    army[0].Set(Monster::GHOST, t.GetQuantity2());
	    ArrangeForBattle();
	    break;

	case MP2::OBJ_CITYDEAD:
            army[0].Set(Monster::ZOMBIE, 20);
            army[1].Set(Monster::LORD_VAMPIRE, 5);
            army[2].Set(Monster::POWER_LICH, 5);
            army[3].Set(Monster::LORD_VAMPIRE, 5);
            army[4].Set(Monster::ZOMBIE, 20);
	    break;

	case MP2::OBJ_TROLLBRIDGE:
            army[0].Set(Monster::TROLL, 4);
            army[1].Set(Monster::WAR_TROLL, 4);
            army[2].Set(Monster::TROLL, 4);
            army[3].Set(Monster::WAR_TROLL, 4);
            army[4].Set(Monster::TROLL, 4);
	    break;

	case MP2::OBJ_DRAGONCITY:
            army[0].Set(Monster::GREEN_DRAGON, 3);
            army[1].Set(Monster::RED_DRAGON, 2);
            army[2].Set(Monster::BLACK_DRAGON, 1);
	    break;

	case MP2::OBJ_DAEMONCAVE:
            army[0].Set(Monster::EARTH_ELEMENT, 2);
            army[1].Set(Monster::EARTH_ELEMENT, 2);
            army[2].Set(Monster::EARTH_ELEMENT, 2);
            army[3].Set(Monster::EARTH_ELEMENT, 2);
	    break;

	default: break;
    }
}

void Army::army_t::Import(const std::vector<Troop> & v)
{
    for(u8 ii = 0; ii < ARMYMAXTROOPS; ++ii)
    {
	if(ii < v.size())
	    army[ii] = v[ii];
	else
	    army[ii].Reset();
    }
}

void Army::army_t::Import(const std::vector<BattleTroop> & v)
{
    for(u8 ii = 0; ii < ARMYMAXTROOPS; ++ii)
    {
	if(ii < v.size())
	    army[ii] = v[ii];
	else
	    army[ii].Reset();
    }
}

void Army::army_t::UpgradeMonsters(const Monster & m)
{
    UpgradeMonsters(m());
}

void Army::army_t::UpgradeMonsters(const Monster::monster_t m)
{
    for(u8 ii = 0; ii < ARMYMAXTROOPS; ++ii) if(army[ii].isValid() && army[ii] == m) army[ii].Upgrade();
}

u8 Army::army_t::Size(void) const
{
    return ARMYMAXTROOPS;
}

Army::Troop & Army::army_t::FirstValid(void)
{
    std::vector<Troop>::iterator it = std::find_if(army.begin(), army.end(), Army::isValidTroop);

    if(it == army.end())
    {
	Error::Warning("Army::FirstValid: not found, return first invalid..");
	it = army.begin();
    }

    return *it;
}

Army::Troop & Army::army_t::At(u8 index)
{
    return index < ARMYMAXTROOPS ? army[index] : army[ARMYMAXTROOPS - 1];
}

const Army::Troop & Army::army_t::At(u8 index) const
{
    return index < ARMYMAXTROOPS ? army[index] : army[ARMYMAXTROOPS - 1];
}

u8 Army::army_t::GetCount(void) const
{
    return std::count_if(army.begin(), army.end(), Army::isValidTroop);
}

u8 Army::army_t::GetUniqCount(void) const
{
    std::vector<Monster::monster_t> troops;
    troops.reserve(ARMYMAXTROOPS);

    std::vector<Troop>::const_iterator it1 = army.begin();
    std::vector<Troop>::const_iterator it2 = army.end();
    for(; it1 != it2; ++it1) if((*it1).isValid()) troops.push_back((*it1).GetID());
    troops.resize(std::unique(troops.begin(), troops.end()) - troops.begin());

    return troops.size();
}

u16 Army::army_t::GetCountMonsters(const Monster & m) const
{
    return GetCountMonsters(m());
}

u16 Army::army_t::GetCountMonsters(const Monster::monster_t m) const
{
    u16 c = 0;

    std::vector<Troop>::const_iterator it1 = army.begin();
    std::vector<Troop>::const_iterator it2 = army.end();
    for(; it1 != it2; ++it1) if((*it1).isValid() && (*it1) == m) c += (*it1).Count();

    return c;
}

bool Army::army_t::JoinTroop(const Monster & mon, const u16 count)
{
    return JoinTroop(Troop(mon.GetID(), count));
}

bool Army::army_t::JoinTroop(const Monster::monster_t mon, const u16 count)
{
    return JoinTroop(Troop(mon, count));
}

bool Army::army_t::JoinTroop(const Troop & troop)
{
    if(!troop.isValid()) return false;

    std::vector<Troop>::iterator it;
    it = std::find_if(army.begin(), army.end(), std::bind2nd(std::mem_fun_ref(&Troop::HasMonster), troop()));

    if(it != army.end())
    {
	(*it).SetCount((*it).Count() + troop.Count());
	if(2 < Settings::Get().Debug()) Error::Verbose("Army::JoinTroop: monster: " + troop.GetName() + ", count: ", troop.Count());
	return true;
    }

    it = std::find_if(army.begin(), army.end(), std::not1(std::mem_fun_ref(&Troop::isValid)));
    if(it != army.end())
    {
	(*it).Set(troop, troop.Count());
	if(2 < Settings::Get().Debug()) Error::Verbose("Army::JoinTroop: monster: " + troop.GetName() + ", count: ", troop.Count());
	return true;
    }

    return true;
}

bool Army::army_t::isValid(void) const
{
    return army.end() != std::find_if(army.begin(),army.end(), Army::isValidTroop);
}

bool Army::army_t::HasMonster(const Monster & mons) const
{
    return HasMonster(mons());
}

bool Army::army_t::HasMonster(const Monster::monster_t mons) const
{
    return army.end() != std::find_if(army.begin(), army.end(), std::bind2nd(std::mem_fun_ref(&Troop::HasMonster), mons));
}

Color::color_t Army::army_t::GetColor(void) const
{
    return commander ? commander->GetColor() : Color::GRAY;
}

Race::race_t Army::army_t::GetRace(void) const
{
    std::vector<Troop>::const_iterator it1 = army.begin();
    std::vector<Troop>::const_iterator it2 = army.end();
    std::vector<Race::race_t> races;
    races.reserve(ARMYMAXTROOPS);

    for(; it1 != it2; ++it1) if((*it1).isValid()) races.push_back((*it1).GetRace());
    races.resize(std::unique(races.begin(), races.end()) - races.begin());

    if(races.empty())
    {
        Error::Warning("Army::GetRaceArmy: empty");
        return Race::MULT;
    }

    return 1 < races.size() ? Race::MULT : races.at(0);
}

s8 Army::army_t::GetLuck(void) const
{
    return GetLuckWithModificators();
}

s8 Army::army_t::GetLuckWithModificators(std::list<std::string> *list) const
{
    return Luck::NORMAL;
}

s8 Army::army_t::GetMorale(void) const
{
    return GetMoraleWithModificators();
}

// TODO:: need optimize
s8 Army::army_t::GetMoraleWithModificators(std::list<std::string> *list) const
{
    s8 result(Morale::NORMAL);

    // different race penalty
    std::vector<Troop>::const_iterator it1 = army.begin();
    std::vector<Troop>::const_iterator it2 = army.end();
    u8 count = 0;
    u8 count_kngt = 0;
    u8 count_barb = 0;
    u8 count_sorc = 0;
    u8 count_wrlk = 0;
    u8 count_wzrd = 0;
    u8 count_necr = 0;
    u8 count_bomg = 0;
    bool ghost_present = false;
    for(; it1 != it2; ++it1) if((*it1).isValid())
    {
        switch((*it1).GetRace())
	{
            case Race::KNGT: ++count_kngt; break;
            case Race::BARB: ++count_barb; break;
            case Race::SORC: ++count_sorc; break;
            case Race::WRLK: ++count_wrlk; break;
            case Race::WZRD: ++count_wzrd; break;
            case Race::NECR: ++count_necr; break;
            case Race::BOMG: ++count_bomg; break;
            default: break;
	}
        if(*it1 == Monster::GHOST) ghost_present = true;
    }

    Race::race_t r = Race::MULT;
    if(count_kngt){ ++count; r = Race::KNGT; }
    if(count_barb){ ++count; r = Race::BARB; }
    if(count_sorc){ ++count; r = Race::SORC; }
    if(count_wrlk){ ++count; r = Race::WRLK; }
    if(count_wzrd){ ++count; r = Race::WZRD; }
    if(count_necr){ ++count; r = Race::NECR; }
    if(count_bomg){ ++count; r = Race::BOMG; }
    const u8 uniq_count = GetUniqCount();

    switch(count)
    {
        case 2:
        case 0: break;
        case 1:
    	    if(0 == count_necr && !ghost_present && 1 < uniq_count)
            {
                ++result;
                if(list) list->push_back("All " + Race::String(r) + " troops +1");
            }
            break;
        case 3:
            result -= 1;
            if(list) list->push_back("Troops of 3 alignments -1");
            break;
        case 4:
    	    result -= 2;
            if(list) list->push_back("Troops of 4 alignments -2");
            break;
        default:
            result -= 3;
            if(list) list->push_back("Troops of 5 alignments -3");
            break;
    }

    // undead in life group
    if(1 < uniq_count && (count_necr || ghost_present) && (count_kngt || count_barb || count_sorc || count_wrlk || count_wzrd || count_bomg))
    {
        result -= 1;
        if(list) list->push_back("Some undead in groups -1");
    }

    return result;
}

Army::Troop & Army::army_t::GetSlowestTroop(void)
{
    return *MinElement(army.begin(), army.end(), SlowestTroop);
}

Army::Troop & Army::army_t::GetFastestTroop(void)
{
    return *MaxElement(army.begin(), army.end(), FastestTroop);
}

Army::Troop & Army::army_t::GetStrongestTroop(void)
{
    return *MaxElement(army.begin(), army.end(), StrongestTroop);
}

Army::Troop & Army::army_t::GetWeakestTroop(void)
{
    return *MinElement(army.begin(), army.end(), WeakestTroop);
}

const Army::Troop & Army::army_t::GetSlowestTroop(void) const
{
    return *MinElement(army.begin(), army.end(), SlowestTroop);
}

const Army::Troop & Army::army_t::GetFastestTroop(void) const
{
    return *MaxElement(army.begin(), army.end(), FastestTroop);
}

const Army::Troop & Army::army_t::GetStrongestTroop(void) const
{
    return *MaxElement(army.begin(), army.end(), StrongestTroop);
}

const Army::Troop & Army::army_t::GetWeakestTroop(void) const
{
    return *MinElement(army.begin(), army.end(), WeakestTroop);
}

/* draw MONS32 sprite in line, first valid = 0, count = 0 */
void Army::army_t::DrawMons32Line(s16 cx, s16 cy, u8 width, u8 first, u8 count) const
{
    if(!isValid()) return;

    if(0 == count) count = GetCount();
    else
    if(ARMYMAXTROOPS < count) count = ARMYMAXTROOPS;

    const u8 chunk = width / count;
    cx += chunk / 2;

    std::string str;
    Text text;
    text.Set(Font::SMALL);

    for(u8 ii = 0; ii < ARMYMAXTROOPS; ++ii)
    {
	const Army::Troop & troop = army[ii];

    	if(troop.isValid())
	{
	    if(0 == first && count)
    	    {
		const Sprite & monster = AGG::GetICN(ICN::MONS32, troop.GetSpriteIndex());

    		Display::Get().Blit(monster, cx - monster.w() / 2, cy + 30 - monster.h());

    		str.clear();
    		String::AddInt(str, troop.Count());
		text.Set(str);
		text.Blit(cx - text.width() / 2, cy + 28);

		cx += chunk;
		--count;
	    }
	    else
		--first;
	}
    }
}

void Army::army_t::ArrangeForBattle(void)
{
    std::vector<Troop> priority;
    priority.reserve(ARMYMAXTROOPS);

    std::vector<Troop>::iterator it1 = army.begin();
    std::vector<Troop>::const_iterator it2 = army.end();
    
    for(; it1 != it2; ++it1)
    {
	Troop & troop = *it1;
	if(!troop.isValid()) continue;

	std::vector<Troop>::iterator it = std::find_if(priority.begin(), priority.end(), std::bind2nd(std::mem_fun_ref(&Troop::HasMonster), troop()));

	if(it == priority.end())
	    priority.push_back(troop);
	else
	    (*it).SetCount((*it).Count() + troop.Count());

	troop.Reset();
    }

    switch(priority.size())
    {
	case 1:
	{
	    const Monster & m = priority.back();
	    const u16 count = priority.back().Count();
	    if(49 < count)
	    {
		const u16 c = count / 5;
		army[0].Set(m, c);
		army[1].Set(m, c);
		army[2].Set(m, c + count - (c * 5));
		army[3].Set(m, c);
		army[4].Set(m, c);
	    }
	    else
	    if(20 < count)
	    {
		const u16 c = count / 3;
		army[1].Set(m, c);
		army[2].Set(m, c + count - (c * 3));
		army[3].Set(m, c);
	    }
	    else
		army[2].Set(m, count);
	    break;
	}
	case 2:
	{
	    // TODO: need modify army for 2 troops
	    Import(priority);
	    break;
	}
	case 3:
	{
	    // TODO: need modify army for 3 troops
	    Import(priority);
	    break;
	}
	case 4:
	{
	    // TODO: need modify army for 4 troops
	    Import(priority);
	    break;
	}
	case 5:
	{    // possible change orders monster
	    // store
	    Import(priority);
	    break;
	}
	default: break;
    }
}

u32 Army::army_t::CalculateExperience(void) const
{
    u32 res = 0;

    for(u8 ii = 0; ii < ARMYMAXTROOPS; ++ii)
	res += army[ii].GetHitPoints();

    if(commander) res += 500;

    return res;

}

void Army::army_t::BattleNewTurn(void)
{
    std::for_each(army.begin(), army.end(), std::mem_fun_ref(&Troop::BattleNewTurn));
}

void Army::army_t::BattleLoadContours(bool inv)
{
    std::for_each(army.begin(), army.end(), std::bind2nd(std::mem_fun_ref(&Troop::BattleLoadContours), inv));
}

void Army::army_t::SetModes(u32 f)
{
    std::for_each(army.begin(), army.end(), std::bind2nd(std::mem_fun_ref(&Troop::SetModes), f));
}

void Army::army_t::ResetModes(u32 f)
{
    std::for_each(army.begin(), army.end(), std::bind2nd(std::mem_fun_ref(&Troop::ResetModes), f));
}

void Army::army_t::Clear(void)
{
    std::for_each(army.begin(), army.end(), std::mem_fun_ref(&Troop::Reset));
}

void Army::army_t::Reset(bool soft)
{
    Clear();

    if(commander)
    {
    	const Monster mons1(commander->GetRace(), Castle::DWELLING_MONSTER1);

	if(soft)
	{
    	    const Monster mons2(commander->GetRace(), Castle::DWELLING_MONSTER2);

	    switch(Rand::Get(1, 3))
	    {
		case 1:
		    JoinTroop(mons1, 3 * mons1.GetGrown());
		    break;
		case 2:
		    JoinTroop(mons2, static_cast<u8>(1.5 * mons2.GetGrown()));
		    break;
		default:
		    JoinTroop(mons1, 2 * mons1.GetGrown());
		    JoinTroop(mons2, mons2.GetGrown());
		    break;
	    }
	}
	else
	{
	    JoinTroop(mons1, 1);
	}
    }
}

void Army::army_t::JoinStrongestFromArmy(army_t & army2)
{
    std::vector<Troop> priority;
    priority.reserve(ARMYMAXTROOPS * 2);

    std::vector<Troop>::iterator it1 = army.begin();
    std::vector<Troop>::const_iterator it2 = army.end();
    
    for(; it1 != it2; ++it1)
    {
	Troop & troop = *it1;
	if(!troop.isValid()) continue;

	std::vector<Troop>::iterator it = std::find_if(priority.begin(), priority.end(), std::bind2nd(std::mem_fun_ref(&Troop::HasMonster), troop()));

	if(it == priority.end())
	    priority.push_back(troop);
	else
	    (*it).SetCount((*it).Count() + troop.Count());

	troop.Reset();
    }

    it1 = army2.army.begin();
    it2 = army2.army.end();
    
    for(; it1 != it2; ++it1)
    {
	Troop & troop = *it1;
	if(!troop.isValid()) continue;

	std::vector<Troop>::iterator it = std::find_if(priority.begin(), priority.end(), std::bind2nd(std::mem_fun_ref(&Troop::HasMonster), troop()));

	if(it == priority.end())
	    priority.push_back(troop);
	else
	    (*it).SetCount((*it).Count() + troop.Count());

	troop.Reset();
    }

    // sort: strongest
    std::sort(priority.begin(), priority.end(), StrongestTroop);

    // weakest to army2
    while(ARMYMAXTROOPS < priority.size())
    {
	army2.JoinTroop(priority.back());
	priority.pop_back();
    }

    // save half weak of strongest to army2
    if(ARMYMAXTROOPS > army2.army.size())
    {
	Troop & last = priority.back();

	army2.JoinTroop(last, last.Count() - last.Count() / 2);
	JoinTroop(last, last.Count() / 2);
	priority.pop_back();
    }

    // strongest to army
    while(priority.size())
    {
	JoinTroop(priority.back());
	priority.pop_back();
    }
}

void Army::army_t::KeepOnlyWeakestTroops(army_t & army2)
{
    std::vector<Troop> priority;
    priority.reserve(ARMYMAXTROOPS * 2);

    std::vector<Troop>::iterator it1 = army.begin();
    std::vector<Troop>::const_iterator it2 = army.end();
    
    for(; it1 != it2; ++it1)
    {
	Troop & troop = *it1;
	if(!troop.isValid()) continue;

	std::vector<Troop>::iterator it = std::find_if(priority.begin(), priority.end(), std::bind2nd(std::mem_fun_ref(&Troop::HasMonster), troop()));

	if(it == priority.end())
	    priority.push_back(troop);
	else
	    (*it).SetCount((*it).Count() + troop.Count());

	troop.Reset();
    }

    it1 = army2.army.begin();
    it2 = army2.army.end();
    
    for(; it1 != it2; ++it1)
    {
	Troop & troop = *it1;
	if(!troop.isValid()) continue;

	std::vector<Troop>::iterator it = std::find_if(priority.begin(), priority.end(), std::bind2nd(std::mem_fun_ref(&Troop::HasMonster), troop()));

	if(it == priority.end())
	    priority.push_back(troop);
	else
	    (*it).SetCount((*it).Count() + troop.Count());

	troop.Reset();
    }

    // sort: weakest
    std::sort(priority.begin(), priority.end(), WeakestTroop);

    // strongest to army2
    while(1 < priority.size() && ARMYMAXTROOPS > army2.GetCount())
    {
	army2.JoinTroop(priority.back());
	priority.pop_back();
    }

    // weakest to army
    while(priority.size())
    {
	JoinTroop(priority.back());
	priority.pop_back();
    }
}

void Army::army_t::UpgradeTroops(const Castle & castle)
{
    std::vector<Troop>::iterator it1 = army.begin();
    std::vector<Troop>::const_iterator it2 = army.end();
    
    for(; it1 != it2; ++it1) if((*it1).isValid())
    {
	Troop & troop = *it1;
        PaymentConditions::UpgradeMonster payment(troop());
        payment *= troop.Count();

	if(castle.GetRace() == troop.GetRace() &&
	   castle.isBuild(Monster::GetDwelling(Monster::Upgrade(troop))) &&
	   payment <= world.GetKingdom(castle.GetColor()).GetFundsResource())
	{
    	    world.GetMyKingdom().OddFundsResource(payment);
            troop.Upgrade();
	}
    }
}

void Army::army_t::Dump(void) const
{
    std::cout << "Army::Dump: ";

    std::vector<Troop>::const_iterator it1 = army.begin();
    std::vector<Troop>::const_iterator it2 = army.end();
    
    for(; it1 != it2; ++it1) if((*it1).isValid()) std::cout << (*it1).GetName() << "(" << (*it1).Count() << "), ";

    std::cout << std::endl;
}

u16 Army::army_t::GetAttack(void) const
{
    std::vector<Troop>::const_iterator it1 = army.begin();
    std::vector<Troop>::const_iterator it2 = army.end();
    u16 res = 0;
    u8 count = 0;

    for(; it1 != it2; ++it1) if((*it1).isValid()){ res += (*it1).GetAttack(); ++count; }

    return res / count;
}

u16 Army::army_t::GetDefense(void) const
{
    std::vector<Troop>::const_iterator it1 = army.begin();
    std::vector<Troop>::const_iterator it2 = army.end();
    u16 res = 0;
    u8 count = 0;

    for(; it1 != it2; ++it1) if((*it1).isValid()){ res += (*it1).GetDefense(); ++count; }

    return res / count;
}

u32 Army::army_t::GetHitPoints(void) const
{
    std::vector<Troop>::const_iterator it1 = army.begin();
    std::vector<Troop>::const_iterator it2 = army.end();
    u32 res = 0;

    for(; it1 != it2; ++it1) if((*it1).isValid()) res += (*it1).GetHitPoints();

    return res;
}

u32 Army::army_t::GetDamageMin(void) const
{
    std::vector<Troop>::const_iterator it1 = army.begin();
    std::vector<Troop>::const_iterator it2 = army.end();
    u32 res = 0;

    for(; it1 != it2; ++it1) if((*it1).isValid()) res += (*it1).GetDamageMin();

    return res;
}

u32 Army::army_t::GetDamageMax(void) const
{
    std::vector<Troop>::const_iterator it1 = army.begin();
    std::vector<Troop>::const_iterator it2 = army.end();
    u32 res = 0;

    for(; it1 != it2; ++it1) if((*it1).isValid()) res += (*it1).GetDamageMax();

    return res;
}

bool Army::army_t::StrongerEnemyArmy(const army_t & a)
{
    u16 a1 = GetAttack();
    u16 d1 = GetDefense();
    u32 h1 = GetHitPoints();
    u32 m1 = GetDamageMin();
    u32 x1 = GetDamageMax();
    u32 r1 = 0;

    u16 a2 = a.GetAttack();
    u16 d2 = a.GetDefense();
    u32 h2 = a.GetHitPoints();
    u32 m2 = a.GetDamageMin();
    u32 x2 = a.GetDamageMax();
    u32 r2 = 0;

    // total damage: from FAQ
    if(a1 > d2)
	r1 = static_cast<u32>((m1 + x1) / 2 * (1 + 0.1 * std::min(a1 - d2, 20)));
    else
	r1 = static_cast<u32>((m1 + x1) / 2 * (1 + 0.05 * std::min(d2 - a1, 14)));

    if(a2 > d1)
	r2 = static_cast<u32>((m2 + x2) / 2 * (1 + 0.1 * std::min(a2 - d1, 20)));
    else
	r2 = static_cast<u32>((m2 + x2) / 2 * (1 + 0.05 * std::min(d1 - a2, 14)));

    return h1 / r2 > h2 / r1;
}

const Skill::Primary* Army::army_t::GetCommander(void) const
{
    return commander;
}
