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

#include "monster.h"
#include "heroes.h"

E_LEVELSKILL HeroesLevelSkill(S_HEROES *heroes, E_SKILL skill){

    if(NULL == heroes || SNONE == skill) return LNONE;

    Uint8 i;

    for(i = 0; i < HEROESMAXSKILL; ++i)
	if(skill == heroes->skill[i].type) return heroes->skill[i].level;

    return LNONE;
}

BOOL    HeroesArtifactPresent(S_HEROES *heroes, E_ARTIFACT artifact){

    if(NULL == heroes) return FALSE;

    Uint8 i;

    for(i = 0; i < HEROESMAXARTIFACT; ++i)
	if(artifact == heroes->artifact[i]) return TRUE;

    return FALSE;
}

Uint8   CalculateHeroesAttack(S_HEROES *heroes){

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

Uint8   CalculateHeroesDefence(S_HEROES *heroes){

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

Uint8   CalculateHeroesPower(S_HEROES *heroes){

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

Uint8	CalculateHeroesKnowledge(S_HEROES *heroes){

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

E_MORALE CalculateHeroesMorale(S_HEROES *heroes){

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
		if(MORALE_AWFUL < result) result-=2;
		else result = MORALE_TREASON;
		break;

	    default:
		break;
	}

    // bonus leadership
    switch(HeroesLevelSkill(heroes, LEADERSHIP)){
    
	case EXPERT:
	    if(MORALE_IRISH > result++);
	    if(MORALE_IRISH > result++);
	    if(MORALE_IRISH > result++);
	    break;
	    
	case ADVANCED:
	    if(MORALE_IRISH > result++);
	    if(MORALE_IRISH > result++);
	    break;
	    
	case BASIC:
	    if(MORALE_IRISH > result++);
	    break;
	    
	default:
	    break;
    }

    return result;
}

E_LUCK  CalculateHeroesLuck(S_HEROES *heroes){

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

    // bonus luck
    switch(HeroesLevelSkill(heroes, LUCK)){
    
	case EXPERT:
	    if(LUCK_IRISH > result++);
	    if(LUCK_IRISH > result++);
	    if(LUCK_IRISH > result++);
	    break;
	    
	case ADVANCED:
	    if(LUCK_IRISH > result++);
	    if(LUCK_IRISH > result++);
	    break;
	    
	case BASIC:
	    if(LUCK_IRISH > result++);
	    break;
	    
	default:
	    break;
    }

    return result;
}

Uint16  CalculateHeroesMagicPoint(S_HEROES *heroes){

    if(NULL == heroes) return 0;

    return CalculateHeroesKnowledge(heroes) * 10;
}

Uint8   CalculateHeroesScouting(S_HEROES *heroes){

    // base scouting
    Uint8 scout = SCOUTINGBASE;
    
    // bonus scouting
    switch(HeroesLevelSkill(heroes, SCOUTING)){
    
	case EXPERT:
	    scout += 3;
	    break;
	    
	case ADVANCED:
	    scout += 2;
	    break;
	    
	case BASIC:
	    scout += 1;
	    break;
	    
	default:
	    break;
    }

    // bonus artifact
    if(HeroesArtifactPresent(heroes, TELESCOPE))
	++scout;

    return scout;
}

Uint8   CalculateHeroesMoveLandPoint(S_HEROES *heroes){

    if(NULL == heroes) return 0;
    
    Uint8 i;

    float log = 0;
    E_MONSTERSPEED speed = INSTANT;

    // Slowest Army Speed
    for(i = 0; i < HEROESMAXARMY; ++i)
	if((GetStatMonster(heroes->army[i].monster))->speed < speed)
	    speed = (GetStatMonster(heroes->army[i].monster))->speed;
    
    Uint8 sas = speed + 8;

    // bonus logistic
    switch(HeroesLevelSkill(heroes, LOGISTICS)){
    
	case EXPERT:
	    log = 1.299;
	    break;
	    
	case ADVANCED:
	    log = 1.2;
	    break;
	    
	case BASIC:
	    log = 1.1;
	    break;
	    
	default:
	    log = 1;
	    break;
    }

    float tp = (2 * sas * log) / 2;
    
    // bonus artefact
    if(HeroesArtifactPresent(heroes, NOMAD_BOOTS_MOBILITY))
	tp += 6;
    if(HeroesArtifactPresent(heroes, TRAVELER_BOOTS_MOBILITY))
	tp += 3;
    if(HeroesArtifactPresent(heroes, TRUE_COMPASS_MOBILITY))
	tp += 5;

/*
    GetTypeSurface(POS);
    move cost

          None  Basic   Adv.  Expert (PATHFINDING)
  Desert  2.00   1.75   1.50   1.00
  Snow    1.75   1.50   1.25   1.00
  Swamp   1.75   1.50   1.25   1.00
  Cracked 1.25   1.00   1.00   1.00
  Beach   1.25   1.00   1.00   1.00
  Lava    1.00   1.00   1.00   1.00
  Dirt    1.00   1.00   1.00   1.00
  Grass   1.00   1.00   1.00   1.00
  Water   1.00   1.00   1.00   1.00
  Road    0.75   0.75   0.75   0.75
*/

    return tp;
}

Uint8   CalculateHeroesMoveSeaPoint(S_HEROES *heroes){

    if(NULL == heroes) return 0;

    float tp = 15;
    
    // bonus navigation
    switch(HeroesLevelSkill(heroes, NAVIGATION)){
    
	case EXPERT:
	    tp += 15;
	    break;
	    
	case ADVANCED:
	    tp += 9.5;
	    break;
	    
	case BASIC:
	    tp += 4.5;
	    break;
	
	default:
	    break;
    }
    
    // bonus artefact
    if(HeroesArtifactPresent(heroes, TRUE_COMPASS_MOBILITY))
	tp += 5;
    if(HeroesArtifactPresent(heroes, SAILORS_ASTROLABE_MOBILITY))
	tp += 10;

    return tp;
}
