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
#include "config.h"
#include "spell.h"

static S_SPELL	*ptrSpell = NULL;

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

    ptrSpell[EARTHSUMMON].name = "Summon Earth Elemental";
    ptrSpell[EARTHSUMMON].cost = 30;
    
    ptrSpell[AIRSUMMON].name = "Summon Air Elemental";
    ptrSpell[AIRSUMMON].cost = 30;

    ptrSpell[FIRESUMMON].name = "Summon Fire Elemental";
    ptrSpell[FIRESUMMON].cost = 30;

    ptrSpell[WATERSUMMON].name = "Summon Water Elemental";
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




    ptrSpell[FIREBALL].descriptions = "Fireball";
    ptrSpell[FIREBLAST].descriptions = "Fireblast";
    ptrSpell[LIGHTNINGBOLT].descriptions = "Lightning Bolt";
    ptrSpell[CHAINLIGHTNING].descriptions = "Chain Lightning";
    ptrSpell[TELEPORT].descriptions = "Teleport";
    ptrSpell[CURE].descriptions = "Cure";
    ptrSpell[MASSCURE].descriptions = "Mass Cure";
    ptrSpell[RESURRECT].descriptions = "Resurrect";
    ptrSpell[RESURRECTTRUE].descriptions = "Resurrect True";
    ptrSpell[HASTE].descriptions = "Haste";
    ptrSpell[MASSHASTE].descriptions = "Mass Haste";
    ptrSpell[SPELLSLOW].descriptions = "Slow";
    ptrSpell[MASSSLOW].descriptions = "Mass Slow";
    ptrSpell[BLIND].descriptions = "Blind";
    ptrSpell[BLESS].descriptions = "Bless";
    ptrSpell[MASSBLESS].descriptions = "Mass Bless";
    ptrSpell[STONESKIN].descriptions = "Stoneskin";
    ptrSpell[STEELSKIN].descriptions = "Steelskin";
    ptrSpell[CURSE].descriptions = "Curse";
    ptrSpell[MASSCURSE].descriptions = "Mass Curse";
    ptrSpell[HOLYWORD].descriptions = "Holy Word";
    ptrSpell[HOLYSHOUT].descriptions = "Holy Shout";
    ptrSpell[ANTIMAGIC].descriptions = "Anti-Magic";
    ptrSpell[DISPELMAGIC].descriptions = "Dispel Magic";
    ptrSpell[MASSDISPEL].descriptions = "Mass Dispel";
    ptrSpell[MAGICARROW].descriptions = "Magic Arrow";
    ptrSpell[BERZERKER].descriptions = "Berzerker";
    ptrSpell[ARMAGEDDON].descriptions = "Armageddon";
    ptrSpell[ELEMENTALSTORM].descriptions = "Elemental Storm";
    ptrSpell[METEORSHOWER].descriptions = "Meteor Shower";
    ptrSpell[PARALYZE].descriptions = "Paralyze";
    ptrSpell[HYPNOTIZE].descriptions = "Hypnotize";
    ptrSpell[COLDRAY].descriptions = "Cold Ray";
    ptrSpell[COLDRING].descriptions = "Cold Ring";
    ptrSpell[DISRUPTINGRAY].descriptions = "Disrupting Ray";
    ptrSpell[DEATHRIPPLE].descriptions = "Death Ripple";
    ptrSpell[DEATHWAVE].descriptions = "Death Wave";
    ptrSpell[DRAGONSLAYER].descriptions = "Dragon Slayer";
    ptrSpell[BLOODLUST].descriptions = "Blood Lust";
    ptrSpell[ANIMATEDEAD].descriptions = "Animate Dead";
    ptrSpell[MIRRORIMAGE].descriptions = "Mirror Image";
    ptrSpell[SHIELD].descriptions = "Shield";
    ptrSpell[MASSSHIELD].descriptions = "Mass Shield";
    ptrSpell[EARTHSUMMON].descriptions = "Summon Earth Elemental";
    ptrSpell[AIRSUMMON].descriptions = "Summon Air Elemental";
    ptrSpell[FIRESUMMON].descriptions = "Summon Fire Elemental";
    ptrSpell[WATERSUMMON].descriptions = "Summon Water Elemental";
    ptrSpell[EARTHQUAKE].descriptions = "Earthquake";
    ptrSpell[VIEWMINES].descriptions = "View Mines";
    ptrSpell[VIEWRESOURCES].descriptions = "View Resources";
    ptrSpell[VIEWARTIFACTS].descriptions = "View Artifacts";
    ptrSpell[VIEWTOWNS].descriptions = "View Towns";
    ptrSpell[VIEWHEROES].descriptions = "View Heroes";
    ptrSpell[VIEWALL].descriptions = "View All";
    ptrSpell[IDENTIFYHERO].descriptions = "Identify Hero";
    ptrSpell[SUMMONBOAT].descriptions = "Summon Boat";
    ptrSpell[DIMENSIONDOOR].descriptions = "Dimension Door";
    ptrSpell[TOWNGATE].descriptions = "Town Gate";
    ptrSpell[TOWNPORTAL].descriptions = "Town Portal";
    ptrSpell[VISIONS].descriptions = "Visions";
    ptrSpell[HAUNT].descriptions = "Haunt";
    ptrSpell[EARTHGUARDIAN].descriptions = "Set Earth Guardian";
    ptrSpell[AIRGUARDIAN].descriptions = "Set Air Guardian";
    ptrSpell[FIREGUARDIAN].descriptions = "Set Fire Guardian";
    ptrSpell[WATERGUARDIAN].descriptions = "Set Water Guardian";

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

const char * GetStringSpellMagic(E_SPELL spell){

    return ptrSpell[spell].name;
}
