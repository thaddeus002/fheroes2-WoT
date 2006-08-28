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
#include "castle.h"
#include "taverninfo.h"

void RedrawTavernAnimation(void);

S_ANIMATION *animTavern = NULL;

ACTION ShowTavernInfo(const char *rumor){

    CursorOff();
    SetIntValue(ANIM3, FALSE);
    
    Uint32 cursor = GetCursor();
    SDL_Surface *back, *image, *video;
    SDL_Rect rectBack, rectCur;
    ACTION result = NONE;
    BOOL exit = FALSE;
    ICNHEADER *header = NULL;
    INTERFACEACTION action;
    INTERFACEACTION *dialog = NULL;        
    AGGSPRITE sprite;
    Uint8 i;
    animTavern = NULL;
    
    const char *buybuild = "BUYBUILD.ICN";
    const char *system = "SYSTEM.ICN";
    if(GetIntValue(EVILINTERFACE)){
	buybuild = "BUYBUILE.ICN";
	system = "SYSTEME.ICN";
    }
    
    Uint16 height = 0;
    Uint16 width = 0;
    Uint8 count = 0;
    // получаем левый верхний спрайт
    FillSPRITE(&sprite, buybuild, 4);
    image = GetICNSprite(&sprite);
    width += image->w;
    // получаем правый верхний спрайт
    FillSPRITE(&sprite, buybuild, 0);
    width += image->w;

    Uint8 max = (GetLengthText(rumor, FONT_BIG) / (width - 70));

    // получаем левый средний спрайт
    FillSPRITE(&sprite, buybuild, 5);
    image = GetICNSprite(&sprite);
    // количество средних блоков
    count = 3 + max;
    height += count * image->h;

    // отрисовка диалога по центру экрана
    video = SDL_GetVideoSurface();
    rectBack.w = width;
    rectBack.h = height;
    // поправка на верхний и нижний
    FillSPRITE(&sprite, buybuild, 4);
    image = GetICNSprite(&sprite);
    rectBack.h += image->h;
    FillSPRITE(&sprite, buybuild, 6);
    image = GetICNSprite(&sprite);
    rectBack.h += image->h;
    rectBack.x = (video->w - rectBack.w) / 2;
    rectBack.y = (video->h - rectBack.h) / 2;

    // сохраняем бакгроунд
    if(NULL == (back = SDL_CreateRGBSurface(SDL_SWSURFACE, rectBack.w, rectBack.h, 16, 0, 0, 0, 0))){
	fprintf(stderr, "ShowTavernInfo: CreateRGBSurface failed: %s\n", SDL_GetError());
	return EXIT;
    }    

    SDL_BlitSurface(video, &rectBack, back, NULL);

    // получаем левый верхний спрайт
    FillSPRITE(&sprite, buybuild, 4);
    image = GetICNSprite(&sprite);
    if(GetIntValue(EVILINTERFACE)) rectCur.x = rectBack.x; else rectCur.x = rectBack.x + 1;
    rectCur.y = rectBack.y;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // получаем левый средний спрайт
    FillSPRITE(&sprite, buybuild, 5);
    image = GetICNSprite(&sprite);
    rectCur.x = rectBack.x;
    rectCur.w = image->w;
    rectCur.y += rectCur.h;
    rectCur.h = image->h;
    for(i = 0; i < count; ++i){
	SDL_BlitSurface(image, NULL, video, &rectCur);
	rectCur.y += rectCur.h;
    }

    // получаем левый нижний спрайт
    FillSPRITE(&sprite, buybuild, 6);
    image = GetICNSprite(&sprite);
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // получаем правый верхний спрайт
    FillSPRITE(&sprite, buybuild, 0);
    image = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + rectCur.w;
    rectCur.y = rectBack.y;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // получаем правый средний спрайт
    FillSPRITE(&sprite, buybuild, 1);
    image = GetICNSprite(&sprite);
    rectCur.w = image->w;
    rectCur.y += rectCur.h;
    rectCur.h = image->h;
    for(i = 0; i < count; ++i){
	SDL_BlitSurface(image, NULL, video, &rectCur);
	rectCur.y += rectCur.h;
    }

    // получаем правый нижний спрайт
    FillSPRITE(&sprite, buybuild, 2);
    image = GetICNSprite(&sprite);
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // text header
    rectCur.x = rectBack.x + 35;
    rectCur.y = rectBack.y + 50;
    rectCur.w = rectBack.w - 70;
    rectCur.h = rectBack.h - 70;
    PrintAlignText(video, &rectCur, GetStringBuilding(KNIGHT, BUILD_TAVERN), FONT_BIG);

    // TAVWIN.ICN 0 1
    FillSPRITE(&sprite, "TAVWIN.ICN", 0);
    image = GetICNSprite(&sprite);
    rectCur.x = (video->w - image->w) / 2;
    rectCur.y = rectBack.y + 75;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);

    FillSPRITE(&sprite, "TAVWIN.ICN", 1);
    image = GetICNSprite(&sprite);
    rectCur.x = (video->w - image->w) / 2;
    rectCur.y = rectBack.y + 75 + 3;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // TAVWIN.ICN anim 2 - 20 frame
    rectCur.x = rectBack.x + 69;
    rectCur.y = rectBack.y + 78;
    rectCur.w = 0;
    rectCur.h = 0;
    FillSPRITE(&sprite, "TAVWIN.ICN", 2);
    header = GetICNHeader(&sprite);
    AddAnimationEvent(&animTavern, &rectCur, header, 20);

    // text info
    rectCur.x = rectBack.x + 35;
    rectCur.y = rectBack.y + 180;
    rectCur.w = rectBack.w - 70;
    rectCur.h = rectBack.h - 70;
    PrintAlignText(video, &rectCur, "A generous tip for the barkeep yields the following rumor:", FONT_BIG);

    // text rumor
    if( ! rumor) rumor = "He he he... Debugger Hero!";
    rectCur.x = rectBack.x + 35;
    rectCur.y = rectBack.y + 235;
    rectCur.w = rectBack.w - 70;
    rectCur.h = rectBack.h - 70;
    PrintAlignText(video, &rectCur, rumor, FONT_BIG);

    // кнопка YES
    FillSPRITE(&sprite, system, 5);
    image = GetICNSprite(&sprite);
    rectCur.x = (video->w - image->w) / 2;
    rectCur.y = rectBack.y + rectBack.h - 73;
    rectCur.w = image->w;
    rectCur.h = image->h;
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, system, 5);
    FillSPRITE(&action.objectPush, system, 6);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressOK;
    AddActionEvent(&dialog, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);

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
            case OK:
                exit = TRUE;
                result = NONE;
                break;

            default:
                break;
    
        }
                                    
    // востанавливаем бакгроунд
    CursorOff();
    SDL_BlitSurface(back, NULL, video, &rectBack);
    //SDL_Flip(video);

    FreeAnimationEvent(animTavern);
    FreeActionEvent(dialog);
    SDL_FreeSurface(back);

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
