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

#include "rand.h"
#include "spell.h"
#include "tools.h"
#include "army.h"

#define icnnone ICN::UNKNOWN
#define m82none M82::UNKNOWN

namespace Spell
{
    static const stats_t all_spells[] = {
	//  name                   mana lvl cmbt  spr    target  pwr icn            m82           description
	{ "Unknown",                 0, 0, false,  0,  NOTARGET,  0, ICN::UNKNOWN,  M82::UNKNOWN, "Unknown spell." },
	{ "Fireball",                9, 3,  true,  8,  ONEENEMY, 10, ICN::FIREBALL, M82::FIREBALL, "Causes a giant fireball to strike the selected area, damaging all nearby creatures." },
	{ "Fireblast",              15, 4,  true,  9,  ONEENEMY, 10, ICN::FIREBAL2, M82::FIREBALL, "An improved version of fireball, fireblast affects two hexes around the center point of the spell, rather than one." }, 
	{ "Lightning Bolt",          7, 2,  true,  4,  ONEENEMY, 25, ICN::ELECTRIC, m82none, "Causes a bolt of electrical energy to strike the selected creature." }, 
	{ "Chain Lightning",        15, 4,  true,  5,  ONEENEMY, 40, ICN::ELECTRIC, M82::CHAINLTE, "Causes a bolt of electrical energy to strike a selected creature, then strike the nearest creature with half damage, then strike the NEXT nearest creature with half again damage, and so on, until it becomes too weak to be harmful.  Warning:  This spell can hit your own creatures!" }, 
	{ "Teleport",                9, 3,  true, 10, ONEFRIEND,  0, icnnone      , M82::TELEIN, "Teleports the creature you select to any open position on the battlefield." }, 
	{ "Cure",                    6, 1,  true,  6, ONEFRIEND,  5, icnnone      , M82::CURE, "Removes all negative spells cast upon one of your units, and restores up to 5 HP per level of spell power." }, 
	{ "Mass Cure",              15, 4,  true,  2, ALLFRIEND,  5, icnnone      , M82::MASSCURE, "Removes all negative spells cast upon your forces, and restores up to 5 HP per level of spell power, per creature." }, 
	{ "Resurrect",              12, 4,  true, 12, ONEFRIEND, 50, icnnone      , M82::RESURECT, "Resurrects creatures from a damaged or dead unit until end of combat." }, 
	{ "Resurrect True",         15, 5,  true, 13, ONEFRIEND, 50, icnnone      , M82::RESURTRU, "Resurrects creatures from a damaged or dead unit permanently." }, 
	{ "Haste",                   3, 1,  true, 14, ONEFRIEND,  1, ICN::HASTE   , M82::HASTE, "Increases the speed of any creature by two." }, 
	{ "Mass Haste",             10, 3,  true, 14, ALLFRIEND,  1, ICN::HASTE   , M82::MASSHAST, "Increases the speed of all of your creatures by two." }, 
	{ "Slow",                    3, 1,  true,  1,  ONEENEMY,  1, icnnone      , M82::SLOW, "Slows target to half movement rate." }, 
	{ "Mass Slow",              15, 4,  true,  1,  ALLENEMY,  1, icnnone      , M82::MASSSLOW, "Slows all enemies to half movement rate." }, 
	{ "Blind ",                  6, 2,  true, 21,  ONEENEMY,  1, icnnone      , M82::BLIND, "Clouds the affected creatures' eyes, preventing them from moving." }, 
	{ "Bless",                   3, 1,  true,  7, ONEFRIEND,  1, icnnone      , M82::BLESS, "Causes the selected creatures to inflict maximum damage." }, 
	{ "Mass Bless",             12, 3,  true,  7, ALLFRIEND,  1, icnnone      , M82::MASSBLES, "Causes all of your units to inflict maximum damage." },
	{ "Stoneskin",               3, 1,  true, 31, ONEFRIEND,  1, ICN::STONSKIN, M82::STONSKIN, "Magically increases the defense skill of the selected creatures." },
	{ "Steelskin",               6, 2,  true, 30, ONEFRIEND,  1, ICN::STELSKIN, M82::STELSKIN, "Increases the defense skill of the targeted creatures.  This is an improved version of Stoneskin." },
	{ "Curse",                   3, 1,  true,  3,  ONEENEMY,  1, ICN::CURSE   , M82::CURSE, "Causes the selected creatures to inflict minimum damage." },
	{ "Mass Curse",             12, 3,  true,  3,  ALLENEMY,  1, ICN::CURSE   , M82::MASSCURS, "Causes all enemy troops to inflict minimum damage." },
	{ "Holy Word",               9, 3,  true, 22,   ALLDEAD, 10, icnnone      , m82none, "Damages all undead in the battle." },
	{ "Holy Shout",             12, 4,  true, 23,   ALLDEAD, 20, icnnone      , m82none, "Damages all undead in the battle.  This is an improved version of Holy Word." },
	{ "Anti-Magic",              7, 3,  true, 17, ONEFRIEND,  1, icnnone      , M82::ANTIMAGK, "Prevents harmful magic against the selected creatures." },
	{ "Dispel Magic",            5, 1,  true, 18, ONEFRIEND,  0, icnnone      , M82::DIPMAGK, "Removes all magic spells from a single target." },
	{ "Mass Dispel",            12, 3,  true, 18,       ALL,  0, icnnone      , M82::DIPMAGK, "Removes all magic spells from all creatures." },
	{ "Magic Arrow",             3, 1,  true, 28,  ONEENEMY, 10, icnnone      , M82::MAGCAROW, "Causes a magic arrow to strike the selected target." },
	{ "Berzerker",              12, 4,  true, 19,  ONEENEMY,  0, icnnone      , M82::BERZERK, "Causes a creature to attack its nearest neighbor." },
	{ "Armageddon",             20, 5,  true, 16,       ALL, 50, icnnone      , M82::ARMGEDN, "Holy terror strikes the battlefield, causing severe damage to all creatures." },
	{ "Elemental Storm",        15, 4,  true, 11,       ALL, 25, ICN::STORM,    M82::STORM, "Magical elements pour down on the battlefield, damaging all creatures." },
	{ "Meteor Shower",          15, 4,  true, 24,       ALL, 25, ICN::METEOR,   M82::METEOR, "A rain of rocks strikes an area of the battlefield, damaging all nearby creatures." },
	{ "Paralyze",                9, 3,  true, 20,  ONEENEMY,  1, ICN::PARALYZE, M82::PARALIZE, "The targeted creatures are paralyzed, unable to move or retaliate." },
	{ "Hypnotize",              15, 5,  true, 37,  ONEENEMY,  0, ICN::HYPNOTIZ, M82::HYPNOTIZ, "Brings a single enemy unit under your control for one combat round if its hits are less than 25 times the caster's spell power." },
	{ "Cold Ray",                6, 2,  true, 35,  ONEENEMY, 20, ICN::COLDRAY,  M82::COLDRAY, "Drains body heat from a single enemy unit." },
	{ "Cold Ring",               9, 3,  true, 36,  FREECELL, 10, ICN::COLDRING, M82::COLDRING, "Drains body heat from all units surrounding the center point, but not including the center point." },
	{ "Disrupting Ray",          7, 2,  true, 34,  ONEENEMY,  0, ICN::DISRRAY,  M82::DISRUPTR, "Reduces the defense rating of an enemy unit by three." },
	{ "Death Ripple",            6, 2,  true, 28,   ALLLIVE,  5, ICN::REDDEATH, M82::MNRDEATH, "Damages all living (non-undead) units in the battle." },
	{ "Death Wave",             10, 3,  true, 29,   ALLLIVE, 10, ICN::REDDEATH, m82none, "Damages all living (non-undead) units in the battle.  This spell is an improved version of Death Ripple." },
	{ "Dragon Slayer",           6, 2,  true, 32, ONEFRIEND,  1, ICN::DRAGSLAY, m82none, "Greatly increases a unit's attack skill vs. Dragons." },
	{ "Blood Lust",              3, 1,  true, 27, ONEFRIEND,  1, icnnone      , M82::BLOODLUS, "Increases a unit's attack skill." },
	{ "Animate Dead",           10, 3,  true, 25, ONEFRIEND, 50, icnnone      , m82none, "Resurrects creatures from a damaged or dead undead unit permanently." },
	{ "Mirror Image",           25, 5,  true, 26, ONEFRIEND,  0, icnnone      , M82::MIRRORIM, "Creates an illusionary unit that duplicates one of your existing units.  This illusionary unit does the same damages as the original, but will vanish if it takes any damage." },
	{ "Shield",                  3, 1,  true, 15, ONEFRIEND,  1, ICN::SHIELD,   M82::SHIELD, "Halves damage received from ranged attacks for a single unit." },
	{ "Mass Shield",             7, 4,  true, 15, ALLFRIEND,  1, icnnone      , M82::MASSSHIE, "Halves damage received from ranged attacks for all of your units." },
	{ "Summon Earth Elemental", 25, 5,  true, 56,  FREECELL,  3, icnnone      , M82::SUMNELM, "Summons Earth Elementals to fight for your army." },
	{ "Summon Air Elemental",   25, 5,  true, 57,  FREECELL,  3, icnnone      , M82::SUMNELM, "Summons Air Elementals to fight for your army." },
	{ "Summon Fire Elemental",  25, 5,  true, 58,  FREECELL,  3, icnnone      , M82::SUMNELM, "Summons Fire Elementals to fight for your army." },
	{ "Summon Water Elemental", 25, 5,  true, 59,  FREECELL,  3, icnnone      , M82::SUMNELM, "Summons Water Elementals to fight for your army." },
	{ "Earthquake",             15, 3,  true, 33,       ALL,  0, icnnone      , M82::ERTHQUAK, "Damages castle walls." },
	{ "View Mines",              1, 1, false, 39,  NOTARGET,  0, icnnone      , m82none, "Causes all mines across the land to become visible." },
	{ "View Resources",          1, 1, false, 40,  NOTARGET,  0, icnnone      , m82none, "Causes all resources across the land to become visible." },
	{ "View Artifacts",          2, 2, false, 41,  NOTARGET,  0, icnnone      , m82none, "Causes all artifacts across the land to become visible." },
	{ "View Towns",              2, 3, false, 42,  NOTARGET,  0, icnnone      , m82none, "Causes all towns and castles across the land to become visible." },
	{ "View Heroes",             2, 3, false, 43,  NOTARGET,  0, icnnone      , m82none, "Causes all Heroes across the land to become visible." },
	{ "View All",                3, 4, false, 44,  NOTARGET,  0, icnnone      , m82none, "Causes the entire land to become visible." },
	{ "Identify Hero",           3, 3, false, 45,  NOTARGET,  0, icnnone      , m82none, "Allows the caster to view detailed information on enemy Heroes." },
	{ "Summon Boat",             5, 2, false, 46,  NOTARGET,  0, icnnone      , m82none, "Summons the nearest unoccupied, friendly boat to an adjacent shore location.  A friendly boat is one which you just built or were the most recent player to occupy." },
	{ "Dimension Door",         10, 5, false, 47,  NOTARGET,  0, icnnone      , m82none, "Allows the caster to magically transport to a nearby location." },
	{ "Town Gate",              10, 4, false, 48,  NOTARGET,  0, icnnone      , m82none, "Returns the caster to any town or castle currently owned." },
	{ "Town Portal",            20, 5, false, 49,  NOTARGET,  0, icnnone      , m82none, "Returns the hero to the town or castle of choice, provided it is controlled by you." },
	{ "Visions",                 6, 2, false, 50,  NOTARGET,  0, icnnone      , m82none, "Visions predicts the likely outcome of an encounter with a neutral army camp." },
	{ "Haunt",                   8, 2, false, 51,  NOTARGET,  0, icnnone      , M82::H2MINE, "Haunts a mine you control with Ghosts.  This mine stops producing resources.  (If I can't keep it, nobody will!)" },
	{ "Set Earth Guardian",     15, 4, false, 52,  NOTARGET,  4, icnnone      , m82none, "Sets Earth Elementals to guard a mine against enemy armies." },
	{ "Set Air Guardian",       15, 4, false, 53,  NOTARGET,  4, icnnone      , m82none, "Sets Air Elementals to guard a mine against enemy armies." },
	{ "Set Fire Guardian",      15, 4, false, 54,  NOTARGET,  4, icnnone      , m82none, "Sets Fire Elementals to guard a mine against enemy armies." },
	{ "Set Water Guardian",     15, 4, false, 55,  NOTARGET,  4, icnnone      , m82none, "Sets Water Elementals to guard a mine against enemy armies." },
	{ "Stone",                   9, 3, false,  0,  NOTARGET,  1, icnnone      , m82none, "" }
    };

}

std::string Spell::String(spell_t spell)
{ 
    std::string str = Spell::all_spells[spell].name + " ["; 
    String::AddInt(str, Mana(spell));
    str += "]";
    return str;
}

const std::string & Spell::Description(spell_t spell)
{ return Spell::all_spells[spell].description; }

u8 Spell::Mana(spell_t spell)
{
    return Spell::all_spells[spell].mana;
}

Spell::target_t Spell::Target(spell_t spell)
{
    return Spell::all_spells[spell].target;
}

u8 Spell::Power(spell_t spell)
{
    return Spell::all_spells[spell].power;
}

ICN::icn_t Spell::Icn(spell_t spell)
{
    return Spell::all_spells[spell].icn;
}

M82::m82_t Spell::M82(spell_t spell)
{
    return Spell::all_spells[spell].m82;
}

u8 Spell::Level(spell_t spell)
{
    return Spell::all_spells[spell].level;
}

Spell::spell_t Spell::RandCombat(const u8 lvl)
{
    if(1 == lvl)
    switch(Rand::Get(1, 10))
    {
	case 1:  return BLESS;
	case 2:  return BLOODLUST;
	case 3:  return CURE;
	case 4:  return CURSE;
	case 5:  return DISPEL;
	case 6:  return HASTE;
	case 7:  return ARROW;
	case 8:  return SHIELD;
	case 9:  return SLOW;
	case 10: return STONESKIN;
	default: break;
    }
    else
    if(2 == lvl)
    switch(Rand::Get(1, 7))
    {
	case 1: return BLIND;
	case 2: return COLDRAY;
	case 3: return DEATHRIPPLE;
	case 4: return DISRUPTINGRAY;
	case 5: return DRAGONSLAYER;
	case 6: return LIGHTNINGBOLT;
	case 7: return STEELSKIN;
	default: break;
    }
    else
    if(3 == lvl)
    switch(Rand::Get(1, 13))
    {
	case 1: return ANIMATEDEAD;
	case 2: return ANTIMAGIC;
	case 3: return COLDRING;
	case 4: return DEATHWAVE;
	case 5: return EARTHQUAKE;
	case 6: return FIREBALL;
	case 7: return HOLYWORD;
	case 8: return MASSBLESS;
	case 9: return MASSCURSE;
	case 10: return MASSDISPEL;
	case 11: return MASSHASTE;
	case 12: return PARALYZE;
	case 13: return TELEPORT;
	default: break;
    }
    else
    if(4 == lvl)
    switch(Rand::Get(1, 10))
    {
	case 1: return BERZERKER;
	case 2: return CHAINLIGHTNING;
	case 3: return ELEMENTALSTORM;
	case 4: return FIREBLAST;
	case 5: return HOLYSHOUT;
	case 6: return MASSCURE;
	case 7: return MASSSHIELD;
	case 8: return MASSSLOW;
	case 9: return METEORSHOWER;
	case 10: return RESURRECT;
	default: break;
    }
    else
    if(5 == lvl)
    switch(Rand::Get(1, 8))
    {
	case 1: return ARMAGEDDON;
	case 2: return HYPNOTIZE;
	case 3: return MIRRORIMAGE;
	case 4: return RESURRECTTRUE;
	case 5: return SUMMONEELEMENT;
	case 6: return SUMMONAELEMENT;
	case 7: return SUMMONFELEMENT;
	case 8: return SUMMONWELEMENT;
	default: break;
    }

    return Spell::NONE;    
}

Spell::spell_t Spell::RandAdventure(const u8 lvl)
{
    if(1 == lvl)
    switch(Rand::Get(1, 2))
    {
	case 1: return VIEWMINES;
	case 2: return VIEWRESOURCES;
	default: break;
    }
    else
    if(2 == lvl)
    switch(Rand::Get(1, 4))
    {
	case 1: return HAUNT;
	case 2: return SUMMONBOAT;
	case 3: return VIEWARTIFACTS;
	case 4: return VISIONS;
	default: break;
    }
    else
    if(3 == lvl)
    switch(Rand::Get(1, 3))
    {
	case 1: return IDENTIFYHERO;
	case 2: return VIEWHEROES;
	case 3: return VIEWTOWNS;
	default: break;
    }
    else
    if(4 == lvl)
    switch(Rand::Get(1, 6))
    {
	case 1: return SETEGUARDIAN;
	case 2: return SETAGUARDIAN;
	case 3: return SETFGUARDIAN;
	case 4: return SETWGUARDIAN;
	case 5: return TOWNGATE;
	case 6: return VIEWALL;

	default: break;
    }
    else
    if(5 == lvl)
    switch(Rand::Get(1, 2))
    {
	case 1: return DIMENSIONDOOR;
	case 2: return TOWNPORTAL;
	default: break;
    }

    return Spell::NONE;
}

bool Spell::isCombat(spell_t spell)
{
    return Spell::all_spells[spell].combat;
}

u8 Spell::GetIndexSprite(spell_t spell)
{
    return Spell::all_spells[spell].sprite;
}

bool Spell::AllowSpell(spell_t spell, const Army::Troops &troop)
{
    switch(troop.Monster()) {
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
	    return Monster::GetRace(troop.Monster()) != Race::NECR;
	case HOLYWORD:
	case HOLYSHOUT:
	    return Monster::GetRace(troop.Monster()) == Race::NECR;
	case ANIMATEDEAD:
	    return Monster::GetRace(troop.Monster()) == Race::NECR &&
		( Monster::GetStats(troop.Monster()).hp > troop.hp ||
		  troop.oldcount > troop.Count());
	case CURE:
	case MASSCURE:
	    return Monster::GetStats(troop.Monster()).hp > troop.hp;
	case RESURRECT:
	case RESURRECTTRUE:
	    return Monster::GetStats(troop.Monster()).hp > troop.hp ||
		troop.oldcount > troop.Count();

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
	DISPEL,
	MASSDISPEL,
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

Spell::spell_t Spell::TroopAttack(Monster::monster_t monster)
{
    switch(monster) {
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

u8 Spell::GetInlIndexSprite(spell_t spell)
{
    switch(spell) {
    case HASTE:
    case MASSHASTE:
	return 0;
    case SLOW:
    case MASSSLOW:
	return 1;
    case BLIND:
	return 2;
    case BLESS:
    case MASSBLESS:
	return 3;
    case CURSE:
    case MASSCURSE:
	return 4;
    case BERZERKER:
	return 5;
    case PARALYZE:
	return 6;
    case HYPNOTIZE:
	return 7;
    case DRAGONSLAYER:
	return 8;
    case BLOODLUST:
	return 9;
    case SHIELD:
    case MASSSHIELD:
	return 10;
    case STONE:
	return 11;
    case ANTIMAGIC:
	return 12;
    case STONESKIN:
	return 13;
    case STEELSKIN:
	return 14;
    default:
	return 0;
    }
    return 0;
}
