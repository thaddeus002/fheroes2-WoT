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

static BOOL useArtifact[ARTIFACTMAXCOUNT];

            
E_ARTIFACT CheckValidArtifact(Uint8 type){

    E_ARTIFACT a;
            
    for(a = ULTIMATE_BOOK; a <= RND_ARTIFACT3; ++a)
        if(a == type)
            return a;

    if(GetIntValue(DEBUG))
        fprintf(stderr, "CheckValidArtifact: unknown object: 0x%hhX\n", type);

    return ARTIFACTNONE;
}

E_ARTIFACT GetRNDArtifact(E_LEVELARTIFACT level){

    Uint8 i;

    E_ARTIFACT level1[] = { MEDAL_VALOR, MEDAL_COURAGE, MEDAL_HONOR, MEDAL_DISTINCTION, THUNDER_MACE, ARMORED_GAUNTLETS, DEFENDER_HELM, GIANT_FLAIL, RABBIT_FOOT, GOLDEN_HORSESHOE, GAMBLER_LUCKY_COIN, FOUR_LEAF_CLOVER, ENCHANTED_HOURGLASS, ICE_CLOAK, FIRE_CLOAK, LIGHTNING_HELM, SNAKE_RING, HOLY_PENDANT, PENDANT_FREE_WILL, PENDANT_LIFE, PENDANT_DEATH, GOLDEN_BOW, TELESCOPE, SERENITY_PENDANT, STATESMAN_QUILL, KINETIC_PENDANT, SEEING_EYE_PENDANT };
    E_ARTIFACT level2[] = { CASTER_BRACELET, MAGE_RING, STEALTH_SHIELD, POWER_AXE, MINOR_SCROLL, ENDLESS_PURSE_GOLD, SAILORS_ASTROLABE_MOBILITY, ENDLESS_CORD_WOOD, ENDLESS_CART_ORE, SPIKED_HELM, WHITE_PEARL, EVIL_EYE, GOLD_WATCH, ANKH, BOOK_ELEMENTS, ELEMENTAL_RING, SKULLCAP, EVERCOLD_ICICLE, POWER_RING, AMMO_CART, EVERHOT_LAVA_ROCK };
    E_ARTIFACT level3[] = { ARCANE_NECKLACE, WITCHES_BROACH, BALLISTA, DRAGON_SWORD, DIVINE_BREASTPLATE, MAJOR_SCROLL, SUPERIOR_SCROLL, FOREMOST_SCROLL, ENDLESS_SACK_GOLD, ENDLESS_BAG_GOLD, NOMAD_BOOTS_MOBILITY, TRAVELER_BOOTS_MOBILITY, TRUE_COMPASS_MOBILITY, ENDLESS_POUCH_SULFUR, ENDLESS_POUCH_GEMS, ENDLESS_POUCH_CRYSTAL, ENDLESS_VIAL_MERCURY, SPIKED_SHIELD, BLACK_PEARL, LIGHTNING_ROD, WAND_NEGATION, WIZARD_HAT };
    E_ARTIFACT result;

    switch(level){
    
	case ART_LEVEL1:
	    result = level1[rand() % ARTIFACTCOUNT_LEVEL1];
	    if(! GetIntValue(UNIQUEARTIFACT)) return result;
	    for(i = 0; i < ARTIFACTCOUNT_LEVEL1; ++i)
		if(! useArtifact[level1[i]]){
		    while(useArtifact[result]) result = level1[rand() % ARTIFACTCOUNT_LEVEL1];
		    useArtifact[result] = TRUE;
		}
	    if(GetIntValue(DEBUG)) fprintf(stderr, "GetRNDArtifact: ART_LEVEL1 all busy. Use uniqueartifact = off.\n");
	    return result;
	    break;
	    
	case ART_LEVEL2:
	    result = level2[rand() % ARTIFACTCOUNT_LEVEL2];
	    if(! GetIntValue(UNIQUEARTIFACT)) return result;
	    for(i = 0; i < ARTIFACTCOUNT_LEVEL2; ++i)
		if(! useArtifact[level2[i]]){
		    while(useArtifact[result]) result = level2[rand() % ARTIFACTCOUNT_LEVEL2];
		    useArtifact[result] = TRUE;
		}
	    if(GetIntValue(DEBUG)) fprintf(stderr, "GetRNDArtifact: ART_LEVEL2 all busy. Use uniqueartifact = off.\n");
	    return result;
	    break;
	    
	case ART_LEVEL3:
	    result = level3[rand() % ARTIFACTCOUNT_LEVEL3];
	    if(! GetIntValue(UNIQUEARTIFACT)) return result;
	    for(i = 0; i < ARTIFACTCOUNT_LEVEL3; ++i)
		if(! useArtifact[level3[i]]){
		    while(useArtifact[result]) result = level3[rand() % ARTIFACTCOUNT_LEVEL3];
		    useArtifact[result] = TRUE;
		}
	    if(GetIntValue(DEBUG)) fprintf(stderr, "GetRNDArtifact: ART_LEVEL3 all busy. Use uniqueartifact = off.\n");
	    return result;
	    break;

	default:
	    break;
    }

    // ART_ALL
    result = 9 + (rand() % (ARTIFACTMAXCOUNT - 10));
    if(! GetIntValue(UNIQUEARTIFACT)) return result;
    for(i = 9; i < ARTIFACTMAXCOUNT - 10; ++i)
	if(! useArtifact[i]){
	    while(useArtifact[result]) result = 9 + (rand() % (ARTIFACTMAXCOUNT - 10));
	    useArtifact[result] = TRUE;
	}
    if(GetIntValue(DEBUG)) fprintf(stderr, "GetRNDArtifact: ART_ALL all busy. Use uniqueartifact = off.\n");

    return result;
}

void	FreeArtifact(void){
    return;
}

BOOL    InitArtifact(void){

    Uint8 i;
    
    for(i = 0; i < ARTIFACTMAXCOUNT; ++i)
	useArtifact[i] = FALSE;

    return TRUE;
}

const char * GetStringArtifact(E_ARTIFACT art){

    const char *string = NULL;

    switch(art){
	case ULTIMATE_BOOK:
	    string = "Ultimate Book";
	    break;
	case ULTIMATE_SWORD:
	    string = "Ultimate Sword";
	    break;
	case ULTIMATE_CLOAK:
	    string = "Ultimate Cloak";
	    break;
	case ULTIMATE_WAND:
	    string = "Ultimate Wand";
	    break;
	case ULTIMATE_SHIELD:
	    string = "Ultimate Shield";
	    break;
	case ULTIMATE_STAFF:
	    string = "Ultimate Staff";
	    break;
	case ULTIMATE_CROWN:
	    string = "Ultimate Crown";
	    break;
	case GOLDEN_GOOSE:
	    string = "Ultimate Goose";
	    break;
	case ARCANE_NECKLACE:
	    string = "Arcane Necklace";
	    break;
	case CASTER_BRACELET:
	    string = "Caster's Bracelet";
	    break;
	case MAGE_RING:
	    string = "Mage's Ring";
	    break;
	case MEDAL_VALOR:
	    string = "Medal of Valor";
	    break;
	case WITCHES_BROACH:
	    string = "Witches Broach";
	    break;
	case MEDAL_COURAGE:
	    string = "Medal of Courage";
	    break;
	case MEDAL_HONOR:
	    string = "Medal of Honor";
	    break;
	case MEDAL_DISTINCTION:
	    string = "Medal of Distinction";
	    break;
	case FIZBIN_MISFORTUNE:
	    string = "Fizbin of Misfortune";
	    break;
	case THUNDER_MACE:
	    string = "Thunder Mace";
	    break;
	case ARMORED_GAUNTLETS:
	    string = "Armored Gauntlets";
	    break;
	case DEFENDER_HELM:
	    string = "Defender Helm";
	    break;
	case GIANT_FLAIL:
	    string = "Giant Flail";
	    break;
	case BALLISTA:
	    string = "Ballista";
	    break;
	case STEALTH_SHIELD:
	    string = "Stealth Shield";
	    break;
	case DRAGON_SWORD:
	    string = "Dragon Sword";
	    break;
	case POWER_AXE:
	    string = "Power Axe";
	    break;
	case DIVINE_BREASTPLATE:
	    string = "Divine Breastplate";
	    break;
	case MINOR_SCROLL:
	    string = "Minor Scroll";
	    break;
	case MAJOR_SCROLL:
	    string = "Major Scroll";
	    break;
	case SUPERIOR_SCROLL:
	    string = "Superior Scroll";
	    break;
	case FOREMOST_SCROLL:
	    string = "Foremost Scroll";
	    break;
	case ENDLESS_SACK_GOLD:
	    string = "Endless Sack of Gold";
	    break;
	case ENDLESS_BAG_GOLD:
	    string = "Endless Bag of Gold";
	    break;
	case ENDLESS_PURSE_GOLD:
	    string = "Endless Purse of Gold";
	    break;
	case RABBIT_FOOT:
	    string = "Rabbit's Foot";
	    break;
	case GOLDEN_HORSESHOE:
	    string = "Golden Horseshoe";
	    break;
	case GAMBLER_LUCKY_COIN:
	    string = "Gambler's Lucky Coin";
	    break;
	case FOUR_LEAF_CLOVER:
	    string = "Four-Leaf Clover";
	    break;
	case EVIL_EYE:
	    string = "Evil Eye";
	    break;
	case ENCHANTED_HOURGLASS:
	    string = "Enchanted Hourglass";
	    break;
	case GOLD_WATCH:
	    string = "Gold Wath";
	    break;
	case SKULLCAP:
	    string = "Skullcap";
	    break;
	case ICE_CLOAK:
	    string = "Ice Clock";
	    break;
	case FIRE_CLOAK:
	    string = "Fire Cloak";
	    break;
	case LIGHTNING_HELM:
	    string = "Lightning Helm";
	    break;
	case EVERCOLD_ICICLE:
	    string = "Evercold Icicle";
	    break;
	case EVERHOT_LAVA_ROCK:
	    string = "Everhot Lava Rock";
	    break;
	case LIGHTNING_ROD:
	    string = "Lightning Rod";
	    break;
	case SNAKE_RING:
	    string = "Snake-Ring";
	    break;
	case ANKH:
	    string = "Ankh";
	    break;
	case BOOK_ELEMENTS:
	    string = "Book of Elements";
	    break;
	case ELEMENTAL_RING:
	    string = "Elemental Ring";
	    break;
	case HOLY_PENDANT:
	    string = "Holy Pendant";
	    break;
	case PENDANT_FREE_WILL:
	    string = "Pendant of Free Will";
	    break;
	case PENDANT_LIFE:
	    string = "Pendant of Life";
	    break;
	case SERENITY_PENDANT:
	    string = "Serenity Pendant";
	    break;
	case SEEING_EYE_PENDANT:
	    string = "Seeing-eye Pendant";
	    break;
	case KINETIC_PENDANT:
	    string = "Kinetic Pendant";
	    break;
	case PENDANT_DEATH:
	    string = "Pendant of Death";
	    break;
	case WAND_NEGATION:
	    string = "Wand of Negation";
	    break;
	case GOLDEN_BOW:
	    string = "Golden Bow";
	    break;
	case TELESCOPE:
	    string = "Telescope";
	    break;
	case STATESMAN_QUILL:
	    string = "Statesman's Quill";
	    break;
	case WIZARD_HAT:
	    string = "Wizard's Hat";
	    break;
	case POWER_RING:
	    string = "Power Ring";
	    break;
	case AMMO_CART:
	    string = "Ammo Cart";
	    break;
	case TAX_LIEN:
	    string = "Tax Lien";
	    break;
	case HIDEOUS_MASK:
	    string = "Hideous Mask";
	    break;
	case ENDLESS_POUCH_SULFUR:
	    string = "Endless Pouch of Sulfur";
	    break;
	case ENDLESS_VIAL_MERCURY:
	    string = "Endless Vial of Mercury";
	    break;
	case ENDLESS_POUCH_GEMS:
	    string = "Endless Pouch of Gems";
	    break;
	case ENDLESS_CORD_WOOD:
	    string = "Endless Cord of Wood";
	    break;
	case ENDLESS_CART_ORE:
	    string = "Endless Cart of Ore";
	    break;
	case ENDLESS_POUCH_CRYSTAL:
	    string = "Endless Pouch of Crystal";
	    break;
	case SPIKED_HELM:
	    string = "Spiked Helm";
	    break;
	case SPIKED_SHIELD:
	    string = "Spiked Shield";
	    break;
	case WHITE_PEARL:
	    string = "White Pearl";
	    break;
	case BLACK_PEARL:
	    string = "Black Pearl";
	    break;
	case NOMAD_BOOTS_MOBILITY:
	    string = "Nomad Boots of Mobility";
	    break;
	case TRAVELER_BOOTS_MOBILITY:
	    string = "Traveler's Boots of Mobility";
	    break;
	case TRUE_COMPASS_MOBILITY:
	    string = "True Compass of Mobility";
	    break;
	case SAILORS_ASTROLABE_MOBILITY:
	    string = "Sailor's Astrolabe of Mobility";
	    break;
	case MAGIC_BOOK:
	    string = "Magic Book";
	    break;

	default:
	    break;
    }
    
    return string;
}

const char * GetStringDescriptionsArtifact(E_ARTIFACT art){

    const char *string = NULL;

    switch(art){
	    break;
	case FIZBIN_MISFORTUNE:
	    string = "The Fizbin of Misfortune greatly decreases your morale.";
	    break;
	case MEDAL_DISTINCTION:
	    string = "The Medal of Distinction increases your morale.";
	    break;
	case MEDAL_HONOR:
	    string = "The Medal of Honor increases your morale.";
	    break;
	case MEDAL_COURAGE:
	    string = "The Medal of Courage increases your morale.";
	    break;
	case MEDAL_VALOR:
	    string = "The Medal of Valor increases your morale.";
	    break;
	case WITCHES_BROACH:
	    string = "The Witch's Broach of Magic increases your spell power by 3.";
	    break;
	case MAGE_RING:
	    string = "The Mage's Ring of Power increases your spell power by 2.";
	    break;
	case CASTER_BRACELET:
	    string = "The Caster's Bracelet of Magic increases your spell power by 2.";
	    break;
	case ARCANE_NECKLACE:
	    string = "The Arcane Necklace of Magic increases your spell power by 4.";
	    break;
	case GOLDEN_GOOSE:
	    string = "The Golden Goose brings in an income of 10,000 gold per turn.";
	    break;
	case ULTIMATE_CROWN:
	    string = "The Ultimate Crown increases each of your basic skills by 4 points.";
	    break;
	case ULTIMATE_STAFF:
	    string = "The Ultimate Staff increases your spell power and knowledge by 6 each.";
	    break;
	case ULTIMATE_SHIELD:
	    string = "The Ultimate Shield increases your attack and defense skills by 6 each.";
	    break;
	case ULTIMATE_WAND:
	    string = "The Ultimate Wand of Magic increases your spell power by 12.";
	    break;
	case ULTIMATE_CLOAK:
	    string = "The Ultimate Cloak of Protection increases your defense skill by 12.";
	    break;
	case ULTIMATE_SWORD:
	    string = "The Ultimate Sword of Dominion increases your attack skill by 12.";
	    break;
	case ULTIMATE_BOOK:
	    string = "The Ultimate Book of Knowledge increases your knowledge by 12.";
	    break;
	case THUNDER_MACE:
	    string = "The Thunder Mace of Dominion increases your attack skill by 1.";
	    break;
	case ARMORED_GAUNTLETS:
	    string = "The Armored Gauntlets of Protection increase your defense skill by 1.";
	    break;
	case DEFENDER_HELM:
	    string = "The Defender Helm of Protection increases your defense skill by 1.";
	    break;
	case GIANT_FLAIL:
	    string = "The Giant Flail of Dominion increases your attack skill by 1.";
	    break;
	case BALLISTA:
	    string = "The Ballista of Quickness lets your catapult fire twice per combat round.";
	    break;
	case STEALTH_SHIELD:
	    string = "The Stealth Shield of Protection increases your defense skill by 2.";
	    break;
	case DRAGON_SWORD:
	    string = "The Dragon Sword of Dominion increases your attack skill by 3.";
	    break;
	case POWER_AXE:
	    string = "The Power Axe of Dominion increases your attack skill by 2.";
	    break;
	case DIVINE_BREASTPLATE:
	    string = "The Divine Breastplate of Protection increases your defense skill by 3.";
	    break;
	case MINOR_SCROLL:
	    string = "The Minor Scroll of Knowledge increases your knowledge by 2.";
	    break;
	case MAJOR_SCROLL:
	    string = "The Major Scroll of Knowledge increases your knowledge by 3.";
	    break;
	case SUPERIOR_SCROLL:
	    string = "The Superior Scroll of Knowledge increases your knowledge by 4.";
	    break;
	case FOREMOST_SCROLL:
	    string = "The Foremost Scroll of Knowledge increases your knowledge by 5.";
	    break;
	case ENDLESS_SACK_GOLD:
	    string = "The Endless Sack of Gold provides you with 1000 gold per day.";
	    break;
	case ENDLESS_BAG_GOLD:
	    string = "The Endless Bag of Gold provides you with 750 gold per day.";
	    break;
	case ENDLESS_PURSE_GOLD:
	    string = "The Endless Purse of Gold provides you with 500 gold per day.";
	    break;
	case RABBIT_FOOT:
	    string = "The Lucky Rabbit's Foot increases your luck in combat.";
	    break;
	case GOLDEN_HORSESHOE:
	    string = "The Golden Horseshoe increases your luck in combat.";
	    break;
	case GAMBLER_LUCKY_COIN:
	    string = "The Gambler's Lucky Coin increases your luck in combat.";
	    break;
	case FOUR_LEAF_CLOVER:
	    string = "The Four_Leaf Clover increases your luck in combat.";
	    break;
	case EVIL_EYE:
	    string = "The Evil Eye reduces the casting cost of curse spells by half.";
	    break;
	case ENCHANTED_HOURGLASS:
	    string = "The Enchanted Hourglass extends the duration of all your spells by 2 turns.";
	    break;
	case GOLD_WATCH:
	    string = "The Gold Watch doubles the effectiveness of your hypnotize spells.";
	    break;
	case SKULLCAP:
	    string = "The Skullcap halves the casting cost of all mind influencing spells.";
	    break;
	case ICE_CLOAK:
	    string = "The Ice Cloak halves all damage your troops take from cold spells.";
	    break;
	case FIRE_CLOAK:
	    string = "The Fire Cloak halves all damage your troops take from fire spells.";
	    break;
	case LIGHTNING_HELM:
	    string = "The Lightning Helm halves all damage your troops take from lightning spells.";
	    break;
	case HIDEOUS_MASK:
	    string = "The Hideous Mask prevents all 'wandering' armies from joining your hero.";
	    break;
	case TAX_LIEN:
	    string = "The Tax Lien costs you 250 gold pieces/turn.";
	    break;
	case AMMO_CART:
	    string = "The Ammo Cart provides endless ammunition for all your troops that shoot.";
	    break;
	case POWER_RING:
	    string = "The Power Ring returns 2 extra power points/turn to your hero.";
	    break;
	case WIZARD_HAT:
	    string = "The Wizard's Hat increases the duration of your spells by 10 turns!";
	    break;
	case STATESMAN_QUILL:
	    string = "The Statesman's Quill reduces the cost of surrender to 10% of the total cost of troops you have in your army.";
	    break;
	case TELESCOPE:
	    string = "The Telescope increases the amount of terrain your hero reveals when adventuring by 1 extra square.";
	    break;
	case GOLDEN_BOW:
	    string = "The Golden Bow eliminates the 50% penalty for your troops shooting past obstacles. (e.g. castle walls)";
	    break;
	case WAND_NEGATION:
	    string = "The Wand of Negation protects your troops from the Dispel Magic spell.";
	    break;
	case PENDANT_DEATH:
	    string = "The Pendant of Death makes all your troops immune to holy spells.";
	    break;
	case KINETIC_PENDANT:
	    string = "The Kinetic Pendant makes all your troops immune to paralyze spells.";
	    break;
	case SEEING_EYE_PENDANT:
	    string = "The Seeing_eye Pendant makes all your troops immune to blindness spells.";
	    break;
	case SERENITY_PENDANT:
	    string = "The Serenity Pendant makes all your troops immune to berserk spells.";
	    break;
	case PENDANT_LIFE:
	    string = "The Pendant of Life makes all your troops immune to death spells.";
	    break;
	case PENDANT_FREE_WILL:
	    string = "The Pendant of Free Will makes all your troops immune to hypnotize spells.";
	    break;
	case HOLY_PENDANT:
	    string = "The Holy Pendant makes all your troops immune to curse spells.";
	    break;
	case ELEMENTAL_RING:
	    string = "The Elemental Ring halves the casting cost of all summoning spells.";
	    break;
	case BOOK_ELEMENTS:
	    string = "The Book of Elements doubles the effectiveness of all your summoning spells.";
	    break;
	case ANKH:
	    string = "The Ankh doubles the effectiveness of all your resurrect and animate spells.";
	    break;
	case SNAKE_RING:
	    string = "The Snake Ring halves the casting cost of all your bless spells.";
	    break;
	case MAGIC_BOOK:
	    string = "The Magic Book enables you to cast spells.";
	    break;
	case SAILORS_ASTROLABE_MOBILITY:
	    string = "The Sailors' Astrolabe of Mobility increases your movement on sea.";
	    break;
	case TRUE_COMPASS_MOBILITY:
	    string = "The True Compass of Mobility increases your movement on land and sea.";
	    break;
	case TRAVELER_BOOTS_MOBILITY:
	    string = "The Traveler's Boots of Mobility increase your movement on land.";
	    break;
	case NOMAD_BOOTS_MOBILITY:
	    string = "The Nomad Boots of Mobility increase your movement on land.";
	    break;
	case BLACK_PEARL:
	    string = "The Black Pearl increases your spell power and knowledge by 2 each.";
	    break;
	case WHITE_PEARL:
	    string = "The White Pearl increases your spell power and knowledge by 1 each.";
	    break;
	case SPIKED_SHIELD:
	    string = "The Spiked Shield increases your attack and defense skills by 2 each.";
	    break;
	case SPIKED_HELM:
	    string = "The Spiked Helm increases your attack and defense skills by 1 each.";
	    break;
	case ENDLESS_POUCH_CRYSTAL:
	    string = "The Endless Pouch of Crystal provides 1 unit of crystal/day.";
	    break;
	case ENDLESS_CART_ORE:
	    string = "The Endless Cart of Ore provides 1 unit of ore per day.";
	    break;
	case ENDLESS_CORD_WOOD:
	    string = "The Endless Cord of Wood provides 1 unit of wood per day.";
	    break;
	case ENDLESS_POUCH_GEMS:
	    string = "The Endless Pouch of Gems provides 1 unit of gems per day.";
	    break;
	case ENDLESS_VIAL_MERCURY:
	    string = "The Endless Vial of Mercury provides 1 unit of mercury per day.";
	    break;
	case ENDLESS_POUCH_SULFUR:
	    string = "The Endless Pouch of Sulfur provides 1 unit of sulfur per day.";
	    break;
	
	default:
	    break;
    }
    
    return string;
}
