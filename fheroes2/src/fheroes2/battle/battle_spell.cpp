/***************************************************************************
 *   Copyright (C) 2008 by Vasily Makarov <drmoriarty@rambler.ru>          *
 *   Copyright (C) 2008 by Josh Matthews  <josh@joshmatthews.net>          *
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

bool Spell::AllowSpell(spell_t spell, const Army::BattleTroop &troop)
{
    target_t target = Target(spell);
    if(troop == Monster::DWARF || troop == Monster::BATTLE_DWARF) {
	if(!Rand::Get(0,3) && target != ONEFRIEND && target != ALLFRIEND) return false;
    }
    if(troop.FindMagic(ANTIMAGIC) && target != ONEFRIEND && target != ALLFRIEND)
	return false;
    if(spell == ANTIMAGIC && troop.summoned) return false;
    switch(troop()) {
    case Monster::GREEN_DRAGON:
    case Monster::RED_DRAGON:
    case Monster::BLACK_DRAGON:
	return false;
    default:
	switch(spell) {
	case BLESS:
	case MASSBLESS:
	case CURSE:
	case MASSCURSE:
	case BERZERKER:
	case BLIND:
	case PARALYZE:
	case HYPNOTIZE:
	case DEATHRIPPLE:
	case DEATHWAVE:
	    return troop.GetRace() != Race::NECR;
	case HOLYWORD:
	case HOLYSHOUT:
	    return troop.GetRace() == Race::NECR;
	case ANIMATEDEAD:
	    return troop.GetRace() == Race::NECR &&
		( Monster(troop()).GetHitPoints() > troop.hp ||
		  troop.oldcount > troop.Count());
	case CURE:
	case MASSCURE:
	    return Monster(troop()).GetHitPoints() > troop.hp;
	case RESURRECT:
	case RESURRECTTRUE:
	    return Monster(troop()).GetHitPoints() > troop.hp ||
		troop.oldcount > troop.Count();

	case DISPEL:
	case MASSDISPEL:
	    return troop.Magics().size();
/*	FIREBALL,
	FIREBLAST,
	LIGHTNINGBOLT,
	CHAINLIGHTNING,
	TELEPORT,
	HASTE,
	MASSHASTE,
	SLOW,
	MASSSLOW,
	STONESKIN,
	STEELSKIN,
	ANTIMAGIC,
	ARROW,
	ARMAGEDDON,
	ELEMENTALSTORM,
	METEORSHOWER,
	COLDRAY,
	COLDRING,
	DISRUPTINGRAY,
	DRAGONSLAYER,
	BLOODLUST,
	MIRRORIMAGE,
	SHIELD,
	MASSSHIELD,
	SUMMONEELEMENT,
	SUMMONAELEMENT,
	SUMMONFELEMENT,
	SUMMONWELEMENT,
	EARTHQUAKE*/
	default: 
	    return true;
	}
	return true;
    }
}

void Spell::ApplySpell(int spower, spell_t spell, Army::BattleTroop &troop)
{
    //Dialog::Message("apply spell", Spell::String(spell), Font::BIG, Dialog::OK);
    magic_t magic;
    magic.spell = spell;
    magic.duration = spower;
    if(!Power(spell)) {
	switch(spell) {
	case MASSDISPEL:
	case DISPEL:
	    troop.ClearMagic();
	return;
// 	TELEPORT,
// 	MIRRORIMAGE,
// 	BERZERKER,
// 	HYPNOTIZE,
// 	DISRUPTINGRAY,
// 	EARTHQUAKE,
	default:
	    break;
	// TODO
	}
    } else if(Power(spell) == 1) {
	switch(spell) {
	case MASSBLESS:
	    magic.spell = BLESS;
	case BLESS:
	    troop.RemoveMagic(CURSE);
	    troop.RemoveMagic(MASSCURSE);
	    break;
	case MASSCURSE:
	    magic.spell = CURSE;
	case CURSE:
	    troop.RemoveMagic(BLESS);
	    troop.RemoveMagic(MASSBLESS);
	    break;
	case STONESKIN:
	    troop.RemoveMagic(STEELSKIN);
	    break;
	case STEELSKIN:
	    troop.RemoveMagic(STONESKIN);
	    break;
	case MASSSLOW:
	    magic.spell = SLOW;
	    break;
	case MASSSHIELD:
	    magic.spell = SHIELD;
	    break;
	case MASSHASTE:
	    magic.spell = HASTE;
	    break;
// 	BLIND,
// 	SHIELD,
// 	ANTIMAGIC,
// 	PARALYZE,
// 	STONE,
// 	DRAGONSLAYER,
// 	BLOODLUST,
	default:
	    break;
	}
	troop.SetMagic(magic);
    } else {
	int damage = spower * Power(spell);
	switch(spell) {
	case MASSCURE:
	    spell = CURE;
	case CURE:
	case RESURRECT:
	case RESURRECTTRUE:
	case ANIMATEDEAD: {
	    int hp = Monster(troop()).GetHitPoints();
	    troop.hp += damage;
	    if(troop.hp > hp) {
		if(spell == CURE) troop.hp = hp;
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
	case SUMMONEELEMENT:
	case SUMMONAELEMENT:
	case SUMMONFELEMENT:
	case SUMMONWELEMENT:
	    // TODO
	    return;
// 	ARROW,
// 	FIREBALL,
// 	FIREBLAST,
// 	LIGHTNINGBOLT,
// 	CHAINLIGHTNING,
// 	HOLYWORD,
// 	HOLYSHOUT,
// 	COLDRAY,
// 	COLDRING,
// 	DEATHRIPPLE,
// 	DEATHWAVE,
// 	ARMAGEDDON,
// 	ELEMENTALSTORM,
// 	METEORSHOWER,
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

Spell::spell_t Spell::TroopAttack(const Monster & monster)
{
    switch(monster())
    {
    case Monster::CYCLOPS:
	if(!Rand::Get(0, 4)) return PARALYZE;
	break;
    case Monster::UNICORN:
	if(!Rand::Get(0, 4)) return BLIND;
	break;
    case Monster::MUMMY:
	if(!Rand::Get(0, 4)) return CURSE;
	break;
    case Monster::ROYAL_MUMMY:
	if(!Rand::Get(0, 3)) return CURSE;
	break;
    case Monster::MEDUSA:
	if(!Rand::Get(0, 4)) return PARALYZE;
	break;
    case Monster::ARCHMAGE:
	if(!Rand::Get(0, 4)) return DISPEL;
	break;
    default:
	return NONE;
    }
    return NONE;
}
