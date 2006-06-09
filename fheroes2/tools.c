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

#include <string.h>
#include "gamedefs.h"
#include "SDL.h"
#include "tools.h"

BOOL ValidPoint(SDL_Rect *rect, Uint16 x, Uint16 y){

    if(x >= rect->x && x <= rect->x + rect->w && y >= rect->y && y <= rect->y + rect->h)
        return TRUE;
    else
        return FALSE;
}

BOOL CompareRect(SDL_Rect *a, SDL_Rect *b){
    if(a->x == b->x && a->y == b->y && a->w == b->w && a->h == b->h)
	return TRUE;
    else
	return FALSE;
}

void FillSPRITE(AGGSPRITE *sprite, const char *name, Uint16 number){

    strncpy(sprite->name, name, strlen(name) + 1);
    sprite->number = number;
}

Uint16 GetHiUint32(Uint32 object){

    object >>= sizeof(Uint16) * 8;
        
        return (Uint16) object;
}
	
Uint16 GetLoUint32(Uint32 object){

    object <<= sizeof(Uint16) * 8;
    object >>= sizeof(Uint16) * 8;

    return (Uint16) object;
}

Uint8 GetHiUint16(Uint16 object){

    object >>= sizeof(Uint8) * 8;

        return (Uint8) object;
}

Uint8 GetLoUint16(Uint16 object){

    object <<= sizeof(Uint8) * 8;
    object >>= sizeof(Uint8) * 8;

    return (Uint8) object;
}

Uint32 PackUint32(Uint16 hiWord, Uint16 loWord){

    Uint32 dword = hiWord;

    dword <<= sizeof(Uint16) * 8;
    dword |= loWord;
    
    return dword;
}

Uint16 PackUint16(Uint8 hiByte, Uint8 loByte){

    Uint16 word = hiByte;

    word <<= sizeof(Uint8) * 8;
    word |= loByte;
    
    return word;
}

void PrintText(SDL_Surface *surface, SDL_Rect *rect, const char *string, ENUMFONT font){

    SDL_Surface	*letter = NULL;
    SDL_Rect	dst = *rect;
    Uint16	rezy = rect->y;
    AGGSPRITE	sprite;

    // парсим string по буквам
    while(*string){

    	// здесь пробел
	if(0x20 == *string)
	    switch(font){
		case FONT_SMALL:
		    dst.x += FONT_WIDTHSMALL / 2;
		    break;
		case FONT_BIG:
		    dst.x += FONT_WIDTHBIG / 2;
		    break;
		default:
		    break;
	    }

	// если \t
	else if(0x09 == *string){
	    switch(font){
		case FONT_SMALL:
		    dst.x += 4 * FONT_WIDTHSMALL / 2;
		    break;
		case FONT_BIG:
		    dst.x += 4 * FONT_WIDTHBIG / 2;
		    break;
		default:
		    break;
	    }

	// если \n
	}else if(0x0A == *string){

		dst.x = rect->x;

		switch(font){
		    case FONT_BIG:
			rezy += FONT_HEIGHTBIG + 4;
			break;
		    case FONT_SMALL:
			rezy += FONT_HEIGHTSMALL + 2;
			break;
		    default:
			break;
		}

		dst.y = rezy;

		if(dst.y > (rect->y + rect->h)) break;

	// если в диаппазоне FONT_LETTER_BEGIN FONT_LETTER_END
	}else if(FONT_LETTER_BEGIN < *string && FONT_LETTER_END > *string){

	    // рисуем букву
	    if(font == FONT_BIG)
		FillSPRITE(&sprite,"FONT.ICN", *string - FONT_LETTER_BEGIN);
	    else
		FillSPRITE(&sprite,"SMALFONT.ICN", *string - FONT_LETTER_BEGIN);
	    letter = GetICNSprite(&sprite);

	    // выравнивание по высоте буквы
	    dst.y = rezy;
	    if(font == FONT_SMALL)
		switch(*string){

		    case 'y':
		    case 'g':
		    case 'p':
		    case 'q':
		    case 'j':
	    		    dst.y += FONT_HEIGHTSMALL - letter->h + 2;
			break;

		    default:
			if(letter->h < FONT_HEIGHTSMALL)
	    		    dst.y += FONT_HEIGHTSMALL - letter->h;
			break;
		}
	    else if(font == FONT_BIG)
		switch(*string){

		    case 'y':
		    case 'g':
		    case 'p':
		    case 'q':
		    case 'j':
	    		    dst.y += FONT_HEIGHTBIG - letter->h + 3;
			break;

		    default:
			if(letter->h < FONT_HEIGHTBIG)
	    		    dst.y += FONT_HEIGHTBIG - letter->h;
			break;
		}
	    
	    SDL_BlitSurface(letter, NULL, surface, &dst);
	    dst.x += letter->w;
	    if(dst.x > (rect->x + rect->w)){
		dst.x = rect->x;

		switch(font){
		    case FONT_BIG:
			rezy += FONT_HEIGHTBIG;
			dst.y = rezy;
			break;
		    case FONT_SMALL:
			rezy += FONT_HEIGHTSMALL;
			dst.y = rezy;
			break;
		    default:
			break;
		}

		if(rezy > (rect->y + rect->h)) break;
	    }
	}

	++string;
    }
}

Uint32 GetSizeSurface(SDL_Surface *surface){

    if(NULL == surface) return 0;

    Uint32 result;
    
    result = sizeof(SDL_Surface) + sizeof(SDL_PixelFormat) + surface->pitch * surface->h;

    if(surface->format->palette != NULL)
	result += sizeof(SDL_Palette) + surface->format->palette->ncolors * sizeof(SDL_Color);

    return result;
}
