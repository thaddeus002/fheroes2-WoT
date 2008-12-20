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

#include "spell.h"
#include "army.h"

#define icnnone ICN::UNKNOWN
#define m82none M82::UNKNOWN

namespace Spell
{
    static const struct
    {
        const std::string name;
        u8 mana;
        u8 sprite;
        u8 power;
        ICN::icn_t icn;
        M82::m82_t m82;
        const std::string description;
    } all_spells[] = {
	//  name                   mana spr pwr icn            m82           description
	{ "Unknown",                 0,  0,  0, ICN::UNKNOWN,  M82::UNKNOWN, "Unknown spell." },
	{ "Fireball",                9,  8, 10, ICN::FIREBALL, M82::FIREBALL, "Causes a giant fireball to strike the selected area, damaging all nearby creatures." },
	{ "Fireblast",              15,  9, 10, ICN::FIREBAL2, M82::FIREBALL, "An improved version of fireball, fireblast affects two hexes around the center point of the spell, rather than one." }, 
	{ "Lightning Bolt",          7,  4, 25, ICN::SPARKS,   M82::LIGHTBLT, "Causes a bolt of electrical energy to strike the selected creature." }, 
	{ "Chain Lightning",        15,  5, 40, ICN::SPARKS,   M82::CHAINLTE, "Causes a bolt of electrical energy to strike a selected creature, then strike the nearest creature with half damage, then strike the NEXT nearest creature with half again damage, and so on, until it becomes too weak to be harmful.  Warning:  This spell can hit your own creatures!" }, 
	{ "Teleport",                9, 10,  0, icnnone      , M82::TELEIN, "Teleports the creature you select to any open position on the battlefield." }, 
	{ "Cure",                    6,  6,  5, ICN::MAGIC01 , M82::CURE, "Removes all negative spells cast upon one of your units, and restores up to 5 HP per level of spell power." }, 
	{ "Mass Cure",              15,  2,  5, ICN::MAGIC01 , M82::MASSCURE, "Removes all negative spells cast upon your forces, and restores up to 5 HP per level of spell power, per creature." }, 
	{ "Resurrect",              12, 12, 50, icnnone      , M82::RESURECT, "Resurrects creatures from a damaged or dead unit until end of combat." }, 
	{ "Resurrect True",         15, 13, 50, icnnone      , M82::RESURTRU, "Resurrects creatures from a damaged or dead unit permanently." }, 
	{ "Haste",                   3, 14,  1, ICN::HASTE   , M82::HASTE, "Increases the speed of any creature by two." }, 
	{ "Mass Haste",             10, 14,  1, ICN::HASTE   , M82::MASSHAST, "Increases the speed of all of your creatures by two." }, 
	{ "Slow",                    3,  1,  1, ICN::MAGIC02 , M82::SLOW, "Slows target to half movement rate." }, 
	{ "Mass Slow",              15,  1,  1, ICN::MAGIC02 , M82::MASSSLOW, "Slows all enemies to half movement rate." }, 
	{ "Blind ",                  6, 21,  1, ICN::BLIND   , M82::BLIND, "Clouds the affected creatures' eyes, preventing them from moving." }, 
	{ "Bless",                   3,  7,  1, ICN::BLESS   , M82::BLESS, "Causes the selected creatures to inflict maximum damage." }, 
	{ "Mass Bless",             12,  7,  1, ICN::BLESS   , M82::MASSBLES, "Causes all of your units to inflict maximum damage." },
	{ "Stoneskin",               3, 31,  1, ICN::STONSKIN, M82::STONSKIN, "Magically increases the defense skill of the selected creatures." },
	{ "Steelskin",               6, 30,  1, ICN::STELSKIN, M82::STELSKIN, "Increases the defense skill of the targeted creatures.  This is an improved version of Stoneskin." },
	{ "Curse",                   3,  3,  1, ICN::CURSE   , M82::CURSE, "Causes the selected creatures to inflict minimum damage." },
	{ "Mass Curse",             12,  3,  1, ICN::CURSE   , M82::MASSCURS, "Causes all enemy troops to inflict minimum damage." },
	{ "Holy Word",               9, 22, 10, icnnone      , m82none, "Damages all undead in the battle." },
	{ "Holy Shout",             12, 23, 20, icnnone      , m82none, "Damages all undead in the battle.  This is an improved version of Holy Word." },
	{ "Anti-Magic",              7, 17,  1, ICN::MAGIC06 , M82::ANTIMAGK, "Prevents harmful magic against the selected creatures." },
	{ "Dispel Magic",            5, 18,  0, ICN::MAGIC07 , M82::DIPMAGK, "Removes all magic spells from a single target." },
	{ "Mass Dispel",            12, 18,  0, ICN::MAGIC07 , M82::DIPMAGK, "Removes all magic spells from all creatures." },
	{ "Magic Arrow",             3, 38, 10, icnnone      , M82::MAGCAROW, "Causes a magic arrow to strike the selected target." },
	{ "Berzerker",              12, 19,  0, icnnone      , M82::BERZERK, "Causes a creature to attack its nearest neighbor." },
	{ "Armageddon",             20, 16, 50, icnnone      , M82::ARMGEDN, "Holy terror strikes the battlefield, causing severe damage to all creatures." },
	{ "Elemental Storm",        15, 11, 25, ICN::STORM,    M82::STORM, "Magical elements pour down on the battlefield, damaging all creatures." },
	{ "Meteor Shower",          15, 24, 25, ICN::METEOR,   M82::METEOR, "A rain of rocks strikes an area of the battlefield, damaging all nearby creatures." },
	{ "Paralyze",                9, 20,  1, ICN::PARALYZE, M82::PARALIZE, "The targeted creatures are paralyzed, unable to move or retaliate." },
	{ "Hypnotize",              15, 37,  0, ICN::HYPNOTIZ, M82::HYPNOTIZ, "Brings a single enemy unit under your control for one combat round if its hits are less than 25 times the caster's spell power." },
	{ "Cold Ray",                6, 36, 20, ICN::ICECLOUD/*COLDRAY*/, M82::COLDRAY, "Drains body heat from a single enemy unit." },
	{ "Cold Ring",               9, 35, 10, ICN::COLDRING, M82::COLDRING, "Drains body heat from all units surrounding the center point, but not including the center point." },
	{ "Disrupting Ray",          7, 34,  0, ICN::DISRRAY,  M82::DISRUPTR, "Reduces the defense rating of an enemy unit by three." },
	{ "Death Ripple",            6, 28,  5, ICN::REDDEATH, M82::MNRDEATH, "Damages all living (non-undead) units in the battle." },
	{ "Death Wave",             10, 29, 10, ICN::REDDEATH, m82none, "Damages all living (non-undead) units in the battle.  This spell is an improved version of Death Ripple." },
	{ "Dragon Slayer",           6, 32,  1, ICN::DRAGSLAY, M82::DRGNSLAY, "Greatly increases a unit's attack skill vs. Dragons." },
	{ "Blood Lust",              3, 27,  1, icnnone      , M82::BLOODLUS, "Increases a unit's attack skill." },
	{ "Animate Dead",           10, 25, 50, icnnone      , M82::RESURECT, "Resurrects creatures from a damaged or dead undead unit permanently." },
	{ "Mirror Image",           25, 26,  0, icnnone      , M82::MIRRORIM, "Creates an illusionary unit that duplicates one of your existing units.  This illusionary unit does the same damages as the original, but will vanish if it takes any damage." },
	{ "Shield",                  3, 15,  1, ICN::SHIELD,   M82::SHIELD, "Halves damage received from ranged attacks for a single unit." },
	{ "Mass Shield",             7, 15,  1, ICN::SHIELD  , M82::MASSSHIE, "Halves damage received from ranged attacks for all of your units." },
	{ "Summon Earth Elemental", 25, 56,  3, icnnone      , M82::SUMNELM, "Summons Earth Elementals to fight for your army." },
	{ "Summon Air Elemental",   25, 57,  3, icnnone      , M82::SUMNELM, "Summons Air Elementals to fight for your army." },
	{ "Summon Fire Elemental",  25, 58,  3, icnnone      , M82::SUMNELM, "Summons Fire Elementals to fight for your army." },
	{ "Summon Water Elemental", 25, 59,  3, icnnone      , M82::SUMNELM, "Summons Water Elementals to fight for your army." },
	{ "Earthquake",             15, 33,  0, icnnone      , M82::ERTHQUAK, "Damages castle walls." },
	{ "View Mines",              1, 39,  0, icnnone      , m82none, "Causes all mines across the land to become visible." },
	{ "View Resources",          1, 40,  0, icnnone      , m82none, "Causes all resources across the land to become visible." },
	{ "View Artifacts",          2, 41,  0, icnnone      , m82none, "Causes all artifacts across the land to become visible." },
	{ "View Towns",              2, 42,  0, icnnone      , m82none, "Causes all towns and castles across the land to become visible." },
	{ "View Heroes",             2, 43,  0, icnnone      , m82none, "Causes all Heroes across the land to become visible." },
	{ "View All",                3, 44,  0, icnnone      , m82none, "Causes the entire land to become visible." },
	{ "Identify Hero",           3, 45,  0, icnnone      , m82none, "Allows the caster to view detailed information on enemy Heroes." },
	{ "Summon Boat",             5, 46,  0, icnnone      , m82none, "Summons the nearest unoccupied, friendly boat to an adjacent shore location.  A friendly boat is one which you just built or were the most recent player to occupy." },
	{ "Dimension Door",         10, 47,  0, icnnone      , m82none, "Allows the caster to magically transport to a nearby location." },
	{ "Town Gate",              10, 48,  0, icnnone      , m82none, "Returns the caster to any town or castle currently owned." },
	{ "Town Portal",            20, 49,  0, icnnone      , m82none, "Returns the hero to the town or castle of choice, provided it is controlled by you." },
	{ "Visions",                 6, 50,  0, icnnone      , m82none, "Visions predicts the likely outcome of an encounter with a neutral army camp." },
	{ "Haunt",                   8, 51,  0, icnnone      , M82::H2MINE, "Haunts a mine you control with Ghosts.  This mine stops producing resources.  (If I can't keep it, nobody will!)" },
	{ "Set Earth Guardian",     15, 52,  4, icnnone      , m82none, "Sets Earth Elementals to guard a mine against enemy armies." },
	{ "Set Air Guardian",       15, 53,  4, icnnone      , m82none, "Sets Air Elementals to guard a mine against enemy armies." },
	{ "Set Fire Guardian",      15, 54,  4, icnnone      , m82none, "Sets Fire Elementals to guard a mine against enemy armies." },
	{ "Set Water Guardian",     15, 55,  4, icnnone      , m82none, "Sets Water Elementals to guard a mine against enemy armies." },
	{ "Stone",     		     0,  0,  0, icnnone      , M82::PARALIZE, "Stone spell from Medusa." },
    };
};

const std::string &Spell::String(spell_t spell)
{ 
//     std::string str = Spell::all_spells[spell].name + " ["; 
//     String::AddInt(str, Mana(spell));
//     str += "]";
    return Spell::all_spells[spell].name;
}

const std::string & Spell::Description(spell_t spell)
{ return Spell::all_spells[spell].description; }

u8 Spell::Mana(spell_t spell)
{
    return Spell::all_spells[spell].mana;
}

Spell::target_t Spell::Target(spell_t spell)
{
    switch(spell)
    {
	case HOLYWORD:
	case HOLYSHOUT:		return ALLDEAD;

	case FIREBALL:
	case FIREBLAST:
	case LIGHTNINGBOLT:
	case CHAINLIGHTNING:
	case SLOW:
	case BLIND:
	case CURSE:
	case ARROW:
	case BERZERKER:
	case PARALYZE:
	case HYPNOTIZE:
	case COLDRAY:
	case DISRUPTINGRAY:	return ONEENEMY;

	case MASSSLOW:
	case MASSCURSE:		return ALLENEMY;

	case DEATHRIPPLE:
	case DEATHWAVE:		return ALLLIVE;

	case TELEPORT:
	case CURE:
	case RESURRECT:
	case RESURRECTTRUE:
	case HASTE:
	case BLESS:
	case MASSBLESS:
	case STONESKIN:
	case STEELSKIN:
	case ANTIMAGIC:
	case DISPEL:
	case DRAGONSLAYER:
	case BLOODLUST:
	case ANIMATEDEAD:
	case MIRRORIMAGE:
	case SHIELD:		return ONEFRIEND;

	case MASSCURE:
	case MASSHASTE:
	case MASSSHIELD:	return ALLFRIEND;

	case COLDRING:
	case SUMMONEELEMENT:
	case SUMMONAELEMENT:
	case SUMMONFELEMENT:
	case SUMMONWELEMENT:	return FREECELL;

	case MASSDISPEL:
	case ARMAGEDDON:
	case ELEMENTALSTORM:
	case METEORSHOWER:
	case EARTHQUAKE:	return ALL;

	default: break;
    }

    return NOTARGET;
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

u8 Spell::Level(u8 spell)
{
    switch(spell)
    {
	case BLESS:
	case BLOODLUST:
	case CURE:
	case CURSE:
	case DISPEL:
	case HASTE:
	case ARROW:
	case SHIELD:
	case SLOW:
	case STONESKIN:

	case VIEWMINES:
	case VIEWRESOURCES:
		return 1;

	case BLIND:
	case COLDRAY:
	case DEATHRIPPLE:
	case DISRUPTINGRAY:
	case DRAGONSLAYER:
	case LIGHTNINGBOLT:
	case STEELSKIN:

	case HAUNT:
	case SUMMONBOAT:
	case VIEWARTIFACTS:
	case VISIONS:
		return 2;

	case ANIMATEDEAD:
	case ANTIMAGIC:
	case COLDRING:
	case DEATHWAVE:
	case EARTHQUAKE:
	case FIREBALL:
	case HOLYWORD:
	case MASSBLESS:
	case MASSCURSE:
	case MASSDISPEL:
	case MASSHASTE:
	case PARALYZE:
	case TELEPORT:

	case IDENTIFYHERO:
	case VIEWHEROES:
	case VIEWTOWNS:
		return 3;

	case BERZERKER:
	case CHAINLIGHTNING:
	case ELEMENTALSTORM:
	case FIREBLAST:
	case HOLYSHOUT:
	case MASSCURE:
	case MASSSHIELD:
	case MASSSLOW:
	case METEORSHOWER:
	case RESURRECT:

	case SETEGUARDIAN:
	case SETAGUARDIAN:
	case SETFGUARDIAN:
	case SETWGUARDIAN:
	case TOWNGATE:
	case VIEWALL:
		return 4;

	case ARMAGEDDON:
	case HYPNOTIZE:
	case MIRRORIMAGE:
	case RESURRECTTRUE:
	case SUMMONEELEMENT:
	case SUMMONAELEMENT:
	case SUMMONFELEMENT:
	case SUMMONWELEMENT:

	case DIMENSIONDOOR:
	case TOWNPORTAL:
		return 5;

	default: break;
    }

    return 0;
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

bool Spell::isCombat(u8 spell)
{
    switch(spell)
    {
	case NONE:
	case VIEWMINES:
	case VIEWRESOURCES:
	case VIEWARTIFACTS:
	case VIEWTOWNS:
	case VIEWHEROES:
	case VIEWALL:
	case IDENTIFYHERO:
	case SUMMONBOAT:
	case DIMENSIONDOOR:
	case TOWNGATE:
	case TOWNPORTAL:
	case VISIONS:
	case HAUNT:
	case SETEGUARDIAN:
	case SETAGUARDIAN:
	case SETFGUARDIAN:
	case SETWGUARDIAN:	return false;
	default: break;
    }
    return true;
}

u8 Spell::GetIndexSprite(u8 spell)
{
    return Spell::all_spells[spell].sprite;
}

u8 Spell::GetInlIndexSprite(u8 spell)
{
    switch(spell)
    {
	case HASTE:
	case MASSHASTE:		return 0;
	case SLOW:
	case MASSSLOW:		return 1;
	case BLIND:		return 2;
	case BLESS:
	case MASSBLESS:		return 3;
	case CURSE:
	case MASSCURSE:		return 4;
	case BERZERKER:		return 5;
	case PARALYZE:		return 6;
	case HYPNOTIZE:		return 7;
	case DRAGONSLAYER:	return 8;
	case BLOODLUST:		return 9;
	case SHIELD:
	case MASSSHIELD:	return 10;
	case STONE:		return 11;
	case ANTIMAGIC:		return 12;
	case STONESKIN:		return 13;
	case STEELSKIN:		return 14;
	default: break;
    }

    return 0;
}

u16 Spell::GetInflictDamage(u8 spell, u8 sp)
{
    switch(spell)
    {
        case DEATHRIPPLE:        return 5 * sp;
        case ARROW:
        case COLDRING:
        case DEATHWAVE:
        case FIREBALL:
        case HOLYWORD:
        case FIREBLAST:          return 10 * sp;
        case COLDRAY:
        case HOLYSHOUT:          return 20 * sp;
        case LIGHTNINGBOLT:
        case ELEMENTALSTORM:
        case METEORSHOWER:       return 25 * sp;
        case CHAINLIGHTNING:     return 40 * sp;
        case ARMAGEDDON:         return 50 * sp;

	default: break;
    }

    return 0;
}

bool Spell::isBad(u8 spell)
{
    switch(spell)
    {
	case FIREBALL:
	case FIREBLAST:
	case LIGHTNINGBOLT:
	case CHAINLIGHTNING:
	case SLOW:
	case MASSSLOW:
	case BLIND:
	case CURSE:
	case MASSCURSE:
	case HOLYWORD:
	case HOLYSHOUT:
	case ARROW:
	case BERZERKER:
	case ARMAGEDDON:
	case ELEMENTALSTORM:
	case METEORSHOWER:
	case PARALYZE:
	case HYPNOTIZE:
	case COLDRAY:
	case COLDRING:
	case DISRUPTINGRAY:
	case DEATHRIPPLE:
	case DEATHWAVE:
		return true;

	default: break;
    }

    return false;
}
