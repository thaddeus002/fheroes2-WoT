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

namespace Spell
{
    static const stats_t all_spells[] = {
	{ "Unknown", "Unknown spell." },
	{ "Fireball", "Causes a giant fireball to strike the selected area, damaging all nearby creatures." },
	{ "Fireblast", "An improved version of fireball, fireblast affects two hexes around the center point of the spell, rather than one." }, 
	{ "Lightning Bolt", "Causes a bolt of electrical energy to strike the selected creature." }, 
	{ "Chain Lightning", "Causes a bolt of electrical energy to strike a selected creature, then strike the nearest creature with half damage, then strike the NEXT nearest creature with half again damage, and so on, until it becomes too weak to be harmful.  Warning:  This spell can hit your own creatures!" }, 
	{ "Teleport", "Teleports the creature you select to any open position on the battlefield." }, 
	{ "Cure", "Removes all negative spells cast upon one of your units, and restores up to 5 HP per level of spell power." }, 
	{ "Mass Cure", "Removes all negative spells cast upon your forces, and restores up to 5 HP per level of spell power, per creature." }, 
	{ "Resurrect", "Resurrects creatures from a damaged or dead unit until end of combat." }, 
	{ "Resurrect True", "Resurrects creatures from a damaged or dead unit permanently." }, 
	{ "Haste", "Increases the speed of any creature by two." }, 
	{ "Mass Haste", "Increases the speed of all of your creatures by two." }, 
	{ "Slow", "Slows target to half movement rate." }, 
	{ "Mass Slow", "Slows all enemies to half movement rate." }, 
	{ "Blind ", "Clouds the affected creatures' eyes, preventing them from moving." }, 
	{ "Bless", "Causes the selected creatures to inflict maximum damage." }, 
	{ "Mass Bless", "Causes all of your units to inflict maximum damage." },
	{ "Stoneskin", "Magically increases the defense skill of the selected creatures." },
	{ "Steelskin", "Increases the defense skill of the targeted creatures.  This is an improved version of Stoneskin." },
	{ "Curse", "Causes the selected creatures to inflict minimum damage." },
	{ "Mass Curse", "Causes all enemy troops to inflict minimum damage." },
	{ "Holy Word", "Damages all undead in the battle." },
	{ "Holy Shout", "Damages all undead in the battle.  This is an improved version of Holy Word." },
	{ "Anti-Magic", "Prevents harmful magic against the selected creatures." },
	{ "Dispel Magic", "Removes all magic spells from a single target." },
	{ "Mass Dispel", "Removes all magic spells from all creatures." },
	{ "Magic Arrow", "Causes a magic arrow to strike the selected target." },
	{ "Berzerker", "Causes a creature to attack its nearest neighbor." },
	{ "Armageddon", "Holy terror strikes the battlefield, causing severe damage to all creatures." },
	{ "Elemental Storm", "Magical elements pour down on the battlefield, damaging all creatures." },
	{ "Meteor Shower", "A rain of rocks strikes an area of the battlefield, damaging all nearby creatures." },
	{ "Paralyze", "The targeted creatures are paralyzed, unable to move or retaliate." },
	{ "Hypnotize", "Brings a single enemy unit under your control for one combat round if its hits are less than 25 times the caster's spell power." },
	{ "Cold Ray", "Drains body heat from a single enemy unit." },
	{ "Cold Ring", "Drains body heat from all units surrounding the center point, but not including the center point." },
	{ "Disrupting Ray", "Reduces the defense rating of an enemy unit by three." },
	{ "Death Ripple", "Damages all living (non-undead) units in the battle." },
	{ "Death Wave", "Damages all living (non-undead) units in the battle.  This spell is an improved version of Death Ripple." },
	{ "Dragon Slayer", "Greatly increases a unit's attack skill vs. Dragons." },
	{ "Blood Lust", "Increases a unit's attack skill." },
	{ "Animate Dead", "Resurrects creatures from a damaged or dead undead unit permanently." },
	{ "Mirror Image", "Creates an illusionary unit that duplicates one of your existing units.  This illusionary unit does the same damages as the original, but will vanish if it takes any damage." },
	{ "Shield", "Halves damage received from ranged attacks for a single unit." },
	{ "Mass Shield", "Halves damage received from ranged attacks for all of your units." },
	{ "Summon Earth Elemental", "Summons Earth Elementals to fight for your army." },
	{ "Summon Air Elemental", "Summons Air Elementals to fight for your army." },
	{ "Summon Fire Elemental", "Summons Fire Elementals to fight for your army." },
	{ "Summon Water Elemental", "Summons Water Elementals to fight for your army." },
	{ "Earthquake", "Damages castle walls." },
	{ "View Mines", "Causes all mines across the land to become visible." },
	{ "View Resources", "Causes all resources across the land to become visible." },
	{ "View Artifacts", "Causes all artifacts across the land to become visible." },
	{ "View Towns", "Causes all towns and castles across the land to become visible." },
	{ "View Heroes", "Causes all Heroes across the land to become visible." },
	{ "View All", "Causes the entire land to become visible." },
	{ "Identify Hero", "Allows the caster to view detailed information on enemy Heroes." },
	{ "Summon Boat", "Summons the nearest unoccupied, friendly boat to an adjacent shore location.  A friendly boat is one which you just built or were the most recent player to occupy." },
	{ "Dimension Door", "Allows the caster to magically transport to a nearby location." },
	{ "Town Gate", "Returns the caster to any town or castle currently owned." },
	{ "Town Portal", "Returns the hero to the town or castle of choice, provided it is controlled by you." },
	{ "Visions", "Visions predicts the likely outcome of an encounter with a neutral army camp." },
	{ "Haunt", "Haunts a mine you control with Ghosts.  This mine stops producing resources.  (If I can't keep it, nobody will!)" },
	{ "Set Earth Guardian", "Sets Earth Elementals to guard a mine against enemy armies." },
	{ "Set Air Guardian", "Sets Air Elementals to guard a mine against enemy armies." },
	{ "Set Fire Guardian", "Sets Fire Elementals to guard a mine against enemy armies." },
	{ "Set Water Guardian", "Sets Water Elementals to guard a mine against enemy armies." }
    };

}

const std::string & Spell::String(spell_t spell)
{ return Spell::all_spells[spell].name; }

const std::string & Spell::Description(spell_t spell)
{ return Spell::all_spells[spell].description; }

u8 Spell::Level(spell_t spell)
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

bool Spell::isCombat(spell_t spell)
{
    switch(spell)
    {
	case VIEWMINES:
	case VIEWRESOURCES:

	case HAUNT:
	case SUMMONBOAT:
	case VIEWARTIFACTS:
	case VISIONS:

	case IDENTIFYHERO:
	case VIEWHEROES:
	case VIEWTOWNS:

	case SETEGUARDIAN:
	case SETAGUARDIAN:
	case SETFGUARDIAN:
	case SETWGUARDIAN:
	case TOWNGATE:
	case VIEWALL:

	case DIMENSIONDOOR:
	case TOWNPORTAL:
	    return false;

	default: break;
    }
    
    return true;
}

u8 Spell::GetIndexSprite(spell_t spell)
{
    switch(spell)
    {
	case SLOW:		return 1;
	case MASSCURE:		return 2;
	case CURSE:		return 3;
	case LIGHTNINGBOLT:	return 4;
	case CHAINLIGHTNING:	return 5;
	case CURE:		return 6;
	case BLESS:		return 7;
	case FIREBALL:		return 8;
	case FIREBLAST:		return 9;
	case TELEPORT:		return 10;
	case ELEMENTALSTORM:	return 11;
	case RESURRECT:		return 12;
	case RESURRECTTRUE:	return 13;
	case HASTE:		return 14;
	case SHIELD:		return 15;
	case ARMAGEDDON:	return 16;
	case ANTIMAGIC:		return 17;
	case DISPEL:		return 18;
	case BERZERKER:		return 19;
	case PARALYZE:		return 20;
	case BLIND:		return 21;
	case HOLYWORD:		return 22;
	case HOLYSHOUT:		return 23;
	case METEORSHOWER:	return 24;
	case ANIMATEDEAD:	return 25;
	case MIRRORIMAGE:	return 26;
	case BLOODLUST:		return 27;
	case DEATHRIPPLE:	return 28;
	case DEATHWAVE:		return 29;
	case STEELSKIN:		return 30;
	case STONESKIN:		return 31;
	case DRAGONSLAYER:	return 32;
	case EARTHQUAKE:	return 33;
	case DISRUPTINGRAY:	return 34;
	case COLDRAY:		return 35;
	case COLDRING:		return 36;
	case HYPNOTIZE:		return 37;
	case ARROW:		return 38;
	case VIEWMINES:		return 39;
	case VIEWRESOURCES:	return 40;
	case VIEWARTIFACTS:	return 41;
	case VIEWTOWNS:		return 42;
	case VIEWHEROES:	return 43;
	case VIEWALL:		return 44;
	case IDENTIFYHERO:	return 45;
	case SUMMONBOAT:	return 46;
	case DIMENSIONDOOR:	return 47;
	case TOWNGATE:		return 48;
	case TOWNPORTAL:	return 49;
	case VISIONS:		return 50;
	case HAUNT:		return 51;
	case SETEGUARDIAN:	return 52;
	case SETAGUARDIAN:	return 53;
	case SETFGUARDIAN:	return 54;
	case SETWGUARDIAN:	return 55;
	case SUMMONEELEMENT:	return 56;
	case SUMMONAELEMENT:	return 57;
	case SUMMONFELEMENT:	return 58;
	case SUMMONWELEMENT:	return 59;
		
	case MASSBLESS:		return 7;
	case MASSCURSE: 	return 3;
	case MASSDISPEL:	return 18;
	case MASSHASTE:		return 14;
	case MASSSHIELD:	return 15;
	case MASSSLOW:		return 1;

	default: break;
    }

    return 0;
}

