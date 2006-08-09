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
#include "castle.h"
#include "config.h"
#include "element.h"
#include "actionevent.h"
#include "castle.h"
#include "monster.h"
#include "wellinfo.h"

ACTION ActionWellInfoPressExit(void);

ACTION ShowWellInfo(void){

    CursorOff();
    SetIntValue(ANIM3, FALSE);

    SDL_Surface *back, *image, *video;
    SDL_Rect rectBack, rectCur;
    Uint16 cx, cy;
    AGGSPRITE sprite;
    BOOL exit = FALSE;
//    char str[64];

//    INTERFACEACTION action;
    INTERFACEACTION *dialog = NULL;        
    ACTION result;

//    const S_CASTLE *castle = GetCurrentCastle();

    FillSPRITE(&sprite, "WELLBKG.ICN", 0);
    image = GetICNSprite(&sprite);

    // отрисовка диалога по центру экрана
    video = SDL_GetVideoSurface();
    if(GetIntValue(VIDEOMODE)){
        rectBack.x = video->w / 2 - 320 - BORDERWIDTH - SHADOWWIDTH;
        rectBack.y = video->h / 2 - 240 - BORDERWIDTH;
	rectBack.w = 640 + 2 * BORDERWIDTH + SHADOWWIDTH;
        rectBack.h = 480 + 2 * BORDERWIDTH + SHADOWWIDTH;
    }else{
        rectBack.x = 0;
        rectBack.y = 0;
        rectBack.w = 640;
        rectBack.h = 480;
    }

    // сохраняем бакгроунд
    if(NULL == (back = SDL_CreateRGBSurface(SDL_SWSURFACE, rectBack.w, rectBack.h, 16, 0, 0, 0, 0))){
	fprintf(stderr, "DialogRecrutMonster: CreateRGBSurface failed: %s\n", SDL_GetError());
	return 0;
    }
    SDL_BlitSurface(video, &rectBack, back, NULL);

    // отображаем фон диалога
    if(GetIntValue(VIDEOMODE)) ShowBorder(&rectBack);
    rectCur.x = 0;
    rectCur.y = 0;
    rectCur.w = image->w;
    rectCur.h = image->h;
    if(GetIntValue(VIDEOMODE)){
        rectCur.x = rectBack.x + BORDERWIDTH + SHADOWWIDTH;
        rectCur.y = rectBack.y + BORDERWIDTH;
    }
    SDL_FillRect(video, &rectCur, 0);
    SDL_BlitSurface(image, NULL, video, &rectCur);
    cx = rectCur.x;
    cy = rectCur.y;

/*
    // портрет героя PORT00XX
    FillSPRITE(&sprite, HeroesBigNamePortrait(name), 0);
    image = GetICNSprite(&sprite);
    rectCur.x = cx + 49;
    rectCur.y = cy + 31;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // знак
    switch(heroes->color){
        default:
        case BLUE:
            FillSPRITE(&sprite, "CREST.ICN", 0);
            break;
        case GREEN:
            FillSPRITE(&sprite, "CREST.ICN", 1);
            break;
        case RED:
            FillSPRITE(&sprite, "CREST.ICN", 2);
            break;
        case YELLOW:
            FillSPRITE(&sprite, "CREST.ICN", 3);
    	    break;
        case ORANGE:
            FillSPRITE(&sprite, "CREST.ICN", 4);
    	    break;
        case PURPLE:
            FillSPRITE(&sprite, "CREST.ICN", 5);
            break;
    }
    image = GetICNSprite(&sprite);
    rectCur.x = cx + 49;
    rectCur.y = cy + 130;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // фон монстров STRIP.ICN 2
    // фон спец SECSKILL 0
    // фон артифактов ARTIFACT 0

    // название монстра
    sprintf(str, "Recrut %s", monster->descriptions);
    rectCur.x = rectBack.x + 160;
    rectCur.y = rectBack.y + 25;
    rectCur.w = GetLengthText(str, FONT_BIG);
    rectCur.h = FONT_HEIGHTBIG;
    rectCur.x = rectCur.x - rectCur.w / 2;
    PrintText(video, &rectCur, str, FONT_BIG);

    // картинка монстра
    char number[5];
    Uint8 j;
    sprintf(number, "%4d", emonster);
    for(j = 0; j < 4; j++)
        if(0 == strncmp(&number[j], " ", 1)) number[j] = '0';
    sprintf(str, "MONH%4s.ICN", number);
    FillSPRITE(&sprite, str, 0);
    image = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + 50;
    rectCur.y = rectBack.y + 45;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // text number to buy
    sprintf(str, "Number to buy:");
    rectCur.x = rectBack.x + 30;
    rectCur.y = rectBack.y + 163;
    rectCur.w = GetLengthText(str, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, str, FONT_SMALL);


    // рисуем кнопки
    // кнопка Dismiss
    FillSPRITE(&sprite, "HSBTNS.ICN", 0);
    image = GetICNSprite(&sprite);
    rectCur.x = cx + 5;
    rectCur.y = cy + 318;
    rectCur.w = image->w;
    rectCur.h = image->h;
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, "HSBTNS.ICN", 0);
    FillSPRITE(&action.objectPush, "HSBTNS.ICN", 1);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionHeroesInfoPressDismiss;
    AddActionEvent(&dialog, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // кнопка Exit
    FillSPRITE(&sprite, "HSBTNS.ICN", 2);
    image = GetICNSprite(&sprite);
    rectCur.x = cx + 603;
    rectCur.y = cy + 318;
    rectCur.w = image->w;
    rectCur.h = image->h;
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, "HSBTNS.ICN", 2);
    FillSPRITE(&action.objectPush, "HSBTNS.ICN", 3);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionHeroesInfoPressExit;
    AddActionEvent(&dialog, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);
*/
    // Отрисовка диалога
    CursorOn();

    // цикл событий
    while(! exit)

        switch(ActionCycle(dialog)){

    	    case EXIT:
    		exit = TRUE;
    		result = EXIT;
    		break;

    	    case ESC:
    	    case CANCEL:
    		exit = TRUE;
    		result = NONE;
    		break;

    	    case DISMISS:
    		exit = TRUE;
    		result = DISMISS;
    		break;

    	    default:
		result = NONE;
    		break;

	}

    // востанавливаем бакгроунд
    CursorOff();
    SDL_BlitSurface(back, NULL, video, &rectBack);

    FreeActionEvent(dialog);
    SDL_FreeSurface(back);

    SetIntValue(ANIM3, TRUE);
    CursorOn();

    return result;
}

ACTION ActionWellInfoPressExit(void){

    return CANCEL;
}
