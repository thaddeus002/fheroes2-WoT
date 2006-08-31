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

#include "spliter.h"

S_SPLITER * InitSpliter(AGGSPRITE *sprite){

    S_SPLITER *sp = (S_SPLITER *) malloc(sizeof(S_SPLITER));
    if(sp == NULL){
        fprintf(stderr, "InitSpliter: error malloc: %d\n", sizeof(S_SPLITER));
        return NULL;
    }

    sp->cursor = GetICNSprite(sprite);
    sp->background = NULL;
    sp->pos.x = 0;
    sp->pos.y = 0;
    sp->pos.w = 0;
    sp->pos.h = 0;
    sp->step = 0;
    sp->start = 0;
    sp->max = 0;
    sp->cur = 0;
    sp->vertical = FALSE;
    
    return sp;
}

void FreeSpliter(S_SPLITER *sp){

    if(! sp) return;
    
    if(sp->background) SDL_FreeSurface(sp->background);
    free(sp);
}

void SetRangeSpliter(S_SPLITER *sp, SDL_Rect *rect, Uint16 count){

    if(! sp || !rect) return;

    SDL_Surface *video = SDL_GetVideoSurface();

    sp->pos = *rect;
    sp->max = count;
    sp->cur = 0;
    
    // spliter вертикальный
    if(rect->w < rect->h) sp->vertical = TRUE;

    // шаг сплитера
    if(sp->max){
	if(sp->vertical) sp->step = (rect->h - sp->cursor->h) / sp->max; else sp->step = (rect->w - sp->cursor->w) / sp->max;
    }
    if(sp->vertical) sp->start = rect->y; else sp->start = rect->x;
    
    sp->pos.w = sp->cursor->w;
    sp->pos.h = sp->cursor->h;
    
    if(NULL == (sp->background = SDL_CreateRGBSurface(SDL_SWSURFACE, sp->cursor->w, sp->cursor->h, 16, 0 , 0, 0, 0))){
         fprintf(stderr, "InitSpliter: CreateRGBSurface failed: %s\n", SDL_GetError());
         return;
    }
    
    SDL_BlitSurface(video, &sp->pos, sp->background, NULL);
}

void RedrawSpliter(S_SPLITER *sp){

    if(! sp) return;

    CursorOff();

    SDL_Surface *video = SDL_GetVideoSurface();

    if(sp->cursor)
	SDL_BlitSurface(sp->cursor, NULL, video, &sp->pos);

    CursorOn();
}

void MoveForwardSpliter(S_SPLITER *sp){

    if(! sp || sp->cur == sp->max) return;

    CursorOff();

    SDL_Surface *video = SDL_GetVideoSurface();

    if(sp->background)
	SDL_BlitSurface(sp->background, NULL, video, &sp->pos);

    ++sp->cur;

    if(sp->vertical)
	sp->pos.y = sp->start + sp->cur * sp->step;
    else
	sp->pos.x = sp->start + sp->cur * sp->step;
    printf("%d, %d, %f\n", sp->start, sp->cur, sp->cur * sp->step);
    
    if(sp->cursor)
	SDL_BlitSurface(sp->cursor, NULL, video, &sp->pos);

    CursorOn();
}

void MoveBackwardSpliter(S_SPLITER *sp){

    if(! sp || ! sp->cur) return;

    CursorOff();

    SDL_Surface *video = SDL_GetVideoSurface();

    if(sp->background)
	SDL_BlitSurface(sp->background, NULL, video, &sp->pos);

    --sp->cur;

    if(sp->vertical)
	sp->pos.y = sp->start + sp->cur * sp->step;
    else
	sp->pos.x = sp->start + sp->cur * sp->step;
    
    if(sp->cursor)
	SDL_BlitSurface(sp->cursor, NULL, video, &sp->pos);

    CursorOn();
}
