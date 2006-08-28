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

#include "tools.h"
#include "cursor.h"
#include "debug.h"
#include "config.h"
#include "element.h"
#include "actionevent.h"
#include "spell.h"
#include "gamedefs.h"
#include "actionevent.h"
#include "thievesguild.h"


ACTION  ShowThievesGuildInfo(void){

    CursorOff();
    SetIntValue(ANIM2, FALSE);
    SetIntValue(ANIM3, FALSE);

    SDL_Surface *back, *image, *video;
    SDL_Rect rectBack, rectCur;
    Uint16 cx, cy;
    AGGSPRITE sprite;
    const char *message = NULL;
    BOOL exit = FALSE;

    INTERFACEACTION action;
    INTERFACEACTION *dialog = NULL;        
    ACTION result = NONE;

    FillSPRITE(&sprite, "STONEBAK.ICN", 0);
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
	fprintf(stderr, "ShowThievesGuildInfo: CreateRGBSurface failed: %s\n", SDL_GetError());
	return EXIT;
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

    // отображаем bar
    FillSPRITE(&sprite, "WELLXTRA.ICN", 2);
    image = GetICNSprite(&sprite);
    rectCur.x = cx;
    rectCur.y = cy + 480 - image->h;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // Text
    message = "Thieves' Guild: Player RanKings";
    rectCur.x = video->w / 2;
    rectCur.y = video->h / 2 + 240 - BORDERWIDTH;
    rectCur.x = rectCur.x - GetLengthText(message, FONT_BIG) / 2;
    rectCur.y = rectCur.y - 2;
    rectCur.w = GetLengthText(message, FONT_BIG);
    rectCur.h = FONT_HEIGHTBIG;
    PrintText(video, &rectCur, message, FONT_BIG);
                            
    // кнопка Exit
    FillSPRITE(&sprite, "WELLXTRA.ICN", 0);
    image = GetICNSprite(&sprite);
    rectCur.x = cx + 578;
    rectCur.y = cy + 461;
    rectCur.w = image->w;
    rectCur.h = image->h;
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, "WELLXTRA.ICN", 0);
    FillSPRITE(&action.objectPush, "WELLXTRA.ICN", 1);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressCANCEL;
    AddActionEvent(&dialog, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // Number of Towns:
    message = "Number of Towns:";
    rectCur.x = cx + 190;
    rectCur.y = cy + 10 + FONT_HEIGHTBIG + FONT_HEIGHTBIG / 2;
    rectCur.w = GetLengthText(message, FONT_BIG);
    rectCur.h = FONT_HEIGHTBIG;
    rectCur.x -= rectCur.w;
    PrintText(video, &rectCur, message, FONT_BIG);
    // Number of Castles:
    message = "Number of Castles:";
    rectCur.x = cx + 190;
    rectCur.y += FONT_HEIGHTBIG + FONT_HEIGHTBIG / 2;
    rectCur.w = GetLengthText(message, FONT_BIG);
    rectCur.h = FONT_HEIGHTBIG;
    rectCur.x -= rectCur.w;
    PrintText(video, &rectCur, message, FONT_BIG);
    // Number of Heroes:
    message = "Number of Heroes:";
    rectCur.x = cx + 190;
    rectCur.y += FONT_HEIGHTBIG + FONT_HEIGHTBIG / 2;
    rectCur.w = GetLengthText(message, FONT_BIG);
    rectCur.h = FONT_HEIGHTBIG;
    rectCur.x -= rectCur.w;
    PrintText(video, &rectCur, message, FONT_BIG);
    // Gold in Treasury:
    message = "Gold in Treasury:";
    rectCur.x = cx + 190;
    rectCur.y += FONT_HEIGHTBIG + FONT_HEIGHTBIG / 2;
    rectCur.w = GetLengthText(message, FONT_BIG);
    rectCur.h = FONT_HEIGHTBIG;
    rectCur.x -= rectCur.w;
    PrintText(video, &rectCur, message, FONT_BIG);
    // Wood & Ore:
    message = "Wood & Ore:";
    rectCur.x = cx + 190;
    rectCur.y += FONT_HEIGHTBIG + FONT_HEIGHTBIG / 2;
    rectCur.w = GetLengthText(message, FONT_BIG);
    rectCur.h = FONT_HEIGHTBIG;
    rectCur.x -= rectCur.w;
    PrintText(video, &rectCur, message, FONT_BIG);
    // Gems, Cr, Slf & Mer:
    message = "Gems, Cr, Slf & Mer:";
    rectCur.x = cx + 190;
    rectCur.y += FONT_HEIGHTBIG + FONT_HEIGHTBIG / 2;
    rectCur.w = GetLengthText(message, FONT_BIG);
    rectCur.h = FONT_HEIGHTBIG;
    rectCur.x -= rectCur.w;
    PrintText(video, &rectCur, message, FONT_BIG);
    // Obelisks Found:
    message = "Obelisks Found:";
    rectCur.x = cx + 190;
    rectCur.y += FONT_HEIGHTBIG + FONT_HEIGHTBIG / 2;
    rectCur.w = GetLengthText(message, FONT_BIG);
    rectCur.h = FONT_HEIGHTBIG;
    rectCur.x -= rectCur.w;
    PrintText(video, &rectCur, message, FONT_BIG);
    // Total Army Strength:
    message = "Total Army Strength:";
    rectCur.x = cx + 190;
    rectCur.y += FONT_HEIGHTBIG + FONT_HEIGHTBIG / 2;
    rectCur.w = GetLengthText(message, FONT_BIG);
    rectCur.h = FONT_HEIGHTBIG;
    rectCur.x -= rectCur.w;
    PrintText(video, &rectCur, message, FONT_BIG);
    // Income:
    message = "Income:";
    rectCur.x = cx + 190;
    rectCur.y += FONT_HEIGHTBIG + FONT_HEIGHTBIG / 2;
    rectCur.w = GetLengthText(message, FONT_BIG);
    rectCur.h = FONT_HEIGHTBIG;
    rectCur.x -= rectCur.w;
    PrintText(video, &rectCur, message, FONT_BIG);
    //
    //
    // Best Hero Stats:
    message = "Best Hero Stats:";
    rectCur.x = cx + 190;
    rectCur.y += 3 * ( FONT_HEIGHTBIG + FONT_HEIGHTBIG / 2 );
    rectCur.w = GetLengthText(message, FONT_BIG);
    rectCur.h = FONT_HEIGHTBIG;
    rectCur.x -= rectCur.w;
    PrintText(video, &rectCur, message, FONT_BIG);
    //
    // Personality:
    message = "Personality:";
    rectCur.x = cx + 190;
    rectCur.y += 2 * ( FONT_HEIGHTBIG + FONT_HEIGHTBIG / 2 );
    rectCur.w = GetLengthText(message, FONT_BIG);
    rectCur.h = FONT_HEIGHTBIG;
    rectCur.x -= rectCur.w;
    PrintText(video, &rectCur, message, FONT_BIG);
    //
    // Best Monster:
    message = "Best Monster:";
    rectCur.x = cx + 190;
    rectCur.y += 2 * ( FONT_HEIGHTBIG + FONT_HEIGHTBIG / 2 );
    rectCur.w = GetLengthText(message, FONT_BIG);
    rectCur.h = FONT_HEIGHTBIG;
    rectCur.x -= rectCur.w;
    PrintText(video, &rectCur, message, FONT_BIG);

    // Отрисовка диалога
    CursorOn();

    // цикл событий
    while(! exit)

        switch(ActionCycle(dialog, NULL)){

    	    case EXIT:
    		exit = TRUE;
    		result = EXIT;
    		break;

    	    case ESC:
    	    case CANCEL:
    		exit = TRUE;
    		result = NONE;
    		break;

    	    default:
    		break;

	}

    // востанавливаем бакгроунд
    CursorOff();
    SDL_BlitSurface(back, NULL, video, &rectBack);

    FreeActionEvent(dialog);
    SDL_FreeSurface(back);

    SetIntValue(ANIM2, TRUE);
    SetIntValue(ANIM3, TRUE);
    CursorOn();

    return result;
}
