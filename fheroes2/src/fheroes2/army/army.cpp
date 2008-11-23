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

#include "army.h"
#include "agg.h"

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

/*Army::Troop &Army::Troop::operator=(const Troop &t)
{
  monster = t.monster;
  count = t.count;
  return *this;
}*/

Army::BattleTroop::BattleTroop(const Troop &t)
: Troop(t)
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
