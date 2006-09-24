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
#include "actionevent.h"
#include "cursor.h"
#include "tools.h"
#include "config.h"
#include "box.h"

#define BOX_WIDTH	306
#define BOX_TOP		99
#define BOX_MIDDLE	45
#define BOX_BOTTOM	81
#define BOXE_TOP	88
#define BOXE_MIDDLE	45
#define BOXE_BOTTOM	81
#define AREA_TOP	30
#define AREA_MIDDLE	45
#define AREA_BOTTOM	30
#define AREA_WIDTH	234
#define BUTTON_HEIGHT	50

// draw box && button YES|NO, OK|CANCEL, YES, OK, EXIT
S_BOX * InitBox(Uint16 height, INTERFACEACTION **ptr, Uint32 flag){

    S_BOX *box = NULL;

    if(NULL == (box = (S_BOX *) malloc(sizeof(S_BOX)))){
	fprintf(stderr, "InitBox: error malloc: %d\n", sizeof(S_BOX));
        return NULL;
    }

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Surface *image = NULL;
    AGGSPRITE sprite;
    INTERFACEACTION action;
    SDL_Rect dst;
    const char *buybuild = NULL;
    const char *button = NULL;
    Uint8 i, count;

    box->action = NULL;

    if(ptr && flag) height += BUTTON_HEIGHT;

    count = (height < AREA_TOP + AREA_BOTTOM ? 0 : 1 + (height - AREA_TOP - AREA_BOTTOM) / AREA_MIDDLE);

    if(GetIntValue(EVILINTERFACE)) buybuild = "BUYBUILE.ICN"; else buybuild = "BUYBUILD.ICN";

    box->rectBack.w = BOX_WIDTH;
    box->rectBack.h = (GetIntValue(EVILINTERFACE) ? BOXE_TOP + BOXE_BOTTOM + count * BOXE_MIDDLE : BOX_TOP + BOX_BOTTOM + count * BOX_MIDDLE);
    box->rectBack.x = (video->w - box->rectBack.w) / 2;
    box->rectBack.y = (video->h - box->rectBack.h) / 2;

    box->rectArea.x = box->rectBack.x + 41;
    box->rectArea.y = (GetIntValue(EVILINTERFACE) ? box->rectBack.y + BOXE_TOP - 30 : box->rectBack.y + BOX_TOP - 30);
    box->rectArea.w = AREA_WIDTH;
    box->rectArea.h = AREA_TOP + AREA_BOTTOM + count * AREA_MIDDLE;
    if(ptr && flag) box->rectArea.h -= BUTTON_HEIGHT;
    
    // сохраняем бакгроунд
    if(NULL == (box->background = SDL_CreateRGBSurface(SDL_SWSURFACE, box->rectBack.w, box->rectBack.h, 16, 0, 0, 0, 0))){
	fprintf(stderr, "InitBox: CreateRGBSurface failed: %s\n", SDL_GetError());
	return NULL;
    }
    SDL_BlitSurface(video, &box->rectBack, box->background, NULL);

    // получаем левый верхний спрайт
    FillSPRITE(&sprite, buybuild, 4);
    image = GetICNSprite(&sprite);
    dst.x = (GetIntValue(EVILINTERFACE) ? box->rectBack.x : box->rectBack.x + 1);
    dst.y = box->rectBack.y;
    dst.w = image->w;
    dst.h = image->h;
    SDL_BlitSurface(image, NULL, video, &dst);

    // получаем левый средний спрайт
    FillSPRITE(&sprite, buybuild, 5);
    image = GetICNSprite(&sprite);
    dst.x = box->rectBack.x;
    dst.w = image->w;
    dst.y += dst.h;
    dst.h = image->h;
    for(i = 0; i < count; ++i){
	SDL_BlitSurface(image, NULL, video, &dst);
	dst.y += dst.h;
    }

    // получаем левый нижний спрайт
    FillSPRITE(&sprite, buybuild, 6);
    image = GetICNSprite(&sprite);
    dst.w = image->w;
    dst.h = image->h;
    SDL_BlitSurface(image, NULL, video, &dst);

    // получаем правый верхний спрайт
    FillSPRITE(&sprite, buybuild, 0);
    image = GetICNSprite(&sprite);
    dst.x = box->rectBack.x + dst.w;
    dst.y = box->rectBack.y;
    dst.w = image->w;
    dst.h = image->h;
    SDL_BlitSurface(image, NULL, video, &dst);

    // получаем правый средний спрайт
    FillSPRITE(&sprite, buybuild, 1);
    image = GetICNSprite(&sprite);
    dst.w = image->w;
    dst.y += dst.h;
    dst.h = image->h;
    for(i = 0; i < count; ++i){
	SDL_BlitSurface(image, NULL, video, &dst);
	dst.y += dst.h;
    }

    // получаем правый нижний спрайт
    FillSPRITE(&sprite, buybuild, 2);
    image = GetICNSprite(&sprite);
    dst.w = image->w;
    dst.h = image->h;
    SDL_BlitSurface(image, NULL, video, &dst);

    if(!ptr) return NULL;

    button = (GetIntValue(EVILINTERFACE) ? "SYSTEME.ICN" : "SYSTEM.ICN");

    switch(flag){
    
	case YES|NO:
	    // button YES
	    FillSPRITE(&sprite, button, 5);
	    image = GetICNSprite(&sprite);
	    dst.x = box->rectArea.x;
	    dst.y = box->rectArea.y + box->rectArea.h + BUTTON_HEIGHT - image->h;
	    dst.w = image->w;
	    dst.h = image->h;
	    ZeroINTERFACEACTION(&action);
	    FillSPRITE(&action.objectUp, button, 5);
	    FillSPRITE(&action.objectPush, button, 6);
	    action.rect = dst;
	    action.mouseEvent = MOUSE_LCLICK;
	    action.pf = ActionPressYES;
	    AddActionEvent(ptr, &action);
	    SDL_BlitSurface(image, NULL, video, &dst);

	    // button NO
	    FillSPRITE(&sprite, button, 7);
	    image = GetICNSprite(&sprite);
	    dst.x = box->rectArea.x + box->rectArea.w - image->w;
	    dst.y = box->rectArea.y + box->rectArea.h + BUTTON_HEIGHT - image->h;
	    dst.w = image->w;
	    dst.h = image->h;
	    ZeroINTERFACEACTION(&action);
	    FillSPRITE(&action.objectUp, button, 7);
	    FillSPRITE(&action.objectPush, button, 8);
	    action.rect = dst;
	    action.mouseEvent = MOUSE_LCLICK;
	    action.pf = ActionPressNO;
	    AddActionEvent(ptr, &action);
	    SDL_BlitSurface(image, NULL, video, &dst);
	    break;

	case OK|CANCEL:
	    // button OKAY
	    FillSPRITE(&sprite, button, 1);
	    image = GetICNSprite(&sprite);
	    dst.x = box->rectArea.x;
	    dst.y = box->rectArea.y + box->rectArea.h + BUTTON_HEIGHT - image->h;
	    dst.w = image->w;
	    dst.h = image->h;
	    ZeroINTERFACEACTION(&action);
	    FillSPRITE(&action.objectUp, button, 1);
	    FillSPRITE(&action.objectPush, button, 2);
	    action.rect = dst;
	    action.mouseEvent = MOUSE_LCLICK;
	    action.pf = ActionPressOK;
	    AddActionEvent(ptr, &action);
	    SDL_BlitSurface(image, NULL, video, &dst);

	    // button CANCEL
	    FillSPRITE(&sprite, button, 3);
	    image = GetICNSprite(&sprite);
	    dst.x = box->rectArea.x + box->rectArea.w - image->w;
	    dst.y = box->rectArea.y + box->rectArea.h + BUTTON_HEIGHT - image->h;
	    dst.w = image->w;
	    dst.h = image->h;
	    ZeroINTERFACEACTION(&action);
	    FillSPRITE(&action.objectUp, button, 3);
	    FillSPRITE(&action.objectPush, button, 4);
	    action.rect = dst;
	    action.mouseEvent = MOUSE_LCLICK;
	    action.pf = ActionPressCANCEL;
	    AddActionEvent(ptr, &action);
	    SDL_BlitSurface(image, NULL, video, &dst);
	    break;

	case OK:
	    // button OKAY
	    FillSPRITE(&sprite, button, 1);
	    image = GetICNSprite(&sprite);
	    dst.x = box->rectArea.x + (box->rectArea.w - image->w) / 2;
	    dst.y = box->rectArea.y + box->rectArea.h + BUTTON_HEIGHT - image->h;
	    dst.w = image->w;
	    dst.h = image->h;
	    ZeroINTERFACEACTION(&action);
	    FillSPRITE(&action.objectUp, button, 1);
	    FillSPRITE(&action.objectPush, button, 2);
	    action.rect = dst;
	    action.mouseEvent = MOUSE_LCLICK;
	    action.pf = ActionPressOK;
	    AddActionEvent(ptr, &action);
	    SDL_BlitSurface(image, NULL, video, &dst);
	    break;

	case YES:
	    // button YES
	    FillSPRITE(&sprite, button, 5);
	    image = GetICNSprite(&sprite);
	    dst.x = box->rectArea.x + (box->rectArea.w - image->w) / 2;
	    dst.y = box->rectArea.y + box->rectArea.h + BUTTON_HEIGHT - image->h;
	    dst.w = image->w;
	    dst.h = image->h;
	    ZeroINTERFACEACTION(&action);
	    FillSPRITE(&action.objectUp, button, 5);
	    FillSPRITE(&action.objectPush, button, 6);
	    action.rect = dst;
	    action.mouseEvent = MOUSE_LCLICK;
	    action.pf = ActionPressYES;
	    AddActionEvent(ptr, &action);
	    SDL_BlitSurface(image, NULL, video, &dst);
	    break;

	case EXIT:
	    // button EXIT
	    button = (GetIntValue(EVILINTERFACE) ? "TRADPOSE.ICN" : "TRADPOST.ICN");
	    FillSPRITE(&sprite, button, 17);
	    image = GetICNSprite(&sprite);
	    dst.x = box->rectArea.x + (box->rectArea.w - image->w) / 2;
	    dst.y = box->rectArea.y + box->rectArea.h + BUTTON_HEIGHT - image->h;
	    dst.w = image->w;
	    dst.h = image->h;
	    ZeroINTERFACEACTION(&action);
	    FillSPRITE(&action.objectUp, button, 17);
	    FillSPRITE(&action.objectPush, button, 18);
	    action.rect = dst;
	    action.mouseEvent = MOUSE_LCLICK;
	    action.pf = ActionPressEXIT;
	    AddActionEvent(ptr, &action);
	    SDL_BlitSurface(image, NULL, video, &dst);
	    break;

	default:
	    break;
    }

    box->action = *ptr;

    return box;
}

void FreeBox(S_BOX *box){

    if(! box) return;

    SDL_Surface *video = SDL_GetVideoSurface();

    if(box->background){
	SDL_BlitSurface(box->background, NULL, video, &box->rectBack);
	SDL_FreeSurface(box->background);
    }

    if(box->action) FreeActionEvent(box->action);

    free(box);
}

Uint16 GetHeightText(const char *string, ENUMFONT font){

    if(! string) return 0;
    
    if(FONT_BIG)
	return FONT_HEIGHTBIG * (1 + GetLengthText(string, font) / (AREA_WIDTH - 54));

    return FONT_HEIGHTSMALL * (1 + GetLengthText(string, font) / (AREA_WIDTH - 54));
}
