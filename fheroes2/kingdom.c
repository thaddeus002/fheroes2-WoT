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
#include "gamedefs.h"
#include "string.h"
#include "config.h"
#include "heroes.h"
#include "kingdom.h"


static S_KINGDOM *kingdom = NULL;

BOOL	InitKingdom(void){

    if(NULL == (kingdom = (S_KINGDOM *) malloc(sizeof(S_KINGDOM) * KINGDOMMAX))){
	fprintf(stderr, "InitKingdom: error malloc: %d\n", sizeof(S_KINGDOM) * KINGDOMMAX);
	return FALSE;
    }
    
    memset(kingdom, 0, sizeof(S_KINGDOM) * KINGDOMMAX);

    kingdom[GRAY].play = TRUE;
    kingdom[GRAY].allowBuild = FALSE;
    kingdom[GRAY].ore = 0;
    kingdom[GRAY].wood = 0;
    kingdom[GRAY].mercury = 0;
    kingdom[GRAY].sulfur = 0;
    kingdom[GRAY].crystal = 0;
    kingdom[GRAY].gems = 0;
    kingdom[GRAY].gold = 0;

    if(GetIntValue(KINGDOMCOLORS) & COLORBLUE){
	kingdom[BLUE].play = TRUE;
	kingdom[BLUE].allowBuild = TRUE;
	kingdom[BLUE].ore = 20;
	kingdom[BLUE].wood = 20;
	kingdom[BLUE].mercury = 5;
	kingdom[BLUE].sulfur = 5;
	kingdom[BLUE].crystal = 5;
	kingdom[BLUE].gems = 5;
	kingdom[BLUE].gold = 7500;
    }

    if(GetIntValue(KINGDOMCOLORS) & COLORGREEN){
	kingdom[GREEN].play = TRUE;
	kingdom[GREEN].allowBuild = TRUE;
	kingdom[GREEN].ore = 20;
	kingdom[GREEN].wood = 20;
	kingdom[GREEN].mercury = 5;
	kingdom[GREEN].sulfur = 5;
	kingdom[GREEN].crystal = 5;
	kingdom[GREEN].gems = 5;
	kingdom[GREEN].gold = 7500;
    }

    if(GetIntValue(KINGDOMCOLORS) & COLORRED){
        kingdom[RED].play = TRUE;
        kingdom[RED].allowBuild = TRUE;
	kingdom[RED].ore = 20;
	kingdom[RED].wood = 20;
	kingdom[RED].mercury = 5;
	kingdom[RED].sulfur = 5;
	kingdom[RED].crystal = 5;
	kingdom[RED].gems = 5;
	kingdom[RED].gold = 7500;
    }

    if(GetIntValue(KINGDOMCOLORS) & COLORYELLOW){
	kingdom[YELLOW].play = TRUE;
	kingdom[YELLOW].allowBuild = TRUE;
	kingdom[YELLOW].ore = 20;
	kingdom[YELLOW].wood = 20;
	kingdom[YELLOW].mercury = 5;
	kingdom[YELLOW].sulfur = 5;
	kingdom[YELLOW].crystal = 5;
	kingdom[YELLOW].gems = 5;
	kingdom[YELLOW].gold = 7500;
    }

    if(GetIntValue(KINGDOMCOLORS) & COLORORANGE){
	kingdom[ORANGE].play = TRUE;
	kingdom[ORANGE].allowBuild = TRUE;
	kingdom[ORANGE].ore = 20;
	kingdom[ORANGE].wood = 20;
	kingdom[ORANGE].mercury = 5;
	kingdom[ORANGE].sulfur = 5;
	kingdom[ORANGE].crystal = 5;
	kingdom[ORANGE].gems = 5;
	kingdom[ORANGE].gold = 7500;
    }

    if(GetIntValue(KINGDOMCOLORS) & COLORPURPLE){
	kingdom[PURPLE].play = TRUE;
	kingdom[PURPLE].allowBuild = TRUE;
	kingdom[PURPLE].ore = 20;
	kingdom[PURPLE].wood = 20;
	kingdom[PURPLE].mercury = 5;
	kingdom[PURPLE].sulfur = 5;
	kingdom[PURPLE].crystal = 5;
	kingdom[PURPLE].gems = 5;
	kingdom[PURPLE].gold = 7500;
    }

    if(GetIntValue(DEBUG)){
	fprintf(stderr, "InitKingdom:");
	if(GetIntValue(KINGDOMCOLORS) & COLORBLUE)   fprintf(stderr, " BLUE");
	if(GetIntValue(KINGDOMCOLORS) & COLORGREEN)  fprintf(stderr, " GREEN");
	if(GetIntValue(KINGDOMCOLORS) & COLORRED)    fprintf(stderr, " RED");
	if(GetIntValue(KINGDOMCOLORS) & COLORYELLOW) fprintf(stderr, " YELLOW");
	if(GetIntValue(KINGDOMCOLORS) & COLORORANGE) fprintf(stderr, " ORANGE");
	if(GetIntValue(KINGDOMCOLORS) & COLORPURPLE) fprintf(stderr, " PURPLE");
	if(! GetIntValue(KINGDOMCOLORS)) fprintf(stderr, " (test mode?)");
	fprintf(stderr, "\n");
    }

    return TRUE;
}

void	FreeKingdom(void){

    if(kingdom) free(kingdom);
}

void    KingdomAddHeroes(E_COLORS color, E_NAMEHEROES name){

    if(color == GRAY) return;
    if(! kingdom[color].play) return;

    Uint8 i;
    for(i = 0; i < KINGDOMMAXHEROES; ++i)

	if(HEROESNULL == kingdom[color].nameheroes[i]){
	    kingdom[color].nameheroes[i] = name;
	    return;
	}

    if(GetIntValue(DEBUG)) fprintf(stderr, "KingdomAddHeroes: define KINGDOMMAXHEROES %d\n", KINGDOMMAXHEROES);
}

void    KingdomRemoveHeroes(E_COLORS color, E_NAMEHEROES name){

    Uint8 i;
    for(i = 0; i < KINGDOMMAXHEROES; ++i)

	if(name == kingdom[color].nameheroes[i]){
	    kingdom[color].nameheroes[i] = HEROESNULL;
	    return;
	}
}

void    KingdomAddCastle(E_COLORS color, Uint8 index){

    if(! kingdom[color].play) return;

    Uint8 i;

    for(i = 0; i < KINGDOMMAXCASTLE; ++i)

	if(0xFF == kingdom[color].castle[i]){

	    kingdom[color].castle[index] = index;
	    break;
    }

    if(GetIntValue(DEBUG)) fprintf(stderr, "KingdomAddCastle: %s\n", ((S_CASTLE *) GetStatCastle(index))->name);
}

void KingdomRemoveCastle(E_COLORS color, Uint8 index){

    if(! kingdom[color].play) return;

    Uint8 i;

    for(i = 0; i < KINGDOMMAXCASTLE; ++i)
    
	if(index == kingdom[color].castle[i]){

	    kingdom[color].castle[i] = 0xFF;
	    break;
    }
}

S_KINGDOM *GetStatKingdom(E_COLORS color){

    if(! kingdom) return NULL;
    
    return &kingdom[color];
}
