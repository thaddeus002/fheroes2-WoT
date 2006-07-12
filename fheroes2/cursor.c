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
#include "cursor.h"
#include "tools.h"
#include "agg.h"
#include "gamedefs.h"

typedef struct {
    Uint32	name;
    SDL_Surface *image;
    SDL_Surface *background;
    SDL_Rect 	pos;		// реальная позициа курсора
    BOOL 	show;		// флаг рисуем или нет
} CURSORHEADER;

static CURSORHEADER cursor;

void CursorSaveBackground(void);
void CursorRestoreBackground(void);

Uint32  GetCursor(void){

    return cursor.name;
}

void SetCursor(Uint32 name){

    if(CURSOR_NULL == name) return;
    if(cursor.name == name) return;

    AGGSPRITE sprite;

    // востановить old
    CursorOff();

    switch(GetHiUint32(name)){

	case 0x015A:
	    FillSPRITE(&sprite, "ADVMCO.ICN", GetLoUint32(name));
	    break;
	case 0x015B:
	    FillSPRITE(&sprite, "CMSECO.ICN", GetLoUint32(name));
	    break;
	case 0x015C:
	    FillSPRITE(&sprite, "SPELCO.ICN", GetLoUint32(name));
	    break;
	default:
	    FillSPRITE(&sprite, "ADVMCO.ICN", GetLoUint32(CURSOR_POINTER));
	    fprintf(stderr, "SetCursor: unknown name cursor %d\n", name);
	    break;
    }

    cursor.name  = name;
    cursor.image = GetICNSprite(&sprite);

    // рисуем курсор
    CursorOn();
}

void CursorSaveBackground(void){

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_BlitSurface(video, &cursor.pos, cursor.background, NULL);
}

void CursorRestoreBackground(void){

    if(NULL == cursor.background) return;

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect rest = cursor.pos;
    rest.x = 0;
    rest.y = 0;
	     
    SDL_BlitSurface(cursor.background, &rest, video, &cursor.pos);
}

void CursorShow(){

    if(0 == cursor.show) return;

    Sint32 x;
    Sint32 y;

    SDL_GetMouseState(&x, &y);

    if(cursor.pos.x == x && cursor.pos.y == y) return;

    // востановить old
    CursorRestoreBackground();

    // сохранить new
    cursor.pos.x = x;
    cursor.pos.y = y;
    cursor.pos.w = cursor.image->w;
    cursor.pos.h = cursor.image->h;
    SDL_Surface *video = SDL_GetVideoSurface();

    // рисуем курсор
    CursorSaveBackground();
    SDL_BlitSurface(cursor.image, NULL, video, &cursor.pos);
    SDL_Flip(video);

    return;
}

BOOL InitCursor(void){

    SDL_Surface *formatSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, CURSOR_WIDTH, CURSOR_HEIGHT, 16, 0, 0, 0, 0);

    SDL_ShowCursor(SDL_DISABLE);
    cursor.name = CURSOR_NULL;
    cursor.image = NULL;
    cursor.show = TRUE;
    cursor.background = SDL_DisplayFormat(formatSurface);

    if(NULL == cursor.background) return FALSE;

    SetCursor(CURSOR_POINTER);

    if(formatSurface) SDL_FreeSurface(formatSurface);

    fprintf(stderr, "Init cursor.\n");

    return TRUE;
}

void FreeCursor(void){

    if(cursor.background) SDL_FreeSurface(cursor.background);

    SDL_ShowCursor(SDL_ENABLE);
}


void CursorOff(void){

    if(cursor.show){

	CursorRestoreBackground();
	cursor.show = FALSE;
    }
}

void CursorOn(void){

    if(cursor.show) return;

    cursor.show = TRUE;
    CursorSaveBackground();
    
    Sint32 x;
    Sint32 y;

    SDL_GetMouseState(&x, &y);

    // востановить old
    CursorRestoreBackground();

    // сохранить new
    cursor.pos.x = x;
    cursor.pos.y = y;
    cursor.pos.w = cursor.image->w;
    cursor.pos.h = cursor.image->h;
    SDL_Surface *video = SDL_GetVideoSurface();

    // рисуем курсор
    CursorSaveBackground();
    SDL_BlitSurface(cursor.image, NULL, video, &cursor.pos);
    SDL_Flip(video);

    return;
}

