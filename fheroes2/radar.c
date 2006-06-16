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
#include "draw.h"
#include "loadgame.h"
#include "mp2maps.h"


SDL_Surface	*radarArea = NULL;
SDL_Surface	*radarCursor = NULL;

BOOL InitRadar(void){

    if(radarArea || radarCursor) return FALSE;

    int i, j;
    Uint16 index = 0;

    SDL_Surface *tiles = NULL;
    SDL_Surface *image = NULL;

    Uint8 widthMaps = GetWidthMaps();
    Uint8 heightMaps = GetHeightMaps();
    
    SDL_Rect dest, dst, src;
    dest.x = 0;
    dest.y = 0;

    S_CELLMAPS *ptrTile = NULL;

    if(NULL == (radarArea = SDL_CreateRGBSurface(SDL_SWSURFACE, RADARWIDTH, RADARWIDTH, 16, 0, 0, 0, 0))){
	fprintf(stderr, "InitRadar: CreateRGBSurface failed: %s, %d, %d\n", SDL_GetError(), RADARWIDTH, RADARWIDTH);
	return FALSE;
    }
    for(j = 0; j < heightMaps; ++j){

	// scale radar heigth (MAPS_LARGE)
	if(heightMaps == MAPS_LARGE && 2 == j % 3){
	    src.x = 0;
	    src.y = dest.y - dest.h;
	    src.w = RADARWIDTH;
	    src.h = dest.h;
	    dst = src;
	    dst.y += dest.h;
	    SDL_BlitSurface(radarArea, &src, radarArea, &dst);
	    dest.y += dest.h;
	}

	for(i = 0; i < widthMaps; ++i){

	    ptrTile = GetCELLMAPS(index);
	    tiles = ptrTile->tile;

    	    if(NULL == (image = SDL_CreateRGBSurface(SDL_SWSURFACE, RADARWIDTH / widthMaps, RADARWIDTH / heightMaps, 16, 0, 0, 0, 0))){
		fprintf(stderr, "InitRadar: CreateRGBSurface failed: %s, %d, %d\n", SDL_GetError(), RADARWIDTH / widthMaps, RADARWIDTH / heightMaps);
		return FALSE;
	    }

	    ScaleSurface(tiles, image);
	    dest.w = image->w;
	    dest.h = image->h;
	    SDL_BlitSurface(image, NULL, radarArea, &dest);

	    // scale radar width (MAPS_LARGE)
	    if(widthMaps == MAPS_LARGE && 2 == i % 3){
	    	dest.x += dest.w;
		SDL_BlitSurface(image, NULL, radarArea, &dest);
	    }

	    SDL_FreeSurface(image);

	    dest.x += dest.w;
	    ++index;
	}
	dest.x = 0;
	dest.y += dest.h;
    }

    // рисуем курсор радара
    float scale = 0;
    scale = RADARWIDTH / (float) widthMaps;
    Uint8 widthCursor = GetAreaWidth() * scale;
    scale = RADARWIDTH / (float) heightMaps;
    Uint8 heightCursor = GetAreaHeight() * scale;

    if(NULL == (radarCursor = SDL_CreateRGBSurface(SDL_SWSURFACE, widthCursor, heightCursor, 16, 0, 0, 0, 0))){
	fprintf(stderr, "InitRadar: CreateRGBSurface failed: %s, %d, %d\n", SDL_GetError(), widthCursor, heightCursor);
	return FALSE;
    }

    SDL_FillRect(radarCursor, NULL, COLORKEY);
    SDL_SetColorKey(radarCursor, SDL_SRCCOLORKEY|SDL_RLEACCEL, COLORKEY);
    // индекс цвета из палитры
    Uint16 color = RADARCOLOR;

    j = 0;
    for(i = 0; i < widthCursor; ++i){
	DrawPixel(radarCursor, i, j, color);
	DrawPixel(radarCursor, i + 1, j, color);
	i += 3;
    }
    j = heightCursor - 1;
    for(i = 0; i < widthCursor; ++i){
	DrawPixel(radarCursor, i, j, color);
	DrawPixel(radarCursor, i + 1, j, color);
	i += 3;
    }
    i = 0;
    for(j = 0; j < heightCursor; ++j){
	DrawPixel(radarCursor, i, j, color);
	DrawPixel(radarCursor, i, j + 1, color);
	j += 3;
    }
    i = widthCursor - 1;
    for(j = 0; j < heightCursor; ++j){
	DrawPixel(radarCursor, i, j, color);
	DrawPixel(radarCursor, i, j + 1, color);
	j += 3;
    }

    return TRUE;
}

void FreeRadar(void){

    if(NULL != radarArea) SDL_FreeSurface(radarArea);
    if(NULL != radarCursor) SDL_FreeSurface(radarCursor);

    radarArea = NULL;
    radarCursor = NULL;
}

void RedrawRadar(void){

    if(NULL == radarArea) return;

    SDL_Surface *video = SDL_GetVideoSurface();
    S_DISPLAY *display = GetDisplayPos();

    SDL_Rect dest;
    dest.x = video->w - BORDERWIDTH - RADARWIDTH;
    dest.y = BORDERWIDTH;
    dest.w = radarArea->w;
    dest.h = radarArea->h;
    
    SDL_BlitSurface(radarArea, NULL, video, &dest);

    dest.x = video->w - BORDERWIDTH - RADARWIDTH + display->offsetX * RADARWIDTH / GetWidthMaps();
    dest.y = BORDERWIDTH + display->offsetY * RADARWIDTH / GetHeightMaps();
    dest.w = radarCursor->w;
    dest.h = radarCursor->h;

    SDL_BlitSurface(radarCursor, NULL, video, &dest);

    SDL_Flip(video);
}

void RedrawRadarCursor(void){

    if(NULL == radarArea) return;

    SDL_Surface *video = SDL_GetVideoSurface();

    SDL_Rect dest, src;

    S_DISPLAY *display = GetDisplayPos();
    
    // востанавливаем старое положение
    src.x = display->lastOffsetX * RADARWIDTH / GetWidthMaps();
    src.y = display->lastOffsetY * RADARWIDTH / GetHeightMaps();
    src.w = radarCursor->w;
    src.h = radarCursor->h;

    dest.x = video->w - BORDERWIDTH - RADARWIDTH + src.x;
    dest.y = BORDERWIDTH + src.y;
    dest.w = src.w;
    dest.h = src.h;
    
    SDL_BlitSurface(radarArea, &src, video, &dest);

    // рисуем новое положение
    dest.x = video->w - BORDERWIDTH - RADARWIDTH + display->offsetX * RADARWIDTH / GetWidthMaps();
    dest.y = BORDERWIDTH + display->offsetY * RADARWIDTH / GetHeightMaps();
    dest.w = radarCursor->w;
    dest.h = radarCursor->h;

    SDL_BlitSurface(radarCursor, NULL, video, &dest);

    SDL_Flip(video);
}
