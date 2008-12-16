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
#include "castle.h"
#include "army.h"

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

Army::Troop::Troop(Monster::monster_t m, u16 c) : monster(m), count(c), army(NULL)
{
}

Army::Troop::Troop(const Troop & t) : army(NULL)
{
    monster = t.monster;
    count = t.count;
}

Army::Troop & Army::Troop::operator= (const Troop & t)
{
    monster = t.monster;
    count = t.count;

    return *this;
}

bool Army::Troop::HasMonster(Monster::monster_t m) const
{
    return monster == m;
}

void Army::Troop::Set(Monster::monster_t m, u16 c)
{
    monster = m;
    count = c;
}

void Army::Troop::SetMonster(Monster::monster_t m)
{
    monster = m;
}

void Army::Troop::UpgradeMonster(void)
{
    monster = Monster::Upgrade(monster);
}

void Army::Troop::SetCount(u16 c)
{
    count = c;
}

void Army::Troop::Reset(void)
{
    monster = Monster::UNKNOWN;
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

Monster::monster_t Army::Troop::Monster(void) const
{
    return monster;
}

u16 Army::Troop::Count(void) const
{
    return count;
}

u8 Army::Troop::Attack(void) const
{
    return Monster::GetStats(monster).attack;
}

u8 Army::Troop::Defence(void) const
{
    return Monster::GetStats(monster).defence;
}

u32 Army::Troop::HitPoint(void) const
{
    return Monster::GetStats(monster).hp * count;
}

u16 Army::Troop::DamageMin(void) const
{
    return Monster::GetStats(monster).damageMin * count;
}

u16 Army::Troop::DamageMax(void) const
{
    return Monster::GetStats(monster).damageMax * count;
}

bool Army::Troop::isValid(void) const
{
    return Monster::UNKNOWN > monster && count;
}

bool Army::isValidTroop(const Troop & troop)
{
    return troop.isValid();
}

bool Army::StrongestTroop(const Troop & t1, const Troop & t2)
{
    return t1.isValid() && t2.isValid() && (Monster::GetStats(t1.Monster()).damageMin > Monster::GetStats(t2.Monster()).damageMin);
}

bool Army::WeakestTroop(const Troop & t1, const Troop & t2)
{
    return t1.isValid() && t2.isValid() && (Monster::GetStats(t1.Monster()).damageMin < Monster::GetStats(t2.Monster()).damageMin);
}

bool Army::SlowestTroop(const Troop & t1, const Troop & t2)
  {
    return t1.isValid() && t2.isValid() && (Monster::GetStats(t1.Monster()).speed < Monster::GetStats(t2.Monster()).speed);
}

bool Army::FastestTroop(const Troop & t1, const Troop & t2)
{
    return t1.isValid() && t2.isValid() && (Monster::GetStats(t1.Monster()).speed > Monster::GetStats(t2.Monster()).speed);
}

void Army::SwapTroops(Troop & t1, Troop & t2)
{
    std::swap(t1, t2);
}






Army::army_t::army_t(const Skill::Primary* s) : army(ARMYMAXTROOPS), commander(s), flags(0)
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

    switch(t.GetObject())
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

	default: break;
    }

    ArrangeForBattle();
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

void Army::army_t::UpgradeMonsters(const Monster::monster_t m)
{
    for(u8 ii = 0; ii < ARMYMAXTROOPS; ++ii) if(army[ii].isValid() && m == army[ii].Monster()) army[ii].UpgradeMonster();
}

void Army::army_t::SetModes(flags_t f)
{
    flags |= f;
}

void Army::army_t::ResetModes(flags_t f)
{
    flags &= ~f;
}

bool Army::army_t::Modes(flags_t f) const
{
    return flags & f;
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
    for(; it1 != it2; ++it1) if((*it1).isValid()) troops.push_back((*it1).Monster());
    troops.resize(std::unique(troops.begin(), troops.end()) - troops.begin());

    return troops.size();
}

u16 Army::army_t::GetCountMonsters(const Monster::monster_t m) const
{
    u16 c = 0;

    std::vector<Troop>::const_iterator it1 = army.begin();
    std::vector<Troop>::const_iterator it2 = army.end();
    for(; it1 != it2; ++it1) if((*it1).isValid() && m == (*it1).Monster()) c += (*it1).Count();

    return c;
}

bool Army::army_t::JoinTroop(const Monster::monster_t mon, const u16 count)
{
    return JoinTroop(Troop(mon, count));
}

bool Army::army_t::JoinTroop(const Troop & troop)
{
    if(!troop.isValid()) return false;

    std::vector<Troop>::iterator it;
    it = std::find_if(army.begin(), army.end(), std::bind2nd(std::mem_fun_ref(&Troop::HasMonster), troop.Monster()));

    if(it != army.end())
    {
	(*it).SetCount((*it).Count() + troop.Count());
	if(2 < Settings::Get().Debug()) Error::Verbose("Army::JoinTroop: monster: " + Monster::String(troop.Monster()) + ", count: ", troop.Count());
	return true;
    }

    it = std::find_if(army.begin(), army.end(), std::not1(std::mem_fun_ref(&Troop::isValid)));
    if(it != army.end())
    {
	(*it).Set(troop.Monster(), troop.Count());
	if(2 < Settings::Get().Debug()) Error::Verbose("Army::JoinTroop: monster: " + Monster::String(troop.Monster()) + ", count: ", troop.Count());
	return true;
    }

    return true;
}

bool Army::army_t::isValid(void) const
{
    return army.end() != std::find_if(army.begin(),army.end(), Army::isValidTroop);
}

bool Army::army_t::HasMonster(const Monster::monster_t mons) const
{
    return army.end() != std::find_if(army.begin(), army.end(), std::bind2nd(std::mem_fun_ref(&Troop::HasMonster), mons));
}

Race::race_t Army::army_t::GetRace(void) const
{
    std::vector<Troop>::const_iterator it1 = army.begin();
    std::vector<Troop>::const_iterator it2 = army.end();
    std::vector<Race::race_t> races;
    races.reserve(ARMYMAXTROOPS);

    for(; it1 != it2; ++it1) if((*it1).isValid()) races.push_back(Monster::GetRace((*it1).Monster()));
    races.resize(std::unique(races.begin(), races.end()) - races.begin());

    if(races.empty())
    {
        Error::Warning("Army::GetRaceArmy: empty");
        return Race::MULT;
    }

    return 1 < races.size() ? Race::MULT : races.at(0);
}

s8 Army::army_t::GetLuckWithModificators(std::list<std::string> *list) const
{
    return 0;
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
    for(; it1 != it2; ++it1) if(Monster::UNKNOWN != (*it1).Monster())
    {
        switch(Monster::GetRace((*it1).Monster()))
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
        if(Monster::GHOST == (*it1).Monster()) ghost_present = true;
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
    return *min_element(army.begin(), army.end(), SlowestTroop);
}

Army::Troop & Army::army_t::GetFastestTroop(void)
{
    return *min_element(army.begin(), army.end(), FastestTroop);
}

Army::Troop & Army::army_t::GetStrongestTroop(void)
{
    return *min_element(army.begin(), army.end(), StrongestTroop);
}

Army::Troop & Army::army_t::GetWeakestTroop(void)
{
    return *min_element(army.begin(), army.end(), WeakestTroop);
}

const Army::Troop & Army::army_t::GetSlowestTroop(void) const
{
    return *min_element(army.begin(), army.end(), SlowestTroop);
}

const Army::Troop & Army::army_t::GetFastestTroop(void) const
{
    return *min_element(army.begin(), army.end(), FastestTroop);
}

const Army::Troop & Army::army_t::GetStrongestTroop(void) const
{
    return *min_element(army.begin(), army.end(), StrongestTroop);
}

const Army::Troop & Army::army_t::GetWeakestTroop(void) const
{
    return *min_element(army.begin(), army.end(), WeakestTroop);
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
    Text text("", Font::SMALL);

    for(u8 ii = 0; ii < ARMYMAXTROOPS; ++ii)
    {
	const Army::Troop & troop = army[ii];

    	if(troop.isValid())
	{
	    if(0 == first && count)
    	    {
		const Sprite & monster = AGG::GetICN(ICN::MONS32, troop.Monster());

    		Display::Get().Blit(monster, cx - monster.w() / 2, cy + 30 - monster.h());

    		str.clear();
    		String::AddInt(str, troop.Count());
		text.SetText(str);
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

	std::vector<Troop>::iterator it = std::find_if(priority.begin(), priority.end(), std::bind2nd(std::mem_fun_ref(&Troop::HasMonster), troop.Monster()));

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
	    const Monster::monster_t m = priority.back().Monster();
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
	res += army[ii].Count() * Monster::GetStats(army[ii].Monster()).hp;

    if(commander) res += 500;

    return res;

}

void Army::army_t::Reset(bool soft)
{
    std::for_each(army.begin(), army.end(), std::mem_fun_ref(&Troop::Reset));

    if(commander)
    {
    	const Monster::monster_t mons1 = Monster::Monster(commander->GetRace(), Castle::DWELLING_MONSTER1);

	if(soft)
	{
    	    const Monster::monster_t mons2 = Monster::Monster(commander->GetRace(), Castle::DWELLING_MONSTER2);

	    switch(Rand::Get(1, 3))
	    {
		case 1:
		    JoinTroop(mons1, 3 * Monster::GetGrown(mons1));
		    break;
		case 2:
		    JoinTroop(mons2, static_cast<u8>(1.5 * Monster::GetGrown(mons2)));
		    break;
		default:
		    JoinTroop(mons1, 2 * Monster::GetGrown(mons1));
		    JoinTroop(mons2, Monster::GetGrown(mons2));
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

	std::vector<Troop>::iterator it = std::find_if(priority.begin(), priority.end(), std::bind2nd(std::mem_fun_ref(&Troop::HasMonster), troop.Monster()));

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

	std::vector<Troop>::iterator it = std::find_if(priority.begin(), priority.end(), std::bind2nd(std::mem_fun_ref(&Troop::HasMonster), troop.Monster()));

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

	army2.JoinTroop(last.Monster(), last.Count() - last.Count() / 2);
	JoinTroop(last.Monster(), last.Count() / 2);
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

	std::vector<Troop>::iterator it = std::find_if(priority.begin(), priority.end(), std::bind2nd(std::mem_fun_ref(&Troop::HasMonster), troop.Monster()));

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

	std::vector<Troop>::iterator it = std::find_if(priority.begin(), priority.end(), std::bind2nd(std::mem_fun_ref(&Troop::HasMonster), troop.Monster()));

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
        PaymentConditions::UpgradeMonster payment(troop.Monster());
        payment *= troop.Count();

	if(castle.GetRace() == Monster::GetRace(troop.Monster()) &&
	   castle.isBuild(Monster::Dwelling(Monster::Upgrade(troop.Monster()))) &&
	   payment <= world.GetKingdom(castle.GetColor()).GetFundsResource())
	{
    	    world.GetMyKingdom().OddFundsResource(payment);
            troop.UpgradeMonster();
	}
    }
}

void Army::army_t::Dump(void) const
{
    std::cout << "Army::Dump: ";

    std::vector<Troop>::const_iterator it1 = army.begin();
    std::vector<Troop>::const_iterator it2 = army.end();
    
    for(; it1 != it2; ++it1) if((*it1).isValid()) std::cout << Monster::String((*it1).Monster()) << "(" << (*it1).Count() << "), ";

    std::cout << std::endl;
}

u16 Army::army_t::Attack(void) const
{
    std::vector<Troop>::const_iterator it1 = army.begin();
    std::vector<Troop>::const_iterator it2 = army.end();
    u16 res = 0;
    u8 count = 0;

    for(; it1 != it2; ++it1) if((*it1).isValid()){ res += (*it1).Attack(); ++count; }

    return res / count;
}

u16 Army::army_t::Defence(void) const
{
    std::vector<Troop>::const_iterator it1 = army.begin();
    std::vector<Troop>::const_iterator it2 = army.end();
    u16 res = 0;
    u8 count = 0;

    for(; it1 != it2; ++it1) if((*it1).isValid()){ res += (*it1).Defence(); ++count; }

    return res / count;
}

u32 Army::army_t::HitPoint(void) const
{
    std::vector<Troop>::const_iterator it1 = army.begin();
    std::vector<Troop>::const_iterator it2 = army.end();
    u32 res = 0;

    for(; it1 != it2; ++it1) if((*it1).isValid()) res += (*it1).HitPoint();

    return res;
}

u32 Army::army_t::DamageMin(void) const
{
    std::vector<Troop>::const_iterator it1 = army.begin();
    std::vector<Troop>::const_iterator it2 = army.end();
    u32 res = 0;

    for(; it1 != it2; ++it1) if((*it1).isValid()) res += (*it1).DamageMin();

    return res;
}

u32 Army::army_t::DamageMax(void) const
{
    std::vector<Troop>::const_iterator it1 = army.begin();
    std::vector<Troop>::const_iterator it2 = army.end();
    u32 res = 0;

    for(; it1 != it2; ++it1) if((*it1).isValid()) res += (*it1).DamageMax();

    return res;
}

// this algorithm
bool Army::army_t::StrongerEnemyArmy(const army_t & a)
{
    u16 a1 = Attack();
    u16 d1 = Defence();
    u32 h1 = HitPoint();
    u32 m1 = DamageMin();
    u32 x1 = DamageMax();
    u32 r1 = 0;

    u16 a2 = a.Attack();
    u16 d2 = a.Defence();
    u32 h2 = a.HitPoint();
    u32 m2 = a.DamageMin();
    u32 x2 = a.DamageMax();
    u32 r2 = 0;

    // total damage1
    if(a1 > d2)
	r1 = static_cast<u32>((m1 + x1) / 2 * (1 + 0.1 * std::min(a1 - d2, 20)));
    else
	r1 = static_cast<u32>((m1 + x1) / 2 * (1 + 0.05 * std::min(d2 - a1, 14)));

    // total damage1
    if(a2 > d1)
	r2 = static_cast<u32>((m2 + x2) / 2 * (1 + 0.1 * std::min(a2 - d1, 20)));
    else
	r2 = static_cast<u32>((m2 + x2) / 2 * (1 + 0.05 * std::min(d1 - a2, 14)));

    return r1 > h2 && r2 < h1;
}
