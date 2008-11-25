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
#include "agg.h"
#include "settings.h"
#include "battle_troop.h"
#include "text.h"
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

    std::vector<Troop>::iterator it1 = army.begin();
    std::vector<Troop>::const_iterator it2 = army.end();

    for(; it1 != it2; it1++) if(troop.Monster() == (*it1).Monster())
    {
        (*it1).SetCount((*it1).Count() + troop.Count());
        if(Settings::Get().Debug()) Error::Verbose("Army::JoinTroop: monster: " + Monster::String(troop.Monster()) + ", count: ", troop.Count());
        return true;
    }

    it1 = army.begin();
    if(ARMYMAXTROOPS > GetCount())
    {
        for(; it1 != it2; it1++) if(Monster::UNKNOWN == (*it1).Monster())
        {
            *it1 = troop;
            if(Settings::Get().Debug()) Error::Verbose("Heroes::JoinTroops: monster: " + Monster::String(troop.Monster()) + ", count: ", troop.Count());
            return true;
        }
    }

    return false;
}

bool Army::army_t::isValid(void) const
{
    return army.end() != std::find_if(army.begin(),army.end(), Army::isValidTroop);
}

bool Army::army_t::HasMonster(const Monster::monster_t mon) const
{
    std::vector<Troop>::const_iterator it1 = army.begin();
    std::vector<Troop>::const_iterator it2 = army.end();
    for(; it1 != it2; it1++) if(mon == (*it1).Monster()) return true;

    return false;
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

void Army::ArrangeTroopsForBattle(army_t & army, Monster::monster_t monster, u16 count)
{
    if(count > ARMYMAXTROOPS)
    {
	const s16 c = count / ARMYMAXTROOPS;
	army.At(0).Set(monster, c);
	army.At(1).Set(monster, c);
	army.At(2).Set(monster, c + count - (c * ARMYMAXTROOPS));
	army.At(3).Set(monster, c);
	army.At(4).Set(monster, c);
    }
    else
    {
	army.At(0).Set(monster, count);
    }
}
