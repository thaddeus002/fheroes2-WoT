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
#include "config.h"
#include "actionevent.h"
#include "animation.h"
#include "box.h"
#include "taverninfo.h"

void RedrawTavernAnimation(void);

S_ANIMATION *animTavern = NULL;

ACTION ShowTavernInfo(const char *rumor){

    CursorOff();
    SetIntValue(ANIM3, FALSE);
    
    Uint32 cursor = GetCursor();
    SDL_Surface *image = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect rect;
    ACTION result = NONE;
    BOOL exit = FALSE;
    ICNHEADER *header = NULL;
    INTERFACEACTION *dialog = NULL;        
    AGGSPRITE sprite;
    animTavern = NULL;
    
    const char *info = "A generous tip for the barkeep yields the following rumor:";
    
    S_BOX *box = NULL;

    if(NULL == (box = InitBox(160 + GetHeightText(info, FONT_BIG) + GetHeightText(rumor, FONT_BIG), &dialog, YES))) return NONE;

    // text header
    rect = box->rectArea;
    PrintAlignText(video, &rect, GetStringBuilding(KNIGHT, BUILD_TAVERN), FONT_BIG);


    // TAVWIN.ICN 0 1
    FillSPRITE(&sprite, "TAVWIN.ICN", 0);
    image = GetICNSprite(&sprite);
    rect.x = (video->w - image->w) / 2 + 5;
    rect.y = box->rectArea.y + 30;
    rect.w = image->w;
    rect.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rect);

    FillSPRITE(&sprite, "TAVWIN.ICN", 1);
    image = GetICNSprite(&sprite);
    rect.x = (video->w - image->w) / 2 + 5;
    rect.y = box->rectArea.y + 30 + 3;
    rect.w = image->w;
    rect.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rect);

    // TAVWIN.ICN anim 2 - 20 frame
    rect.x = box->rectArea.x + 26;
    rect.y = box->rectArea.y + 33;
    rect.w = 0;
    rect.h = 0;
    FillSPRITE(&sprite, "TAVWIN.ICN", 2);
    header = GetICNHeader(&sprite);
    AddAnimationEvent(&animTavern, &rect, header, 20);

    // text info
    rect = box->rectArea;
    rect.y = box->rectArea.y + 140;
    PrintAlignText(video, &rect, info, FONT_BIG);

    // text rumor
    if( ! rumor) rumor = "He he he... Debugger Hero!";
    rect.y += 30 + GetHeightText(rumor, FONT_BIG);
    PrintAlignText(video, &rect, rumor, FONT_BIG);

    // Отрисовка диалога
    SDL_Flip(video);

    SetCursor(CURSOR_POINTER);

    CursorOn();

    // цикл событий
    while(! exit)
    
        switch(ActionCycle(dialog, RedrawTavernAnimation)){
    
            case EXIT:
                exit = TRUE;
                result = EXIT;
                break;
    
            case ESC:
            case ENTER:
            case YES:
                exit = TRUE;
                result = NONE;
                break;

            default:
                break;
    
        }
                                    
    CursorOff();

    FreeAnimationEvent(animTavern);
    FreeBox(box);

    SetCursor(cursor);

    SetIntValue(ANIM3, TRUE);
    CursorOn();

    return result;
}


void RedrawTavernAnimation(void){

    if(! GetIntValue(ANIMATION)) return;

    static Uint32 animationFrame = 0;
 
    Sint32 x; 
    Sint32 y; 
     
    SDL_Surface *video = SDL_GetVideoSurface(); 
    SDL_GetMouseState(&x, &y); 
    S_ANIMATION *ptr = animTavern;
 
    while(ptr){ 
 
        CursorOff(); 
 
	// востановить фон предыдущего спрайта
	if(ptr->background)

    	    SDL_BlitSurface(ptr->background, NULL, video, &ptr->rectBack);

	else if(NULL == (ptr->background = SDL_CreateRGBSurface(SDL_SWSURFACE, ptr->rect[animationFrame % ptr->count].w, ptr->rect[animationFrame % ptr->count].h, 16, 0, 0, 0, 0))){
            fprintf(stderr, "RedrawMenuAnimation: CreateRGBSurface failed: %s, %d, %d\n", SDL_GetError(), ptr->rect[animationFrame % ptr->count].w, ptr->rect[animationFrame % ptr->count].h);
            return;
        }

	// сохраняем фон нового спрайта
    	SDL_BlitSurface(video, &ptr->rect[animationFrame % ptr->count], ptr->background, NULL);
	ptr->rectBack = ptr->rect[animationFrame % ptr->count];

	// рисуем спрайт
        SDL_BlitSurface(ptr->surface[animationFrame % ptr->count], NULL, video, &ptr->rect[animationFrame % ptr->count]); 

        CursorOn(); 
        ptr = ptr->next; 
    } 
 
    SDL_Flip(video); 
    SDL_Delay(GetIntValue(ANIMATIONDELAY));     

    ++animationFrame;

    return;
}
