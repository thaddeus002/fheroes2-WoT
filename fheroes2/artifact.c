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
#include <string.h>
#include "SDL.h"

#include "heroes.h"
#include "config.h"
#include "resource.h"
#include "artifact.h"

static S_ARTIFACT * ptrArtifact = NULL;

            
E_ARTIFACT CheckValidArtifact(Uint8 type){

    E_ARTIFACT a;
            
    for(a = ULTIMATE_BOOK; a < ARTIFACTNONE; ++a)
        if(a == type)
            return a;

    if(GetIntValue(DEBUG))
        fprintf(stderr, "CheckValidArtifact: unknown object: 0x%hhX\n", type);

    return ARTIFACTNONE;
}

S_ARTIFACT * GetStatArtifact(E_ARTIFACT artifact){

    return &ptrArtifact[artifact];
}

void FreeArtifact(void){

    if(ptrArtifact) free(ptrArtifact);
    
    ptrArtifact = NULL;
}

E_ARTIFACT GetRNDArtifact(E_LEVELARTIFACT level){

    E_ARTIFACT level1[] = { MEDAL_VALOR, MEDAL_COURAGE, MEDAL_HONOR, MEDAL_DISTINCTION, THUNDER_MACE, ARMORED_GAUNTLETS, DEFENDER_HELM, GIANT_FLAIL, RABBIT_FOOT, GOLDEN_HORSESHOE, GAMBLER_LUCKY_COIN, FOUR_LEAF_CLOVER, ENCHANTED_HOURGLASS, ICE_CLOAK, FIRE_CLOAK, LIGHTNING_HELM, SNAKE_RING, HOLY_PENDANT, PENDANT_FREE_WILL, PENDANT_LIFE, PENDANT_DEATH, GOLDEN_BOW, TELESCOPE, SERENITY_PENDANT, STATESMAN_QUILL, KINETIC_PENDANT, SEEING_EYE_PENDANT };
    E_ARTIFACT level2[] = { CASTER_BRACELET, MAGE_RING, STEALTH_SHIELD, POWER_AXE, MINOR_SCROLL, ENDLESS_PURSE_GOLD, SAILORS_ASTROLABE_MOBILITY, ENDLESS_CORD_WOOD, ENDLESS_CART_ORE, SPIKED_HELM, WHITE_PEARL, EVIL_EYE, GOLD_WATCH, ANKH, BOOK_ELEMENTS, ELEMENTAL_RING, SKULLCAP, EVERCOLD_ICICLE, POWER_RING, AMMO_CART, EVERHOT_LAVA_ROCK };
    E_ARTIFACT level3[] = { ARCANE_NECKLACE, WITCHES_BROACH, BALLISTA, DRAGON_SWORD, DIVINE_BREASTPLATE, MAJOR_SCROLL, SUPERIOR_SCROLL, FOREMOST_SCROLL, ENDLESS_SACK_GOLD, ENDLESS_BAG_GOLD, NOMAD_BOOTS_MOBILITY, TRAVELER_BOOTS_MOBILITY, TRUE_COMPASS_MOBILITY, ENDLESS_POUCH_SULFUR, ENDLESS_POUCH_GEMS, ENDLESS_POUCH_CRYSTAL, ENDLESS_VIAL_MERCURY, SPIKED_SHIELD, BLACK_PEARL, LIGHTNING_ROD, WAND_NEGATION, WIZARD_HAT };
    E_ARTIFACT result;

    switch(level){
    
	case ART_LEVEL1:
	    result = level1[rand() % ARTIFACTCOUNT_LEVEL1];
	    while(ptrArtifact[result].use) result = level1[rand() % ARTIFACTCOUNT_LEVEL1];
	    ptrArtifact[result].use = TRUE;
	    return result;
	    break;
	    
	case ART_LEVEL2:
	    result = level2[rand() % ARTIFACTCOUNT_LEVEL2];
	    while(ptrArtifact[result].use) result = level2[rand() % ARTIFACTCOUNT_LEVEL2];
	    ptrArtifact[result].use = TRUE;
	    return result;
	    break;
	    
	case ART_LEVEL3:
	    result = level3[rand() % ARTIFACTCOUNT_LEVEL3];
	    while(ptrArtifact[result].use) result = level3[rand() % ARTIFACTCOUNT_LEVEL3];
	    ptrArtifact[result].use = TRUE;
	    return result;
	    break;
	    
	default:
	    break;
    }

    // ART_ALL
    result = 9 + (rand() % (ARTIFACTMAXCOUNT - 10));
    while(ptrArtifact[result].use) result = 9 + (rand() % (ARTIFACTMAXCOUNT - 10));
    ptrArtifact[result].use = TRUE;
    return result;
}

BOOL    InitArtifact(void){

	if(NULL == (ptrArtifact = malloc(sizeof(S_ARTIFACT) * ARTIFACTMAXCOUNT))){
	    fprintf(stderr, "InitArtifact: error malloc: %d\n", sizeof(S_ARTIFACT) * ARTIFACTMAXCOUNT);
	    return FALSE;
	}

	ptrArtifact[ULTIMATE_BOOK].name = "Ultimate Book";
	ptrArtifact[ULTIMATE_BOOK].description = "The Ultimate Book of Knowledge increases your knowledge by 12.";
	ptrArtifact[ULTIMATE_BOOK].use = FALSE;

	ptrArtifact[ULTIMATE_SWORD].name = "Ultimate Sword";
	ptrArtifact[ULTIMATE_SWORD].description = "The Ultimate Sword of Dominion increases your attack skill by 12.";
	ptrArtifact[ULTIMATE_SWORD].use = FALSE;

	ptrArtifact[ULTIMATE_CLOAK].name = "Ultimate Cloak";
	ptrArtifact[ULTIMATE_CLOAK].description = "The Ultimate Cloak of Protection increases your defense skill by 12.";
	ptrArtifact[ULTIMATE_CLOAK].use = FALSE;

	ptrArtifact[ULTIMATE_WAND].name = "Ultimate Wand";
	ptrArtifact[ULTIMATE_WAND].description = "The Ultimate Wand of Magic increases your spell power by 12.";
	ptrArtifact[ULTIMATE_WAND].use = FALSE;

	ptrArtifact[ULTIMATE_SHIELD].name = "Ultimate Shield";
	ptrArtifact[ULTIMATE_SHIELD].description = "The Ultimate Shield increases your attack and defense skills by 6 each.";
	ptrArtifact[ULTIMATE_SHIELD].use = FALSE;

	ptrArtifact[ULTIMATE_STAFF].name = "Ultimate Staff";
	ptrArtifact[ULTIMATE_STAFF].description = "The Ultimate Staff increases your spell power and knowledge by 6 each.";
	ptrArtifact[ULTIMATE_STAFF].use = FALSE;

	ptrArtifact[ULTIMATE_CROWN].name = "Ultimate Crown";
	ptrArtifact[ULTIMATE_CROWN].description = "The Ultimate Crown increases each of your basic skills by 4 points.";
	ptrArtifact[ULTIMATE_CROWN].use = FALSE;

	ptrArtifact[GOLDEN_GOOSE].name = "Ultimate Goose";
	ptrArtifact[GOLDEN_GOOSE].description = "The Golden Goose brings in an income of 10,000 gold per turn.";
	ptrArtifact[GOLDEN_GOOSE].use = FALSE;

	ptrArtifact[ARCANE_NECKLACE].name = "Arcane Necklace";
	ptrArtifact[ARCANE_NECKLACE].description = "The Arcane Necklace of Magic increases your spell power by 4.";
	ptrArtifact[ARCANE_NECKLACE].use = FALSE;

	ptrArtifact[CASTER_BRACELET].name = "Caster's Bracelet";
	ptrArtifact[CASTER_BRACELET].description = "The Caster's Bracelet of Magic increases your spell power by 2.";
	ptrArtifact[CASTER_BRACELET].use = FALSE;

	ptrArtifact[MAGE_RING].name = "Mage's Ring";
	ptrArtifact[MAGE_RING].description = "The Mage's Ring of Power increases your spell power by 2.";
	ptrArtifact[MAGE_RING].use = FALSE;

	ptrArtifact[WITCHES_BROACH].name = "Witches Broach";
	ptrArtifact[WITCHES_BROACH].description = "The Witch's Broach of Magic increases your spell power by 3.";
	ptrArtifact[WITCHES_BROACH].use = FALSE;

	ptrArtifact[MEDAL_VALOR].name = "Medal of Valor";
	ptrArtifact[MEDAL_VALOR].description = "The Medal of Valor increases your morale.";
	ptrArtifact[MEDAL_VALOR].use = FALSE;

	ptrArtifact[MEDAL_COURAGE].name = "Medal of Courage";
	ptrArtifact[MEDAL_COURAGE].description = "The Medal of Courage increases your morale.";
	ptrArtifact[MEDAL_COURAGE].use = FALSE;

	ptrArtifact[MEDAL_HONOR].name = "Medal of Honor";
	ptrArtifact[MEDAL_HONOR].description = "The Medal of Honor increases your morale.";
	ptrArtifact[MEDAL_HONOR].use = FALSE;

	ptrArtifact[MEDAL_DISTINCTION].name = "Medal of Distinction";
	ptrArtifact[MEDAL_DISTINCTION].description = "The Medal of Distinction increases your morale.";
	ptrArtifact[MEDAL_DISTINCTION].use = FALSE;

	ptrArtifact[FIZBIN_MISFORTUNE].name = "Fizbin of Misfortune";
	ptrArtifact[FIZBIN_MISFORTUNE].description = "The Fizbin of Misfortune greatly decreases your morale.";
	ptrArtifact[FIZBIN_MISFORTUNE].use = FALSE;

	ptrArtifact[THUNDER_MACE].name = "Thunder Mace";
	ptrArtifact[THUNDER_MACE].description = "The Thunder Mace of Dominion increases your attack skill by 1.";
	ptrArtifact[THUNDER_MACE].use = FALSE;

	ptrArtifact[ARMORED_GAUNTLETS].name = "Armored Gauntlets";
	ptrArtifact[ARMORED_GAUNTLETS].description = "The Armored Gauntlets of Protection increase your defense skill by 1.";
	ptrArtifact[ARMORED_GAUNTLETS].use = FALSE;

	ptrArtifact[DEFENDER_HELM].name = "Defender Helm";
	ptrArtifact[DEFENDER_HELM].description = "The Defender Helm of Protection increases your defense skill by 1.";
	ptrArtifact[DEFENDER_HELM].use = FALSE;

	ptrArtifact[GIANT_FLAIL].name = "Giant Flail";
	ptrArtifact[GIANT_FLAIL].description = "The Giant Flail of Dominion increases your attack skill by 1.";
	ptrArtifact[GIANT_FLAIL].use = FALSE;

	ptrArtifact[BALLISTA].name = "Ballista";
	ptrArtifact[BALLISTA].description = "The Ballista of Quickness lets your catapult fire twice per combat round.";
	ptrArtifact[BALLISTA].use = FALSE;

	ptrArtifact[STEALTH_SHIELD].name = "Stealth Shield";
	ptrArtifact[STEALTH_SHIELD].description = "The Stealth Shield of Protection increases your defense skill by 2.";
	ptrArtifact[STEALTH_SHIELD].use = FALSE;

	ptrArtifact[DRAGON_SWORD].name = "Dragon Sword";
	ptrArtifact[DRAGON_SWORD].description = "The Dragon Sword of Dominion increases your attack skill by 3.";
	ptrArtifact[DRAGON_SWORD].use = FALSE;

	ptrArtifact[POWER_AXE].name = "Power Axe";
	ptrArtifact[POWER_AXE].description = "The Power Axe of Dominion increases your attack skill by 2.";
	ptrArtifact[POWER_AXE].use = FALSE;

	ptrArtifact[DIVINE_BREASTPLATE].name = "Divine Breastplate";
	ptrArtifact[DIVINE_BREASTPLATE].description = "The Divine Breastplate of Protection increases your defense skill by 3.";
	ptrArtifact[DIVINE_BREASTPLATE].use = FALSE;

	ptrArtifact[MINOR_SCROLL].name = "Minor Scroll";
	ptrArtifact[MINOR_SCROLL].description = "The Minor Scroll of Knowledge increases your knowledge by 2.";
	ptrArtifact[MINOR_SCROLL].use = FALSE;

	ptrArtifact[MAJOR_SCROLL].name = "Major Scroll";
	ptrArtifact[MAJOR_SCROLL].description = "The Major Scroll of Knowledge increases your knowledge by 3.";
	ptrArtifact[MAJOR_SCROLL].use = FALSE;

	ptrArtifact[SUPERIOR_SCROLL].name = "Superior Scroll";
	ptrArtifact[SUPERIOR_SCROLL].description = "The Superior Scroll of Knowledge increases your knowledge by 4.";
	ptrArtifact[SUPERIOR_SCROLL].use = FALSE;

	ptrArtifact[FOREMOST_SCROLL].name = "Foremost Scroll";
	ptrArtifact[FOREMOST_SCROLL].description = "The Foremost Scroll of Knowledge increases your knowledge by 5.";
	ptrArtifact[FOREMOST_SCROLL].use = FALSE;

	ptrArtifact[ENDLESS_SACK_GOLD].name = "Endless Sack of Gold";
	ptrArtifact[ENDLESS_SACK_GOLD].description = "The Endless Sack of Gold provides you with 1000 gold per day.";
	ptrArtifact[ENDLESS_SACK_GOLD].use = FALSE;

	ptrArtifact[ENDLESS_BAG_GOLD].name = "Endless Bag of Gold";
	ptrArtifact[ENDLESS_BAG_GOLD].description = "The Endless Bag of Gold provides you with 750 gold per day.";
	ptrArtifact[ENDLESS_BAG_GOLD].use = FALSE;

	ptrArtifact[ENDLESS_PURSE_GOLD].name = "Endless Purse of Gold";
	ptrArtifact[ENDLESS_PURSE_GOLD].description = "The Endless Purse of Gold provides you with 500 gold per day.";
	ptrArtifact[ENDLESS_PURSE_GOLD].use = FALSE;

	ptrArtifact[RABBIT_FOOT].name = "Rabbit's Foot";
	ptrArtifact[RABBIT_FOOT].description = "The Lucky Rabbit's Foot increases your luck in combat.";
	ptrArtifact[RABBIT_FOOT].use = FALSE;

	ptrArtifact[GOLDEN_HORSESHOE].name = "Golden Horseshoe";
	ptrArtifact[GOLDEN_HORSESHOE].description = "The Golden Horseshoe increases your luck in combat.";
	ptrArtifact[GOLDEN_HORSESHOE].use = FALSE;

	ptrArtifact[GAMBLER_LUCKY_COIN].name = "Gambler's Lucky Coin";
	ptrArtifact[GAMBLER_LUCKY_COIN].description = "The Gambler's Lucky Coin increases your luck in combat.";
	ptrArtifact[GAMBLER_LUCKY_COIN].use = FALSE;

	ptrArtifact[FOUR_LEAF_CLOVER].name = "Four-Leaf Clover";
	ptrArtifact[FOUR_LEAF_CLOVER].description = "The Four_Leaf Clover increases your luck in combat.";
	ptrArtifact[FOUR_LEAF_CLOVER].use = FALSE;

	ptrArtifact[EVIL_EYE].name = "Evil Eye";
	ptrArtifact[EVIL_EYE].description = "The Evil Eye reduces the casting cost of curse spells by half.";
	ptrArtifact[EVIL_EYE].use = FALSE;

	ptrArtifact[ENCHANTED_HOURGLASS].name = "Enchanted Hourglass";
	ptrArtifact[ENCHANTED_HOURGLASS].description = "The Enchanted Hourglass extends the duration of all your spells by 2 turns.";
	ptrArtifact[ENCHANTED_HOURGLASS].use = FALSE;

	ptrArtifact[GOLD_WATCH].name = "Gold Wath";
	ptrArtifact[GOLD_WATCH].description = "The Gold Watch doubles the effectiveness of your hypnotize spells.";
	ptrArtifact[GOLD_WATCH].use = FALSE;

	ptrArtifact[SKULLCAP].name = "Skullcap";
	ptrArtifact[SKULLCAP].description = "The Skullcap halves the casting cost of all mind influencing spells.";
	ptrArtifact[SKULLCAP].use = FALSE;

	ptrArtifact[ICE_CLOAK].name = "Ice Clock";
	ptrArtifact[ICE_CLOAK].description = "The Ice Cloak halves all damage your troops take from cold spells.";
	ptrArtifact[ICE_CLOAK].use = FALSE;

	ptrArtifact[FIRE_CLOAK].name = "Fire Cloak";
	ptrArtifact[FIRE_CLOAK].description = "The Fire Cloak halves all damage your troops take from fire spells.";
	ptrArtifact[FIRE_CLOAK].use = FALSE;

	ptrArtifact[LIGHTNING_HELM].name = "Lightning Helm";
	ptrArtifact[LIGHTNING_HELM].description = "The Lightning Helm halves all damage your troops take from lightning spells.";
	ptrArtifact[LIGHTNING_HELM].use = FALSE;

	ptrArtifact[EVERCOLD_ICICLE].name = "Evercold Icicle";
	ptrArtifact[EVERCOLD_ICICLE].description = "The Evercold Icicle causes your cold spells to do 50% more damage to enemy troops.";
	ptrArtifact[EVERCOLD_ICICLE].use = FALSE;

	ptrArtifact[EVERHOT_LAVA_ROCK].name = "Everhot Lava Rock";
	ptrArtifact[EVERHOT_LAVA_ROCK].description = "The Everhot Lava Rock causes your fire spells to do 50% more damage to enemy troops.";
	ptrArtifact[EVERHOT_LAVA_ROCK].use = FALSE;

	ptrArtifact[LIGHTNING_ROD].name = "Lightning Rod";
	ptrArtifact[LIGHTNING_ROD].description = "The Lightning Rod causes your lightning spells to do 50% more damage to enemy troops.";
	ptrArtifact[LIGHTNING_ROD].use = FALSE;

	ptrArtifact[SNAKE_RING].name = "Snake-Ring";
	ptrArtifact[SNAKE_RING].description = "The Snake Ring halves the casting cost of all your bless spells.";
	ptrArtifact[SNAKE_RING].use = FALSE;

	ptrArtifact[ANKH].name = "Ankh";
	ptrArtifact[ANKH].description = "The Ankh doubles the effectiveness of all your resurrect and animate spells.";
	ptrArtifact[ANKH].use = FALSE;

	ptrArtifact[BOOK_ELEMENTS].name = "Book of Elements";
	ptrArtifact[BOOK_ELEMENTS].description = "The Book of Elements doubles the effectiveness of all your summoning spells.";
	ptrArtifact[BOOK_ELEMENTS].use = FALSE;

	ptrArtifact[ELEMENTAL_RING].name = "Elemental Ring";
	ptrArtifact[ELEMENTAL_RING].description = "The Elemental Ring halves the casting cost of all summoning spells.";
	ptrArtifact[ELEMENTAL_RING].use = FALSE;

	ptrArtifact[HOLY_PENDANT].name = "Holy Pendant";
	ptrArtifact[HOLY_PENDANT].description = "The Holy Pendant makes all your troops immune to curse spells.";
	ptrArtifact[HOLY_PENDANT].use = FALSE;

	ptrArtifact[PENDANT_FREE_WILL].name = "Pendant of Free Will";
	ptrArtifact[PENDANT_FREE_WILL].description = "The Pendant of Free Will makes all your troops immune to hypnotize spells.";
	ptrArtifact[PENDANT_FREE_WILL].use = FALSE;

	ptrArtifact[PENDANT_LIFE].name = "Pendant of Life";
	ptrArtifact[PENDANT_LIFE].description = "The Pendant of Life makes all your troops immune to death spells.";
	ptrArtifact[PENDANT_LIFE].use = FALSE;

	ptrArtifact[SERENITY_PENDANT].name = "Serenity Pendant";
	ptrArtifact[SERENITY_PENDANT].description = "The Serenity Pendant makes all your troops immune to berserk spells.";
	ptrArtifact[SERENITY_PENDANT].use = FALSE;

	ptrArtifact[SEEING_EYE_PENDANT].name = "Seeing-eye Pendant";
	ptrArtifact[SEEING_EYE_PENDANT].description = "The Seeing_eye Pendant makes all your troops immune to blindness spells.";
	ptrArtifact[SEEING_EYE_PENDANT].use = FALSE;

	ptrArtifact[KINETIC_PENDANT].name = "Kinetic Pendant";
	ptrArtifact[KINETIC_PENDANT].description = "The Kinetic Pendant makes all your troops immune to paralyze spells.";
	ptrArtifact[KINETIC_PENDANT].use = FALSE;

	ptrArtifact[PENDANT_DEATH].name = "Pendant of Death";
	ptrArtifact[PENDANT_DEATH].description = "The Pendant of Death makes all your troops immune to holy spells.";
	ptrArtifact[PENDANT_DEATH].use = FALSE;

	ptrArtifact[WAND_NEGATION].name = "Wand of Negation";
	ptrArtifact[WAND_NEGATION].description = "The Wand of Negation protects your troops from the Dispel Magic spell.";
	ptrArtifact[WAND_NEGATION].use = FALSE;

	ptrArtifact[GOLDEN_BOW].name = "Golden Bow";
	ptrArtifact[GOLDEN_BOW].description = "The Golden Bow eliminates the 50% penalty for your troops shooting past obstacles. (e.g. castle walls)";
	ptrArtifact[GOLDEN_BOW].use = FALSE;

	ptrArtifact[TELESCOPE].name = "Telescope";
	ptrArtifact[TELESCOPE].description = "The Telescope increases the amount of terrain your hero reveals when adventuring by 1 extra square.";
	ptrArtifact[TELESCOPE].use = FALSE;

	ptrArtifact[STATESMAN_QUILL].name = "Statesman's Quill";
	ptrArtifact[STATESMAN_QUILL].description = "The Statesman's Quill reduces the cost of surrender to 10% of the total cost of troops you have in your army.";
	ptrArtifact[STATESMAN_QUILL].use = FALSE;

	ptrArtifact[WIZARD_HAT].name = "Wizard's Hat";
	ptrArtifact[WIZARD_HAT].description = "The Wizard's Hat increases the duration of your spells by 10 turns!";
	ptrArtifact[WIZARD_HAT].use = FALSE;

	ptrArtifact[POWER_RING].name = "Power Ring";
	ptrArtifact[POWER_RING].description = "The Power Ring returns 2 extra power points/turn to your hero.";
	ptrArtifact[POWER_RING].use = FALSE;

	ptrArtifact[AMMO_CART].name = "Ammo Cart";
	ptrArtifact[AMMO_CART].description = "The Ammo Cart provides endless ammunition for all your troops that shoot.";
	ptrArtifact[AMMO_CART].use = FALSE;

	ptrArtifact[TAX_LIEN].name = "Tax Lien";
	ptrArtifact[TAX_LIEN].description = "The Tax Lien costs you 250 gold pieces/turn.";
	ptrArtifact[TAX_LIEN].use = FALSE;

	ptrArtifact[HIDEOUS_MASK].name = "Hideous Mask";
	ptrArtifact[HIDEOUS_MASK].description = "The Hideous Mask prevents all 'wandering' armies from joining your hero.";
	ptrArtifact[HIDEOUS_MASK].use = FALSE;

	ptrArtifact[ENDLESS_POUCH_SULFUR].name = "Endless Pouch of Sulfur";
	ptrArtifact[ENDLESS_POUCH_SULFUR].description = "The Endless Pouch of Sulfur provides 1 unit of sulfur per day.";
	ptrArtifact[ENDLESS_POUCH_SULFUR].use = FALSE;

	ptrArtifact[ENDLESS_VIAL_MERCURY].name = "Endless Vial of Mercury";
	ptrArtifact[ENDLESS_VIAL_MERCURY].description = "The Endless Vial of Mercury provides 1 unit of mercury per day.";
	ptrArtifact[ENDLESS_VIAL_MERCURY].use = FALSE;

	ptrArtifact[ENDLESS_POUCH_GEMS].name = "Endless Pouch of Gems";
	ptrArtifact[ENDLESS_POUCH_GEMS].description = "The Endless Pouch of Gems provides 1 unit of gems per day.";
	ptrArtifact[ENDLESS_POUCH_GEMS].use = FALSE;

	ptrArtifact[ENDLESS_CORD_WOOD].name = "Endless Cord of Wood";
	ptrArtifact[ENDLESS_CORD_WOOD].description = "The Endless Cord of Wood provides 1 unit of wood per day.";
	ptrArtifact[ENDLESS_CORD_WOOD].use = FALSE;

	ptrArtifact[ENDLESS_CART_ORE].name = "Endless Cart of Ore";
	ptrArtifact[ENDLESS_CART_ORE].description = "The Endless Cart of Ore provides 1 unit of ore per day.";
	ptrArtifact[ENDLESS_CART_ORE].use = FALSE;

	ptrArtifact[ENDLESS_POUCH_CRYSTAL].name = "Endless Pouch of Crystal";
	ptrArtifact[ENDLESS_POUCH_CRYSTAL].description = "The Endless Pouch of Crystal provides 1 unit of crystal/day.";
	ptrArtifact[ENDLESS_POUCH_CRYSTAL].use = FALSE;

	ptrArtifact[SPIKED_HELM].name = "Spiked Helm";
	ptrArtifact[SPIKED_HELM].description = "The Spiked Helm increases your attack and defense skills by 1 each.";
	ptrArtifact[SPIKED_HELM].use = FALSE;

	ptrArtifact[SPIKED_SHIELD].name = "Spiked Shield";
	ptrArtifact[SPIKED_SHIELD].description = "The Spiked Shield increases your attack and defense skills by 2 each.";
	ptrArtifact[SPIKED_SHIELD].use = FALSE;

	ptrArtifact[WHITE_PEARL].name = "White Pearl";
	ptrArtifact[WHITE_PEARL].description = "The White Pearl increases your spell power and knowledge by 1 each.";
	ptrArtifact[WHITE_PEARL].use = FALSE;

	ptrArtifact[BLACK_PEARL].name = "Black Pearl";
	ptrArtifact[BLACK_PEARL].description = "The Black Pearl increases your spell power and knowledge by 2 each.";
	ptrArtifact[BLACK_PEARL].use = FALSE;

	ptrArtifact[NOMAD_BOOTS_MOBILITY].name = "Nomad Boots of Mobility";
	ptrArtifact[NOMAD_BOOTS_MOBILITY].description = "The Nomad Boots of Mobility increase your movement on land.";
	ptrArtifact[NOMAD_BOOTS_MOBILITY].use = FALSE;

	ptrArtifact[TRAVELER_BOOTS_MOBILITY].name = "Traveler's Boots of Mobility";
	ptrArtifact[TRAVELER_BOOTS_MOBILITY].description = "The Traveler's Boots of Mobility increase your movement on land.";
	ptrArtifact[TRAVELER_BOOTS_MOBILITY].use = FALSE;

	ptrArtifact[TRUE_COMPASS_MOBILITY].name = "True Compass of Mobility";
	ptrArtifact[TRUE_COMPASS_MOBILITY].description = "The True Compass of Mobility increases your movement on land and sea.";
	ptrArtifact[TRUE_COMPASS_MOBILITY].use = FALSE;

	ptrArtifact[SAILORS_ASTROLABE_MOBILITY].name = "Sailor's Astrolabe of Mobility";
	ptrArtifact[SAILORS_ASTROLABE_MOBILITY].description = "The Sailors' Astrolabe of Mobility increases your movement on sea.";
	ptrArtifact[SAILORS_ASTROLABE_MOBILITY].use = FALSE;

	ptrArtifact[MAGIC_BOOK].name = "Magic Book";
	ptrArtifact[MAGIC_BOOK].description = "The Magic Book enables you to cast spells.";
	ptrArtifact[MAGIC_BOOK].use = TRUE;

    fprintf(stderr, "Init artifact.\n");	

    return TRUE;
}
