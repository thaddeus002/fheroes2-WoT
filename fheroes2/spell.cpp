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

namespace Spell
{
    static const stats_t all_spells[] = {
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
