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

    if(GetIntValue(KINGDOMCOLORS) & COLORBLUE)   kingdom[BLUE].play   = TRUE;
    if(GetIntValue(KINGDOMCOLORS) & COLORGREEN)  kingdom[GREEN].play  = TRUE;
    if(GetIntValue(KINGDOMCOLORS) & COLORRED)    kingdom[RED].play    = TRUE;
    if(GetIntValue(KINGDOMCOLORS) & COLORYELLOW) kingdom[YELLOW].play = TRUE;
    if(GetIntValue(KINGDOMCOLORS) & COLORORANGE) kingdom[ORANGE].play = TRUE;
    if(GetIntValue(KINGDOMCOLORS) & COLORPURPLE) kingdom[PURPLE].play = TRUE;

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

void    KingdomRemoveCastle(E_COLORS color, Uint8 index){

    if(! kingdom[color].play) return;

    Uint8 i;

    for(i = 0; i < KINGDOMMAXCASTLE; ++i)
    
	if(index == kingdom[color].castle[i]){

	    kingdom[color].castle[i] = 0xFF;
	    break;
    }
}
