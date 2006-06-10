/* 
    tools.c
*/

#include <stdio.h>
#include <stdlib.h>
#include "SDL.h"
#include "tools.h"

void Slock(SDL_Surface *surface){ 

    if(SDL_MUSTLOCK(surface))
	SDL_LockSurface(surface);
} 

void Sulock(SDL_Surface *surface){

    if(SDL_MUSTLOCK(surface))
	SDL_UnlockSurface(surface); 
}

void DrawPixel(SDL_Surface *screen, Uint16 x, Uint16 y, Uint8 color){

    if(x > screen->w || y > screen->h) return;

    Uint8 *bufp; 
    bufp = (Uint8 *) screen->pixels + y * screen->pitch + x;
    *bufp = color; 
}

void DrawScene(SDL_Surface *surface, Uint8 *ptr, Uint32 sizesprite){

    Uint16 x = 0;
    Uint16 w = surface->w;
    //Uint16 h = surface->h;
    Uint16 y = 0;
    Uint8 i;
    Uint32 maxptr = (Uint32) ptr + sizesprite;

    Slock(surface);

    // цикл по данным
    for(;;){

	// проверка за границу буффера данных
	if((Uint32) ptr > maxptr){
	    fprintf(stderr, "\nSAFE: MEMORY LEAK. PARACHUTE!\n");
	    break;
	}

	// 0x00 конец строки
	if(0 == *ptr){
	    ++y;
	    x = 0;
	    if(ICNRLE_DEBUG) fprintf(stderr, " N:0x00\n");
	    ++ptr;
	    continue;

	// конец данных
	}else if(0x80 == *ptr){
	    if(ICNRLE_DEBUG) fprintf(stderr, "\nEND:80\n");
	    break;

	// (w а может и больше?) и 0 или 0x80 пропускаем
	}else if(w >= *ptr && (0 == ptr[1] || (0x80 == ptr[1] && (Uint32) ptr + 2 >= maxptr))){
	    if(ICNRLE_DEBUG) fprintf(stderr, " W:%hhX", *ptr);
	    ++ptr;
	    continue;

	// сами данные
	}else if(0x80 > *ptr){
	    if(ICNRLE_DEBUG) fprintf(stderr, " M:%hhX C:%d", *ptr, *ptr);
	    Uint8 current = *ptr;
	    ++ptr;
	    for(i = 0; i < current; ++i){
		DrawPixel(surface, x, y, *ptr);
		if(ICNRLE_DEBUG) fprintf(stderr, " D:%hhX", *ptr);
		++ptr;
		++x;
	    }
	    continue;

	// диаппазон 0x80 < смещение > 0xC0 
	}else if(0x80 < *ptr && 0xC0 > *ptr){
	    if(ICNRLE_DEBUG) fprintf(stderr, " Z:%hhX S:%d", *ptr, *ptr - 0x80);
	    x = x + *ptr - 0x80;
	    ++ptr;
	    continue;

	// 0xC0 смещение
	}else if(0xC0 == *ptr){
	    if(ICNRLE_DEBUG) fprintf(stderr, " M:%hhX", *ptr);
	    ++ptr;

	    if( 0 == *ptr % 4){
		if(ICNRLE_DEBUG) fprintf(stderr, " Z:%hhX", *ptr);
		++ptr;
		if(ICNRLE_DEBUG) fprintf(stderr, " S:%d", *ptr);
		x += *ptr;
		++ptr;
		continue;

	    }else{
		if(ICNRLE_DEBUG) fprintf(stderr, " Z:%hhX S:%d", *ptr, *ptr % 4);
		x += *ptr % 4;
		++ptr;
		continue;
	    }

	// 0xC1 N D количество данные
	}else if(0xC1 == *ptr){
	    if(ICNRLE_DEBUG) fprintf(stderr, " M:%hhX", *ptr);
	    ++ptr;
	    Uint8 count = *ptr;
	    if(ICNRLE_DEBUG) fprintf(stderr, " C:%d", count);
	    ++ptr;
	    for(i = 0; i < count; ++i){
	    	DrawPixel(surface, x, y, *ptr);
	    	if(ICNRLE_DEBUG) fprintf(stderr, " D:%hhX", *ptr);
	    	++x;
	    }
	    ++ptr;
	    continue;

	// 0xC2 и больше
	}else if(0xC1 < *ptr){
	    if(ICNRLE_DEBUG) fprintf(stderr, " M:%hhX", *ptr);
	    Uint8 count;
	    count = *ptr - 0xC0;
	    if(ICNRLE_DEBUG) fprintf(stderr, " C:%hhX:%d", *ptr, count);
	    for(i = 0; i < count; ++i){
		DrawPixel(surface, x, y, ptr[1]);
		if(ICNRLE_DEBUG) fprintf(stderr, " D:%hhX", ptr[1]);
		++x;
	    }
	    ptr += 2;
	    continue;

	// неизвестно (пропарсить!)
	}else{
	    fprintf(stderr, " UNK:%hhX", *ptr);
		DrawPixel(surface, x, y, *ptr);
		++x;
		++ptr;
	}
    }

    Sulock(surface);
}

