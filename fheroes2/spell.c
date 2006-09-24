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

E_SPELL GetRandomSpell(E_SPELLTYPE, E_MAGICLEVEL);

static S_SPELL	*ptrSpell = NULL;

BOOL InitSpell(void){

    if(NULL == (ptrSpell = malloc(SPELLMAXCOUNT * sizeof(S_SPELL)))){
	fprintf(stderr, "InitSpel: error malloc: %d\n", SPELLMAXCOUNT * sizeof(S_SPELL));
	return FALSE;
    }
    
    ptrSpell[FIREBALL].cost = 9;
    ptrSpell[FIREBALL].level = MAGIC_LEVEL3;
    ptrSpell[FIREBALL].type = COMBAT;

    ptrSpell[FIREBLAST].cost = 15;
    ptrSpell[FIREBLAST].level = MAGIC_LEVEL4;
    ptrSpell[FIREBLAST].type = COMBAT;

    ptrSpell[LIGHTNINGBOLT].cost = 7;
    ptrSpell[LIGHTNINGBOLT].level = MAGIC_LEVEL2;
    ptrSpell[LIGHTNINGBOLT].type = COMBAT;

    ptrSpell[CHAINLIGHTNING].cost = 15;
    ptrSpell[CHAINLIGHTNING].level = MAGIC_LEVEL4;
    ptrSpell[CHAINLIGHTNING].type = COMBAT;

    ptrSpell[TELEPORT].cost = 9;
    ptrSpell[TELEPORT].level = MAGIC_LEVEL3;
    ptrSpell[TELEPORT].type = COMBAT;

    ptrSpell[CURE].cost = 6;
    ptrSpell[CURE].level = MAGIC_LEVEL1;
    ptrSpell[CURE].type = COMBAT;

    ptrSpell[MASSCURE].cost = 15;
    ptrSpell[MASSCURE].level = MAGIC_LEVEL4;
    ptrSpell[MASSCURE].type = COMBAT;

    ptrSpell[RESURRECT].cost = 12;
    ptrSpell[RESURRECT].level = MAGIC_LEVEL4;
    ptrSpell[RESURRECT].type = COMBAT;

    ptrSpell[RESURRECTTRUE].cost = 15;
    ptrSpell[RESURRECTTRUE].level = MAGIC_LEVEL5;
    ptrSpell[RESURRECTTRUE].type = COMBAT;

    ptrSpell[HASTE].cost = 3;
    ptrSpell[HASTE].level = MAGIC_LEVEL1;
    ptrSpell[HASTE].type = COMBAT;

    ptrSpell[MASSHASTE].cost = 10;
    ptrSpell[MASSHASTE].level = MAGIC_LEVEL3;
    ptrSpell[MASSHASTE].type = COMBAT;

    ptrSpell[SPELLSLOW].cost = 3;
    ptrSpell[SPELLSLOW].level = MAGIC_LEVEL1;
    ptrSpell[SPELLSLOW].type = COMBAT;

    ptrSpell[MASSSLOW].cost = 15;
    ptrSpell[MASSSLOW].level = MAGIC_LEVEL4;
    ptrSpell[MASSSLOW].type = COMBAT;

    ptrSpell[BLIND].cost = 6;
    ptrSpell[BLIND].level = MAGIC_LEVEL2;
    ptrSpell[BLIND].type = COMBAT;

    ptrSpell[BLESS].cost = 3;
    ptrSpell[BLESS].level = MAGIC_LEVEL1;
    ptrSpell[BLESS].type = COMBAT;

    ptrSpell[MASSBLESS].cost = 12;
    ptrSpell[MASSBLESS].level = MAGIC_LEVEL3;
    ptrSpell[MASSBLESS].type = COMBAT;

    ptrSpell[STONESKIN].cost = 3;
    ptrSpell[STONESKIN].level = MAGIC_LEVEL1;
    ptrSpell[STONESKIN].type = COMBAT;

    ptrSpell[STEELSKIN].cost = 6;
    ptrSpell[STEELSKIN].level = MAGIC_LEVEL2;
    ptrSpell[STEELSKIN].type = COMBAT;

    ptrSpell[CURSE].cost = 3;
    ptrSpell[CURSE].level = MAGIC_LEVEL1;
    ptrSpell[CURSE].type = COMBAT;

    ptrSpell[MASSCURSE].cost = 12;
    ptrSpell[MASSCURSE].level = MAGIC_LEVEL3;
    ptrSpell[MASSCURSE].type = COMBAT;

    ptrSpell[HOLYWORD].cost = 9;
    ptrSpell[HOLYWORD].level = MAGIC_LEVEL3;
    ptrSpell[HOLYWORD].type = COMBAT;

    ptrSpell[HOLYSHOUT].cost = 12;
    ptrSpell[HOLYSHOUT].level = MAGIC_LEVEL4;
    ptrSpell[HOLYSHOUT].type = COMBAT;

    ptrSpell[ANTIMAGIC].cost = 7;
    ptrSpell[ANTIMAGIC].level = MAGIC_LEVEL3;
    ptrSpell[ANTIMAGIC].type = COMBAT;

    ptrSpell[DISPELMAGIC].cost = 5;
    ptrSpell[DISPELMAGIC].level = MAGIC_LEVEL1;
    ptrSpell[DISPELMAGIC].type = COMBAT;

    ptrSpell[MASSDISPEL].cost = 12;
    ptrSpell[MASSDISPEL].level = MAGIC_LEVEL3;
    ptrSpell[MASSDISPEL].type = COMBAT;

    ptrSpell[MAGICARROW].cost = 3;
    ptrSpell[MAGICARROW].level = MAGIC_LEVEL1;
    ptrSpell[MAGICARROW].type = COMBAT;

    ptrSpell[BERZERKER].cost = 12;
    ptrSpell[BERZERKER].level = MAGIC_LEVEL4;
    ptrSpell[BERZERKER].type = COMBAT;

    ptrSpell[ARMAGEDDON].cost = 20;
    ptrSpell[ARMAGEDDON].level = MAGIC_LEVEL5;
    ptrSpell[ARMAGEDDON].type = COMBAT;

    ptrSpell[ELEMENTALSTORM].cost = 15;
    ptrSpell[ELEMENTALSTORM].level = MAGIC_LEVEL4;
    ptrSpell[ELEMENTALSTORM].type = COMBAT;

    ptrSpell[METEORSHOWER].cost = 15;
    ptrSpell[METEORSHOWER].level = MAGIC_LEVEL4;
    ptrSpell[METEORSHOWER].type = COMBAT;

    ptrSpell[PARALYZE].cost = 9;
    ptrSpell[PARALYZE].level = MAGIC_LEVEL3;
    ptrSpell[PARALYZE].type = COMBAT;

    ptrSpell[HYPNOTIZE].cost = 15;
    ptrSpell[HYPNOTIZE].level = MAGIC_LEVEL5;
    ptrSpell[HYPNOTIZE].type = COMBAT;

    ptrSpell[COLDRAY].cost = 6;
    ptrSpell[COLDRAY].level = MAGIC_LEVEL2;
    ptrSpell[COLDRAY].type = COMBAT;

    ptrSpell[COLDRING].cost = 9;
    ptrSpell[COLDRING].level = MAGIC_LEVEL3;
    ptrSpell[COLDRING].type = COMBAT;

    ptrSpell[DISRUPTINGRAY].cost = 7;
    ptrSpell[DISRUPTINGRAY].level = MAGIC_LEVEL2;
    ptrSpell[DISRUPTINGRAY].type = COMBAT;

    ptrSpell[DEATHRIPPLE].cost = 6;
    ptrSpell[DEATHRIPPLE].level = MAGIC_LEVEL2;
    ptrSpell[DEATHRIPPLE].type = COMBAT;

    ptrSpell[DEATHWAVE].cost = 10;
    ptrSpell[DEATHWAVE].level = MAGIC_LEVEL3;
    ptrSpell[DEATHWAVE].type = COMBAT;

    ptrSpell[DRAGONSLAYER].cost = 6;
    ptrSpell[DRAGONSLAYER].level = MAGIC_LEVEL2;
    ptrSpell[DRAGONSLAYER].type = COMBAT;

    ptrSpell[BLOODLUST].cost = 3;
    ptrSpell[BLOODLUST].level = MAGIC_LEVEL1;
    ptrSpell[BLOODLUST].type = COMBAT;

    ptrSpell[ANIMATEDEAD].cost = 10;
    ptrSpell[ANIMATEDEAD].level = MAGIC_LEVEL3;
    ptrSpell[ANIMATEDEAD].type = COMBAT;

    ptrSpell[MIRRORIMAGE].cost = 25;
    ptrSpell[MIRRORIMAGE].level = MAGIC_LEVEL5;
    ptrSpell[MIRRORIMAGE].type = COMBAT;

    ptrSpell[SHIELD].cost = 3;
    ptrSpell[SHIELD].level = MAGIC_LEVEL1;
    ptrSpell[SHIELD].type = COMBAT;

    ptrSpell[MASSSHIELD].cost = 7;
    ptrSpell[MASSSHIELD].level = MAGIC_LEVEL4;
    ptrSpell[MASSSHIELD].type = COMBAT;

    ptrSpell[EARTHSUMMON].cost = 30;
    ptrSpell[EARTHSUMMON].level = MAGIC_LEVEL5;
    ptrSpell[EARTHSUMMON].type = COMBAT;
    
    ptrSpell[AIRSUMMON].cost = 30;
    ptrSpell[AIRSUMMON].level = MAGIC_LEVEL5;
    ptrSpell[AIRSUMMON].type = COMBAT;

    ptrSpell[FIRESUMMON].cost = 30;
    ptrSpell[FIRESUMMON].level = MAGIC_LEVEL5;
    ptrSpell[FIRESUMMON].type = COMBAT;

    ptrSpell[WATERSUMMON].cost = 30;
    ptrSpell[WATERSUMMON].level = MAGIC_LEVEL5;
    ptrSpell[WATERSUMMON].type = COMBAT;

    ptrSpell[EARTHQUAKE].cost = 15;
    ptrSpell[EARTHQUAKE].level = MAGIC_LEVEL3;
    ptrSpell[EARTHQUAKE].type = COMBAT;

    ptrSpell[VIEWMINES].cost = 1;
    ptrSpell[VIEWMINES].level = MAGIC_LEVEL1;
    ptrSpell[VIEWMINES].type = ADVENTURE;

    ptrSpell[VIEWRESOURCES].cost = 1;
    ptrSpell[VIEWRESOURCES].level = MAGIC_LEVEL1;
    ptrSpell[VIEWRESOURCES].type = ADVENTURE;

    ptrSpell[VIEWARTIFACTS].cost = 2;
    ptrSpell[VIEWARTIFACTS].level = MAGIC_LEVEL2;
    ptrSpell[VIEWARTIFACTS].type = ADVENTURE;

    ptrSpell[VIEWTOWNS].cost = 2;
    ptrSpell[VIEWTOWNS].level = MAGIC_LEVEL3;
    ptrSpell[VIEWTOWNS].type = ADVENTURE;

    ptrSpell[VIEWHEROES].cost = 2;
    ptrSpell[VIEWHEROES].level = MAGIC_LEVEL3;
    ptrSpell[VIEWHEROES].type = ADVENTURE;

    ptrSpell[VIEWALL].cost = 3;
    ptrSpell[VIEWALL].level = MAGIC_LEVEL4;
    ptrSpell[VIEWALL].type = ADVENTURE;

    ptrSpell[IDENTIFYHERO].cost = 3;
    ptrSpell[IDENTIFYHERO].level = MAGIC_LEVEL3;
    ptrSpell[IDENTIFYHERO].type = ADVENTURE;

    ptrSpell[SUMMONBOAT].cost = 5;
    ptrSpell[SUMMONBOAT].level = MAGIC_LEVEL2;
    ptrSpell[SUMMONBOAT].type = ADVENTURE;

    ptrSpell[DIMENSIONDOOR].cost = 10;
    ptrSpell[DIMENSIONDOOR].level = MAGIC_LEVEL5;
    ptrSpell[DIMENSIONDOOR].type = ADVENTURE;

    ptrSpell[TOWNGATE].cost = 10;
    ptrSpell[TOWNGATE].level = MAGIC_LEVEL4;
    ptrSpell[TOWNGATE].type = ADVENTURE;

    ptrSpell[TOWNPORTAL].cost = 20;
    ptrSpell[TOWNPORTAL].level = MAGIC_LEVEL5;
    ptrSpell[TOWNPORTAL].type = ADVENTURE;

    ptrSpell[VISIONS].cost = 6;
    ptrSpell[VISIONS].level = MAGIC_LEVEL2;
    ptrSpell[VISIONS].type = ADVENTURE;

    ptrSpell[HAUNT].cost = 8;
    ptrSpell[HAUNT].level = MAGIC_LEVEL2;
    ptrSpell[HAUNT].type = ADVENTURE;

    ptrSpell[EARTHGUARDIAN].cost = 15;
    ptrSpell[EARTHGUARDIAN].level = MAGIC_LEVEL4;
    ptrSpell[EARTHGUARDIAN].type = ADVENTURE;
    
    ptrSpell[AIRGUARDIAN].cost = 15;
    ptrSpell[AIRGUARDIAN].level = MAGIC_LEVEL4;
    ptrSpell[AIRGUARDIAN].type = ADVENTURE;

    ptrSpell[FIREGUARDIAN].cost = 15;
    ptrSpell[FIREGUARDIAN].level = MAGIC_LEVEL4;
    ptrSpell[FIREGUARDIAN].type = ADVENTURE;

    ptrSpell[WATERGUARDIAN].cost = 15;
    ptrSpell[WATERGUARDIAN].level = MAGIC_LEVEL4;
    ptrSpell[WATERGUARDIAN].type = ADVENTURE;

    fprintf(stderr, "Init spell.\n");
    
    return TRUE;
}

void	FreeSpell(void){
    
    if(ptrSpell) free(ptrSpell);
}

E_MAGICLEVEL GetLevelSpellMagic(E_SPELL spell){

    return ptrSpell[spell].level;
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
	    spell[0] = GetRandomSpell(COMBAT, MAGIC_LEVEL1);
	    spell[1] = GetRandomSpell(COMBAT, MAGIC_LEVEL1);
	    while(spell[1] == spell[0]) spell[1] = GetRandomSpell(COMBAT, MAGIC_LEVEL1);
	    spell[2] = GetRandomSpell(ADVENTURE, MAGIC_LEVEL1);
	    break;
	
	case NECROMANCER:
	    spell[0] = GetRandomSpell(COMBAT, MAGIC_LEVEL1);
	    while(BLESS == spell[0]) spell[0] = GetRandomSpell(COMBAT, MAGIC_LEVEL1);
	    spell[1] = GetRandomSpell(COMBAT, MAGIC_LEVEL1);
	    while(BLESS == spell[1] || spell[1] == spell[0]) spell[1] = GetRandomSpell(COMBAT, MAGIC_LEVEL1);
	    spell[2] = GetRandomSpell(ADVENTURE, MAGIC_LEVEL1);
	    break;
	
	case WIZARD:
	    if(spell[0] == SPELLNONE && spell[1] == SPELLNONE && spell[2] == SPELLNONE){
		spell[0] = GetRandomSpell(COMBAT, MAGIC_LEVEL1);
		spell[1] = GetRandomSpell(COMBAT, MAGIC_LEVEL1);
		while(spell[1] == spell[0]) spell[1] = GetRandomSpell(COMBAT, MAGIC_LEVEL1);
		spell[2] = GetRandomSpell(ADVENTURE, MAGIC_LEVEL1);
	    }else{
		spell[3] = spell[2];
		spell[2] = GetRandomSpell(COMBAT, MAGIC_LEVEL1);
		while(spell[2] == spell[0] || spell[2] == spell[1]) spell[2] = GetRandomSpell(COMBAT, MAGIC_LEVEL1);
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
	    spell[0] = GetRandomSpell(COMBAT, MAGIC_LEVEL2);
	    while(DEATHRIPPLE == spell[0]) spell[0] = GetRandomSpell(COMBAT, MAGIC_LEVEL2);
	    spell[1] = GetRandomSpell(COMBAT, MAGIC_LEVEL2);
	    while(DEATHRIPPLE == spell[1] || spell[0] == spell[1]) spell[1] = GetRandomSpell(COMBAT, MAGIC_LEVEL2);
	    spell[2] = GetRandomSpell(ADVENTURE, MAGIC_LEVEL2);
	    break;
	    
	case NECROMANCER:
	    spell[0] = DEATHRIPPLE;
	    spell[1] = GetRandomSpell(COMBAT, MAGIC_LEVEL2);
	    while(spell[0] == spell[1]) spell[1] = GetRandomSpell(COMBAT, MAGIC_LEVEL2);
	    spell[2] = GetRandomSpell(ADVENTURE, MAGIC_LEVEL2);
	    break;
	
	case WIZARD:
	    if(spell[0] == SPELLNONE && spell[1] == SPELLNONE && spell[2] == SPELLNONE){
		spell[0] = GetRandomSpell(COMBAT, MAGIC_LEVEL2);
		while(DEATHRIPPLE == spell[0]) spell[0] = GetRandomSpell(COMBAT, MAGIC_LEVEL2);
		spell[1] = GetRandomSpell(COMBAT, MAGIC_LEVEL2);
		while(DEATHRIPPLE == spell[1] || spell[0] == spell[1]) spell[1] = GetRandomSpell(COMBAT, MAGIC_LEVEL2);
		spell[2] = GetRandomSpell(ADVENTURE, MAGIC_LEVEL2);
	    }else{
		spell[3] = spell[2];
		spell[2] = GetRandomSpell(COMBAT, MAGIC_LEVEL2);
		while(DEATHRIPPLE == spell[2] || spell[0] == spell[2] || spell[1] == spell[2]) spell[2] = GetRandomSpell(COMBAT, MAGIC_LEVEL2);
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
	    spell[0] = GetRandomSpell(COMBAT, MAGIC_LEVEL3);
	    while(DEATHWAVE == spell[0] || ANIMATEDEAD == spell[0]) spell[0] = GetRandomSpell(COMBAT, MAGIC_LEVEL3);
	    spell[1] = GetRandomSpell(ADVENTURE, MAGIC_LEVEL3);
	    break;
	    
	case NECROMANCER:
	    spell[0] = GetRandomSpell(COMBAT, MAGIC_LEVEL3);
	    while(HOLYWORD == spell[0] || MASSBLESS == spell[0]) spell[0] = GetRandomSpell(COMBAT, MAGIC_LEVEL3);
	    spell[1] = GetRandomSpell(ADVENTURE, MAGIC_LEVEL3);
	    break;
	
	case WIZARD:
	    if(spell[0] == SPELLNONE && spell[1] == SPELLNONE){
		spell[0] = GetRandomSpell(COMBAT, MAGIC_LEVEL3);
		while(DEATHWAVE == spell[0] || ANIMATEDEAD == spell[0]) spell[0] = GetRandomSpell(COMBAT, MAGIC_LEVEL3);
		spell[1] = GetRandomSpell(ADVENTURE, MAGIC_LEVEL3);
	    }else{
		spell[2] = spell[1];
		spell[1] = GetRandomSpell(COMBAT, MAGIC_LEVEL3);
		while(DEATHWAVE == spell[1] || ANIMATEDEAD == spell[1] || spell[0] == spell[1]) spell[1] = GetRandomSpell(COMBAT, MAGIC_LEVEL3);
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
	    spell[0] = GetRandomSpell(COMBAT, MAGIC_LEVEL4);
	    spell[1] = GetRandomSpell(ADVENTURE, MAGIC_LEVEL4);
	    break;
	    
	
	case NECROMANCER:
	    spell[0] = GetRandomSpell(COMBAT, MAGIC_LEVEL4);
	    while(MASSCURE == spell[0] || HOLYSHOUT == spell[0]) spell[0] = GetRandomSpell(COMBAT, MAGIC_LEVEL4);
	    spell[1] = GetRandomSpell(ADVENTURE, MAGIC_LEVEL4);
	    break;
	
	
	case WIZARD:
	    if(spell[0] == SPELLNONE && spell[1] == SPELLNONE){
		spell[0] = GetRandomSpell(COMBAT, MAGIC_LEVEL4);
		spell[1] = GetRandomSpell(ADVENTURE, MAGIC_LEVEL4);
	    }else{
		spell[2] = spell[1];
		spell[1] = GetRandomSpell(COMBAT, MAGIC_LEVEL4);
		while(spell[0] == spell[1]) spell[1] = GetRandomSpell(COMBAT, MAGIC_LEVEL4);
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
	    spell[0] = GetRandomSpell(COMBAT, MAGIC_LEVEL5);
	    break;
	    
	case NECROMANCER:
	    spell[0] = GetRandomSpell(COMBAT, MAGIC_LEVEL5);
	    break;
	
	case WIZARD:
	    if(spell[0] == SPELLNONE)
		spell[0] = GetRandomSpell(COMBAT, MAGIC_LEVEL5);
	    else
		spell[1] = GetRandomSpell(COMBAT, MAGIC_LEVEL5);
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
            
    S_BOX *box = NULL;
    Uint16 height = 0;

    if(MASSSHIELD == spell) FillSPRITE(&sprite, "SPELLS.ICN", SHIELD);
    else if(MASSSLOW == spell) FillSPRITE(&sprite, "SPELLS.ICN", SLOW);
    else if(MASSBLESS == spell) FillSPRITE(&sprite, "SPELLS.ICN", BLESS);
    else if(MASSCURSE == spell) FillSPRITE(&sprite, "SPELLS.ICN", CURSE);
    else if(MASSDISPEL == spell) FillSPRITE(&sprite, "SPELLS.ICN", DISPELMAGIC);
    else if(MASSHASTE == spell) FillSPRITE(&sprite, "SPELLS.ICN", HASTE);
    else FillSPRITE(&sprite, "SPELLS.ICN", spell);
    image = GetICNSprite(&sprite);
    
    height += GetHeightText(GetStringSpell(spell), FONT_BIG);
    height += 20;
    height += GetHeightText(GetStringDescriptionsSpell(spell), FONT_BIG);
    height += 60;
    
    if(NULL == (box = InitBox(height, &dialog, flag))) return NONE;

    // text header
    dst = box->rectArea;
    PrintAlignText(video, &dst, GetStringSpell(spell), FONT_BIG);

    // text info
    dst.y += GetHeightText(GetStringSpell(spell), FONT_BIG);
    dst.y += 20;
    PrintAlignText(video, &dst, GetStringDescriptionsSpell(spell), FONT_BIG);
    
    // sprite
    dst.y += GetHeightText(GetStringDescriptionsSpell(spell), FONT_BIG);
    dst.y += 20;
    dst.y += ((box->rectArea.y + box->rectArea.h - dst.y) - image->h) / 2;
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

    FreeBox(box);

    CursorOn();

    return result;
}

E_SPELL GetRandomSpell(E_SPELLTYPE type, E_MAGICLEVEL level){

    Uint8 count = 0;
    Uint8 index = 0;
    E_SPELL spell;

    // get count magic
    for(spell = SPELLSLOW; spell <= MASSSLOW; ++spell)
	if(ptrSpell[spell].level == level && ptrSpell[spell].type == type) ++count;

    index = rand() % count;
    count = 0;

    // get select spell
    for(spell = SPELLSLOW; spell <= MASSSLOW; ++spell)
	if(ptrSpell[spell].level == level && ptrSpell[spell].type == type){
	    if(index == count) return spell;
	    ++count;
	}

    return 0;
}
