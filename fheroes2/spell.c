/*
    freeHeroes2 engine
    turn-based game engine (clone of Heroes Of the Might and Magic II)
    Copyright (C) 2006

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    3DO, New World Computing, Heroes of Might and Magic, and the related
    3DO logos are trademarks and or registered trademarks of 3DO Company.
    All other trademarks belong to their respective owners.

    Web: http://sourceforge.net/projects/fheroes2

    Description:
*/

#include <stdlib.h>
#include "SDL.h"

#include "gamedefs.h"
#include "actionevent.h"
#include "debug.h"
#include "cursor.h"
#include "tools.h"
#include "box.h"
#include "config.h"
#include "spell.h"

static S_SPELL	*ptrSpell = NULL;

static const E_SPELL fightingSpellLevel1[FIGHTINGSPELL_LEVEL1] = {BLESS, BLOODLUST, CURE, CURSE, DISPELMAGIC, HASTE, MAGICARROW, SHIELD, SPELLSLOW, STONESKIN};
static const E_SPELL fightingSpellLevel2[FIGHTINGSPELL_LEVEL2] = {BLIND, COLDRAY, DEATHRIPPLE, DISRUPTINGRAY, DRAGONSLAYER, LIGHTNINGBOLT, STEELSKIN};
static const E_SPELL fightingSpellLevel3[FIGHTINGSPELL_LEVEL3] = {ANIMATEDEAD, ANTIMAGIC, COLDRING, DEATHWAVE, EARTHQUAKE, FIREBALL, HOLYWORD, MASSBLESS, MASSCURSE, MASSDISPEL, MASSHASTE, PARALYZE};
static const E_SPELL fightingSpellLevel4[FIGHTINGSPELL_LEVEL4] = {BERZERKER, CHAINLIGHTNING, ELEMENTALSTORM, FIREBLAST, HOLYSHOUT, MASSCURE, MASSSHIELD, MASSSLOW, METEORSHOWER, RESURRECT};
static const E_SPELL fightingSpellLevel5[FIGHTINGSPELL_LEVEL5] = {ARMAGEDDON, HYPNOTIZE, MIRRORIMAGE, RESURRECTTRUE, EARTHSUMMON, AIRSUMMON, FIRESUMMON, WATERSUMMON};

static const E_SPELL lifeSpellLevel1[LIFESPELL_LEVEL1] = {VIEWMINES, VIEWRESOURCES};
static const E_SPELL lifeSpellLevel2[LIFESPELL_LEVEL2] = {SUMMONBOAT, HAUNT, VISIONS, VIEWARTIFACTS};
static const E_SPELL lifeSpellLevel3[LIFESPELL_LEVEL3] = {TELEPORT, IDENTIFYHERO, VIEWHEROES, VIEWTOWNS};
static const E_SPELL lifeSpellLevel4[LIFESPELL_LEVEL4] = {VIEWALL, TOWNGATE, EARTHGUARDIAN, AIRGUARDIAN, FIREGUARDIAN, WATERGUARDIAN};
static const E_SPELL lifeSpellLevel5[LIFESPELL_LEVEL5] = {DIMENSIONDOOR, TOWNPORTAL};


BOOL	InitSpell(void){

    if(NULL == (ptrSpell = malloc(SPELLMAXCOUNT * sizeof(S_SPELL)))){
	fprintf(stderr, "InitSpel: error malloc: %d\n", SPELLMAXCOUNT * sizeof(S_SPELL));
	return FALSE;
    }
    
    ptrSpell[FIREBALL].name = "Fireball";
    ptrSpell[FIREBALL].cost = 9;

    ptrSpell[FIREBLAST].name = "Fireblast";
    ptrSpell[FIREBLAST].cost = 15;

    ptrSpell[LIGHTNINGBOLT].name = "Lightning Bolt";
    ptrSpell[LIGHTNINGBOLT].cost = 7;

    ptrSpell[CHAINLIGHTNING].name = "Chain Lightning";
    ptrSpell[CHAINLIGHTNING].cost = 15;

    ptrSpell[TELEPORT].name = "Teleport";
    ptrSpell[TELEPORT].cost = 9;

    ptrSpell[CURE].name = "Cure";
    ptrSpell[CURE].cost = 6;

    ptrSpell[MASSCURE].name = "Mass Cure";
    ptrSpell[MASSCURE].cost = 15;

    ptrSpell[RESURRECT].name = "Resurrect";
    ptrSpell[RESURRECT].cost = 12;

    ptrSpell[RESURRECTTRUE].name = "Resurrect True";
    ptrSpell[RESURRECTTRUE].cost = 15;

    ptrSpell[HASTE].name = "Haste";
    ptrSpell[HASTE].cost = 3;

    ptrSpell[MASSHASTE].name = "Mass Haste";
    ptrSpell[MASSHASTE].cost = 10;

    ptrSpell[SPELLSLOW].name = "Slow";
    ptrSpell[SPELLSLOW].cost = 3;

    ptrSpell[MASSSLOW].name = "Mass Slow";
    ptrSpell[MASSSLOW].cost = 15;

    ptrSpell[BLIND].name = "Blind";
    ptrSpell[BLIND].cost = 6;

    ptrSpell[BLESS].name = "Bless";
    ptrSpell[BLESS].cost = 3;

    ptrSpell[MASSBLESS].name = "Mass Bless";
    ptrSpell[MASSBLESS].cost = 12;

    ptrSpell[STONESKIN].name = "Stoneskin";
    ptrSpell[STONESKIN].cost = 3;

    ptrSpell[STEELSKIN].name = "Steelskin";
    ptrSpell[STEELSKIN].cost = 6;

    ptrSpell[CURSE].name = "Curse";
    ptrSpell[CURSE].cost = 3;

    ptrSpell[MASSCURSE].name = "Mass Curse";
    ptrSpell[MASSCURSE].cost = 12;

    ptrSpell[HOLYWORD].name = "Holy Word";
    ptrSpell[HOLYWORD].cost = 9;

    ptrSpell[HOLYSHOUT].name = "Holy Shout";
    ptrSpell[HOLYSHOUT].cost = 12;

    ptrSpell[ANTIMAGIC].name = "Anti-Magic";
    ptrSpell[ANTIMAGIC].cost = 7;

    ptrSpell[DISPELMAGIC].name = "Dispel Magic";
    ptrSpell[DISPELMAGIC].cost = 5;

    ptrSpell[MASSDISPEL].name = "Mass Dispel";
    ptrSpell[MASSDISPEL].cost = 12;

    ptrSpell[MAGICARROW].name = "Magic Arrow";
    ptrSpell[MAGICARROW].cost = 3;

    ptrSpell[BERZERKER].name = "Berzerker";
    ptrSpell[BERZERKER].cost = 12;

    ptrSpell[ARMAGEDDON].name = "Armageddon";
    ptrSpell[ARMAGEDDON].cost = 20;

    ptrSpell[ELEMENTALSTORM].name = "Elemental Storm";
    ptrSpell[ELEMENTALSTORM].cost =15;

    ptrSpell[METEORSHOWER].name = "Meteor Shower";
    ptrSpell[METEORSHOWER].cost = 15;

    ptrSpell[PARALYZE].name = "Paralyze";
    ptrSpell[PARALYZE].cost = 9;

    ptrSpell[HYPNOTIZE].name = "Hypnotize";
    ptrSpell[HYPNOTIZE].cost = 15;

    ptrSpell[COLDRAY].name = "Cold Ray";
    ptrSpell[COLDRAY].cost = 6;

    ptrSpell[COLDRING].name = "Cold Ring";
    ptrSpell[COLDRING].cost = 9;

    ptrSpell[DISRUPTINGRAY].name = "Disrupting Ray";
    ptrSpell[DISRUPTINGRAY].cost = 7;

    ptrSpell[DEATHRIPPLE].name = "Death Ripple";
    ptrSpell[DEATHRIPPLE].cost = 6;

    ptrSpell[DEATHWAVE].name = "Death Wave";
    ptrSpell[DEATHWAVE].cost = 10;

    ptrSpell[DRAGONSLAYER].name = "Dragon Slayer";
    ptrSpell[DRAGONSLAYER].cost = 6;

    ptrSpell[BLOODLUST].name = "Blood Lust";
    ptrSpell[BLOODLUST].cost = 3;

    ptrSpell[ANIMATEDEAD].name = "Animate Dead";
    ptrSpell[ANIMATEDEAD].cost = 10;

    ptrSpell[MIRRORIMAGE].name = "Mirror Image";
    ptrSpell[MIRRORIMAGE].cost = 25;

    ptrSpell[SHIELD].name = "Shield";
    ptrSpell[SHIELD].cost = 3;

    ptrSpell[MASSSHIELD].name = "Mass Shield";
    ptrSpell[MASSSHIELD].cost = 7;

    ptrSpell[EARTHSUMMON].name = "Summon Earth El.";
    ptrSpell[EARTHSUMMON].cost = 30;
    
    ptrSpell[AIRSUMMON].name = "Summon Air El.";
    ptrSpell[AIRSUMMON].cost = 30;

    ptrSpell[FIRESUMMON].name = "Summon Fire El.";
    ptrSpell[FIRESUMMON].cost = 30;

    ptrSpell[WATERSUMMON].name = "Summon Water El.";
    ptrSpell[WATERSUMMON].cost = 30;

    ptrSpell[EARTHQUAKE].name = "Earthquake";
    ptrSpell[EARTHQUAKE].cost = 15;

    ptrSpell[VIEWMINES].name = "View Mines";
    ptrSpell[VIEWMINES].cost = 1;

    ptrSpell[VIEWRESOURCES].name = "View Resources";
    ptrSpell[VIEWRESOURCES].cost = 1;

    ptrSpell[VIEWARTIFACTS].name = "View Artifacts";
    ptrSpell[VIEWARTIFACTS].cost = 2;

    ptrSpell[VIEWTOWNS].name = "View Towns";
    ptrSpell[VIEWTOWNS].cost = 2;

    ptrSpell[VIEWHEROES].name = "View Heroes";
    ptrSpell[VIEWHEROES].cost = 2;

    ptrSpell[VIEWALL].name = "View All";
    ptrSpell[VIEWALL].cost = 3;

    ptrSpell[IDENTIFYHERO].name = "Identify Hero";
    ptrSpell[IDENTIFYHERO].cost = 3;

    ptrSpell[SUMMONBOAT].name = "Summon Boat";
    ptrSpell[SUMMONBOAT].cost = 5;

    ptrSpell[DIMENSIONDOOR].name = "Dimension Door";
    ptrSpell[DIMENSIONDOOR].cost = 10;

    ptrSpell[TOWNGATE].name = "Town Gate";
    ptrSpell[TOWNGATE].cost = 10;

    ptrSpell[TOWNPORTAL].name = "Town Portal";
    ptrSpell[TOWNPORTAL].cost = 20;

    ptrSpell[VISIONS].name = "Visions";
    ptrSpell[VISIONS].cost = 6;

    ptrSpell[HAUNT].name = "Haunt";
    ptrSpell[HAUNT].cost = 8;

    ptrSpell[EARTHGUARDIAN].name = "Earth Guardian";
    ptrSpell[EARTHGUARDIAN].cost = 15;
    
    ptrSpell[AIRGUARDIAN].name = "Air Guardian";
    ptrSpell[AIRGUARDIAN].cost = 15;

    ptrSpell[FIREGUARDIAN].name = "Fire Guardian";
    ptrSpell[FIREGUARDIAN].cost = 15;

    ptrSpell[WATERGUARDIAN].name = "Water Guardian";
    ptrSpell[WATERGUARDIAN].cost = 15;


    ptrSpell[FIREBALL].descriptions = "Causes a giant fireball to strike the selected area, damaging all nearby creatures.";
    ptrSpell[FIREBLAST].descriptions = "An improved version of fireball, fireblast affects two hexes around the center point of the spell, rather than one.";
    ptrSpell[LIGHTNINGBOLT].descriptions = "Causes a bolt of electrical energy to strike the selected creature.";
    ptrSpell[CHAINLIGHTNING].descriptions = "Causes a bolt of electrical energy to strike a selected creature, then strike the nearest creature with half damage, then strike the NEXT nearest creature with half again damage, and so on, until it becomes too weak to be harmful.  Warning:  This spell can hit your own creatures!";
    ptrSpell[TELEPORT].descriptions = "Teleports the creature you select to any open position on the battlefield.";
    ptrSpell[CURE].descriptions = "Removes all negative spells cast upon one of your units, and restores up to 5 HP per level of spell power.";
    ptrSpell[MASSCURE].descriptions = "Removes all negative spells cast upon your forces, and restores up to 5 HP per level of spell power, per creature.";
    ptrSpell[RESURRECT].descriptions = "Resurrects creatures from a damaged or dead unit until end of combat.";
    ptrSpell[RESURRECTTRUE].descriptions = "Resurrects creatures from a damaged or dead unit permanently.";
    ptrSpell[HASTE].descriptions = "Increases the speed of any creature by two.";
    ptrSpell[MASSHASTE].descriptions = "Increases the speed of all of your creatures by two.";
    ptrSpell[SPELLSLOW].descriptions = "Slows target to half movement rate.";
    ptrSpell[MASSSLOW].descriptions = "Slows all enemies to half movement rate.";
    ptrSpell[BLIND].descriptions = "Clouds the affected creatures' eyes, preventing them from moving.";
    ptrSpell[BLESS].descriptions = "Causes the selected creatures to inflict maximum damage.";
    ptrSpell[MASSBLESS].descriptions = "Causes all of your units to inflict maximum damage.";
    ptrSpell[STONESKIN].descriptions = "Magically increases the defense skill of the selected creatures.";
    ptrSpell[STEELSKIN].descriptions = "Increases the defense skill of the targeted creatures.  This is an improved version of Stoneskin.";
    ptrSpell[CURSE].descriptions = "Causes the selected creatures to inflict minimum damage.";
    ptrSpell[MASSCURSE].descriptions = "Causes all enemy troops to inflict minimum damage.";
    ptrSpell[HOLYWORD].descriptions = "Damages all undead in the battle.";
    ptrSpell[HOLYSHOUT].descriptions = "Damages all undead in the battle.  This is an improved version of Holy Word.";
    ptrSpell[ANTIMAGIC].descriptions = "Prevents harmful magic against the selected creatures.";
    ptrSpell[DISPELMAGIC].descriptions = "Removes all magic spells from a single target.";
    ptrSpell[MASSDISPEL].descriptions = "Removes all magic spells from all creatures.";
    ptrSpell[MAGICARROW].descriptions = "Causes a magic arrow to strike the selected target.";
    ptrSpell[BERZERKER].descriptions = "Causes a creature to attack its nearest neighbor.";
    ptrSpell[ARMAGEDDON].descriptions = "Holy terror strikes the battlefield, causing severe damage to all creatures.";
    ptrSpell[ELEMENTALSTORM].descriptions = "Magical elements pour down on the battlefield, damaging all creatures.";
    ptrSpell[METEORSHOWER].descriptions = "A rain of rocks strikes an area of the battlefield, damaging all nearby creatures.";
    ptrSpell[PARALYZE].descriptions = "The targeted creatures are paralyzed, unable to move or retaliate.";
    ptrSpell[HYPNOTIZE].descriptions = "Brings a single enemy unit under your control for one combat round if its hits are less than 25 times the caster's spell power.";
    ptrSpell[COLDRAY].descriptions = "Drains body heat from a single enemy unit.";
    ptrSpell[COLDRING].descriptions = "Drains body heat from all units surrounding the center point, but not including the center point.";
    ptrSpell[DISRUPTINGRAY].descriptions = "Reduces the defense rating of an enemy unit by three.";
    ptrSpell[DEATHRIPPLE].descriptions = "Damages all living (non-undead) units in the battle.";
    ptrSpell[DEATHWAVE].descriptions = "Damages all living (non-undead) units in the battle.  This spell is an improved version of Death Ripple.";
    ptrSpell[DRAGONSLAYER].descriptions = "Greatly increases a unit's attack skill vs. Dragons.";
    ptrSpell[BLOODLUST].descriptions = "Increases a unit's attack skill.";
    ptrSpell[ANIMATEDEAD].descriptions = "'Resurrects' creatures from a damaged or dead undead unit permanently.";
    ptrSpell[MIRRORIMAGE].descriptions = "Creates an illusionary unit that duplicates one of your existing units.  This illusionary unit does the same damages as the original, but will vanish if it takes any damage.";
    ptrSpell[SHIELD].descriptions = "Halves damage received from ranged attacks for a single unit.";
    ptrSpell[MASSSHIELD].descriptions = "Halves damage received from ranged attacks for all of your units.";
    ptrSpell[EARTHSUMMON].descriptions = "Summons Earth Elementals to fight for your army.";
    ptrSpell[AIRSUMMON].descriptions = "Summons Air Elementals to fight for your army.";
    ptrSpell[FIRESUMMON].descriptions = "Summons Fire Elementals to fight for your army.";
    ptrSpell[WATERSUMMON].descriptions = "Summons Water Elementals to fight for your army.";
    ptrSpell[EARTHQUAKE].descriptions = "Damages castle walls.";
    ptrSpell[VIEWMINES].descriptions = "Causes all mines across the land to become visible.";
    ptrSpell[VIEWRESOURCES].descriptions = "Causes all resources across the land to become visible.";
    ptrSpell[VIEWARTIFACTS].descriptions = "Causes all artifacts across the land to become visible.";
    ptrSpell[VIEWTOWNS].descriptions = "Causes all towns and castles across the land to become visible.";
    ptrSpell[VIEWHEROES].descriptions = "Causes all Heroes across the land to become visible.";
    ptrSpell[VIEWALL].descriptions = "Causes the entire land to become visible.";
    ptrSpell[IDENTIFYHERO].descriptions = "Allows the caster to view detailed information on enemy Heroes.";
    ptrSpell[SUMMONBOAT].descriptions = "Summons the nearest unoccupied, friendly boat to an adjacent shore location.  A friendly boat is one which you just built or were the most recent player to occupy.";
    ptrSpell[DIMENSIONDOOR].descriptions = "Allows the caster to magically transport to a nearby location.";
    ptrSpell[TOWNGATE].descriptions = "Returns the caster to any town or castle currently owned.";
    ptrSpell[TOWNPORTAL].descriptions = "Returns the hero to the town or castle of choice, provided it is controlled by you.";
    ptrSpell[VISIONS].descriptions = "Visions predicts the likely outcome of an encounter with a neutral army camp.";
    ptrSpell[HAUNT].descriptions = "Haunts a mine you control with Ghosts.  This mine stops producing resources.  (If I can't keep it, nobody will!)";
    ptrSpell[EARTHGUARDIAN].descriptions = "Sets Earth Elementals to guard a mine against enemy armies.";
    ptrSpell[AIRGUARDIAN].descriptions = "Sets Air Elementals to guard a mine against enemy armies.";
    ptrSpell[FIREGUARDIAN].descriptions = "Sets Fire Elementals to guard a mine against enemy armies.";
    ptrSpell[WATERGUARDIAN].descriptions = "Sets Water Elementals to guard a mine against enemy armies.";

    fprintf(stderr, "Init spell.\n");
    
    return TRUE;
}

void	FreeSpell(void){
    
    if(ptrSpell) free(ptrSpell);
}

E_MAGICLEVEL GetLevelSpellMagic(E_SPELL spell){

    switch(spell){

	case BLESS:
	case BLOODLUST:
	case CURE:
	case CURSE:
	case DISPELMAGIC:
	case HASTE:
	case MAGICARROW:
	case SHIELD:
	case SPELLSLOW:
	case STONESKIN:
	case VIEWMINES:
	case VIEWRESOURCES:
	    return MAGIC_LEVEL1;
	    break;
	
	case BLIND:
	case COLDRAY:
	case DEATHRIPPLE:
	case DISRUPTINGRAY:
	case DRAGONSLAYER:
	case LIGHTNINGBOLT:
	case STEELSKIN:
	case SUMMONBOAT:
	case HAUNT:
	case VISIONS:
	case VIEWARTIFACTS:
	    return MAGIC_LEVEL2;
	    break;
	
	
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
	    return MAGIC_LEVEL3;
	    break;

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
	case VIEWALL:
	case TOWNGATE:
	case EARTHGUARDIAN:
	case AIRGUARDIAN:
	case FIREGUARDIAN:
	case WATERGUARDIAN:
	    return MAGIC_LEVEL4;
	    break;
	
	case ARMAGEDDON:
	case HYPNOTIZE:
	case MIRRORIMAGE:
	case RESURRECTTRUE:
	case EARTHSUMMON:
	case AIRSUMMON:
	case FIRESUMMON:
	case WATERSUMMON:
	case DIMENSIONDOOR:
	case TOWNPORTAL:
	    return MAGIC_LEVEL5;
	    break;
	
	case SPELLNONE:
	    break;
    }
    
    if(GetIntValue(DEBUG)) fprintf(stderr, "GetLevelSpellMagic: unknown spell: %d\n", spell);
    
    return MAGIC_NONE;
}

const char * GetStringNameSpellMagic(E_SPELL spell){

    return ptrSpell[spell].name;
}

const char * GetStringDescriptionsSpellMagic(E_SPELL spell){

    return ptrSpell[spell].descriptions;
}

Uint8 GetCostSpellMagic(E_SPELL spell){

    return ptrSpell[spell].cost;
}

void FillSpellLevel1(E_RACE race, E_SPELL *spell){

    switch(race){

	case KNIGHT:
	case BARBARIAN:
	case SORCERESS:
	case WARLOCK:
	    spell[0] = fightingSpellLevel1[rand() % FIGHTINGSPELL_LEVEL1];
	    spell[1] = fightingSpellLevel1[rand() % FIGHTINGSPELL_LEVEL1];
	    while(spell[1] == spell[0]) spell[1] = fightingSpellLevel1[rand() % FIGHTINGSPELL_LEVEL1];
	    spell[2] = lifeSpellLevel1[rand() % LIFESPELL_LEVEL1];
	    break;
	
	case NECROMANCER:
	    spell[0] = fightingSpellLevel1[rand() % FIGHTINGSPELL_LEVEL1];
	    while(BLESS == spell[0] || CURE == spell[0]) spell[0] = fightingSpellLevel1[rand() % FIGHTINGSPELL_LEVEL1];
	    spell[1] = fightingSpellLevel1[rand() % FIGHTINGSPELL_LEVEL1];
	    while(BLESS == spell[0] || CURE == spell[0] || spell[1] == spell[0]) spell[1] = fightingSpellLevel1[rand() % FIGHTINGSPELL_LEVEL1];
	    spell[2] = lifeSpellLevel1[rand() % LIFESPELL_LEVEL1];
	    break;
	
	case WIZARD:
	    if(spell[0] == SPELLNONE && spell[1] == SPELLNONE && spell[2] == SPELLNONE){
		spell[0] = fightingSpellLevel1[rand() % FIGHTINGSPELL_LEVEL1];
		spell[1] = fightingSpellLevel1[rand() % FIGHTINGSPELL_LEVEL1];
		while(spell[1] == spell[0]) spell[1] = fightingSpellLevel1[rand() % FIGHTINGSPELL_LEVEL1];
		spell[2] = lifeSpellLevel1[rand() % LIFESPELL_LEVEL1];
	    }else{
		spell[3] = spell[2];
		spell[2] = fightingSpellLevel1[rand() % FIGHTINGSPELL_LEVEL1];
		while(spell[2] == spell[0] || spell[2] == spell[1]) spell[2] = fightingSpellLevel1[rand() % FIGHTINGSPELL_LEVEL1];
	    }
	    break;

	default:
	    return;
	    break;
    }
}

void FillSpellLevel2(E_RACE race, E_SPELL *spell){

    switch(race){

	case KNIGHT:
	case BARBARIAN:
	case SORCERESS:
	case WARLOCK:
	    spell[0] = fightingSpellLevel2[rand() % FIGHTINGSPELL_LEVEL2];
	    while(DEATHRIPPLE == spell[0]) spell[0] = fightingSpellLevel2[rand() % FIGHTINGSPELL_LEVEL2];
	    spell[1] = fightingSpellLevel2[rand() % FIGHTINGSPELL_LEVEL2];
	    while(DEATHRIPPLE == spell[1] || spell[0] == spell[1]) spell[1] = fightingSpellLevel2[rand() % FIGHTINGSPELL_LEVEL2];
	    spell[2] = lifeSpellLevel2[rand() % LIFESPELL_LEVEL2];
	    break;
	    
	case NECROMANCER:
	    spell[0] = DEATHRIPPLE;
	    spell[1] = fightingSpellLevel2[rand() % FIGHTINGSPELL_LEVEL2];
	    while(spell[0] == spell[1]) spell[1] = fightingSpellLevel2[rand() % FIGHTINGSPELL_LEVEL2];
	    spell[2] = lifeSpellLevel2[rand() % LIFESPELL_LEVEL2];
	    break;
	
	case WIZARD:
	    if(spell[0] == SPELLNONE && spell[1] == SPELLNONE && spell[2] == SPELLNONE){
		spell[0] = fightingSpellLevel2[rand() % FIGHTINGSPELL_LEVEL2];
		while(DEATHRIPPLE == spell[0]) spell[0] = fightingSpellLevel2[rand() % FIGHTINGSPELL_LEVEL2];
		spell[1] = fightingSpellLevel2[rand() % FIGHTINGSPELL_LEVEL2];
		while(DEATHRIPPLE == spell[1] || spell[0] == spell[1]) spell[1] = fightingSpellLevel2[rand() % FIGHTINGSPELL_LEVEL2];
		spell[2] = lifeSpellLevel2[rand() % LIFESPELL_LEVEL2];
	    }else{
		spell[3] = spell[2];
		spell[2] = fightingSpellLevel2[rand() % FIGHTINGSPELL_LEVEL2];
		while(DEATHRIPPLE == spell[2] || spell[0] == spell[2] || spell[1] == spell[2]) spell[2] = fightingSpellLevel2[rand() % FIGHTINGSPELL_LEVEL2];
	    }
	    break;

	default:
	    return;
	    break;
    }
}

void FillSpellLevel3(E_RACE race, E_SPELL *spell){

    switch(race){

	case KNIGHT:
	case BARBARIAN:
	case SORCERESS:
	case WARLOCK:
	    spell[0] = fightingSpellLevel3[rand() % FIGHTINGSPELL_LEVEL3];
	    while(DEATHWAVE == spell[0] || ANIMATEDEAD == spell[0]) spell[0] = fightingSpellLevel3[rand() % FIGHTINGSPELL_LEVEL3];
	    spell[1] = lifeSpellLevel3[rand() % LIFESPELL_LEVEL3];
	    break;
	    
	case NECROMANCER:
	    spell[0] = fightingSpellLevel3[rand() % FIGHTINGSPELL_LEVEL3];
	    while(HOLYWORD == spell[0] || MASSBLESS == spell[0]) spell[0] = fightingSpellLevel3[rand() % FIGHTINGSPELL_LEVEL3];
	    spell[1] = lifeSpellLevel3[rand() % LIFESPELL_LEVEL3];
	    break;
	
	case WIZARD:
	    if(spell[0] == SPELLNONE && spell[1] == SPELLNONE){
		spell[0] = fightingSpellLevel3[rand() % FIGHTINGSPELL_LEVEL3];
		while(DEATHWAVE == spell[0] || ANIMATEDEAD == spell[0]) spell[0] = fightingSpellLevel3[rand() % FIGHTINGSPELL_LEVEL3];
		spell[1] = lifeSpellLevel3[rand() % LIFESPELL_LEVEL3];
	    }else{
		spell[2] = spell[1];
		spell[1] = fightingSpellLevel3[rand() % FIGHTINGSPELL_LEVEL3];
		while(DEATHWAVE == spell[1] || ANIMATEDEAD == spell[1] || spell[0] == spell[1]) spell[1] = fightingSpellLevel3[rand() % FIGHTINGSPELL_LEVEL3];
	    }
	    break;
	
	default:
	    return;
	    break;
    }
}

void FillSpellLevel4(E_RACE race, E_SPELL *spell){

    switch(race){

	case KNIGHT:
	case BARBARIAN:
	case SORCERESS:
	case WARLOCK:
	    spell[0] = fightingSpellLevel4[rand() % FIGHTINGSPELL_LEVEL4];
	    spell[1] = lifeSpellLevel4[rand() % LIFESPELL_LEVEL4];
	    break;
	    
	
	case NECROMANCER:
	    spell[0] = fightingSpellLevel4[rand() % FIGHTINGSPELL_LEVEL4];
	    while(MASSCURE == spell[0] || HOLYSHOUT == spell[0]) spell[0] = fightingSpellLevel4[rand() % FIGHTINGSPELL_LEVEL4];
	    spell[1] = lifeSpellLevel4[rand() % LIFESPELL_LEVEL4];
	    break;
	
	
	case WIZARD:
	    if(spell[0] == SPELLNONE && spell[1] == SPELLNONE){
		spell[0] = fightingSpellLevel4[rand() % FIGHTINGSPELL_LEVEL4];
		spell[1] = lifeSpellLevel4[rand() % LIFESPELL_LEVEL4];
	    }else{
		spell[2] = spell[1];
		spell[1] = fightingSpellLevel4[rand() % FIGHTINGSPELL_LEVEL4];
		while(spell[0] == spell[1]) spell[1] = fightingSpellLevel4[rand() % FIGHTINGSPELL_LEVEL4];
	    }
	    break;
	
	default:
	    return;
	    break;
    }
}

void FillSpellLevel5(E_RACE race, E_SPELL *spell){

    switch(race){

	case KNIGHT:
	case BARBARIAN:
	case SORCERESS:
	case WARLOCK:
	    spell[0] = fightingSpellLevel5[rand() % FIGHTINGSPELL_LEVEL5];
	    break;
	    
	case NECROMANCER:
	    spell[0] = fightingSpellLevel5[rand() % FIGHTINGSPELL_LEVEL5];
	    break;
	
	case WIZARD:
	    if(spell[0] == SPELLNONE)
		spell[0] = fightingSpellLevel5[rand() % FIGHTINGSPELL_LEVEL5];
	    else
		spell[1] = lifeSpellLevel5[rand() % LIFESPELL_LEVEL5];
	    break;

	default:
	    return;
	    break;
    }
}

ACTION ShowSpellInfo(E_SPELL spell, Uint32 flag){
    
    CursorOff();
        
    SDL_Surface *image = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();

    SDL_Rect dst;
    SDL_Event event;

    ACTION result = NONE;
    INTERFACEACTION *dialog = NULL;
    AGGSPRITE sprite;
    BOOL exit = FALSE;
            
    S_BOX box;
    Uint16 height = 0;

    if(MASSSHIELD == spell) FillSPRITE(&sprite, "SPELLS.ICN", SHIELD);
    else if(MASSSLOW == spell) FillSPRITE(&sprite, "SPELLS.ICN", SLOW);
    else if(MASSBLESS == spell) FillSPRITE(&sprite, "SPELLS.ICN", BLESS);
    else if(MASSCURSE == spell) FillSPRITE(&sprite, "SPELLS.ICN", CURSE);
    else if(MASSDISPEL == spell) FillSPRITE(&sprite, "SPELLS.ICN", DISPELMAGIC);
    else if(MASSHASTE == spell) FillSPRITE(&sprite, "SPELLS.ICN", HASTE);
    else FillSPRITE(&sprite, "SPELLS.ICN", spell);
    image = GetICNSprite(&sprite);
    
    height += GetHeightText(GetStringNameSpellMagic(spell), FONT_BIG);
    height += 20;
    height += GetHeightText(GetStringDescriptionsSpellMagic(spell), FONT_BIG);
    height += 60;
    
    if( ! InitBox(&box, height, &dialog, flag)) return NONE;

    // text header
    dst = box.rectArea;
    PrintAlignText(video, &dst, GetStringNameSpellMagic(spell), FONT_BIG);

    // text info
    dst.y += GetHeightText(GetStringNameSpellMagic(spell), FONT_BIG);
    dst.y += 20;
    PrintAlignText(video, &dst, GetStringDescriptionsSpellMagic(spell), FONT_BIG);
    
    // sprite
    dst.y += GetHeightText(GetStringDescriptionsSpellMagic(spell), FONT_BIG);
    dst.y += 20;
    dst.y += ((box.rectArea.y + box.rectArea.h - dst.y) - image->h) / 2;
    dst.x = (video->w - image->w) / 2 + 5;
    dst.w = image->w;
    dst.h = image->h;
    SDL_BlitSurface(image, NULL, video, &dst);
    
    SDL_Flip(video);

    CursorOn();
        
    if(flag)
        while(! exit)
            switch(ActionCycle(dialog, NULL)){
                case EXIT:
                    exit = TRUE;
		    result = EXIT;
                    break;
        
                case ESC:
                case OK:
                case ENTER:
                    exit = TRUE;
                    result = NONE;
                    break;

                default:
                    break;

            }
    else
        while(! exit){
            while(SDL_PollEvent(&event))
                if( SDL_BUTTON_RIGHT == event.button.button && SDL_RELEASED == event.button.state) exit = TRUE;

            if(GetIntValue(CYCLELOOP)) SDL_Delay(CYCLEDELAY * 10);
        }

    CursorOff();

    FreeBox(&box);

    CursorOn();

    return result;
}
