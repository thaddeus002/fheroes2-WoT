/***************************************************************************
 *   Copyright (C) 2008 by Vasily Makarov <drmoriarty@rambler.ru>          *
 *   Copyright (C) 2009 by Josh Matthews  <josh@joshmatthews.net>          *
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
#include "battle.h"
#include "battle_troop.h"
#include "agg.h"
#include "speed.h"

Army::BattleTroop::BattleTroop(const Troop &t)
: Troop(t)
{
    Init();
}

Army::BattleTroop::BattleTroop(monster_t m, u16 c)
: Troop(m, c)
{
    Init();
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
, screenPos(troop.screenPos)
, saved(false)
, reflect(troop.reflect)
, origReflect(troop.origReflect)
, lastReflect(troop.lastReflect)
, moving(troop.moving)
, disruptingray(troop.disruptingray)
{
}

Army::BattleTroop::~BattleTroop()
{
    if(contours.size())
    {
        std::vector<Surface *>::const_iterator it1 = contours.begin();
        std::vector<Surface *>::const_iterator it2 = contours.end();

        for(; it1 != it2; ++it1) if(*it1) delete *it1;
    }
}

void Army::BattleTroop::Init()
{
    summoned = false;
    moving = NOT_MOVING;
    saved = false;
    reflect = false;
    origReflect = false;
    lastReflect = false;
    attackRanged = false;

    disruptingray = 0;
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
    id = troop.id;
    count = troop.count;
    pos = troop.pos;
    reflect = troop.reflect;
    origReflect = troop.origReflect;
    moving = troop.moving;
    disruptingray = troop.disruptingray;

    return *this;
}

Army::BattleTroop & Army::BattleTroop::operator= (const Army::Troop & troop)
{
    const BattleTroop &other(troop);
    *this = other;
    return *this;
}

void Army::BattleTroop::BlitR(const Point& dst_pt, bool reflect, int frame)
{
    Display & display = Display::Get();
    if(saved) bg.Restore();
    const Sprite & sp = AGG::GetICN(ICNFile(), frame<0 ? aframe : frame, reflect);
    saved = true;
    Point p(dst_pt.x + (reflect ? -sp.w()-sp.x() : sp.x()), dst_pt.y + sp.y());
    bg.Save(p.x, p.y, sp.w(), sp.h());
    display.Blit(sp, p);
}

Point Army::BattleTroop::GetBlitOffset(int frame, bool reflect)
{
    const Sprite & sp = AGG::GetICN(ICNFile(), frame<0 ? aframe : frame, reflect);
    return Point(reflect ? -sp.w() - sp.x() : sp.x(), sp.y());
}

void Army::BattleTroop::Blit(const Point& dst_pt, bool reflect, int frame)
{
    Display & display = Display::Get();
    saved = true;
    const Sprite & sp = AGG::GetICN(ICNFile(), frame<0 ? aframe : frame, reflect);
    Point p = dst_pt + GetBlitOffset(frame, reflect);
    bg.Save(p.x, p.y, sp.w(), sp.h());
    display.Blit(sp, p);
}

const Surface* Army::BattleTroop::GetContour(u8 index)
{
    if(contours.empty())
        LoadContours(IsReflected());
    return (contours.empty() || index >= contours.size() ? NULL : contours[index]);
}

void Army::BattleTroop::LoadContours(bool inv)
{
    if(!isValid()) return;

	if(contours.size())
    {
		std::vector<Surface *>::const_iterator it1 = contours.begin();
		std::vector<Surface *>::const_iterator it2 = contours.end();

		for(; it1 != it2; ++it1) if(*it1) delete *it1;
    }
    
    u8 idx = 0;
    u8 start = 0;
    u8 length = 0;
    Monster::animstate_t states[] = { AS_NONE, AS_IDLE };

    for(idx = 0; idx < sizeof(states) / sizeof(states[0]); idx++)
    {
        GetAnimFrames(states[idx], start, length);

        for(u8 ii = start; ii < start + length; ++ii)
        {
            const Sprite & sprite = AGG::GetICN(ICNFile(), ii, inv);
            
            Surface *contour = new Surface();
            sprite.MakeContour(*contour, sprite.GetColor(0xDA));
            
            contours.push_back(contour);
        }
    }
}

void Army::BattleTroop::Animate(u8 as)
{
    bool ranged = attackRanged && isArchers();
    u8 start, count;
    if(as != AS_NONE) {
        astate = as;
        GetAnimFrames(as & AS_ATTPREP ? AS_ATTPREP : as, start, count, ranged);
        aframe = start;
    } else {
        GetAnimFrames(astate & AS_ATTPREP ? AS_ATTPREP : astate, start, count, ranged);
        aframe++;
        if(aframe >= start+count) {
            if(astate == AS_DIE) {
                aframe --;
            } else if(astate & AS_ATTPREP) {
                astate = astate & ~AS_ATTPREP;
                GetAnimFrames(astate, start, count, ranged);
                aframe = start;
            } else {
                astate = AS_NONE;
                GetAnimFrames(astate, start, count, ranged);
                aframe = start;
            }
        }
    }
}

int Army::BattleTroop::ApplyDamage(long damage)
{
    //printf("damage: %d\nhp: %d\n\n", damage, TotalHP());
    int perished = 0;
    while(damage >= hp)
    {
        damage -= hp;
        hp = Monster::GetHitPoints();
        count--;
        perished++;
        if(!count)
        {
            hp = 0;
            damage = 0;
            break;
        }
    }
    hp -= damage;
    return perished;
}

void Army::BattleTroop::SetMagic(Battle::magic_t &magic)
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

bool Army::BattleTroop::ApplySpell(Spell::spell_t spell, u8 sp)
{
    //TODO: Artifacts that affect spell damage (holy pendant, etc)

    if(!Battle::isTroopAffectedBySpell(spell, *this))
        return false;

    u16 dmg = Battle::GetInflictDamageVersus(spell, sp, id);
    ApplyDamage(dmg);

    if(Spell::CURE == spell || Spell::MASSCURE == spell)
    {
        if(army && army->GetCommander())
            hp = std::min(Monster::GetHitPoints(), (u16)(hp + 5 * sp));
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

void Army::BattleTroop::NewTurn(void)
{
    switch(GetID())
    {
        case Monster::TROLL:
        case Monster::WAR_TROLL:
            hp = Monster::GetHitPoints() * count;
            break;
            
        default: break;
    }
    
    ResetModes(Army::ATTACKED);
    ResetModes(Army::RETALIATED);
    ResetModes(Army::HANDFIGHTING);
    ResetModes(Army::LUCK_GOOD);
    ResetModes(Army::LUCK_BAD);
    ResetModes(Army::MORALE_GOOD);
    ResetModes(Army::MORALE_BAD);
}

void Army::BattleTroop::SetModes(u32 f)
{
    if(!isValid()) return;

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

    Troop::SetModes(f);
}

u8 Army::BattleTroop::GetAttack(void) const
{
    u8 mod = Troop::GetAttack();

    if(Modes(SP_BLODLUST)) mod += 3;

    return mod;
}

u8 Army::BattleTroop::GetDefense(void) const
{
    u8 mod = Troop::GetDefense();

    if(Modes(SP_STONESKIN)) mod += 3;
    if(Modes(SP_STEELSKIN)) mod += 5;

    if(disruptingray) mod = (mod <= disruptingray * 3 ? 1 : mod - disruptingray * 3);

    return mod;
}

u32 Army::BattleTroop::GetHitPoints(void) const
{
    return hp;
}

bool Army::BattleTroop::IsDamageFatal(u16 damage) const
{
    //printf("damage: %u\n", damage);
    //printf("hp: %u\n\n", TotalHP());
    return damage >= TotalHP();
}

u16 Army::BattleTroop::GetDamageVersus(const Army::BattleTroop &defender) const
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
            if(army && army->GetCommander())
            {
                switch(army->GetCommander()->GetLevelSkill(Skill::Secondary::ARCHERY))
                {
                    case Skill::Level::BASIC:	dmg += (dmg * 10) / 100; break;
                    case Skill::Level::ADVANCED:dmg += (dmg * 25) / 100; break;
                    case Skill::Level::EXPERT:	dmg += (dmg * 50) / 100; break;
                    default: break;
                }
            }
        }
    }

    if(id == Monster::CRUSADER && defender.GetRace() == Race::NECR)
        dmg *= 2;
    
    if(Modes(LUCK_GOOD)) dmg *= 2;
    else if(Modes(LUCK_BAD)) dmg /= 2;
    
    return dmg;
}

bool Army::BattleTroop::CanRetaliateAgainst(const BattleTroop &attacker) const
{   
    switch(attacker())
    {
        case Monster::VAMPIRE:
        case Monster::LORD_VAMPIRE:
        case Monster::SPRITE:
        case Monster::ROGUE:
            return false;
        default:
            break;
    }

    if(GetID() == Monster::GRIFFIN)
        return true;
    
    return !Modes(RETALIATED);
}

u8 Army::BattleTroop::GetSpeed(void) const
{
    if(Modes(SP_BLIND | SP_PARALYZE | SP_HYPNOTIZE | SP_STONE)) return Speed::STANDING;

    //if(!Modes(MORALE_GOOD) && Modes(MOVED)) return Speed::STANDING;

    if(Modes(SP_HASTE)) return (Speed::ULTRAFAST < Monster::GetSpeed() ? Speed::INSTANT : Monster::GetSpeed() + 2);
    else
    if(Modes(SP_SLOW)) return (Speed::SLOW > Monster::GetSpeed() ? Speed::CRAWLING : Monster::GetSpeed() - 2);

    return Troop::GetSpeed();
}

//////////////////////////////////////////////////////////////////////////////////////

void Army::GetTroopsOfSpeed(const BattleArmy_t &army, u8 speed, IndexList &list)
{
    for(u16 i = 0; i < army.size(); i++)
    {
        if(!army[i].isValid())
            continue;

        if(army[i].GetSpeed() == speed)
            list.push_back(i);
    }
}

bool Army::isArmyValid(const BattleArmy_t &army)
{
    for(u16 i = 0; i < army.size(); i++)
        if(army[i].isValid())
            return true;
    return false;
}

void Army::NewTurn(BattleArmy_t &army)
{
    //printf("resetting\n");
    std::for_each(army.begin(), army.end(), std::mem_fun_ref(&Army::BattleTroop::NewTurn));
}

void Army::LoadContours(BattleArmy_t &army, bool inv)
{
    std::for_each(army.begin(), army.end(), std::bind2nd(std::mem_fun_ref(&BattleTroop::LoadContours), inv));
}

