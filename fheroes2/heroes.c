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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "artifact.h"
#include "spell.h"
#include "magicbook.h"
#include "config.h"
#include "cursor.h"
#include "debug.h"
#include "tools.h"
#include "monster.h"
#include "heroes.h"

static S_HEROES *	allHeroes = NULL;
static char 		heroesPortrait[13];

S_HEROES *GetStatHeroes(E_NAMEHEROES name){

    if(allHeroes && name < HEROESNULL) return &allHeroes[name];
    
    return NULL;
}

BOOL	InitHeroes(void){

    allHeroes = (S_HEROES *) malloc(HEROESMAXCOUNT * sizeof(S_HEROES));

    if(allHeroes == NULL){
        fprintf(stderr, "InitHeroes: error malloc: %d\n", HEROESMAXCOUNT * sizeof(S_HEROES));
        return FALSE;
    }

    HeroesDefaultValues(&allHeroes[LORDKILBURN], KNIGHT);
    HeroesDefaultValues(&allHeroes[SIRGALLANTH], KNIGHT);
    HeroesDefaultValues(&allHeroes[ECTOR], KNIGHT);
    HeroesDefaultValues(&allHeroes[GVENNETH], KNIGHT);
    HeroesDefaultValues(&allHeroes[TYRO], KNIGHT);
    HeroesDefaultValues(&allHeroes[AMBROSE], KNIGHT);
    HeroesDefaultValues(&allHeroes[RUBY], KNIGHT);
    HeroesDefaultValues(&allHeroes[MAXIMUS], KNIGHT);
    HeroesDefaultValues(&allHeroes[DIMITRY], KNIGHT);

    HeroesDefaultValues(&allHeroes[THUNDAX], BARBARIAN);
    HeroesDefaultValues(&allHeroes[FINEOUS], BARBARIAN);
    HeroesDefaultValues(&allHeroes[JOJOSH], BARBARIAN);
    HeroesDefaultValues(&allHeroes[CRAGHACK], BARBARIAN);
    HeroesDefaultValues(&allHeroes[JEZEBEL], BARBARIAN);
    HeroesDefaultValues(&allHeroes[JACLYN], BARBARIAN);
    HeroesDefaultValues(&allHeroes[ERGON], BARBARIAN);
    HeroesDefaultValues(&allHeroes[TSABU], BARBARIAN);
    HeroesDefaultValues(&allHeroes[ATLAS], BARBARIAN);

    HeroesDefaultValues(&allHeroes[ASTRA], SORCERESS);
    HeroesDefaultValues(&allHeroes[NATASHA], SORCERESS);
    HeroesDefaultValues(&allHeroes[TROYAN], SORCERESS);
    HeroesDefaultValues(&allHeroes[VATAWNA], SORCERESS);
    HeroesDefaultValues(&allHeroes[REBECCA], SORCERESS);
    HeroesDefaultValues(&allHeroes[GEM], SORCERESS);
    HeroesDefaultValues(&allHeroes[ARIEL], SORCERESS);
    HeroesDefaultValues(&allHeroes[CARLAWN], SORCERESS);
    HeroesDefaultValues(&allHeroes[LUNA], SORCERESS);

    HeroesDefaultValues(&allHeroes[ARIE], WARLOCK);
    HeroesDefaultValues(&allHeroes[ALAMAR], WARLOCK);
    HeroesDefaultValues(&allHeroes[VESPER], WARLOCK);
    HeroesDefaultValues(&allHeroes[CRODO], WARLOCK);
    HeroesDefaultValues(&allHeroes[BAROK], WARLOCK);
    HeroesDefaultValues(&allHeroes[KASTORE], WARLOCK);
    HeroesDefaultValues(&allHeroes[AGAR], WARLOCK);
    HeroesDefaultValues(&allHeroes[FALAGAR], WARLOCK);
    HeroesDefaultValues(&allHeroes[WRATHMONT], WARLOCK);

    HeroesDefaultValues(&allHeroes[MYRA], WIZARD);
    HeroesDefaultValues(&allHeroes[FLINT], WIZARD);
    HeroesDefaultValues(&allHeroes[DAWN], WIZARD);
    HeroesDefaultValues(&allHeroes[HALON], WIZARD);
    HeroesDefaultValues(&allHeroes[MYRINI], WIZARD);
    HeroesDefaultValues(&allHeroes[WILFREY], WIZARD);
    HeroesDefaultValues(&allHeroes[SARAKIN], WIZARD);
    HeroesDefaultValues(&allHeroes[KALINDRA], WIZARD);
    HeroesDefaultValues(&allHeroes[MANDIGAL], WIZARD);

    HeroesDefaultValues(&allHeroes[ZOM], NECROMANCER);
    HeroesDefaultValues(&allHeroes[DARLANA], NECROMANCER);
    HeroesDefaultValues(&allHeroes[ZAM], NECROMANCER);
    HeroesDefaultValues(&allHeroes[RANLOO], NECROMANCER);
    HeroesDefaultValues(&allHeroes[CHARITY], NECROMANCER);
    HeroesDefaultValues(&allHeroes[RIALDO], NECROMANCER);
    HeroesDefaultValues(&allHeroes[ROXANA], NECROMANCER);
    HeroesDefaultValues(&allHeroes[SANDRO], NECROMANCER);
    HeroesDefaultValues(&allHeroes[CELIA], NECROMANCER);

    HeroesDefaultValues(&allHeroes[ROLAND], KNIGHT);
    HeroesDefaultValues(&allHeroes[NAME_UNK2], NECROMANCER);
    HeroesDefaultValues(&allHeroes[NAME_UNK3], SORCERESS);
    HeroesDefaultValues(&allHeroes[ARCHIBALD], KNIGHT);
    HeroesDefaultValues(&allHeroes[SANDYSANDY], WARLOCK);
    HeroesDefaultValues(&allHeroes[BRAX], NECROMANCER);

    allHeroes[LORDKILBURN].name	= "Lord Kilburn";
    allHeroes[SIRGALLANTH].name	= "Sir Gallanth";
    allHeroes[ECTOR].name	= "Ector";
    allHeroes[GVENNETH].name	= "Gvenneth";
    allHeroes[TYRO].name	= "Tyro";
    allHeroes[AMBROSE].name	= "Ambrose";
    allHeroes[RUBY].name	= "Ruby";
    allHeroes[MAXIMUS].name	= "Maximus";
    allHeroes[DIMITRY].name	= "Dimitry";

    allHeroes[THUNDAX].name	= "Thundax";
    allHeroes[FINEOUS].name	= "Fineous";
    allHeroes[JOJOSH].name	= "Jojosh";
    allHeroes[CRAGHACK].name	= "Crag Hack";
    allHeroes[JEZEBEL].name	= "Jezebel";
    allHeroes[JACLYN].name	= "Jaclin";
    allHeroes[ERGON].name	= "Ergon";
    allHeroes[TSABU].name	= "Tsabu";
    allHeroes[ATLAS].name	= "Atlas";

    allHeroes[ASTRA].name	= "Astra";
    allHeroes[NATASHA].name	= "Natasha";
    allHeroes[TROYAN].name	= "Troyan";
    allHeroes[VATAWNA].name	= "Vatawna";
    allHeroes[REBECCA].name	= "Rebecca";
    allHeroes[GEM].name		= "Gem";
    allHeroes[ARIEL].name	= "Ariel";
    allHeroes[CARLAWN].name	= "Carlawn";
    allHeroes[LUNA].name	= "Luna";

    allHeroes[ARIE].name	= "Arie";
    allHeroes[ALAMAR].name	= "Alamar";
    allHeroes[VESPER].name	= "Vesper";
    allHeroes[CRODO].name	= "Crodo";
    allHeroes[BAROK].name	= "Barok";
    allHeroes[KASTORE].name	= "Kastore";
    allHeroes[AGAR].name	= "Agar";
    allHeroes[FALAGAR].name	= "Falagar";
    allHeroes[WRATHMONT].name	= "Wrathmont";

    allHeroes[MYRA].name	= "Myra";
    allHeroes[FLINT].name	= "Flint";
    allHeroes[DAWN].name	= "Dawn";
    allHeroes[HALON].name	= "Halon";
    allHeroes[MYRINI].name	= "Myrini";
    allHeroes[WILFREY].name	= "Wilfrey";
    allHeroes[SARAKIN].name	= "Sarakin";
    allHeroes[KALINDRA].name	= "Kalindra";
    allHeroes[MANDIGAL].name	= "Mandigal";

    allHeroes[ZOM].name		= "Zom";
    allHeroes[DARLANA].name	= "Darlana";
    allHeroes[ZAM].name		= "Zam";
    allHeroes[RANLOO].name	= "Ranloo";
    allHeroes[CHARITY].name	= "Charity";
    allHeroes[RIALDO].name	= "Rialdo";
    allHeroes[ROXANA].name	= "Roxana";
    allHeroes[SANDRO].name	= "Sandro";
    allHeroes[CELIA].name	= "Celia";

    allHeroes[ROLAND].name	= "Roland";
    allHeroes[NAME_UNK2].name	= "Unknown Name 2";
    allHeroes[NAME_UNK3].name	= "Unknown Name 3";
    allHeroes[ARCHIBALD].name	= "Archibald";

    if(GetIntValue(DEBUG)){
	allHeroes[SANDYSANDY].name = "Debugger";
	allHeroes[SANDYSANDY].army[0].count = 3;
	allHeroes[SANDYSANDY].army[0].monster = RED_DRAGON;
	allHeroes[SANDYSANDY].army[1].count = 2;
	allHeroes[SANDYSANDY].army[1].monster = BLACK_DRAGON;
    } else
	allHeroes[SANDYSANDY].name = "SandySandy";

    allHeroes[BRAX].name = "Brother Brax";
    
    fprintf(stderr, "Init heroes.\n");

    return TRUE;
}

void	FreeHeroes(void){

    if(allHeroes) free(allHeroes);

}

void HeroesDefaultValues(S_HEROES *heroes, E_RACE race){

    Uint8 i = 0;

    for(i = 0; i < HEROESMAXSKILL; ++i){
	heroes->skill[i].type	= SKILLNONE;
	heroes->skill[i].level	= LEVELNONE;
    }

    for(i = 0; i < HEROESMAXARTIFACT; ++i)
	heroes->artifact[i]	= ARTIFACTNONE;

    for(i = 0; i < HEROESMAXARMY; ++i){
	heroes->army[i].count	= 0;
	heroes->army[i].monster	= MONSTERNONE;
    }

    switch(race){
    
	case KNIGHT:

	    heroes->race		= KNIGHT;
	    heroes->attack		= DEFAULT_KNGT_ATTACK;
	    heroes->defence		= DEFAULT_KNGT_DEFENCE;
	    heroes->power		= DEFAULT_KNGT_POWER;
	    heroes->knowledge		= DEFAULT_KNGT_KNOWLEDGE;
	    heroes->skill[0].type	= LEADERSHIP;
	    heroes->skill[0].level	= BASIC;
	    heroes->skill[1].type	= BALLISTICS;
	    heroes->skill[1].level	= BASIC;
	    heroes->army[0].count	= GetMonsterGrownCastle(NULL, PEASANT);
	    heroes->army[0].monster	= PEASANT;
	    heroes->army[1].count	= GetMonsterGrownCastle(NULL, ARCHER);
	    heroes->army[1].monster	= ARCHER;
	
	    break;
	    
	case BARBARIAN:

	    heroes->race		= BARBARIAN;
	    heroes->attack		= DEFAULT_BRBN_ATTACK;
	    heroes->defence		= DEFAULT_BRBN_DEFENCE;
	    heroes->power		= DEFAULT_BRBN_POWER;
	    heroes->knowledge		= DEFAULT_BRBN_KNOWLEDGE;
	    heroes->skill[0].type	= PATHFINDING;
	    heroes->skill[0].level	= ADVANCED;
	    heroes->army[0].count	= GetMonsterGrownCastle(NULL, GOBLIN);
	    heroes->army[0].monster	= GOBLIN;
	    heroes->army[1].count	= GetMonsterGrownCastle(NULL, ORC);
	    heroes->army[1].monster	= ORC;
	
	    break;

        case NECROMANCER:

	    heroes->race		= NECROMANCER;
	    heroes->attack		= DEFAULT_NCRM_ATTACK;
	    heroes->defence		= DEFAULT_NCRM_DEFENCE;
	    heroes->power		= DEFAULT_NCRM_POWER;
	    heroes->knowledge		= DEFAULT_NCRM_KNOWLEDGE;
	    heroes->skill[0].type	= NECROMANCY;
	    heroes->skill[0].level	= BASIC;
	    heroes->skill[1].type	= WISDOM;
	    heroes->skill[1].level	= BASIC;
	    heroes->army[0].count	= GetMonsterGrownCastle(NULL, SKELETON);
	    heroes->army[0].monster	= SKELETON;
	    heroes->army[1].count	= GetMonsterGrownCastle(NULL, ZOMBIE);
	    heroes->army[1].monster	= ZOMBIE;
	    heroes->artifact[0]		= MAGIC_BOOK;
	    AddMagicToBook(HASTE);
	    break;

        case SORCERESS:

	    heroes->race		= SORCERESS;
	    heroes->attack		= DEFAULT_SCRS_ATTACK;
	    heroes->defence		= DEFAULT_SCRS_DEFENCE;
	    heroes->power		= DEFAULT_SCRS_POWER;
	    heroes->knowledge		= DEFAULT_SCRS_KNOWLEDGE;
	    heroes->skill[0].type	= NAVIGATION;
	    heroes->skill[0].level	= ADVANCED;
	    heroes->skill[1].type	= WISDOM;
	    heroes->skill[1].level	= BASIC;
	    heroes->army[0].count	= GetMonsterGrownCastle(NULL, SPRITE);
	    heroes->army[0].monster	= SPRITE;
	    heroes->army[1].count	= GetMonsterGrownCastle(NULL, DWARF);
	    heroes->army[1].monster	= DWARF;
	    heroes->artifact[0]		= MAGIC_BOOK;
	    AddMagicToBook(BLESS);
	    break;

        case WARLOCK:

	    heroes->race		= WARLOCK;
	    heroes->attack		= DEFAULT_WRLK_ATTACK;
	    heroes->defence		= DEFAULT_WRLK_DEFENCE;
	    heroes->power		= DEFAULT_WRLK_POWER;
	    heroes->knowledge		= DEFAULT_WRLK_KNOWLEDGE;
	    heroes->skill[0].type	= SCOUTING;
	    heroes->skill[0].level	= ADVANCED;
	    heroes->skill[1].type	= WISDOM;
	    heroes->skill[1].level	= BASIC;
	    heroes->army[0].count	= GetMonsterGrownCastle(NULL, CENTAUR);
	    heroes->army[0].monster	= CENTAUR;
	    heroes->army[1].count	= GetMonsterGrownCastle(NULL, GARGOYLE);
	    heroes->army[1].monster	= GARGOYLE;
	    heroes->artifact[0]		= MAGIC_BOOK;
	    AddMagicToBook(CURSE);
	    break;

        case WIZARD:

	    heroes->race		= WIZARD;
	    heroes->attack		= DEFAULT_WZRD_ATTACK;
	    heroes->defence		= DEFAULT_WZRD_DEFENCE;
	    heroes->power		= DEFAULT_WZRD_POWER;
	    heroes->knowledge		= DEFAULT_WZRD_KNOWLEDGE;
	    heroes->skill[0].type	= WISDOM;
	    heroes->skill[0].level	= ADVANCED;
	    heroes->army[0].count	= GetMonsterGrownCastle(NULL, HALFLING);
	    heroes->army[0].monster	= HALFLING;
	    heroes->artifact[0]		= MAGIC_BOOK;
	    heroes->army[1].count	= GetMonsterGrownCastle(NULL, BOAR);
	    heroes->army[1].monster	= BOAR;
	    AddMagicToBook(STONESKIN);
	    break;

	default:
	    break;
    }

    heroes->morale	= MORALE_GOOD;
    heroes->luck	= LUCK_NORMAL;
    heroes->experience	= 0;
    heroes->magicPoint	= CalculateHeroesMagicPoint(heroes);
    heroes->movePoint	= 0;
    heroes->af		= SPREAD;

}

E_LEVELSKILL HeroesLevelSkill(const S_HEROES *heroes, E_SKILL skill){

    if(NULL == heroes || SKILLNONE == skill) return LEVELNONE;

    Uint8 i;

    for(i = 0; i < HEROESMAXSKILL; ++i)
	if(skill == heroes->skill[i].type) return heroes->skill[i].level;

    return LEVELNONE;
}

BOOL    HeroesArtifactPresent(const S_HEROES *heroes, E_ARTIFACT artifact){

    if(NULL == heroes) return FALSE;

    Uint8 i;

    for(i = 0; i < HEROESMAXARTIFACT; ++i)
	if(artifact == heroes->artifact[i]) return TRUE;

    return FALSE;
}

Uint8   CalculateHeroesAttack(const S_HEROES *heroes){

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

Uint8   CalculateHeroesDefence(const S_HEROES *heroes){

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

Uint8   CalculateHeroesPower(const S_HEROES *heroes){

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

Uint8	CalculateHeroesKnowledge(const S_HEROES *heroes){

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

E_MORALE CalculateHeroesMorale(const S_HEROES *heroes){

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

E_LUCK  CalculateHeroesLuck(const S_HEROES *heroes){

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

Uint16  CalculateHeroesMagicPoint(const S_HEROES *heroes){

    if(NULL == heroes) return 0;

    return CalculateHeroesKnowledge(heroes) * 10;
}

Uint8   CalculateHeroesScouting(const S_HEROES *heroes){

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

Uint8   CalculateHeroesMoveLandPoint(const S_HEROES *heroes){

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

Uint8   CalculateHeroesMoveSeaPoint(const S_HEROES *heroes){

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

Uint8 HeroesCountArmy(const S_HEROES *heroes){

    if(! heroes) return 0;
    
    Uint8 i;
    Uint8 res = 0;
    for(i = 0; i < HEROESMAXARMY; ++i)
	if(heroes->army[i].count) ++res;

    return res;
}

const char * HeroesBigNamePortrait(E_NAMEHEROES name){

    char num[5];
    char *ptr;

    snprintf(num, 5, "%4d", name);

    for(ptr = num; *ptr; ++ptr) if(0 == strncmp(ptr, " ", 1)) *ptr = '0';

    snprintf(heroesPortrait, 13, "PORT%4s.ICN", num);

    return heroesPortrait;
}

const char * CapitanBigNamePortrait(E_RACE race){

    char num[5];
    char *ptr;

    switch(race){ 
        case KNIGHT: 
    	    snprintf(num, 5, "%4d", 90); 
            break; 
        case BARBARIAN: 
            snprintf(num, 5, "%4d", 91); 
            break; 
        case SORCERESS: 
            snprintf(num, 5, "%4d", 92); 
            break; 
        case WARLOCK: 
            snprintf(num, 5, "%4d", 93); 
    	    break; 
        case WIZARD: 
            snprintf(num, 5, "%4d", 94); 
            break; 
        case NECROMANCER: 
            snprintf(num, 5, "%4d", 95); 
            break; 
        default: 
            return NULL;
            break; 
    } 

    for(ptr = num; *ptr; ++ptr) if(0 == strncmp(ptr, " ", 1)) *ptr = '0';

    snprintf(heroesPortrait, 13, "PORT%4s.ICN", num);

    return heroesPortrait;
}

E_NAMEHEROES    GetRecrutPrimaryHeroes(void){

    return ROLAND;
}

E_NAMEHEROES    GetRecrutSecondaryHeroes(void){

    return ARCHIBALD;
}

const char * HeroesGetStringName(E_NAMEHEROES name){

    return allHeroes[name].name;
}
