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
#include "castle.h"
#include "box.h"
#include "payment.h"
#include "castledialog.h"

#define NUMLEN	8
#define TEXTLEN	128

ACTION BuyBuildingBox(E_RACE race, E_BUILDINGCASTLE build, Uint32 flag){

    CursorOff();
    SetIntValue(ANIM2, FALSE);
    SetIntValue(ANIM3, FALSE);
    
    Uint32 cursor = GetCursor();
    SDL_Surface *image = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect dst, src;
    SDL_Event event;

    ACTION result = NONE;
    INTERFACEACTION *dialog = NULL;        
    AGGSPRITE sprite;
    BOOL exit = FALSE;
    Uint8 index = 0;
    const S_PAYMENT *payment = PaymentConditionsBuilding(race, build);    
    const char *icnsprite = NULL;
    const char *depens = GetStringDepenceDwelling(race, build);
    char text[TEXTLEN + 1];
    char num[NUMLEN + 1];

    S_BOX *box = NULL;
    Uint8 countres = 0;
    Uint16 height;

    if(payment->ore)     ++countres;
    if(payment->wood)    ++countres;
    if(payment->sulfur)	 ++countres;
    if(payment->mercury) ++countres;
    if(payment->crystal) ++countres;
    if(payment->gems)    ++countres;
    if(payment->gold)    ++countres;
    
    height  = 100;
    height += GetHeightText(GetStringDescriptionsBuilding(race, build), FONT_BIG);
    height += 55;
    if(countres > 3) height += 55; // res
    if(countres > 6) height += 55; // res
    if(depens) height += 20 + GetHeightText(depens, FONT_BIG);

    if(NULL == (box = InitBox(height, &dialog, flag))) return NONE;

    switch(race){
	case KNIGHT:
	    icnsprite = "CSTLKNGT.ICN";
	    break;
	case BARBARIAN:
	    icnsprite = "CSTLBARB.ICN";
	    break;
	case SORCERESS:
	    icnsprite = "CSTLSORC.ICN";
	    break;
	case WARLOCK:
	    icnsprite = "CSTLWRLK.ICN";
	    break;
	case WIZARD:
	    icnsprite = "CSTLWZRD.ICN";
	    break;
	case NECROMANCER:
	    icnsprite = "CSTLNECR.ICN";
	    break;
	default:
	    if(GetIntValue(DEBUG)) fprintf(stderr, "BuyBuildingBox:, race: BOMG\n");
	    return NONE;
	    break;
    }

    switch(build){
	case BUILD_THIEVESGUILD:
	    index = 1;
	    break;
	case BUILD_TAVERN:
	    index = 2;
	    break;
	case BUILD_SHIPYARD:
	    index = 3;
	    break;
	case BUILD_WELL:
	    index = 4;
	    break;
	case BUILD_STATUE:
	    index = 7;
	    break;
	case BUILD_LEFTTURRET:
	    index = 8;
	    break;
	case BUILD_RIGHTTURRET:
	    index = 9;
	    break;
	case BUILD_MARKETPLACE:
	    index = 10;
	    break;
	case BUILD_WEL2:
	    index = 11;
	    break;
	case BUILD_MOAT:
	    index = 12;
	    break;
	case BUILD_SPEC:
	    index = 13;
	    break;
	case BUILD_CASTLE:
	    index = 5;
	    break;
	case BUILD_CAPTAIN:
	    index = 15;
	    break;
	case BUILD_MAGEGUILD1:
	case BUILD_MAGEGUILD2:
	case BUILD_MAGEGUILD3:
	case BUILD_MAGEGUILD4:
	case BUILD_MAGEGUILD5:
	    index = 0;
	    break;
	case DWELLING_MONSTER1:
	    index = 19;
	    break;
	case DWELLING_MONSTER2:
	    index = 20;
	    break;
	case DWELLING_MONSTER3:
	    index = 21;
	    break;
	case DWELLING_MONSTER4:
	    index = 22;
	    break;
	case DWELLING_MONSTER5:
	    index = 23;
	    break;
	case DWELLING_MONSTER6:
	    index = 24;
	    break;
	case DWELLING_UPGRADE2:
	    index = 25;
	    break;
	case DWELLING_UPGRADE3:
	    index = 26;
	    break;
	case DWELLING_UPGRADE4:
	    index = 27;
	    break;
	case DWELLING_UPGRADE5:
	    index = 28;
	    break;
	case DWELLING_UPGRADE6:
	    index = 29;
	    break;
	case DWELLING_UPGRADE7:
	    index = 30;
	    break;
	default:
	    if(GetIntValue(DEBUG)) fprintf(stderr, "BuyBuildingBox: build unknown\n");
	    return NONE;
	    break;
    }
    
    // sprite building
    FillSPRITE(&sprite, "CASLWIND.ICN", 0);
    image = GetICNSprite(&sprite);
    src.x = 5;
    src.y = 2;
    src.w = 137;
    src.h = 72;
    dst.y = box->rectArea.y + 5;
    dst.x = (video->w - src.w) / 2 + 5;
    dst.w = src.w;
    dst.h = src.h;
    SDL_BlitSurface(image, &src, video, &dst);

    FillSPRITE(&sprite, icnsprite, index);
    image = GetICNSprite(&sprite);
    dst.y = box->rectArea.y + 6;
    dst.x = (video->w - src.w) / 2 + 6;
    dst.w = image->w;
    dst.h = image->h;
    SDL_BlitSurface(image, NULL, video, &dst);

    snprintf(text, TEXTLEN, "%s", GetStringBuilding(race, build));
    dst.x = (video->w - src.w) / 2 + 6;
    dst.y = box->rectArea.y + 63;
    dst.w = image->w + 2;
    dst.h = FONT_HEIGHTSMALL;
    PrintAlignText(video, &dst, text, FONT_SMALL);

    // text info
    BuildingIsDwelling(build) ? snprintf(text, TEXTLEN, "The %s produces %s.", GetStringBuilding(race, build), GetStringMonsterFromDwelling(race, build)) : snprintf(text, TEXTLEN, "%s", GetStringDescriptionsBuilding(race, build));
    dst = box->rectArea;
    dst.y = box->rectArea.y + 90;
    PrintAlignText(video, &dst, text, FONT_BIG);
    dst = box->rectArea;
    dst.y = box->rectArea.y + 90;
    PrintAlignText(video, &dst, text, FONT_BIG);
            
    dst.y = box->rectArea.y + 110 + GetHeightText(text, FONT_BIG);

    if(depens){
        PrintAlignText(video, &dst, "Require:", FONT_BIG);
        dst.y += 20;
        PrintAlignText(video, &dst, depens, FONT_BIG);
        dst.y += 20;
    }

    dst.y += 40;
    // resource
    Uint8 count = 0;
    index = (countres > 2 ? box->rectArea.w / 3 : box->rectArea.w / countres);

    if(payment->wood){
	FillSPRITE(&sprite, "RESOURCE.ICN", 0);
	image = GetICNSprite(&sprite);
	src.x = dst.x + index / 2 + count * index - image->w / 2;
	src.y = dst.y - image->h;
	src.w = image->w;
	src.h = image->h;
	SDL_BlitSurface(image, NULL, video, &src);

	snprintf(num, NUMLEN, "%d", payment->wood);
	src.x = dst.x + index / 2 + count * index - GetLengthText(num, FONT_SMALL) / 2;
	src.y = dst.y + 2;
	src.w = GetLengthText(num, FONT_SMALL);
	src.h = FONT_HEIGHTSMALL;
	PrintText(video, &src, num, FONT_SMALL);
	++count;
    }

    if(payment->ore){
	FillSPRITE(&sprite, "RESOURCE.ICN", 2);
	image = GetICNSprite(&sprite);
	src.x = dst.x + index / 2 + count * index - image->w / 2;
	src.y = dst.y - image->h;
	src.w = image->w;
	src.h = image->h;
	SDL_BlitSurface(image, NULL, video, &src);

	snprintf(num, NUMLEN, "%d", payment->ore);
	src.x = dst.x + index / 2 + count * index - GetLengthText(num, FONT_SMALL) / 2;
	src.y = dst.y + 2;
	src.w = GetLengthText(num, FONT_SMALL);
	src.h = FONT_HEIGHTSMALL;
	PrintText(video, &src, num, FONT_SMALL);
	++count;
    }

    if(payment->mercury){
	FillSPRITE(&sprite, "RESOURCE.ICN", 1);
	image = GetICNSprite(&sprite);
	src.x = dst.x + index / 2 + count * index - image->w / 2;
	src.y = dst.y - image->h;
	src.w = image->w;
	src.h = image->h;
	SDL_BlitSurface(image, NULL, video, &src);

	snprintf(num, NUMLEN, "%d", payment->mercury);
	src.x = dst.x + index / 2 + count * index - GetLengthText(num, FONT_SMALL) / 2;
	src.y = dst.y + 2;
	src.w = GetLengthText(num, FONT_SMALL);
	src.h = FONT_HEIGHTSMALL;
	PrintText(video, &src, num, FONT_SMALL);
	++count;
    }

    if(2 < count){ count = 0; dst.y += 50; }
    if(payment->sulfur){
	FillSPRITE(&sprite, "RESOURCE.ICN", 3);
	image = GetICNSprite(&sprite);
	src.x = dst.x + index / 2 + count * index - image->w / 2;
	src.y = dst.y - image->h;
	src.w = image->w;
	src.h = image->h;
	SDL_BlitSurface(image, NULL, video, &src);

	snprintf(num, NUMLEN, "%d", payment->sulfur);
	src.x = dst.x + index / 2 + count * index - GetLengthText(num, FONT_SMALL) / 2;
	src.y = dst.y + 2;
	src.w = GetLengthText(num, FONT_SMALL);
	src.h = FONT_HEIGHTSMALL;
	PrintText(video, &src, num, FONT_SMALL);
	++count;
    }

    if(2 < count){ count = 0; dst.y += 50; }
    if(payment->crystal){
	FillSPRITE(&sprite, "RESOURCE.ICN", 4);
	image = GetICNSprite(&sprite);
	src.x = dst.x + index / 2 + count * index - image->w / 2;
	src.y = dst.y - image->h;
	src.w = image->w;
	src.h = image->h;
	SDL_BlitSurface(image, NULL, video, &src);

	snprintf(num, NUMLEN, "%d", payment->crystal);
	src.x = dst.x + index / 2 + count * index - GetLengthText(num, FONT_SMALL) / 2;
	src.y = dst.y + 2;
	src.w = GetLengthText(num, FONT_SMALL);
	src.h = FONT_HEIGHTSMALL;
	PrintText(video, &src, num, FONT_SMALL);
	++count;
    }

    if(2 < count){ count = 0; dst.y += 50; }
    if(payment->gems){
	FillSPRITE(&sprite, "RESOURCE.ICN", 5);
	image = GetICNSprite(&sprite);
	src.x = dst.x + index / 2 + count * index - image->w / 2;
	src.y = dst.y - image->h;
	src.w = image->w;
	src.h = image->h;
	SDL_BlitSurface(image, NULL, video, &src);

	snprintf(num, NUMLEN, "%d", payment->gems);
	src.x = dst.x + index / 2 + count * index - GetLengthText(num, FONT_SMALL) / 2;
	src.y = dst.y + 2;
	src.w = GetLengthText(num, FONT_SMALL);
	src.h = FONT_HEIGHTSMALL;
	PrintText(video, &src, num, FONT_SMALL);
	++count;
    }

    if(2 < count){ count = 0; dst.y += 50; }
    if(payment->gold){
	FillSPRITE(&sprite, "RESOURCE.ICN", 6);
	image = GetICNSprite(&sprite);
	if(! count) index = box->rectArea.w;
	src.x = dst.x + index / 2 + count * index - image->w / 2;
	src.y = dst.y - image->h;
	src.w = image->w;
	src.h = image->h;
	SDL_BlitSurface(image, NULL, video, &src);

	snprintf(num, NUMLEN, "%d", payment->gold);
	src.x = dst.x + index / 2 + count * index - GetLengthText(num, FONT_SMALL) / 2;
	src.y = dst.y + 2;
	src.w = GetLengthText(num, FONT_SMALL);
	src.h = FONT_HEIGHTSMALL;
	PrintText(video, &src, num, FONT_SMALL);
    }

    // Отрисовка диалога
    SDL_Flip(video);

    SetCursor(CURSOR_POINTER);

    CursorOn();

    // цикл событий
    if(flag)
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

        	case OK:
		case ENTER:
            	    exit = TRUE;
            	    result = OK;
            	    break;

        	default:
            	    break;

    	    }
    else
	while(! exit){
    	    while(SDL_PollEvent(&event))
        	if( SDL_BUTTON_RIGHT == event.button.button && SDL_RELEASED == event.button.state) exit = TRUE;
                        
    	    if(GetIntValue(CYCLELOOP)) SDL_Delay(CYCLEDELAY * 10);
	}

    // востанавливаем бакгроунд
    CursorOff();

    FreeBox(box);
    SetCursor(cursor);

    SetIntValue(ANIM2, TRUE);
    SetIntValue(ANIM3, TRUE);

    CursorOn();

    return result;
}

ACTION BuyBoat(void){

    CursorOff();

    SetIntValue(ANIM2, FALSE);
    SetIntValue(ANIM3, FALSE);

    Uint32 cursor = GetCursor();
    ACTION result = NONE;
    INTERFACEACTION *dialog = NULL;        
    BOOL exit = FALSE;
    S_BOX *box = NULL;
    SDL_Rect rect;
    SDL_Surface *image = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();;
    AGGSPRITE sprite;
    char num[NUMLEN + 1];
    
    if(NULL == (box = InitBox(120, &dialog, OK|CANCEL))) return NONE;

    // text header
    rect = box->rectArea;
    PrintAlignText(video, &rect, "Build a new ship:", FONT_BIG);

    // sprite
    FillSPRITE(&sprite, "BOATWIND.ICN", 0);
    image = GetICNSprite(&sprite);
    rect.y += 25;
    rect.x = (video->w - image->w) / 2 + 5;
    rect.w = image->w;
    rect.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rect);

    // text info
    rect.x = box->rectArea.x;
    rect.y += 10 + image->h;
    rect.w = box->rectArea.w;
    rect.h = box->rectArea.h;
    PrintAlignText(video, &rect, "Resource cost:", FONT_BIG);

    Uint16 cx = rect.y + 50;
    // sprite wood
    FillSPRITE(&sprite, "RESOURCE.ICN", 0);
    image = GetICNSprite(&sprite);
    rect.x = (video->w - image->w) / 2 - 60;
    rect.y = cx - image->h;
    rect.w = image->w;
    rect.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rect);
    // count
    snprintf(num, NUMLEN, "%d", BUILD_BOAT_WOOD);
    rect.x = (video->w - GetLengthText(num, FONT_SMALL)) / 2 - 60;
    rect.y = cx + 3;
    rect.w = GetLengthText(num, FONT_SMALL);
    rect.h = FONT_HEIGHTSMALL;
    PrintText(video, &rect, num, FONT_SMALL);

    // sprite gold
    FillSPRITE(&sprite, "RESOURCE.ICN", 6);
    image = GetICNSprite(&sprite);
    rect.y = cx - image->h;
    rect.x = (video->w - image->w) / 2 + 60;
    rect.w = image->w;
    rect.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rect);
    // count
    snprintf(num, NUMLEN, "%d", BUILD_BOAT_GOLD);
    rect.x = (video->w - GetLengthText(num, FONT_SMALL)) / 2 + 60;
    rect.y = cx + 3;
    rect.w = GetLengthText(num, FONT_SMALL);
    rect.h = FONT_HEIGHTSMALL;
    PrintText(video, &rect, num, FONT_SMALL);

    // Отрисовка диалога
    SDL_Flip(video);

    SetCursor(CURSOR_POINTER);

    CursorOn();

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

    	    case OK:
	    case ENTER:
                exit = TRUE;
                result = OK;
        	break;

        	default:
            	break;
    	    }

    // востанавливаем бакгроунд
    CursorOff();

    FreeBox(box);
    
    SetCursor(cursor);

    SetIntValue(ANIM2, TRUE);
    SetIntValue(ANIM3, TRUE);

    CursorOn();

    return result;
}
