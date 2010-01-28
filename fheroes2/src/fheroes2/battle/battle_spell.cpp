/***************************************************************************
 *   Copyright (C) 2008 by Vasily Makarov <drmoriarty@rambler.ru>          *
 *   Copyright (C) 2008 by Josh Matthews  <josh@joshmatthews.net>          *
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

#ifdef WITH_BATTLE1

#include "battle.h"
#include "battle_troop.h"
#include "battle_spell.h"
#include "battle_types.h"
#include "agg.h"
#include <cmath>

bool Battle::AllowSpell(Spell::spell_t spell, const Army::BattleTroop &troop)
{
    u8 target = SpellTarget(spell);
    if(troop == Monster::DWARF || troop == Monster::BATTLE_DWARF) {
        if(!Rand::Get(0,3) && target != ONEFRIEND && target != ALLFRIEND) return false;
    }
    if(troop.FindMagic(Spell::ANTIMAGIC) && target != ONEFRIEND && target != ALLFRIEND)
        return false;
    if(spell == Spell::ANTIMAGIC && troop.summoned) return false;
    switch(troop()) {
        case Monster::GREEN_DRAGON:
        case Monster::RED_DRAGON:
        case Monster::BLACK_DRAGON:
            return false;
        default:
            switch(spell) {
                case Spell::BLESS:
                case Spell::MASSBLESS:
                case Spell::CURSE:
                case Spell::MASSCURSE:
                case Spell::BERSERKER:
                case Spell::BLIND:
                case Spell::PARALYZE:
                case Spell::HYPNOTIZE:
                case Spell::DEATHRIPPLE:
                case Spell::DEATHWAVE:
                    return troop.GetRace() != Race::NECR;
                case Spell::HOLYWORD:
                case Spell::HOLYSHOUT:
                    return troop.GetRace() == Race::NECR;
                case Spell::ANIMATEDEAD:
                    return troop.GetRace() == Race::NECR &&
                            ( Monster(troop()).GetHitPoints() > troop.hp ||
                              troop.oldcount > troop.Count());
                case Spell::CURE:
                case Spell::MASSCURE:
                    return Monster(troop()).GetHitPoints() > troop.hp;
                case Spell::RESURRECT:
                case Spell::RESURRECTTRUE:
                    return Monster(troop()).GetHitPoints() > troop.hp ||
                            troop.oldcount > troop.Count();

                case Spell::DISPEL:
                case Spell::MASSDISPEL:
                    return troop.Magics().size();
                    /*	Spell::FIREBALL,
                        Spell::FIREBLAST,
                        Spell::LIGHTNINGBOLT,
                        Spell::CHAINLIGHTNING,
                        Spell::TELEPORT,
                        Spell::HASTE,
                        Spell::MASSHASTE,
                        Spell::SLOW,
                        Spell::MASSSLOW,
                        Spell::STONESKIN,
                        Spell::STEELSKIN,
                        Spell::ANTIMAGIC,
                        Spell::ARROW,
                        Spell::ARMAGEDDON,
                        Spell::ELEMENTALSTORM,
                        Spell::METEORSHOWER,
                        Spell::COLDRAY,
                        Spell::COLDRING,
                        Spell::DISRUPTINGRAY,
                        Spell::DRAGONSLAYER,
                        Spell::BLOODLUST,
                        Spell::MIRRORIMAGE,
                        Spell::SHIELD,
                        Spell::MASSSHIELD,
                        Spell::SUMMONEELEMENT,
                        Spell::SUMMONAELEMENT,
                        Spell::SUMMONFELEMENT,
                        Spell::SUMMONWELEMENT,
                        Spell::EARTHQUAKE*/
                default: 
                    return true;
            }
            return true;
    }
}

void Battle::ApplySpell(int spower, Spell::spell_t spell, Army::BattleTroop &troop)
{
    //Dialog::Message("apply spell", Spell::String(spell), Font::BIG, Dialog::OK);
    magic_t magic;
    magic.spell = spell;
    magic.duration = spower;
    if(!Spell::Damage(spell)) {
        switch(spell) {
            case Spell::MASSDISPEL:
            case Spell::DISPEL:
                troop.ClearMagic();
                return;
                // 	Spell::TELEPORT,
                // 	Spell::MIRRORIMAGE,
                // 	Spell::BERSERKER,
                // 	Spell::HYPNOTIZE,
                // 	Spell::DISRUPTINGRAY,
                // 	Spell::EARTHQUAKE,
            default:
                break;
                // TODO
        }
    } else if(Spell::Damage(spell) == 1) {
        switch(spell) {
            case Spell::MASSBLESS:
                magic.spell = Spell::BLESS;
            case Spell::BLESS:
                troop.RemoveMagic(Spell::CURSE);
                troop.RemoveMagic(Spell::MASSCURSE);
                break;
            case Spell::MASSCURSE:
                magic.spell = Spell::CURSE;
            case Spell::CURSE:
                troop.RemoveMagic(Spell::BLESS);
                troop.RemoveMagic(Spell::MASSBLESS);
                break;
            case Spell::STONESKIN:
                troop.RemoveMagic(Spell::STEELSKIN);
                break;
            case Spell::STEELSKIN:
                troop.RemoveMagic(Spell::STONESKIN);
                break;
            case Spell::MASSSLOW:
                magic.spell = Spell::SLOW;
            case Spell::SLOW:
                troop.RemoveMagic(Spell::HASTE);
                troop.RemoveMagic(Spell::MASSHASTE);
                break;
            case Spell::MASSSHIELD:
                magic.spell = Spell::SHIELD;
                break;
            case Spell::MASSHASTE:
                magic.spell = Spell::HASTE;
            case Spell::HASTE:
                troop.RemoveMagic(Spell::SLOW);
                troop.RemoveMagic(Spell::MASSSLOW);
                break;
                // 	Spell::BLIND,
                // 	Spell::SHIELD,
                // 	Spell::ANTIMAGIC,
                // 	Spell::PARALYZE,
                // 	Spell::STONE,
                // 	Spell::DRAGONSLAYER,
                // 	Spell::BLOODLUST,
            default:
                break;
        }
        troop.SetMagic(magic);
    } else {
        int damage = spower * Spell::Damage(spell);
        switch(spell) {
            case Spell::MASSCURE:
                spell = Spell::CURE;
            case Spell::CURE:
            case Spell::RESURRECT:
            case Spell::RESURRECTTRUE:
            case Spell::ANIMATEDEAD: {
                int hp = Monster(troop()).GetHitPoints();
                troop.hp += damage;
                if(troop.hp > hp) {
                    if(spell == Spell::CURE) troop.hp = hp;
                    else {
                        troop.SetCount(troop.Count() + troop.hp/hp);
                        troop.hp = troop.hp%hp;
                        if(!troop.hp) {
                            troop.SetCount(troop.Count()-1);
                            troop.hp = hp;
                        }
                        if(troop.Count() > troop.oldcount) {
                            troop.SetCount(troop.oldcount);
                            troop.hp = hp;
                        }
                    }
                }
                return;
            }
            case Spell::SUMMONEELEMENT:
            case Spell::SUMMONAELEMENT:
            case Spell::SUMMONFELEMENT:
            case Spell::SUMMONWELEMENT:
                // TODO
                return;
                // 	Spell::ARROW,
                // 	Spell::FIREBALL,
                // 	Spell::FIREBLAST,
                // 	Spell::LIGHTNINGBOLT,
                // 	Spell::CHAINLIGHTNING,
                // 	Spell::HOLYWORD,
                // 	Spell::HOLYSHOUT,
                // 	Spell::COLDRAY,
                // 	Spell::COLDRING,
                // 	Spell::DEATHRIPPLE,
                // 	Spell::DEATHWAVE,
                // 	Spell::ARMAGEDDON,
                // 	Spell::ELEMENTALSTORM,
                // 	Spell::METEORSHOWER,
            default: {
                int hp = Monster(troop()).GetHitPoints();
                while(troop.hp < damage) {
                    troop.SetCount(troop.Count() - 1);
                    if(troop.Count() <= 0) {
                        troop.SetCount(0);
                        troop.hp = 0;
                        break;
                    }
                    damage -= troop.hp;
                    troop.hp = hp;
                }
                return;
            }
        }
    }
}

bool Battle::isTroopAffectedBySpell(Spell::spell_t spell, const Army::BattleTroop &troop, bool deterministic)
{
    const Monster::monster_t id = troop.GetID();
    switch(id)
    {
        case Monster::GREEN_DRAGON:
        case Monster::RED_DRAGON:
        case Monster::BLACK_DRAGON: return false;
        default: break;
    }

    if(troop.Modes(Army::SP_ANTIMAGIC)) return false;

    if(troop.isUndead())
        switch(spell)
        {
            case Spell::CURSE:
            case Spell::MASSCURSE:
            case Spell::BLESS:
            case Spell::MASSBLESS:
            case Spell::BLIND:
            case Spell::PARALYZE:
            case Spell::HYPNOTIZE:
            case Spell::BERSERKER: return false;
            default: break;
        }

    if(troop.isElemental())
        switch(spell)
        {
            case Spell::BLIND:
            case Spell::PARALYZE:
            case Spell::HYPNOTIZE:
            case Spell::BERSERKER: return false;
            default: break;
        }

    if(isBadSpell(spell))
        switch(id)
        {
            case Monster::EARTH_ELEMENT:
                if(Spell::LIGHTNINGBOLT == spell ||
                   Spell::CHAINLIGHTNING == spell ||
                   Spell::METEORSHOWER == spell) return false;
                break;
                
            case Monster::AIR_ELEMENT:
                if(Spell::METEORSHOWER == spell) return false;
                break;
                
            case Monster::FIRE_ELEMENT:
                if(Spell::FIREBALL == spell ||
                   Spell::FIREBLAST) return false;
                break;

    	    case Monster::WATER_ELEMENT:
                if(Spell::COLDRAY == spell ||
                   Spell::COLDRING == spell) return false;
                break;

            case Monster::DWARF:
            case Monster::BATTLE_DWARF:
                if(!deterministic && 3 == Rand::Get(1,4)) return false;
                break;

            case Monster::GIANT:
            case Monster::TITAN:
                if(Spell::BLIND == spell ||
                   Spell::PARALYZE == spell ||
                   Spell::BERSERKER == spell ||
                   Spell::HYPNOTIZE == spell) return false;
                break;

            case Monster::PHOENIX:
                if(Spell::COLDRAY == spell ||
                   Spell::LIGHTNINGBOLT == spell ||
                   Spell::COLDRING == spell ||
                   Spell::FIREBALL == spell ||
                   Spell::CHAINLIGHTNING == spell ||
                   Spell::ELEMENTALSTORM == spell ||
                   Spell::FIREBLAST) return false;
                break;

            case Monster::CRUSADER:
                if(Spell::CURSE == spell ||
                   Spell::MASSCURSE == spell) return false;
                break;
                
            default: break;
        }

    return true;
}

u16 Battle::GetInflictDamageVersus(Spell::spell_t spell, u8 spellPower, const Army::BattleTroop &troop)
{
    u16 dmg = SpellInflictDamage(spell, spellPower);

    if(!isTroopAffectedBySpell(spell, troop, false))
        return 0;
    
    if((troop.isUndead() && (spell == Spell::DEATHRIPPLE || spell == Spell::DEATHWAVE))
    || (!troop.isUndead() && (spell == Spell::HOLYSHOUT || spell == Spell::HOLYWORD)))
        return 0;

    switch(troop.GetID())
    {
        case Monster::AIR_ELEMENT:
            if(Spell::LIGHTNINGBOLT == spell ||
               Spell::CHAINLIGHTNING == spell ||
               Spell::ELEMENTALSTORM == spell) dmg *= 2;
            break;

        case Monster::FIRE_ELEMENT:
            if(Spell::COLDRAY == spell ||
               Spell::COLDRING == spell) dmg *= 2;
            break;

        case Monster::WATER_ELEMENT:
            if(Spell::FIREBALL == spell ||
               Spell::FIREBLAST) dmg *= 2;
            break;

        case Monster::IRON_GOLEM:
        case Monster::STEEL_GOLEM:
            if(Spell::COLDRAY == spell ||
               Spell::LIGHTNINGBOLT == spell ||
               Spell::COLDRING == spell ||
               Spell::FIREBALL == spell ||
               Spell::CHAINLIGHTNING == spell ||
               Spell::ELEMENTALSTORM == spell ||
               Spell::FIREBLAST) dmg /=2;
            break;

        default: break;
    }

    return dmg;
}

Battle::BasicSpell *Battle::CreateSpell(Spell::spell_t spell, const Battle::Battlefield &battlefield, const Rect &rect, HeroBase *hero, std::vector<Army::BattleTroop*> &affected, const Point &targetPoint)
{
    bool reflect = battlefield.GetHero(1) == hero;
    const Army::BattleTroop *target = affected.size() ? affected[0] : NULL;
    switch(spell)
    {
        //case Spell::FIREBALL:
        //case Spell::LIGHTNINGBOLT:
        //case Spell::CHAINLIGHTNING:
        case Spell::CURE:
        case Spell::MASSCURE:
        case Spell::BLESS:
        case Spell::MASSBLESS:
        case Spell::CURSE:
        case Spell::MASSCURSE:
        case Spell::DRAGONSLAYER:
        case Spell::PARALYZE:
        case Spell::BERSERKER:
        case Spell::BLIND:
        case Spell::ELEMENTALSTORM:
            return new Hover(spell);
            
        //case Spell::BLIND:
        //case Spell::ANTIMAGIC:
        //case Spell::DISPEL:
        //case Spell::MASSDISPEL:

        case Spell::STONESKIN:
        case Spell::STEELSKIN:
            return new Offsetless(spell);
            
        //case Spell::HYPNOTIZE:

        case Spell::COLDRAY:
        case Spell::DISRUPTINGRAY:
            return new Projectile(spell, reflect, rect, target->Front());

        case Spell::ARROW:
            return new MagicArrow(reflect, rect, target->Front());

        case Spell::BLOODLUST:
            return new Bloodlust(*target);

        case Spell::COLDRING:
            return new ColdRing(affected, targetPoint, battlefield, reflect, rect);

        case Spell::FIREBLAST:
            return new Splash(spell, affected, targetPoint, battlefield);
            
            //case Spell::TELEPORT:
            
        //case Spell::DEATHRIPPLE:
        //case Spell::DEATHWAVE:
        //case Spell::SHIELD:
        //case Spell::MASSSHIELD:
            
        default:
            return new BasicSpell(spell);
    }
}
    
Battle::BasicSpell::BasicSpell(Spell::spell_t spell)
: back(Rect(g_baseOffset.x, g_baseOffset.y, 640, 480)),
  sound(M82::FromSpell(spell)),
  spell(spell)
{
}

void Battle::BasicSpell::TriggerSound()
{
    if(sound != M82::UNKNOWN)
    {
        AGG::PlaySound(sound);
        sound = M82::UNKNOWN;
    }
}

void Battle::BasicSpell::PreHit()
{
    back.Save();
}

void Battle::BasicSpell::OnHit(std::vector<Army::BattleTroop*> &affected, u8 spellPower, bool reflect, Battle::Battlefield &battlefield, Battle::GUI &gui)
{
    Display &display = Display::Get();
    
    TriggerSound();
    
    u16 totalDamage = 0;
    for(u16 i = 0; i < affected.size(); i++)
    {
        u16 damage = Battle::GetInflictDamageVersus(spell, spellPower, affected[i]->GetID());
        if(affected[i]->IsDamageFatal(damage)) {
            affected[i]->Animate(Monster::AS_DIE);
            AGG::PlaySound(affected[i]->M82Kill());
        } else if(damage) {
            affected[i]->Animate(Monster::AS_PAIN);
            AGG::PlaySound(affected[i]->M82Wnce());
        }
        totalDamage += damage;
    }
    
    int animat = 0;
    u16 frame = 0, mframe = AGG::GetICNCount(ICN::ImpactFromSpell(spell));
    while(LocalEvent::Get().HandleEvents())
    {
        if(!Game::ShouldAnimateInfrequent(animat++, 5))
            continue;
        
        battlefield.Redraw();
        gui.Redraw();

        if(ICN::ImpactFromSpell(spell) != ICN::UNKNOWN && frame < mframe)
            DrawSprite(affected, ICN::ImpactFromSpell(spell), frame);
            
        display.Flip();
        if(frame >= mframe)
        {
            if(!totalDamage)
                break;
            
            bool br = true;
            for(u16 i = 0; i < affected.size(); i++)
                if(affected[i]->astate != Monster::AS_NONE)
                {
                    u8 strt = 0, len = 0;
                    affected[i]->GetAnimFrames(Monster::AS_DIE, strt, len, false);
                    if(affected[i]->astate != Monster::AS_DIE || affected[i]->aframe < strt + len - 1)
                        br = false;
                }
            if(br) break;
        }
        frame ++;
    }
}

void Battle::BasicSpell::DrawSprite(std::vector<Army::BattleTroop*> &affected, ICN::icn_t icn, int frame)
{
    const Sprite &spr = AGG::GetICN(icn, frame);
    for(u16 i = 0; i < affected.size(); i++)
        Display::Get().Blit(spr, Bf2Scr(affected[i]->Front()) + spr + SpriteOffset(affected[i]->GetCurrentSprite(), spr));
}

Point Battle::BasicSpell::SpriteOffset(const Sprite &troop, const Sprite &effect)
{
    return Point(0, -troop.h() / 2);
}

Battle::Offsetless::Offsetless(Spell::spell_t spell)
: BasicSpell(spell)
{
}

Point Battle::Offsetless::SpriteOffset(const Sprite &troop, const Sprite &effect)
{
    return Point(0, 0);
}

Battle::Projectile::Projectile(Spell::spell_t spell, bool reflect, const Rect &hrect, const Point &target)
: BasicSpell(spell)
, icon(ICN::PreImpactFromSpell(spell))
, reflect(reflect)
{
    if(reflect)
    {
        start = hrect + Point(hrect.w, hrect.h / 2);
        end = Bf2Scr(target);
    }
    else
    {
        start = hrect + Point(0, hrect.h / 2);
        end = Bf2Scr(target);
    }

    maxFrames = AGG::GetICNCount(icon);
    delta = end - start;
}

void Battle::Projectile::PreHit()
{
    BasicSpell::PreHit();
    
    TriggerSound();
    
    delta.x /= maxFrames;
    delta.y /= maxFrames;

    int animat = 0, frame = 0;
    Display &display = Display::Get();
    while(LocalEvent::Get().HandleEvents() && frame < maxFrames)
    {
        if(!Game::ShouldAnimateInfrequent(animat++, 3))
            continue;

        back.Restore();
        const Sprite &spr = AGG::GetICN(icon, FrameModifier(frame), reflect);
        display.Blit(spr, start);
        display.Flip();
        start += delta - Point(0, spr.h() / (maxFrames - frame));
        frame ++;
    }
}

Battle::MagicArrow::MagicArrow(bool reflect, const Rect &hrect, const Point &target)
: Projectile(Spell::ARROW, reflect, hrect, target)
{
    maxFrames = 8;
    double angle = M_PI_2 - atan2(-delta.y, delta.x);
    index = (int)(angle/M_PI * AGG::GetICNCount(icon));
}

Battle::Hover::Hover(Spell::spell_t spell)
: BasicSpell(spell)
{
}

Point Battle::Hover::SpriteOffset(const Sprite &troop, const Sprite &effect)
{
    return Point(0, -troop.h());
}

Battle::Bloodlust::Bloodlust(const Army::BattleTroop &troop)
: BasicSpell(Spell::BLOODLUST)
{
}

void Battle::Bloodlust::OnHit(std::vector<Army::BattleTroop*> &affected, u8 spellPower, bool reflect, Battlefield &battlefield, GUI &gui)
{
    TriggerSound();

    std::vector<Surface*> stencils;
    for(unsigned int i = 0; i < affected.size(); i++) {
        const Sprite &spr = affected[i]->GetCurrentSprite();
        Surface *stencil = new Surface;
        stencil->Set(spr.w(), spr.h());
        Surface::MakeStencil(*stencil, spr, stencil->MapRGB(150, 0, 0));
        stencils.push_back(stencil);
    }

    const int maxFrames = 20;
    const int maxAlpha = 170;
    const int half = maxFrames / 2;
    const int step = maxAlpha / half;
    int frame = 0, animat = 0;

    while(LocalEvent::Get().HandleEvents() && frame < maxFrames)
    {
        if(!Game::ShouldAnimateInfrequent(animat++, 4))
            continue;
        
        battlefield.Redraw();
        gui.Redraw();

        for(unsigned int i = 0; i < affected.size(); i++) {
            stencils[i]->SetAlpha(frame < half ? frame * step : maxAlpha - (frame - half) * step);
            Army::BattleTroop &troop = *affected[i];
            Display::Get().Blit(*stencils[i], Bf2Scr(troop.Position()) + troop.GetBlitOffset(troop.aframe, troop.IsReflected()));
        }

        Display::Get().Flip();
        frame ++;
    }

    for(unsigned int i = 0; i < stencils.size(); i++)
        delete stencils[i];
}

Battle::SplashDamage::SplashDamage(std::vector<Army::BattleTroop*> &affected, const Point &targetPoint, const Battlefield &battlefield)
: target(targetPoint)
{
    Point delta;
    for(delta.x = -1; delta.x <= 1; delta.x++)
        for(delta.y = -1; delta.y <= 1; delta.y++)
        {
            if(!delta.x && !delta.y) continue;
            if(targetPoint.y % 2 && delta.y && delta.x > 0) continue;
            if(!(targetPoint.y % 2) && delta.y && delta.x < 0) continue;
            Army::BattleTroop *troop = battlefield.GetTroopFromPoint(targetPoint + delta);
            if(!troop) continue;
            std::vector<Army::BattleTroop*>::iterator begin = affected.begin();
            std::vector<Army::BattleTroop*>::iterator end = affected.end();
            for(; begin != end; begin++)
                if(*begin == troop) break;
            if(begin == end) affected.push_back(troop);
        }
}

Battle::Splash::Splash(Spell::spell_t spell, std::vector<Army::BattleTroop*> &affected, const Point &targetPoint, const Battlefield &battlefield)
: BasicSpell(spell)
, SplashDamage(affected, targetPoint, battlefield)
{
}

void Battle::Splash::DrawSprite(std::vector<Army::BattleTroop*> &affected, ICN::icn_t icn, int frame)
{
    const Sprite &spr = AGG::GetICN(icn, frame);
    Display::Get().Blit(spr, Bf2Scr(target) + spr + SpriteOffset(affected[0]->GetCurrentSprite(), spr));

}


Battle::ColdRing::ColdRing(std::vector<Army::BattleTroop*> &affected, const Point &targetPoint, const Battlefield &battlefield, bool reflect, const Rect &hrect)
: Projectile(Spell::COLDRING, reflect, hrect, targetPoint)
, SplashDamage(affected, targetPoint, battlefield)
{
}

void Battle::ColdRing::DrawSprite(std::vector<Army::BattleTroop*> &affected, ICN::icn_t icn, int frame)
{
    const Sprite &spr = AGG::GetICN(icn, frame);
    const Sprite &reflected = AGG::GetICN(icn, frame, true);
    Display::Get().Blit(spr, Bf2Scr(target) + spr - Point(0, spr.h() / 4));
    Display::Get().Blit(reflected, Bf2Scr(target) + spr - Point(reflected.w(), spr.h() / 4));
}

bool Battle::isBadSpell(u8 spell)
{
    switch(spell)
    {
        case Spell::FIREBALL:
        case Spell::FIREBLAST:
        case Spell::LIGHTNINGBOLT:
        case Spell::CHAINLIGHTNING:
        case Spell::SLOW:
        case Spell::MASSSLOW:
        case Spell::BLIND:
        case Spell::CURSE:
        case Spell::MASSCURSE:
        case Spell::HOLYWORD:
        case Spell::HOLYSHOUT:
        case Spell::ARROW:
        case Spell::BERSERKER:
        case Spell::ARMAGEDDON:
        case Spell::ELEMENTALSTORM:
	case Spell::METEORSHOWER:
        case Spell::PARALYZE:
        case Spell::HYPNOTIZE:
        case Spell::COLDRAY:
        case Spell::COLDRING:
        case Spell::DISRUPTINGRAY:
        case Spell::DEATHRIPPLE:
        case Spell::DEATHWAVE:
            return true;
        default: break;
    }
    return false;
}

Spell::spell_t Battle::SpellTroopAttack(u8 monster)
{
    switch(monster)
    {
        case Monster::CYCLOPS:
        if(!Rand::Get(0, 4)) return Spell::PARALYZE;
        break;
        case Monster::UNICORN:
        if(!Rand::Get(0, 4)) return Spell::BLIND;
        break;
        case Monster::MUMMY:
        if(!Rand::Get(0, 4)) return Spell::CURSE;
        break;
        case Monster::ROYAL_MUMMY:
        if(!Rand::Get(0, 3)) return Spell::CURSE;
        break;
	case Monster::MEDUSA:
        if(!Rand::Get(0, 4)) return Spell::PARALYZE;
        break;
        case Monster::ARCHMAGE:
	if(!Rand::Get(0, 4)) return Spell::DISPEL;
        break;
        default: break;
    }
    return Spell::NONE;
}

u16 Battle::SpellInflictDamage(u8 spell, u8 sp)
{
    switch(spell)
    {
	case Spell::DEATHRIPPLE:        return 5 * sp;
        case Spell::ARROW:
        case Spell::COLDRING:
        case Spell::DEATHWAVE:
        case Spell::FIREBALL:
        case Spell::HOLYWORD:
        case Spell::FIREBLAST:          return 10 * sp;
        case Spell::COLDRAY:
        case Spell::HOLYSHOUT:          return 20 * sp;
        case Spell::LIGHTNINGBOLT:
        case Spell::ELEMENTALSTORM:
        case Spell::METEORSHOWER:       return 25 * sp;
        case Spell::CHAINLIGHTNING:     return 40 * sp;
        case Spell::ARMAGEDDON:         return 50 * sp;
        default: break;
    }
    return 0;
}

u8 Battle::SpellTarget(u8 spell)
{
    switch(spell)
    {
        case Spell::HOLYWORD:
        case Spell::HOLYSHOUT:         return ALLDEAD;

        case Spell::FIREBALL:
        case Spell::FIREBLAST:
	case Spell::LIGHTNINGBOLT:
        case Spell::CHAINLIGHTNING:
        case Spell::SLOW:
        case Spell::BLIND:
        case Spell::CURSE:
        case Spell::ARROW:
        case Spell::BERSERKER:
        case Spell::PARALYZE:
        case Spell::HYPNOTIZE:
        case Spell::COLDRAY:
        case Spell::DISRUPTINGRAY:     return ONEENEMY;

        case Spell::MASSSLOW:
        case Spell::MASSCURSE:         return ALLENEMY;

	case Spell::DEATHRIPPLE:
        case Spell::DEATHWAVE:         return ALLLIVE;

        case Spell::TELEPORT:
        case Spell::CURE:
	case Spell::RESURRECT:
        case Spell::RESURRECTTRUE:
        case Spell::HASTE:
        case Spell::BLESS:
        case Spell::STONESKIN:
        case Spell::STEELSKIN:
        case Spell::ANTIMAGIC:
        case Spell::DISPEL:
        case Spell::DRAGONSLAYER:
        case Spell::BLOODLUST:
        case Spell::ANIMATEDEAD:
        case Spell::MIRRORIMAGE:
        case Spell::SHIELD:            return ONEFRIEND;

        case Spell::MASSBLESS:
        case Spell::MASSCURE:
        case Spell::MASSHASTE:
        case Spell::MASSSHIELD:        return ALLFRIEND;

	case Spell::COLDRING:
	case Spell::SUMMONEELEMENT:
        case Spell::SUMMONAELEMENT:
        case Spell::SUMMONFELEMENT:
        case Spell::SUMMONWELEMENT:    return FREECELL;

        case Spell::MASSDISPEL:
        case Spell::ARMAGEDDON:
	case Spell::ELEMENTALSTORM:
        case Spell::METEORSHOWER:
        case Spell::EARTHQUAKE:        return ALL;

	default: break;
    }
    
    return NOTARGET;
}

#endif
