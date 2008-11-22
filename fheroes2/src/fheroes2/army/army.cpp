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
#include "army.h"
#include "display.h"
#include "agg.h"
#include "sprite.h"
#include "spell.h"

const std::string & Army::String(Army::size_t size)
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

Army::size_t Army::GetSize(u16 count)
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

Army::Troop::Troop(Monster::monster_t m, u16 c)
: monster(m)
, count(c)
, master_skill(NULL)
{
}

Army::BattleTroop::BattleTroop(const Troop &t)
: Troop(t.Monster(), t.Count())
, attackRanged(false)
, summoned(false)
{
}

Army::BattleTroop::BattleTroop(Monster::monster_t m, u16 c)
: Troop(m, c)
, attackRanged(false)
, summoned(false)
{
}

Army::BattleTroop::BattleTroop(const Army::BattleTroop & troop)
: Troop(troop)
, astate(troop.astate)
, aframe(troop.aframe)
, attackRanged(troop.attackRanged)
, shots(troop.shots)
, hp(troop.hp)
, oldcount(troop.oldcount)
, summoned(troop.summoned)
, pos(troop.Position())
, saved(false)
, reflect(troop.reflect)
, origReflect(troop.origReflect)
{
}

Army::BattleTroop & Army::BattleTroop::operator= (const Army::BattleTroop & troop)
{
    astate = troop.astate;
    aframe = troop.aframe;
    attackRanged = troop.attackRanged;
    shots = troop.shots;
    hp = troop.hp;
    oldcount = troop.oldcount;
    summoned = troop.summoned;
    monster = troop.Monster();
    count = troop.Count();
    pos = troop.Position();
    reflect = troop.reflect;
    origReflect = troop.origReflect;

    return *this;
}

Army::BattleTroop & Army::BattleTroop::operator= (const Army::Troop & troop)
{
    Troop::operator=(troop);
    return *this;
}

bool Army::Troop::isValid(void) const
{
    return Monster::UNKNOWN > monster && count;
}

bool Army::isValid(const Troop & army)
{
    return army.isValid();
}

void Army::BattleTroop::BlitR(const Point& dst_pt, bool reflect, int frame)
{
    Display & display = Display::Get();
    if(saved) bg.Restore();
    const Sprite & sp = AGG::GetICN(Monster::GetStats(monster).file_icn, frame<0 ? aframe : frame, reflect);
    saved = true;
    Point p(dst_pt.x + (reflect ? -sp.w()-sp.x() : sp.x()), dst_pt.y + sp.y());
    bg.Save(p.x, p.y, sp.w(), sp.h());
    display.Blit(sp, p);
}

void Army::BattleTroop::Blit(const Point& dst_pt, bool reflect, int frame)
{
    Display & display = Display::Get();
    const Sprite & sp = AGG::GetICN(Monster::GetStats(monster).file_icn, frame<0 ? aframe : frame, reflect);
    saved = true;
    Point p(dst_pt.x + (reflect ? -sp.w()-sp.x() : sp.x()), dst_pt.y + sp.y());
    bg.Save(p.x, p.y, sp.w(), sp.h());
    display.Blit(sp, p);
}

void Army::BattleTroop::Animate(Monster::animstate_t as)
{
    bool ranged = attackRanged && Monster::GetStats(monster).miss_icn != ICN::UNKNOWN;
    u8 start, count;
    if(as != Monster::AS_NONE) {
	astate = as;
	Monster::GetAnimFrames(monster, as & Monster::AS_ATTPREP ? Monster::AS_ATTPREP : as, start, count, ranged);
	aframe = start;
    } else {
	Monster::GetAnimFrames(monster, astate & Monster::AS_ATTPREP ? Monster::AS_ATTPREP : astate, start, count, ranged);
	aframe++;
	if(aframe >= start+count) {
            if(astate == Monster::AS_DIE) {
		aframe --;
            } else if(astate & Monster::AS_ATTPREP) {
		astate = (Monster::animstate_t)(astate & ~Monster::AS_ATTPREP);
		Monster::GetAnimFrames(monster, astate, start, count, ranged);
		aframe = start;
	    } else {
		astate = Monster::AS_NONE;
		Monster::GetAnimFrames(monster, astate, start, count, ranged);
		aframe = start;
	    }
	}
    }
}

int Army::BattleTroop::ApplyDamage(int damage)
{
    int perished = 0;
    while(damage >= hp)
    {
        damage -= hp;
        hp = Monster::GetStats(monster).hp;
        count--;
        perished++;
        if(!count)
            break;
    }
    hp -= damage;
    return perished;
}

void Army::BattleTroop::SetMagic(Spell::magic_t &magic)
{
    //Dialog::Message("set magic", Spell::String(magic.spell), Font::BIG, Dialog::OK);
    magics.push_back(magic);
}

bool Army::BattleTroop::FindMagic(Spell::spell_t spell) const
{
    for(u16 i=0; i<magics.size(); i++) {
	if(spell == magics[i].spell) return true;
    }
    return false;
}

void Army::BattleTroop::RemoveMagic(Spell::spell_t spell)
{
    for(u16 i=0; i<magics.size(); i++) {
	if(spell == magics[i].spell) {
	    magics.erase(magics.begin()+i);
	    i--;
	}
    }
}

void Army::BattleTroop::ClearMagic()
{
    magics.clear();
}

void Army::BattleTroop::ProceedMagic()
{
    for(u16 i=0; i<magics.size(); i++) {
	magics[i].duration --;
	if(magics[i].duration <= 0) {
	    magics.erase(magics.begin()+i);
	    i--;
	}
    }
}

bool Army::PredicateStrongestTroop(const Troop & t1, const Troop & t2)
{
    return t1.isValid() && t2.isValid() && (Monster::GetStats(t1.Monster()).damageMin > Monster::GetStats(t2.Monster()).damageMin);
}

bool Army::PredicateWeakestTroop(const Troop & t1, const Troop & t2)
{
    return t1.isValid() && t2.isValid() && (Monster::GetStats(t1.Monster()).damageMin < Monster::GetStats(t2.Monster()).damageMin);
}

bool Army::PredicateSlowestTroop(const Troop & t1, const Troop & t2)
  {
    return t1.isValid() && t2.isValid() && (Monster::GetStats(t1.Monster()).speed < Monster::GetStats(t2.Monster()).speed);
}

bool Army::PredicateFastestTroop(const Troop & t1, const Troop & t2)
{
    return t1.isValid() && t2.isValid() && (Monster::GetStats(t1.Monster()).speed > Monster::GetStats(t2.Monster()).speed);
}

s8 Army::GetLuckWithModificators(const army_t & army, std::list<std::string> *list)
{
    return 0;
}

s8 Army::GetMoraleWithModificators(const army_t & army, std::list<std::string> *list)
{
    s8 result(Morale::NORMAL);

    // different race penalty
    army_t::const_iterator it1_army = army.begin();
    army_t::const_iterator it2_army = army.end();
    u8 count = 0;
    u8 count_kngt = 0;
    u8 count_barb = 0;
    u8 count_sorc = 0;
    u8 count_wrlk = 0;
    u8 count_wzrd = 0;
    u8 count_necr = 0;
    u8 count_bomg = 0;
    bool ghost_present = false;
    for(; it1_army != it2_army; ++it1_army) if(Monster::UNKNOWN != (*it1_army).Monster())
    {
        switch(Monster::GetRace((*it1_army).Monster()))
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
        if(Monster::GHOST == (*it1_army).Monster()) ghost_present = true;
    }

    Race::race_t r = Race::MULT;
    if(count_kngt){ ++count; r = Race::KNGT; }
    if(count_barb){ ++count; r = Race::BARB; }
    if(count_sorc){ ++count; r = Race::SORC; }
    if(count_wrlk){ ++count; r = Race::WRLK; }
    if(count_wzrd){ ++count; r = Race::WZRD; }
    if(count_necr){ ++count; r = Race::NECR; }
    if(count_bomg){ ++count; r = Race::BOMG; }
    const u8 uniq_count = GetCountUniqTroops(army);

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

Army::Troop & Army::GetSlowestTroop(army_t & army)
{
    return *min_element(army.begin(), army.end(), PredicateSlowestTroop);
}

Army::Troop & Army::GetFastestTroop(army_t & army)
{
    return *min_element(army.begin(), army.end(), PredicateFastestTroop);
}

Army::Troop & Army::GetStrongestTroop(army_t & army)
{
    return *min_element(army.begin(), army.end(), PredicateStrongestTroop);
}

Army::Troop & Army::GetWeakestTroop(army_t & army)
{
    return *min_element(army.begin(), army.end(), PredicateWeakestTroop);
}

const Army::Troop & Army::GetSlowestTroop(const army_t & army)
{
    return *min_element(army.begin(), army.end(), PredicateSlowestTroop);
}

const Army::Troop & Army::GetFastestTroop(const army_t & army)
{
    return *min_element(army.begin(), army.end(), PredicateFastestTroop);
}

const Army::Troop & Army::GetStrongestTroop(const army_t & army)
{
    return *min_element(army.begin(), army.end(), PredicateStrongestTroop);
}

const Army::Troop & Army::GetWeakestTroop(const army_t & army)
{
    return *min_element(army.begin(), army.end(), PredicateWeakestTroop);
}

u8 Army::GetCountTroops(const army_t & army)
{
    u8 result = 0;
    army_t::const_iterator it1 = army.begin();
    army_t::const_iterator it2 = army.end();
    for(; it1 != it2; ++it1) if((*it1).isValid()) ++result;

    return result;
}

u8 Army::GetCountUniqTroops(const army_t & army)
{
    std::vector<Monster::monster_t> troops;
    troops.reserve(army.size());

    army_t::const_iterator it1 = army.begin();
    army_t::const_iterator it2 = army.end();
    for(; it1 != it2; ++it1) if((*it1).isValid()) troops.push_back((*it1).Monster());
    troops.resize(std::unique(troops.begin(), troops.end()) - troops.begin());

    return troops.size();
}

Race::race_t Army::GetRace(const army_t & army)
{
    army_t::const_iterator it1 = army.begin();
    army_t::const_iterator it2 = army.end();
    std::vector<Race::race_t> races;
    races.reserve(army.size());

    for(; it1 != it2; ++it1) if((*it1).isValid()) races.push_back(Monster::GetRace((*it1).Monster()));
    races.resize(std::unique(races.begin(), races.end()) - races.begin());

    if(races.empty())
    {
        Error::Warning("Army::GetRaceArmy: empty");
        return Race::MULT;
    }

    return 1 < races.size() ? Race::MULT : races.at(0);
}

bool Army::HasMonster(const army_t & army, const Monster::monster_t mon)
{
    army_t::const_iterator it1 = army.begin();
    army_t::const_iterator it2 = army.end();
    for(; it1 != it2; it1++) if(mon == (*it1).Monster()) return true;

    return false;
}

bool Army::JoinTroop(army_t & army, const Monster::monster_t mon, const u16 count)
{
    return JoinTroop(army, Troop(mon, count));
}

bool Army::JoinTroop(army_t & army, const Troop & troop)
{
    if(!troop.isValid()) return false;

    army_t::iterator it1 = army.begin();
    army_t::const_iterator it2 = army.end();

    for(; it1 != it2; it1++) if(troop.Monster() == (*it1).Monster())
    {
        (*it1).SetCount((*it1).Count() + troop.Count());
        if(Settings::Get().Debug()) Error::Verbose("Army::JoinTroop: monster: " + Monster::String(troop.Monster()) + ", count: ", troop.Count());
        return true;
    }

    it1 = army.begin();
    if(ARMYMAXTROOPS > Army::GetCountTroops(army))
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

void Army::SetMasterSkill(army_t & army, const Skill::Primary & primary)
{
    army_t::iterator it1 = army.begin();
    army_t::const_iterator it2 = army.end();
    for(; it1 != it2; ++it1) (*it1).SetMasterSkill(&primary);
}
