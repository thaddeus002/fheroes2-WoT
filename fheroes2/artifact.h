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
				    
#ifndef _ARTIFACT_H
#define _ARTIFACT_H

#include "SDL.h"
#include "gamedefs.h"
#include "resource.h"

#define	ARTIFACTCOUNT_LEVEL1	27
#define	ARTIFACTCOUNT_LEVEL2	21
#define	ARTIFACTCOUNT_LEVEL3	22
#define	ARTIFACTMAXCOUNT	82	// all (level1 + level2 + level3 + 8 ultimate + 2 bad artifact) + 1 magic book

typedef enum {
	    ART_ALL,
	    ART_LEVEL1,
	    ART_LEVEL2,
	    ART_LEVEL3
	    } E_LEVELARTIFACT;

typedef enum {
		ULTIMATE_BOOK,
		ULTIMATE_SWORD,
		ULTIMATE_CLOAK,
		ULTIMATE_WAND,
		ULTIMATE_SHIELD,
		ULTIMATE_STAFF,
		ULTIMATE_CROWN,
		GOLDEN_GOOSE,
		ARCANE_NECKLACE,
		CASTER_BRACELET,
		MAGE_RING,
		WITCHES_BROACH,
		MEDAL_VALOR,
		MEDAL_COURAGE,
		MEDAL_HONOR,
		MEDAL_DISTINCTION,
		FIZBIN_MISFORTUNE,
		THUNDER_MACE,
		ARMORED_GAUNTLETS,
		DEFENDER_HELM,
		GIANT_FLAIL,
		BALLISTA,
		STEALTH_SHIELD,
		DRAGON_SWORD,
		POWER_AXE,
		DIVINE_BREASTPLATE,
		MINOR_SCROLL,
		MAJOR_SCROLL,
		SUPERIOR_SCROLL,
		FOREMOST_SCROLL,
		ENDLESS_SACK_GOLD,
		ENDLESS_BAG_GOLD,
		ENDLESS_PURSE_GOLD,
		NOMAD_BOOTS_MOBILITY,
		TRAVELER_BOOTS_MOBILITY,
		RABBIT_FOOT,
		GOLDEN_HORSESHOE,
		GAMBLER_LUCKY_COIN,
		FOUR_LEAF_CLOVER,
		TRUE_COMPASS_MOBILITY,
		SAILORS_ASTROLABE_MOBILITY,
		EVIL_EYE,
		ENCHANTED_HOURGLASS,
		GOLD_WATCH,
		SKULLCAP,
		ICE_CLOAK,
		FIRE_CLOAK,
		LIGHTNING_HELM,
		EVERCOLD_ICICLE,
		EVERHOT_LAVA_ROCK,
		LIGHTNING_ROD,
		SNAKE_RING,
		ANKH,
		BOOK_ELEMENTS,
		ELEMENTAL_RING,
		HOLY_PENDANT,
		PENDANT_FREE_WILL,
		PENDANT_LIFE,
		SERENITY_PENDANT,
		SEEING_EYE_PENDANT,
		KINETIC_PENDANT,
		PENDANT_DEATH,
		WAND_NEGATION,
		GOLDEN_BOW,
		TELESCOPE,
		STATESMAN_QUILL,
		WIZARD_HAT,
		POWER_RING,
		AMMO_CART,
		TAX_LIEN,
		HIDEOUS_MASK,
		ENDLESS_POUCH_SULFUR,
		ENDLESS_VIAL_MERCURY,
		ENDLESS_POUCH_GEMS,
		ENDLESS_CORD_WOOD,
		ENDLESS_CART_ORE,
		ENDLESS_POUCH_CRYSTAL,
		SPIKED_HELM,
		SPIKED_SHIELD,
		WHITE_PEARL,
		BLACK_PEARL,

/* HEROES 2.2 (ANDURAN VERSION)
		SPELL_SCROLL,
		ARM_MARTYR,
		BREASTPLATE_ANDURAN,
		BROACH_SHIELDING,
		BATTLE_GARB,
		CRYSTAL_BALL,
		HEART_FIRE,
		HEART_ICE,
		HELMET_ANDURAN,
		HOLY_HAMMER,
		LEGENDARY_SCEPTER,
		MASTHEAD,
		SPHERE_NEGATION,
		STAFF_WIZARDRY,
		SWORD_BREAKER,
		SWORD_ANDURAN,
		SPADE_NECROMANCY,
*/
		MAGIC_BOOK,

		RND_ARTIFACT		= 163,
		RND_ULTIMATEARTIFACT	= 164,
		RND_ARTIFACT1		= 167,
		RND_ARTIFACT2		= 169,
		RND_ARTIFACT3		= 171,
		
		ARTIFACTNONE
	    } E_ARTIFACT;

typedef struct {
		const char *	name;
		const char *	description;
	    } S_ARTIFACT;

BOOL		InitArtifact(void);
void		FreeArtifact(void);
S_ARTIFACT *	GetStatArtifact(E_ARTIFACT);
E_ARTIFACT	GetRNDArtifact(E_LEVELARTIFACT);
E_ARTIFACT	CheckValidArtifact(Uint8);

#endif
