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
#include "tools.h" 
#include "cursor.h" 
#include "debug.h" 

#include "agg.h" 

#include "selector.h"

S_SELECT * InitSelector(AGGSPRITE *sprite, Uint16 ax, Uint16 aw, BOOL vertical){

    S_SELECT *se = (S_SELECT *) malloc(sizeof(S_SELECT));

    if(se == NULL){
        fprintf(stderr, "InitSelector: error malloc: %d\n", sizeof(S_SELECT));
        return NULL;
    }

    se->cursor = GetICNSprite(sprite);
    se->background = NULL;
    se->pos.x = 0;
    se->pos.y = 0;
    se->pos.w = se->cursor->w;
    se->pos.h = se->cursor->h;
    se->ax = ax;
    se->aw = aw;
    se->vertical = vertical;
    se->use = FALSE;

    if(NULL == (se->background = SDL_CreateRGBSurface(SDL_SWSURFACE, se->cursor->w, se->cursor->h, 16, 0, 0, 0, 0))){
        fprintf(stderr, "InitSelector: CreateRGBSurface failed: %s\n", SDL_GetError());
        return NULL;
    }

    return se;
}

void FreeSelector(S_SELECT *se){

    if(! se) return;
    
    if(se->background) SDL_FreeSurface(se->background);
    free(se);

    se = NULL;
}

void RedrawSelector(S_SELECT *se, SDL_Rect *rect){

    if(!se) return;

    SDL_Surface *video = SDL_GetVideoSurface();

    ResetSelector(se);

    CursorOff();

    if(rect){
	se->pos.x = rect->x;
	se->pos.y = rect->y;
    }

    SDL_BlitSurface(video, &se->pos, se->background, NULL);
    SDL_BlitSurface(se->cursor, NULL, video, &se->pos);

    se->use = TRUE;
}

void ResetSelector(S_SELECT *se){

    if(! se || !se->use) return;

    SDL_Surface *video = SDL_GetVideoSurface();
    
    CursorOff();
    
    if(se->background)
	SDL_BlitSurface(se->background, NULL, video, &se->pos);

    se->use = FALSE;
}

Uint8 GetIndexSelected(const S_SELECT *se){

    if(! se->use) return 0;

    return (se->vertical ? 1 + (se->pos.y - se->ax) / se->aw : 1 + (se->pos.x - se->ax) / se->aw);
}
