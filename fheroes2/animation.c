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
#include "agg.h"
#include "tools.h"
#include "cursor.h"
#include "config.h"
#include "animation.h"

S_ANIMATION * AddAnimationEvent(S_ANIMATION **pointer, SDL_Rect *rect, ICNHEADER *header, Uint8 count){

    S_ANIMATION *ptr;
    ICNHEADER   *current;
    Uint8 i;

    if(NULL == rect || NULL == header || 0 == count) return NULL;

    if(NULL == *pointer){

	if(NULL == (*pointer = (S_ANIMATION *) malloc(sizeof(S_ANIMATION)))){
    	    fprintf(stderr, "AddAnimationEvent: error malloc: %d\n", sizeof(S_ANIMATION));
    	    return NULL;
	}
    	ptr = *pointer;

    }else{

	ptr = *pointer;
	while(ptr->next) ptr = (S_ANIMATION *) ptr->next;

	if(NULL == (ptr->next = malloc(sizeof(S_ANIMATION)))){
    	    fprintf(stderr, "AddAnimationEvent: error malloc: %d\n", sizeof(S_ANIMATION));
    	    return NULL;
	}

	ptr = (S_ANIMATION *) ptr->next;
    }

    ptr->next = NULL;
    ptr->count = count;

    if(NULL == (ptr->rect = malloc(count * sizeof(SDL_Rect)))){
    	fprintf(stderr, "AddAnimationEvent: error malloc: %d\n", count * sizeof(SDL_Rect));
    	return NULL;
    }

    if(NULL == (ptr->surface = malloc(count * sizeof(SDL_Surface *)))){
    	fprintf(stderr, "AddAnimationEvent: error malloc: %d\n", count * sizeof(SDL_Surface *));
    	return NULL;
    }

    current = header;

    for(i = 0; i < count; ++i){

	ptr->rect[i].x = rect->x + current->offsetX;
	ptr->rect[i].y = rect->y + current->offsetY;
	ptr->rect[i].w = current->surface->w;
	ptr->rect[i].h = current->surface->h;
	ptr->surface[i] = current->surface;

	if(current->next) current = current->next;
    }

    return ptr;
}

void FreeAnimationEvent(S_ANIMATION *head){

    S_ANIMATION *ptr = head;

    while(ptr){
        head = (S_ANIMATION *) head->next;
	
	if(ptr->rect) free(ptr->rect);
	if(ptr->surface) free(ptr->surface);
        free(ptr);

        ptr = head;
    }
}
