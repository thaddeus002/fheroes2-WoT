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
#include "gamedefs.h"
#include "agg.h"
#include "tools.h"
#include "draw.h"
#include "config.h"
#include "cursor.h"

void LockSurface(SDL_Surface *surface){ 

    if(SDL_MUSTLOCK(surface))
	SDL_LockSurface(surface);
} 

void UnlockSurface(SDL_Surface *surface){

    if(SDL_MUSTLOCK(surface))
	SDL_UnlockSurface(surface); 
}

void DrawPixelAlpha(SDL_Surface *surface, Uint16 x, Uint16 y, Uint8 r, Uint8 g, Uint8 b, Uint8 a){

    if(x > surface->w || y > surface->h){
    	if(ICNRLE_DEBUG) fprintf(stderr, "\nSAFE2: MEMORY LEAK. PARACHUTE!\n");
	return;
    }
    
    Uint16 *bufp;
    bufp = (Uint16 *) surface->pixels + y * surface->pitch / 2 + x;
    *bufp = SDL_MapRGBA(surface->format, r, g, b, a);
}

Uint16 GetPixel(SDL_Surface *surface, Uint16 x, Uint16 y){

    if(x > surface->w || y > surface->h){
	if(ICNRLE_DEBUG) fprintf(stderr, "GetPixel: out of video memory!\n");
	return 0;
    }
    return *((Uint16 *) surface->pixels + y * surface->pitch / 2 + x);
}

void PutPixel(SDL_Surface *surface, Uint16 x, Uint16 y, Uint16 pixel){

    if(x > surface->w || y > surface->h){
	if(ICNRLE_DEBUG) fprintf(stderr, "PutPixel: out of video memory!\n");
	return;
    }

    Uint16 *bufp = (Uint16 *) surface->pixels + y * surface->pitch / 2 + x;

    *bufp = pixel;
}

void DrawPixel(SDL_Surface *surface, Uint16 x, Uint16 y, Uint8 color){

    if(x > surface->w || y > surface->h){
	if(ICNRLE_DEBUG) fprintf(stderr, "DrawPixel: out of video memory!\n");
	return;
    }
    SDL_Color *palette = GetPalette();

    if(COLORKEY == color) color = COLORCHANGE;

    Uint16 *bufp;
    bufp = (Uint16 *) surface->pixels + y * surface->pitch / 2 + x;
    *bufp = SDL_MapRGB(surface->format, palette[color].r, palette[color].g, palette[color].b);
}

BOOL DrawSceneVar2(SDL_Surface *surface, Uint8 *ptr, Uint32 sizesprite){
    /*
    * *ptr цвет палитры, ptr позиция в данных
    * sizesprite размер данных для проверки за выход границы
    */
    Uint16 x = 0;
    Uint16 y = 0;
    Uint8 i, current;
    Uint32 maxptr = (Uint32) ptr + sizesprite;
    BOOL result = FALSE;

    LockSurface(surface);

    for(;;){

	// проверка за границу буффера данных
	if((Uint32) ptr > maxptr){
	    if(ICNRLE_DEBUG) fprintf(stderr, "\nSAFE: MEMORY LEAK. PARACHUTE!\n");
	    break;
	}

	// 0x00 конец строки
	if(0 == *ptr){
	    ++y;
	    x = 0;
	    if(ICNRLE_DEBUG) fprintf(stderr, " M:%hhX\n", *ptr);
	    ++ptr;
	    continue;

	// диаппазон 0x01..0x7F XX
	}else if(0 < *ptr && 0x80 > *ptr){
	    if(ICNRLE_DEBUG) fprintf(stderr, " M:%hhX C:%d:D", *ptr, *ptr);
	    current = *ptr;
	    ++ptr;
	    for(i = 0; i < current; ++i){
		DrawPixel(surface, x, y, *ptr);
		if(ICNRLE_DEBUG) fprintf(stderr, ":%hhX", *ptr);
		++ptr;
		++x;
	    }
	    continue;

	// конец данных
	}else if(0x80 == *ptr){
	    if(ICNRLE_DEBUG) fprintf(stderr, "\nM:%hhX\n", *ptr);
	    break;

	// диаппазон 0x81..0xBF 00 
	}else if(0x80 < *ptr && 0xC0 > *ptr){
	    if(ICNRLE_DEBUG) fprintf(stderr, " M:%hhX Z:%d", *ptr, *ptr - 0x80);
	    x = x + *ptr - 0x80;
	    ++ptr;
	    continue;

	// 0xC0 смещение
	}else if(0xC0 == *ptr){
	    if(ICNRLE_DEBUG) fprintf(stderr, " M:%hhX", *ptr);
	    ++ptr;

	    if( 0 == *ptr % 4){
		if(ICNRLE_DEBUG) fprintf(stderr, " M4:%hhX:%d ALPHA", *ptr, *ptr % 4);
		current = *ptr;
		++ptr;
		for(i = 0; i < *ptr; ++i){
		    DrawPixelAlpha(surface, x, y, 0, 0, 0, current);
		    result = TRUE;
		    if(ICNRLE_DEBUG) fprintf(stderr, ":%hhX", current);
		    ++x;
		}
		++ptr;
		continue;

	    }else{
		if(ICNRLE_DEBUG) fprintf(stderr, " M4:%hhX:%d ALPHA", *ptr, *ptr % 4);
		current = *ptr;
		for(i = 0; i < *ptr % 4; ++i){
		    DrawPixelAlpha(surface, x, y, 0, 0, 0, current);
		    result = TRUE;
		    if(ICNRLE_DEBUG) fprintf(stderr, ":%hhX", current);
		    ++x;
		}
		++ptr;
		continue;
	    }

	// 0xC1 N D количество данные
	}else if(0xC1 == *ptr){
	    if(ICNRLE_DEBUG) fprintf(stderr, " M:%hhX", *ptr);
	    ++ptr;
	    current = *ptr;
	    if(ICNRLE_DEBUG) fprintf(stderr, " C:%d:D", current);
	    ++ptr;
	    for(i = 0; i < current; ++i){
	    	DrawPixel(surface, x, y, *ptr);
	    	if(ICNRLE_DEBUG) fprintf(stderr, ":%hhX", *ptr);
	    	++x;
	    }
	    ++ptr;
	    continue;

	// 0xC2 и больше
	}else if(0xC1 < *ptr){
	    if(ICNRLE_DEBUG) fprintf(stderr, " M:%hhX", *ptr);
	    current = *ptr - 0xC0;
	    if(ICNRLE_DEBUG) fprintf(stderr, " C:%d:D", current);
	    ++ptr;
	    for(i = 0; i < current; ++i){
		DrawPixel(surface, x, y, *ptr);
		if(ICNRLE_DEBUG) fprintf(stderr, ":%hhX", *ptr);
		++x;
	    }
	    ++ptr;
	    continue;
	}
    }

    UnlockSurface(surface);
    
    return result;
}

void DrawSprite(SDL_Rect *rect, AGGSPRITE *object){

    if(object->number == 0xFFFF) return;

    CursorOff();

    SDL_Surface *video;
    SDL_Surface *image;

    // BUG: sdl_rect push button != sdl_rect up button
    SDL_Rect src;
    src.x = 0;
    src.y = 0;
    src.w = rect->w;
    src.h = rect->h;
    
    video = SDL_GetVideoSurface();

    image = GetICNSprite(object);

    if(image)
        SDL_BlitSurface(image, &src, video, rect);

    CursorOn();
}

void DrawTILBmp(SDL_Surface *surface, Uint8 *ptr, Uint16 index, Uint8 shape){
    Sint16 x, y;

    ptr += surface->w * surface->h * index;
    LockSurface(surface);

    switch(shape % 4){
    
	// нормальное отображение
	case 0:
	    for(y = 0; y < surface->h; ++y)
		for( x = 0; x < surface->w; ++x){
		    DrawPixel(surface, x, y, *ptr);
		    ptr++;
	    }
	    break;

	// отразить вертикально
	case 1:
	    for(y = surface->h - 1; y >= 0; --y)
		for( x = 0; x < surface->w; ++x){
		    DrawPixel(surface, x, y, *ptr);
		    ptr++;
	    }
	    break;

	// отразить горизонтально
	case 2:
	    for(y = 0; y < surface->h; ++y)
		for( x = surface->w - 1; x >= 0; --x){
		    DrawPixel(surface, x, y, *ptr);
		    ptr++;
	    }
	    break;

	// повторить последние два
	case 3:
	    for(y = surface->h - 1; y >= 0; --y)
		for( x = surface->w - 1; x >= 0; --x){
		    DrawPixel(surface, x, y, *ptr);
		    ptr++;
	    }
	    break;
    }

    UnlockSurface(surface);
}

/* 
 * функция масштабирования
 * (с уменьшением src -> dst! только квадрат в квадрат!)
 * метод наибольшего цвета в точку
 */
void ScaleSurface(SDL_Surface *src, SDL_Surface *dst){

    if(dst->w > src->w || dst->h > src->h){
	fprintf(stderr, "ScaleSurface: dst surface > src surface.");
	return;
    }

    // маштабируем только квадраты
    if(dst->w != dst->h || src->w != src->h){
	fprintf(stderr, "ScaleSurface: only width == height.");
	return;
    }

    int dstX, dstY;

    Uint16 color = 0;
    Uint16 index;

    Uint16 *p_src = NULL;
    Uint16 *p_dst = NULL;

    Uint16 width = 0;
    Uint16 height = 0;
    Uint16 width2 = 0;
    Uint16 dstX2 = 0;
    Uint16 dstY2 = 0;

    // копируем все цвета src в массив p_src
    if(NULL == (p_src = (Uint16 *) malloc(sizeof(Uint16) * src->w * src->h))){
	fprintf(stderr, "ScaleSurface: error malloc %d\n", src->w * src->h);
	return;
    }
    index = 0;
    for(dstY = 0; dstY < src->h; ++dstY)
	for(dstX = 0; dstX < src->w; ++dstX){
	    p_src[index] = GetPixel(src, dstX, dstY);
	    ++index;
	}

    // количество наименьших иттераций
    Uint16 count = 0;
    
    width = src->w;
    for(;;){
	++count;
	if((width >>= 1) <= dst->w) break;
    }

    width = (Uint16) src->w / 2;
    height = (Uint16) src->h / 2;
    while(count){

	if(NULL == (p_dst = (Uint16 *) malloc(sizeof(Uint16) * width * height))){
	    fprintf(stderr, "ScaleSurface: error malloc %d\n", width * height);
	    return;
	}

	// итерация 2х2 -> 1
	index = 0;
	width2 = 2 * width;

	for(dstY = 0; dstY < height; ++dstY)

	    for(dstX = 0; dstX < width; ++dstX){

		dstX2 = dstX * 2;
		dstY2 = dstY * 2;
		
		if((color = p_src[width2 * dstY2 + dstX2]) == p_src[width2 * (dstY2 + 1) + dstX2 + 1] || 
		    color == p_src[width2 * dstY2 + dstX2 + 1] || color == p_src[width2 * (dstY2 + 1) + dstX2])
		    p_dst[index] = color;

		else if((color = p_src[width2 * dstY2 + dstX2 + 1]) == p_src[width2 * (dstY2 + 1) + dstX2 + 1] || 
	    	    color == p_src[width2 * (dstY2 + 1) + dstX2])
		    p_dst[index] = color;

		else if((color = p_src[width2 * (dstY2 + 1) + dstX2]) == p_src[width2 * (dstY2 + 1) + dstX2 + 1])
		    p_dst[index] = color;
	
		else
		    p_dst[index] = p_src[width2 * (dstY2 + 1) + dstX2 + 1];

		++index;
	    }

	if(NULL != p_src) free(p_src);
	p_src = p_dst;
	p_dst = NULL;
	
	width = width / 2;
	height = height / 2;

	--count;
    }

    // копируем все цвета из массива в dst
    index = 0;
    for(dstY = 0; dstY < dst->h; ++dstY)
	for(dstX = 0; dstX < dst->w; ++dstX){
	    PutPixel(dst, dstX, dstY, p_src[index]);
	    ++index;
	}

    free(p_src);

    return;
}

