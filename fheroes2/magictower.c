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
#include "magictower.h"

#define STRLEN	64

ACTION	ActionLClickSpell(void);
ACTION	ActionRClickSpell(void);

ACTION  ShowMageGuildInfo(void){

    CursorOff();
    SetIntValue(ANIM2, FALSE);
    SetIntValue(ANIM3, FALSE);

    SDL_Surface *back, *image, *video;
    SDL_Rect rectBack, rectCur;
    Uint16 cx, cy;
    Uint8 level, i;
    AGGSPRITE sprite;
    const char *message = NULL;
    char str[STRLEN + 1];
    BOOL exit = FALSE;

    INTERFACEACTION action;
    INTERFACEACTION *dialog = NULL;        
    ACTION result = NONE;

    const S_CASTLE *castle = GetCurrentCastle();
    if(! castle || !GetMageGuildLevel(castle)) return NONE;

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
	fprintf(stderr, "ShowMageGuildInfo: CreateRGBSurface failed: %s\n", SDL_GetError());
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
    message = "The above spells are available here.";
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

    switch(castle->race){
    
	case KNIGHT:
	    message = "MAGEGLDK.ICN";
	    break;
	
	default:
	    if(GetIntValue(DEBUG)) fprintf(stderr, "ShowMageGuildInfo: error, race: BOMG, color %s, ax: %d, ay: %d\n", GetStringColor(castle->color), castle->ax, castle->ay);
	case BARBARIAN:
	    message = "MAGEGLDB.ICN";
	    break;
	
	case SORCERESS:
	    message = "MAGEGLDS.ICN";
	    break;
	
	case WARLOCK:
	    message = "MAGEGLDW.ICN";
	    break;
	
	case WIZARD:
	    message = "MAGEGLDZ.ICN";
	    break;
	
	case NECROMANCER:
	    message = "MAGEGLDN.ICN";
	    break;
    }
    FillSPRITE(&sprite, message, GetMageGuildLevel(castle) - 1);
    image = GetICNSprite(&sprite);
    rectCur.x = cx + 80 - image->w / 2;
    rectCur.y = cy + 270 - image->h;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // LEVEL 5
    for(i = 0; i <  CASTLESPELL_LEVEL5; ++i){

	if(WIZARD != castle->race && i == CASTLESPELL_LEVEL5 - 1 ) break;

	if(MAGIC_LEVEL4 < GetMageGuildLevel(castle) && SPELLNONE != castle->mageGuild.level5[i]) level = 0; else level = 1;

	// свиток
	FillSPRITE(&sprite, "TOWNWIND.ICN", level);
	image = GetICNSprite(&sprite);
	rectCur.x = cx + 250 - image->w / 2 + i * 110;
	rectCur.y = cy;
	rectCur.w = image->w;
	rectCur.h = image->h;
	SDL_BlitSurface(image, NULL, video, &rectCur);

	if(! level){
	    // события
	    rectCur.x += 10;
	    rectCur.y += 5;
	    rectCur.w -= 15;
	    rectCur.h -= 10;
    	    ZeroINTERFACEACTION(&action);
    	    action.rect = rectCur;
    	    action.mouseEvent = MOUSE_LCLICK;
    	    action.pf = ActionLClickSpell;
    	    AddActionEvent(&dialog, &action);
    	    ZeroINTERFACEACTION(&action);
    	    action.rect = rectCur;
    	    action.mouseEvent = MOUSE_RCLICK;
    	    action.pf = ActionRClickSpell;
    	    AddActionEvent(&dialog, &action);
	    // картика заклинания
	    FillSPRITE(&sprite, "SPELLS.ICN", castle->mageGuild.level5[i]);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 250 - image->w / 2 + i * 110 + 5;
	    rectCur.y = cy + 28 - image->h / 2 + 5;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    // text
	    message = GetStringSpell(castle->mageGuild.level5[i]);
	    if(80 < GetLengthText(message, FONT_SMALL)){
		snprintf(str, STRLEN, "%s [%d]", message, GetCostSpellMagic(castle->mageGuild.level5[i]));
		rectCur.x = cx + 250 + i * 110 - 40;
		rectCur.y = cy + 55 + 3;
		rectCur.w = 80;
		rectCur.h = 3 * FONT_HEIGHTSMALL;
		PrintAlignText(video, &rectCur, str, FONT_SMALL);
	    }else{
		rectCur.x = cx + 250 + i * 110 - GetLengthText(message, FONT_SMALL) / 2 + 2;
		rectCur.y = cy + 55 + 5;
		rectCur.w = GetLengthText(message, FONT_SMALL);
		rectCur.h = FONT_HEIGHTSMALL;
		PrintText(video, &rectCur, message, FONT_SMALL);
		snprintf(str, STRLEN, "[%d]", GetCostSpellMagic(castle->mageGuild.level5[i]));
		rectCur.x = cx + 250 + i * 110 - GetLengthText(str, FONT_SMALL) / 2 + 2;
		rectCur.y = cy + 55 + FONT_HEIGHTSMALL + 5;
		rectCur.w = GetLengthText(str, FONT_SMALL);
		rectCur.h = FONT_HEIGHTSMALL;
		PrintText(video, &rectCur, str, FONT_SMALL);
	    }
	}
    }

    // LEVEL 4
    for(i = 0; i <  CASTLESPELL_LEVEL4; ++i){

	if(WIZARD != castle->race && i == CASTLESPELL_LEVEL4 - 1 ) break;

	if(MAGIC_LEVEL3 < GetMageGuildLevel(castle) && SPELLNONE != castle->mageGuild.level4[i]) level = 0; else level = 1;

	// свиток
	FillSPRITE(&sprite, "TOWNWIND.ICN", level);
	image = GetICNSprite(&sprite);
	rectCur.x = cx + 250 - image->w / 2 + i * 110;
	rectCur.y = cy + 90;
	rectCur.w = image->w;
	rectCur.h = image->h;
	SDL_BlitSurface(image, NULL, video, &rectCur);

	if(! level){
	    // события
	    rectCur.x += 10;
	    rectCur.y += 5;
	    rectCur.w -= 15;
	    rectCur.h -= 10;
    	    ZeroINTERFACEACTION(&action);
    	    action.rect = rectCur;
    	    action.mouseEvent = MOUSE_LCLICK;
    	    action.pf = ActionLClickSpell;
    	    AddActionEvent(&dialog, &action);
    	    ZeroINTERFACEACTION(&action);
    	    action.rect = rectCur;
    	    action.mouseEvent = MOUSE_RCLICK;
    	    action.pf = ActionRClickSpell;
    	    AddActionEvent(&dialog, &action);
	    // картика заклинания
            if(MASSSHIELD == castle->mageGuild.level4[i]) FillSPRITE(&sprite, "SPELLS.ICN", SHIELD);
            else if(MASSSLOW == castle->mageGuild.level4[i]) FillSPRITE(&sprite, "SPELLS.ICN", SLOW);
	    else FillSPRITE(&sprite, "SPELLS.ICN", castle->mageGuild.level4[i]);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 250 - image->w / 2 + i * 110 + 5;
	    rectCur.y = cy + 90 + 28 - image->h / 2 + 5;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    // text
	    message = GetStringSpell(castle->mageGuild.level4[i]);
	    if(80 < GetLengthText(message, FONT_SMALL)){
		snprintf(str, STRLEN, "%s [%d]", message, GetCostSpellMagic(castle->mageGuild.level4[i]));
		rectCur.x = cx + 250 + i * 110 - 40;
		rectCur.y = cy + 90 + 55 + 3;
		rectCur.w = 80;
		rectCur.h = 3 * FONT_HEIGHTSMALL;
		PrintAlignText(video, &rectCur, str, FONT_SMALL);
	    }else{
		rectCur.x = cx + 250 + i * 110 - GetLengthText(message, FONT_SMALL) / 2 + 2;
		rectCur.y = cy + 90 + 55 + 5;
		rectCur.w = GetLengthText(message, FONT_SMALL);
		rectCur.h = FONT_HEIGHTSMALL;
		PrintText(video, &rectCur, message, FONT_SMALL);
		snprintf(str, STRLEN, "[%d]", GetCostSpellMagic(castle->mageGuild.level4[i]));
		rectCur.x = cx + 250 + i * 110 - GetLengthText(str, FONT_SMALL) / 2 + 2;
		rectCur.y = cy + 90 + 55 + FONT_HEIGHTSMALL + 5;
		rectCur.w = GetLengthText(str, FONT_SMALL);
		rectCur.h = FONT_HEIGHTSMALL;
		PrintText(video, &rectCur, str, FONT_SMALL);
	    }
	}
    }

    // LEVEL 3
    for(i = 0; i <  CASTLESPELL_LEVEL3; ++i){

	if(WIZARD != castle->race && i == CASTLESPELL_LEVEL3 - 1 ) break;

	if(MAGIC_LEVEL2 < GetMageGuildLevel(castle) && SPELLNONE != castle->mageGuild.level3[i]) level = 0; else level = 1;

	// свиток
	FillSPRITE(&sprite, "TOWNWIND.ICN", level);
	image = GetICNSprite(&sprite);
	rectCur.x = cx + 250 - image->w / 2 + i * 110;
	rectCur.y = cy + 180;
	rectCur.w = image->w;
	rectCur.h = image->h;
	SDL_BlitSurface(image, NULL, video, &rectCur);

	if(! level){
	    rectCur.x += 10;
	    rectCur.y += 5;
	    rectCur.w -= 15;
	    rectCur.h -= 10;
	    // события
    	    ZeroINTERFACEACTION(&action);
    	    action.rect = rectCur;
    	    action.mouseEvent = MOUSE_LCLICK;
    	    action.pf = ActionLClickSpell;
    	    AddActionEvent(&dialog, &action);
    	    ZeroINTERFACEACTION(&action);
    	    action.rect = rectCur;
    	    action.mouseEvent = MOUSE_RCLICK;
    	    action.pf = ActionRClickSpell;
    	    AddActionEvent(&dialog, &action);
	    // картика заклинания
            if(MASSBLESS == castle->mageGuild.level3[i]) FillSPRITE(&sprite, "SPELLS.ICN", BLESS);
            else if(MASSCURSE == castle->mageGuild.level3[i]) FillSPRITE(&sprite, "SPELLS.ICN", CURSE);
            else if(MASSDISPEL == castle->mageGuild.level3[i]) FillSPRITE(&sprite, "SPELLS.ICN", DISPELMAGIC);
            else if(MASSHASTE == castle->mageGuild.level3[i]) FillSPRITE(&sprite, "SPELLS.ICN", HASTE);
	    else FillSPRITE(&sprite, "SPELLS.ICN", castle->mageGuild.level3[i]);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 250 - image->w / 2 + i * 110 + 5;
	    rectCur.y = cy + 180 + 28 - image->h / 2 + 5;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    // text
	    message = GetStringSpell(castle->mageGuild.level3[i]);
	    if(80 < GetLengthText(message, FONT_SMALL)){
		snprintf(str, STRLEN, "%s [%d]", message, GetCostSpellMagic(castle->mageGuild.level4[i]));
		rectCur.x = cx + 250 + i * 110 - 40;
		rectCur.y = cy + 180 + 55 + 3;
		rectCur.w = 80;
		rectCur.h = 3 * FONT_HEIGHTSMALL;
		PrintAlignText(video, &rectCur, str, FONT_SMALL);
	    }else{
		rectCur.x = cx + 250 + i * 110 - GetLengthText(message, FONT_SMALL) / 2 + 2;
		rectCur.y = cy + 180 + 55 + 5;
		rectCur.w = GetLengthText(message, FONT_SMALL);
		rectCur.h = FONT_HEIGHTSMALL;
		PrintText(video, &rectCur, message, FONT_SMALL);
		snprintf(str, STRLEN, "[%d]", GetCostSpellMagic(castle->mageGuild.level3[i]));
		rectCur.x = cx + 250 + i * 110 - GetLengthText(str, FONT_SMALL) / 2 + 2;
		rectCur.y = cy + 180 + 55 + FONT_HEIGHTSMALL + 5;
		rectCur.w = GetLengthText(str, FONT_SMALL);
		rectCur.h = FONT_HEIGHTSMALL;
		PrintText(video, &rectCur, str, FONT_SMALL);
	    }
	}
    }

    // LEVEL 2
    for(i = 0; i <  CASTLESPELL_LEVEL2; ++i){

	if(WIZARD != castle->race && i == CASTLESPELL_LEVEL2 - 1 ) break;

	if(MAGIC_LEVEL1 < GetMageGuildLevel(castle) && SPELLNONE != castle->mageGuild.level2[i]) level = 0; else level = 1;

	// свиток
	FillSPRITE(&sprite, "TOWNWIND.ICN", level);
	image = GetICNSprite(&sprite);
	rectCur.x = cx + 250 - image->w / 2 + i * 110;
	rectCur.y = cy + 270;
	rectCur.w = image->w;
	rectCur.h = image->h;
	SDL_BlitSurface(image, NULL, video, &rectCur);

	if(! level){
	    rectCur.x += 10;
	    rectCur.y += 5;
	    rectCur.w -= 15;
	    rectCur.h -= 10;
	    // события
    	    ZeroINTERFACEACTION(&action);
    	    action.rect = rectCur;
    	    action.mouseEvent = MOUSE_LCLICK;
    	    action.pf = ActionLClickSpell;
    	    AddActionEvent(&dialog, &action);
    	    ZeroINTERFACEACTION(&action);
    	    action.rect = rectCur;
    	    action.mouseEvent = MOUSE_RCLICK;
    	    action.pf = ActionRClickSpell;
    	    AddActionEvent(&dialog, &action);
	    // картика заклинания
	    FillSPRITE(&sprite, "SPELLS.ICN", castle->mageGuild.level2[i]);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 250 - image->w / 2 + i * 110 + 5;
	    rectCur.y = cy + 270 + 28 - image->h / 2 + 5;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    // text
	    message = GetStringSpell(castle->mageGuild.level2[i]);
	    if(80 < GetLengthText(message, FONT_SMALL)){
		snprintf(str, STRLEN, "%s [%d]", message, GetCostSpellMagic(castle->mageGuild.level2[i]));
		rectCur.x = cx + 250 + i * 110 - 40;
		rectCur.y = cy + 270 + 55 + 3;
		rectCur.w = 80;
		rectCur.h = 3 * FONT_HEIGHTSMALL;
		PrintAlignText(video, &rectCur, str, FONT_SMALL);
	    }else{
		rectCur.x = cx + 250 + i * 110 - GetLengthText(message, FONT_SMALL) / 2 + 2;
		rectCur.y = cy + 270 + 55 + 5;
		rectCur.w = GetLengthText(message, FONT_SMALL);
		rectCur.h = FONT_HEIGHTSMALL;
		PrintText(video, &rectCur, message, FONT_SMALL);
		snprintf(str, STRLEN, "[%d]", GetCostSpellMagic(castle->mageGuild.level2[i]));
		rectCur.x = cx + 250 + i * 110 - GetLengthText(str, FONT_SMALL) / 2 + 2;
		rectCur.y = cy + 270 + 55 + FONT_HEIGHTSMALL + 5;
		rectCur.w = GetLengthText(str, FONT_SMALL);
		rectCur.h = FONT_HEIGHTSMALL;
		PrintText(video, &rectCur, str, FONT_SMALL);
	    }
	}
    }

    // LEVEL 1
    for(i = 0; i <  CASTLESPELL_LEVEL1; ++i){

	if(WIZARD != castle->race && i == CASTLESPELL_LEVEL1 - 1 ) break;
	if(GetMageGuildLevel(castle) && SPELLNONE != castle->mageGuild.level1[i]) level = 0; else level = 1;

	// свиток
	FillSPRITE(&sprite, "TOWNWIND.ICN", level);
	image = GetICNSprite(&sprite);
	rectCur.x = cx + 250 - image->w / 2 + i * 110;
	rectCur.y = cy + 360;
	rectCur.w = image->w;
	rectCur.h = image->h;
	SDL_BlitSurface(image, NULL, video, &rectCur);

	if(! level){
	    rectCur.x += 10;
	    rectCur.y += 5;
	    rectCur.w -= 15;
	    rectCur.h -= 10;
	    // события
    	    ZeroINTERFACEACTION(&action);
    	    action.rect = rectCur;
    	    action.mouseEvent = MOUSE_LCLICK;
    	    action.pf = ActionLClickSpell;
    	    AddActionEvent(&dialog, &action);
    	    ZeroINTERFACEACTION(&action);
    	    action.rect = rectCur;
    	    action.mouseEvent = MOUSE_RCLICK;
    	    action.pf = ActionRClickSpell;
    	    AddActionEvent(&dialog, &action);
	    // картика заклинания
	    FillSPRITE(&sprite, "SPELLS.ICN", castle->mageGuild.level1[i]);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 250 - image->w / 2 + i * 110 + 5;
	    rectCur.y = cy + 360 + 28 - image->h / 2 + 5;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    // text
	    message = GetStringSpell(castle->mageGuild.level1[i]);
	    if(80 < GetLengthText(message, FONT_SMALL)){
		snprintf(str, STRLEN, "%s [%d]", message, GetCostSpellMagic(castle->mageGuild.level1[i]));
		rectCur.x = cx + 250 + i * 110 - 40;
		rectCur.y = cy + 360 + 55 + 3;
		rectCur.w = 80;
		rectCur.h = 3 * FONT_HEIGHTSMALL;
		PrintAlignText(video, &rectCur, str, FONT_SMALL);
	    }else{
		rectCur.x = cx + 250 + i * 110 - GetLengthText(message, FONT_SMALL) / 2 + 2;
		rectCur.y = cy + 360 + 55 + 5;
		rectCur.w = GetLengthText(message, FONT_SMALL);
		rectCur.h = FONT_HEIGHTSMALL;
		PrintText(video, &rectCur, message, FONT_SMALL);
		snprintf(str, STRLEN, "[%d]", GetCostSpellMagic(castle->mageGuild.level1[i]));
		rectCur.x = cx + 250 + i * 110 - GetLengthText(str, FONT_SMALL) / 2 + 2;
		rectCur.y = cy + 360 + 55 + FONT_HEIGHTSMALL + 5;
		rectCur.w = GetLengthText(str, FONT_SMALL);
		rectCur.h = FONT_HEIGHTSMALL;
		PrintText(video, &rectCur, str, FONT_SMALL);
	    }
	}
    }

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

ACTION ActionLClickSpell(void){

    Sint32 mx, my;
    const S_CASTLE *castle = GetCurrentCastle();
    
    SDL_GetMouseState(&mx, &my);

    Uint8 level = 5 - (Uint8)(my / 90);
    Uint8 index = (mx - 195) / 110;

    if(1 == level)
	return ShowSpellInfo(castle->mageGuild.level1[index], OK);
    else if(2 == level)
	return ShowSpellInfo(castle->mageGuild.level2[index], OK);
    else if(3 == level)
	return ShowSpellInfo(castle->mageGuild.level3[index], OK);
    else if(4 == level)
	return ShowSpellInfo(castle->mageGuild.level4[index], OK);
    else if(5 == level)
	return ShowSpellInfo(castle->mageGuild.level5[index], OK);

    return NONE;
}

ACTION ActionRClickSpell(void){

    Sint32 mx, my;
    const S_CASTLE *castle = GetCurrentCastle();
    
    SDL_GetMouseState(&mx, &my);

    Uint8 level = 5 - (Uint8)( my / 90);
    Uint8 index = (mx - 195) / 110;

    if(1 == level)
	return ShowSpellInfo(castle->mageGuild.level1[index], NONE);
    else if(2 == level)
	return ShowSpellInfo(castle->mageGuild.level2[index], NONE);
    else if(3 == level)
	return ShowSpellInfo(castle->mageGuild.level3[index], NONE);
    else if(4 == level)
	return ShowSpellInfo(castle->mageGuild.level4[index], NONE);
    else if(5 == level)
	return ShowSpellInfo(castle->mageGuild.level5[index], NONE);

    return NONE;
}
