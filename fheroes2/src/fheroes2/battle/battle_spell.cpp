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

#include "battle_troop.h"
#include "battle_spell.h"

bool Battle::AllowSpell(Spell::spell_t spell, const Army::BattleTroop &troop)
{
    Spell::target_t target = Spell::Target(spell);
    if(troop == Monster::DWARF || troop == Monster::BATTLE_DWARF) {
        if(!Rand::Get(0,3) && target != Spell::ONEFRIEND && target != Spell::ALLFRIEND) return false;
    }
    if(troop.FindMagic(Spell::ANTIMAGIC) && target != Spell::ONEFRIEND && target != Spell::ALLFRIEND)
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
                case Spell::BERZERKER:
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
    if(!Spell::Power(spell)) {
        switch(spell) {
            case Spell::MASSDISPEL:
            case Spell::DISPEL:
                troop.ClearMagic();
                return;
                // 	Spell::TELEPORT,
                // 	Spell::MIRRORIMAGE,
                // 	Spell::BERZERKER,
                // 	Spell::HYPNOTIZE,
                // 	Spell::DISRUPTINGRAY,
                // 	Spell::EARTHQUAKE,
            default:
                break;
                // TODO
        }
    } else if(Spell::Power(spell) == 1) {
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
                break;
            case Spell::MASSSHIELD:
                magic.spell = Spell::SHIELD;
                break;
            case Spell::MASSHASTE:
                magic.spell = Spell::HASTE;
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
        int damage = spower * Spell::Power(spell);
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

bool Battle::isTroopAffectedBySpell(Spell::spell_t spell, const Army::BattleTroop &troop)
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
            case Spell::BERZERKER: return false;
            default: break;
        }

    if(troop.isElemental())
        switch(spell)
        {
            case Spell::BLIND:
            case Spell::PARALYZE:
            case Spell::HYPNOTIZE:
            case Spell::BERZERKER: return false;
            default: break;
        }

    if(Spell::isBad(spell))
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
                if(3 == Rand::Get(1,4)) return false;
                break;

            case Monster::GIANT:
            case Monster::TITAN:
                if(Spell::BLIND == spell ||
                   Spell::PARALYZE == spell ||
                   Spell::BERZERKER == spell ||
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
    u16 dmg = Spell::InflictDamage(spell, spellPower);

    if(!isTroopAffectedBySpell(spell, troop))
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
