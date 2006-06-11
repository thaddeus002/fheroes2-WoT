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

#include "SDL.h"

#include "heroes.h"

BOOL    HeroesArtifactPresent(S_HEROES *heroes, E_ARTIFACT artifact){

    if(NULL == heroes) return FALSE;

    Uint8 i;

    for(i = 0; i < HEROESMAXARTIFACT; ++i)
	if(artifact == heroes->artifact[i]) return TRUE;

    return FALSE;
}

Uint8   CalculationHeroesAttack(S_HEROES *heroes){

    if(NULL == heroes) return 0;

    Uint8 result = heroes->attack;

    Uint8 i;

    for(i = 0; i < HEROESMAXARTIFACT; ++i)

	switch(heroes->artifact[i]){

	    case SPIKED_HELM:
	    case THUNDER_MACE:
	    case GIANT_FLAIL:
		result += 1;
		break;
	    
	    case SPIKED_SHIELD:
	    case POWER_AXE:
		result += 2;
		break;
		
	    case DRAGON_SWORD:
		result += 3;
		break;

	    case ULTIMATE_CROWN:
		result += 4;
		break;
		
	    case ULTIMATE_SHIELD:
		result += 6;
		break;
		
	    case ULTIMATE_SWORD:
		result += 12;
		break;

	    default:
		break;
	}

    return result;
}

Uint8   CalculationHeroesDefence(S_HEROES *heroes){

    if(NULL == heroes) return 0;

    Uint8 result = heroes->defence;

    Uint8 i;

    for(i = 0; i < HEROESMAXARTIFACT; ++i)

	switch(heroes->artifact[i]){

	    case SPIKED_HELM:
	    case ARMORED_GAUNTLETS:
	    case DEFENDER_HELM:
		result += 1;
		break;
	    
	    case SPIKED_SHIELD:
	    case STEALTH_SHIELD:
		result += 2;
		break;
		
	    case DIVINE_BREASTPLATE:
		result += 3;
		break;

	    case ULTIMATE_CROWN:
		result += 4;
		break;
		
	    case ULTIMATE_SHIELD:
		result += 6;
		break;
		
	    case ULTIMATE_CLOAK:
		result += 12;
		break;

	    default:
		break;
	}

    return result;
}

Uint8   CalculationHeroesPower(S_HEROES *heroes){

    if(NULL == heroes) return 0;

    Uint8 result = heroes->power;

    Uint8 i;

    for(i = 0; i < HEROESMAXARTIFACT; ++i)

	switch(heroes->artifact[i]){

	    case WHITE_PEARL:
		result += 1;
		break;
	    
	    case BLACK_PEARL:
	    case CASTER_BRACELET:
	    case MAGE_RING:
		result += 2;
		break;
		
	    case WITCHES_BROACH:
		result += 3;
		break;

	    case ULTIMATE_CROWN:
	    case ARCANE_NECKLACE:
		result += 4;
		break;
		
	    case ULTIMATE_STAFF:
		result += 6;
		break;
		
	    case ULTIMATE_WAND:
		result += 12;
		break;

	    default:
		break;
	}

    return result;
}

Uint8	CalculationHeroesKnowledge(S_HEROES *heroes){

    if(NULL == heroes) return 0;

    Uint8 result = heroes->power;

    Uint8 i;

    for(i = 0; i < HEROESMAXARTIFACT; ++i)

	switch(heroes->artifact[i]){

	    case WHITE_PEARL:

		result += 1;
		break;
	    
	    case BLACK_PEARL:
	    case MINOR_SCROLL:
		result += 2;
		break;
		
	    case MAJOR_SCROLL:
		result += 3;
		break;

	    case ULTIMATE_CROWN:
	    case SUPERIOR_SCROLL:
		result += 4;
		break;

	    case FOREMOST_SCROLL:
		result += 5;
		break;
		
	    case ULTIMATE_STAFF:
		result += 6;
		break;
		
	    case ULTIMATE_BOOK:
		result += 12;
		break;

	    default:
		break;
	}

    return result;
}

E_MORALE CalculationHeroesMorale(S_HEROES *heroes){

    if(NULL == heroes) return MORALE_NORMAL;

    E_MORALE result = heroes->morale;

    Uint8 i;

    for(i = 0; i < HEROESMAXARTIFACT; ++i)

	switch(heroes->artifact[i]){

	    case MEDAL_VALOR:
	    case MEDAL_COURAGE:
	    case MEDAL_HONOR:
	    case MEDAL_DISTINCTION:
		if(MORALE_IRISH > result++);
		break;

	    case FIZBIN_MISFORTUNE:
		if(MORALE_TREASON < result--);
		break;

	    default:
		break;
	}

    return result;
}

E_LUCK  CalculationHeroesLuck(S_HEROES *heroes){

    if(NULL == heroes) return LUCK_NORMAL;

    E_LUCK result = heroes->luck;

    Uint8 i;

    for(i = 0; i < HEROESMAXARTIFACT; ++i)

	switch(heroes->artifact[i]){

	    case RABBIT_FOOT:
	    case GOLDEN_HORSESHOE:
	    case GAMBLER_LUCKY_COIN:
	    case FOUR_LEAF_CLOVER:
		if(LUCK_IRISH > result++);
		break;

	    default:
		break;
	}

    return result;
}

Uint32  CalculationHeroesExperience(S_HEROES *heroes){

    return 0;
}

Uint16  CalculationHeroesMagicPoint(S_HEROES *heroes){

    return 0;
}

Uint8   CalculationHeroesMovementPoint(S_HEROES *heroes){

    return 0;
}
