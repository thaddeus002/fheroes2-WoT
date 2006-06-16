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
	if(ICNRLE_DEBUG) fprintf(stderr, "\nSAFE2: MEMORY LEAK. PARACHUTE!\n");
	return;
    }
    SDL_Color *palette = GetPalette();

    if(COLORKEY == color) color = COLORCHANGE;

    Uint16 *bufp;
    bufp = (Uint16 *) surface->pixels + y * surface->pitch / 2 + x;
    *bufp = SDL_MapRGB(surface->format, palette[color].r, palette[color].g, palette[color].b);
}

void DrawSceneVar2(SDL_Surface *surface, Uint8 *ptr, Uint32 sizesprite){
    /*
    * *ptr цвет палитры, ptr позиция в данных
    * sizesprite размер данных для проверки за выход границы
    */
    Uint16 x = 0;
    Uint16 y = 0;
    Uint8 i, current;
    Uint32 maxptr = (Uint32) ptr + sizesprite;

    LockSurface(surface);

    for(;;){

	// проверка за границу буффера данных
	if((Uint32) ptr > maxptr){
	    if(ICNRLE_DEBUG) fprintf(stderr, "\nSAFE1: MEMORY LEAK. PARACHUTE!\n");
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

    if(image){
        SDL_BlitSurface(image, &src, video, rect);
	SDL_Flip(video);
    }

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

/* функция отрисовки элемента анимации объекта */
void DrawAnimationFrame(SDL_Rect *dest, Uint8 type, Uint8 index, Uint8 frame){

    AGGSPRITE sprite;
    SDL_Surface *video = SDL_GetVideoSurface();
    ICNHEADER *info = NULL;
    SDL_Rect rectObject;

    switch(type){

	// объекты на море
	case 0xC8:
	case 0xC9:
	case 0xCA:
	case 0xCB:
	
	    switch(index){
	    
		// бутылка
		case 0x00:
		// сундук
		case 0x13:
		// дрова
		case 0x2D:
		// лодка у картографа
		case 0x3E:
		// волны у картографа
		case 0x45:
		// зеленая трава на воде (2 варианта)
		case 0x4C:
		case 0x53:
		case 0x5A:
		case 0x61:
		case 0x68:
		// утопающий матрос
		case 0x6F:
		// буй
		case 0xC3:
		// правый broken ship
		case 0xE2:
		case 0xE9:
		case 0xF1:
		case 0xF8:
		    FillSPRITE(&sprite, "OBJNWATR.ICN", index + ((frame - 1) % 6) + 1);
		    info = GetICNHeader(&sprite);
		    rectObject = *dest;
		    rectObject.x += info->offsetX;
		    rectObject.y += info->offsetY;
		    SDL_BlitSurface(info->surface, NULL, video, &rectObject);
		    break;

		// чайка на камнях (2 варианта) (15 кадров)
		case 0x86:
		case 0x96:
		    FillSPRITE(&sprite, "OBJNWATR.ICN", index + ((frame - 1) % 15) + 1);
		    info = GetICNHeader(&sprite);
		    rectObject = *dest;
		    rectObject.x += info->offsetX;
		    rectObject.y += info->offsetY;
		    SDL_BlitSurface(info->surface, NULL, video, &rectObject);
		    break;

		// водоворот (4 кадра!)
		case 0xCA:
		case 0xCE:
		case 0xD2:
		case 0xD6:
		case 0xDA:
		case 0xDE:
		    FillSPRITE(&sprite, "OBJNWATR.ICN", index + ((frame - 1) % 4) + 1);
		    info = GetICNHeader(&sprite);
		    rectObject = *dest;
		    rectObject.x += info->offsetX;
		    rectObject.y += info->offsetY;
		    SDL_BlitSurface(info->surface, NULL, video, &rectObject);
		    break;

		default:
		break;
	    }

	    break;

	// объекты на море №2
	case 0xA0:
	case 0xA1:
	case 0xA2:
	case 0xA3:

	    switch(index){
	    
		// парус левого broken ship
		case 0x03:
		case 0x0C:
		    FillSPRITE(&sprite, "OBJNWAT2.ICN", index + ((frame - 1) % 6) + 1);
		    info = GetICNHeader(&sprite);
		    rectObject = *dest;
		    rectObject.x += info->offsetX;
		    rectObject.y += info->offsetY;
		    SDL_BlitSurface(info->surface, NULL, video, &rectObject);
		    break;

		default:
		break;
	    }

	    break;

	// объекты на crck
	case 0xE4:
	case 0xE5:
	case 0xE6:
	case 0xE7:

	    switch(index){
	    
		// лужа грязи (20 кадров!)
		case 0x50:
		case 0x5B:
		case 0x66:
		case 0x71:
		case 0x7C:
		case 0x89:
		case 0x94:
		case 0x9F:
		case 0xAA:
		    FillSPRITE(&sprite, "OBJNCRCK.ICN", index + ((frame - 1) % 20) + 1);
		    info = GetICNHeader(&sprite);
		    rectObject = *dest;
		    rectObject.x += info->offsetX;
		    rectObject.y += info->offsetY;
		    SDL_BlitSurface(info->surface, NULL, video, &rectObject);
		    break;

		// рынок дым от трубы (10)
		case 0xBE:
		// рынок тень от дыма (10)
		case 0xCA:
		    FillSPRITE(&sprite, "OBJNCRCK.ICN", index + ((frame - 1) % 10) + 1);
		    info = GetICNHeader(&sprite);
		    rectObject = *dest;
		    rectObject.x += info->offsetX;
		    rectObject.y += info->offsetY;
		    SDL_BlitSurface(info->surface, NULL, video, &rectObject);
		    break;

		default:
		break;
	    }

	    break;

	// объекты на dirt
	case 0xE0:
	case 0xE1:
	case 0xE2:
	case 0xE3:

	    switch(index){
	    
		// sprite house (4 кадра)
		case 0x99:
		case 0x9D:
		case 0xA1:
		case 0xA5:
		// мельница (4 кадра)
		case 0xA9:
		case 0xAD:
		case 0xB1:
		case 0xB5:
		case 0xB9:
		case 0xBD:
		    FillSPRITE(&sprite, "OBJNDIRT.ICN", index + ((frame - 1) % 4) + 1);
		    info = GetICNHeader(&sprite);
		    rectObject = *dest;
		    rectObject.x += info->offsetX;
		    rectObject.y += info->offsetY;
		    SDL_BlitSurface(info->surface, NULL, video, &rectObject);
		    break;

		default:
		break;
	    }

	    break;

	// объекты на desert
	case 0xDC:
	case 0xDD:
	case 0xDE:
	case 0xDF:

	    switch(index){
	    
		// костер (6)
		case 0x36:
		case 0x3D:
		    FillSPRITE(&sprite, "OBJNDSRT.ICN", index + ((frame - 1) % 6) + 1);
		    info = GetICNHeader(&sprite);
		    rectObject = *dest;
		    rectObject.x += info->offsetX;
		    rectObject.y += info->offsetY;
		    SDL_BlitSurface(info->surface, NULL, video, &rectObject);
		    break;

		default:
		break;
	    }

	    break;

	// объекты gras
	case 0xC0:
	case 0xC1:
	case 0xC2:
	case 0xC3:

	    switch(index){
	    
		// мельница (3 кадра)
		case 0x17:
		case 0x1B:
		case 0x1F:
		case 0x23:
		case 0x27:
		case 0x2B:
		case 0x2F:
		case 0x33:
		case 0x37:
		case 0x3B:
		    FillSPRITE(&sprite, "OBJNGRA2.ICN", index + ((frame - 1) % 3) + 1);
		    info = GetICNHeader(&sprite);
		    rectObject = *dest;
		    rectObject.x += info->offsetX;
		    rectObject.y += info->offsetY;
		    SDL_BlitSurface(info->surface, NULL, video, &rectObject);
		    break;

		// дым из трубы 6
		case 0x3F:
		case 0x46:
		case 0x4D:
		// домик лучника 6
		case 0x54:
		// дым из трубы 6
		case 0x5D:
		case 0x64:
		// тень 6
		case 0x6B:
		// домик крестьянина 6
		case 0x72:
		    FillSPRITE(&sprite, "OBJNGRA2.ICN", index + ((frame - 1) % 6) + 1);
		    info = GetICNHeader(&sprite);
		    rectObject = *dest;
		    rectObject.x += info->offsetX;
		    rectObject.y += info->offsetY;
		    SDL_BlitSurface(info->surface, NULL, video, &rectObject);
		    break;

		default:
		break;
	    }

	    break;

	// объекты на lava
	case 0xF0:
	case 0xF1:
	case 0xF2:
	case 0xF3:

	    switch(index){

		// вулкан (6)
		case 0x00:
		// тень (6)
		case 0x07:
		case 0x0E:
		// лава (6)
		case 0x15:
		    FillSPRITE(&sprite, "OBJNLAV2.ICN",  index + ((frame - 1) % 6) + 1);
		    info = GetICNHeader(&sprite);
		    rectObject = *dest;
		    rectObject.x += info->offsetX;
		    rectObject.y += info->offsetY;
		    SDL_BlitSurface(info->surface, NULL, video, &rectObject);
		    break;

		// дым (5)
		case 0x21:
		case 0x27:
		    FillSPRITE(&sprite, "OBJNLAV2.ICN",  index + ((frame - 1) % 5) + 1);
		    info = GetICNHeader(&sprite);
		    rectObject = *dest;
		    rectObject.x += info->offsetX;
		    rectObject.y += info->offsetY;
		    SDL_BlitSurface(info->surface, NULL, video, &rectObject);
		    break;

		// тень от дыма (4)
		case 0x2C:
		//case 0x32:
		case 0x37:
		//case 0x3C:
		    FillSPRITE(&sprite, "OBJNLAV2.ICN",  index + ((frame - 1) % 4) + 1);
		    info = GetICNHeader(&sprite);
		    rectObject = *dest;
		    rectObject.x += info->offsetX;
		    rectObject.y += info->offsetY;
		    SDL_BlitSurface(info->surface, NULL, video, &rectObject);
		    break;

		default:
		break;
	    }
	    
	break;

	// объекты на lava
	case 0xE8:
	case 0xE9:
	case 0xEA:
	case 0xEB:

	    // большой вулкан
	    switch(index){

		/*case 0x00:
		    FillSPRITE(&sprite, "OBJNLAV3.ICN",  index + ((frame - 1) % 4) + 1);
		    info = GetICNHeader(&sprite);
		    rectObject = *dest;
		    rectObject.x += info->offsetX;
		    rectObject.y += info->offsetY;
		    SDL_BlitSurface(info->surface, NULL, video, &rectObject);
		    break;*/

		default:
		break;
	    }
	    
	break;

	// объекты на lava
	case 0xD8:
	case 0xD9:
	case 0xDA:
	case 0xDB:

	    switch(index){

		// дым лавы (9)
		case 0x58:
		case 0x62:
		    FillSPRITE(&sprite, "OBJNLAVA.ICN", index + ((frame - 1) % 9) + 1);
		    info = GetICNHeader(&sprite);
		    rectObject = *dest;
		    rectObject.x += info->offsetX;
		    rectObject.y += info->offsetY;
		    SDL_BlitSurface(info->surface, NULL, video, &rectObject);
		    break;

		default:
		break;
	    }
	    
	break;

	// объекты на земле №2
	case 0xA4:
	case 0xA5:
	case 0xA6:
	case 0xA7:

	    switch(index){

		// mercury капли и огонь (6)
		case 0x1B:
		FillSPRITE(&sprite, "OBJNMUL2.ICN", index + ((frame - 1) % 6) + 1);
		info = GetICNHeader(&sprite);
		rectObject = *dest;
		rectObject.x += info->offsetX;
		rectObject.y += info->offsetY;
		SDL_BlitSurface(info->surface, NULL, video, &rectObject);
		break;

		// свет маяка (9)
		case 0x3D:
		FillSPRITE(&sprite, "OBJNMUL2.ICN", index + ((frame - 1) % 9) + 1);
		info = GetICNHeader(&sprite);
		rectObject = *dest;
		rectObject.x += info->offsetX;
		rectObject.y += info->offsetY;
		SDL_BlitSurface(info->surface, NULL, video, &rectObject);
		break;

		// водяная мельница (6)
		case 0x53:
		case 0x5A:
		case 0x62:
		case 0x69:
		// огонь в загоне воров (6)
		case 0x81:
		// дым кузницы 2 трубы (6)
		case 0xA6:
		// дым кузницы 1 труба (6)
		case 0xAD:
		// тень от дыма кузницы (6)
		case 0xB4:
		// наркоман под грибом (6)
		case 0xBE:
		FillSPRITE(&sprite, "OBJNMUL2.ICN", index + ((frame - 1) % 6) + 1);
		info = GetICNHeader(&sprite);
		rectObject = *dest;
		rectObject.x += info->offsetX;
		rectObject.y += info->offsetY;
		SDL_BlitSurface(info->surface, NULL, video, &rectObject);
		break;

		default:
		break;
	    }
	    
	    break;

	// объекты на земле
	case 0xEC:
	case 0xED:
	case 0xEE:
	case 0xEF:

	    switch(index){

		// дым (9)
		case 0x05:
		// тень (9)
		case 0x0F:
		case 0x19:
		// дым (9)
		case 0x23:
		// тень (9)
		case 0x2D:
		FillSPRITE(&sprite, "OBJNMULT.ICN", index + ((frame - 1) % 9) + 1);
		info = GetICNHeader(&sprite);
		rectObject = *dest;
		rectObject.x += info->offsetX;
		rectObject.y += info->offsetY;
		SDL_BlitSurface(info->surface, NULL, video, &rectObject);
		break;

		// дым (6)
		case 0x5A:
		// тень (6)
		case 0x61:
		case 0x68:
		case 0x7C:
		// огонь (6)
		case 0x83:
		FillSPRITE(&sprite, "OBJNMULT.ICN", index + ((frame - 1) % 6) + 1);
		info = GetICNHeader(&sprite);
		rectObject = *dest;
		rectObject.x += info->offsetX;
		rectObject.y += info->offsetY;
		SDL_BlitSurface(info->surface, NULL, video, &rectObject);
		break;

		default:
		break;
	    }

	    break;

	// объекты на snow
	case 0xD0:
	case 0xD1:
	case 0xD2:
	case 0xD3:

	    switch(index){
		// огонь (6)
		case 0x04:
		FillSPRITE(&sprite, "OBJNSNOW.ICN",  index + ((frame - 1) % 6) + 1);
		info = GetICNHeader(&sprite);
		rectObject = *dest;
		rectObject.x += info->offsetX;
		rectObject.y += info->offsetY;
		SDL_BlitSurface(info->surface, NULL, video, &rectObject);
		break;

		// мельница (3 кадра)
		case 0x60:
		case 0x64:
		case 0x68:
		case 0x6C:
		case 0x70:
		case 0x74:
		case 0x78:
		case 0x7C:
		case 0x80:
		case 0x84:
		    FillSPRITE(&sprite, "OBJNSNOW.ICN", index + ((frame - 1) % 3) + 1);
		    info = GetICNHeader(&sprite);
		    rectObject = *dest;
		    rectObject.x += info->offsetX;
		    rectObject.y += info->offsetY;
		    SDL_BlitSurface(info->surface, NULL, video, &rectObject);
		    break;

		// mercury котел и огонь (6)
		case 0x96:
		// водяная мельница
		case 0xA2:
		case 0xA9:
		case 0xB1:
		case 0xB8:
		    FillSPRITE(&sprite, "OBJNSNOW.ICN", index + ((frame - 1) % 6) + 1);
		    info = GetICNHeader(&sprite);
		    rectObject = *dest;
		    rectObject.x += info->offsetX;
		    rectObject.y += info->offsetY;
		    SDL_BlitSurface(info->surface, NULL, video, &rectObject);
		    break;
    
		default:
		break;
	    }

	    break;

	// объекты на swamp
	case 0xD4:
	case 0xD5:
	case 0xD6:
	case 0xD7:

	    switch(index){

		// тень (6)
		case 0x00:
		// дым (6)
		case 0x07:
		// тень (6)
		case 0x0E:
		// свет в окнах (6)
		case 0x16:
		// дым (6)
		case 0x22:
		// тень (6)
		case 0x2B:
		// дым (6)
		case 0x33:
		// свет в окнах (6)
		case 0x3A:
		// свет в окнах (6)
		case 0x43:
		// свет в окнах (6)
		case 0x4A:
		    FillSPRITE(&sprite, "OBJNSWMP.ICN", index + ((frame - 1) % 6) + 1);
		    info = GetICNHeader(&sprite);
		    rectObject = *dest;
		    rectObject.x += info->offsetX;
		    rectObject.y += info->offsetY;
		    SDL_BlitSurface(info->surface, NULL, video, &rectObject);
		    break;
    
		default:
		break;
	    }

	default:
	    break;
    }
}
