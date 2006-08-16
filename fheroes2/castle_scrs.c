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
#include "castle.h"    
#include "castleaction.h"    
#include "castle_scrs.h"

void DrawSCRSCastle(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;

    INTERFACEACTION action;
    const S_CASTLE *castle = GetCurrentCastle();
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    if(castle->castle){
	// анимация
	if(animHead){
	    cur.x = cx;
	    cur.y = cy;
	    cur.w = 0;
	    cur.h = 0;
	    FillSPRITE(&sprite, "TWNSCSTL.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(animHead, &cur, header, 5);
	}
	// спрайт замка
	FillSPRITE(&sprite, "TWNSCSTL.ICN", 0);
    }else FillSPRITE(&sprite, "TWNSTENT.ICN", 0);

    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);

    if(actionHead){
	cur.w -= 40;
	cur.h -=10;
	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_PRESENT;
	action.pf = ActionOverCastle;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionClickCastle;
	AddActionEvent(actionHead, &action);
    }
}

void DrawSCRSCapitan(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    INTERFACEACTION action;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // анимация
    if(animHead){
	cur.x = cx;
	cur.y = cy;
	cur.w = 0;
	cur.h = 0;
	FillSPRITE(&sprite, "TWNSCAPT.ICN", 1);
	header = GetICNHeader(&sprite);
	AddAnimationEvent(animHead, &cur, header, 5);
    }

    // спрайт
    FillSPRITE(&sprite, "TWNSCAPT.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);

    if(actionHead){
	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_PRESENT;
	action.pf = ActionOverCaptain;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionClickCaptain;
	AddActionEvent(actionHead, &action);
    }
}

void DrawSCRSStatue(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    INTERFACEACTION action;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // спрайт
    FillSPRITE(&sprite, "TWNSSTAT.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);

    if(actionHead){
	cur.x += 6;
	cur.w = 54;
	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_PRESENT;
	action.pf = ActionOverStatue;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionClickStatue;
	AddActionEvent(actionHead, &action);
    }
}

void DrawSCRSWell(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    INTERFACEACTION action;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // спрайт
    FillSPRITE(&sprite, "TWNSWELL.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);

    if(actionHead){
	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_PRESENT;
	action.pf = ActionOverWell;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionClickWell;
	AddActionEvent(actionHead, &action);
    }
}

void DrawSCRSThievesGuild(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    INTERFACEACTION action;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // анимация
    if(animHead){
	cur.x = cx;
	cur.y = cy;
	cur.w = 0;
	cur.h = 0;
	FillSPRITE(&sprite, "TWNSTHIE.ICN", 1);
	header = GetICNHeader(&sprite);
	AddAnimationEvent(animHead, &cur, header, 5);
    }

    // спрайт
    FillSPRITE(&sprite, "TWNSTHIE.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);

    if(actionHead){
	cur.w = 85;
	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_PRESENT;
	action.pf = ActionOverThievesGuild;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionClickThievesGuild;
	AddActionEvent(actionHead, &action);
    }
}

void DrawSCRSTavern(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    INTERFACEACTION action;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // анимация
    if(animHead){
	cur.x = cx;
	cur.y = cy;
	cur.w = 0;
	cur.h = 0;
	FillSPRITE(&sprite, "TWNSTVRN.ICN", 1);
	header = GetICNHeader(&sprite);
	AddAnimationEvent(animHead, &cur, header, 5);
    }

    // спрайт
    FillSPRITE(&sprite, "TWNSTVRN.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);

    if(actionHead){
	cur.x += 30;
	cur.w = 120;
	cur.y += 30;
	cur.h = 130;
	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_PRESENT;
	action.pf = ActionOverTavern;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionClickTavern;
	AddActionEvent(actionHead, &action);
    }
}

void DrawSCRSShipyard(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    INTERFACEACTION action;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // анимация
    if(animHead){
	cur.x = cx;
	cur.y = cy;
	cur.w = 0;
	cur.h = 0;
	FillSPRITE(&sprite, "TWNSDOCK.ICN", 1);
	header = GetICNHeader(&sprite);
	AddAnimationEvent(animHead, &cur, header, 5);
    }

    // спрайт
    FillSPRITE(&sprite, "TWNSDOCK.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);

    if(actionHead){
	cur.w = 140;
	cur.y += 10;
	cur.h = 54;
	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_PRESENT;
	action.pf = ActionOverDock;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionClickDock;
	AddActionEvent(actionHead, &action);
    }
}

void DrawSCRSLTurret(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // спрайт
    FillSPRITE(&sprite, "TWNSLTUR.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);
}

void DrawSCRSRTurret(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // спрайт
    FillSPRITE(&sprite, "TWNSRTUR.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);
}

void DrawSCRSMarketplace(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    INTERFACEACTION action;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // спрайт
    FillSPRITE(&sprite, "TWNSMARK.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);

    if(actionHead){
	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_PRESENT;
	action.pf = ActionOverMarketplace;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionClickMarketplace;
	AddActionEvent(actionHead, &action);
    }
}

void DrawSCRSMoat(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    INTERFACEACTION action;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // спрайт
    FillSPRITE(&sprite, "TWNSMOAT.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);

    if(actionHead){
	cur.h = 30;
	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_PRESENT;
	action.pf = ActionOverMoat;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionClickMoat;
	AddActionEvent(actionHead, &action);
    }
}

void DrawSCRSSpec(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    INTERFACEACTION action;

    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // спрайт
    FillSPRITE(&sprite, "TWNSSPEC.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);

    if(actionHead){
	cur.h = 70;
	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_PRESENT;
	action.pf = ActionOverSpec;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionClickSpec;
	AddActionEvent(actionHead, &action);
    }
}

void DrawSCRSWel2(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    INTERFACEACTION action;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // анимация
    if(animHead){
	cur.x = cx;
	cur.y = cy;
	cur.w = 0;
	cur.h = 0;
	FillSPRITE(&sprite, "TWNSWEL2.ICN", 1);
	header = GetICNHeader(&sprite);
	AddAnimationEvent(animHead, &cur, header, 5);
    }

    // спрайт
    FillSPRITE(&sprite, "TWNSWEL2.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);

    if(actionHead){
	cur.y += 10;
	cur.h = 40;
	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_PRESENT;
	action.pf = ActionOverWel2;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionClickWel2;
	AddActionEvent(actionHead, &action);
    }
}

void DrawSCRSExt1(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    INTERFACEACTION action;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // анимация
    if(animHead){
	cur.x = cx;
	cur.y = cy;
	cur.w = 0;
	cur.h = 0;
	FillSPRITE(&sprite, "TWNSEXT1.ICN", 1);
	header = GetICNHeader(&sprite);
	AddAnimationEvent(animHead, &cur, header, 5);
    }

    // спрайт
    FillSPRITE(&sprite, "TWNSEXT1.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);

    if(actionHead){
	cur.h = 33;
	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_PRESENT;
	action.pf = ActionOverWel2;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionClickWel2;
	AddActionEvent(actionHead, &action);

	cur.y += 33;
	cur.h = 33;
	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_PRESENT;
	action.pf = ActionOverStatue;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionClickStatue;
	AddActionEvent(actionHead, &action);
    }
}

void DrawSCRSExt0(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // анимация
    if(animHead){
	cur.x = cx;
	cur.y = cy;
	cur.w = 0;
	cur.h = 0;
	FillSPRITE(&sprite, "TWNSEXT0.ICN", 1);
	header = GetICNHeader(&sprite);
	AddAnimationEvent(animHead, &cur, header, 5);
    }

    // спрайт
    FillSPRITE(&sprite, "TWNSEXT0.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);
}

void DrawSCRSDwelling1(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    INTERFACEACTION action;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // анимация
    if(animHead){
	cur.x = cx;
	cur.y = cy;
	cur.w = 0;
	cur.h = 0;
	FillSPRITE(&sprite, "TWNSDW_0.ICN", 1);
	header = GetICNHeader(&sprite);
	AddAnimationEvent(animHead, &cur, header, 5);
    }

    // спрайт
    FillSPRITE(&sprite, "TWNSDW_0.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);

    if(actionHead){
	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_PRESENT;
	action.pf = ActionOverDwelling1;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionClickDwelling1;
	AddActionEvent(actionHead, &action);
    }
}

void DrawSCRSDwelling2(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    const S_CASTLE *castle = GetCurrentCastle();
    const char *icnname = NULL;
    if(castle->dwelling & DWELLING_UPGRADE2){ icnname = "TWNSUP_1.ICN"; }else{ icnname = "TWNSDW_1.ICN"; }
    INTERFACEACTION action;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // анимация
    if(animHead){
	cur.x = cx;
	cur.y = cy;
	cur.w = 0;
	cur.h = 0;
	FillSPRITE(&sprite, icnname, 1);
	header = GetICNHeader(&sprite);
	AddAnimationEvent(animHead, &cur, header, 5);
    }

    // спрайт
    FillSPRITE(&sprite, icnname, 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);

    if(actionHead){
	cur.x += 5;
	cur.w = 75;
	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_PRESENT;
	action.pf = ActionOverDwelling2;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionClickDwelling2;
	AddActionEvent(actionHead, &action);
    }
}

void DrawSCRSDwelling3(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    const S_CASTLE *castle = GetCurrentCastle();
    const char *icnname = NULL;
    if(castle->dwelling & DWELLING_UPGRADE3){ icnname = "TWNSUP_2.ICN"; }else{ icnname = "TWNSDW_2.ICN"; }
    INTERFACEACTION action;

    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // спрайт
    FillSPRITE(&sprite, icnname, 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);

    if(actionHead){
	cur.w = 80;
	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_PRESENT;
	action.pf = ActionOverDwelling3;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionClickDwelling3;
	AddActionEvent(actionHead, &action);
    }
}

void DrawSCRSDwelling4(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    const S_CASTLE *castle = GetCurrentCastle();
    const char *icnname = NULL;
    if(castle->dwelling & DWELLING_UPGRADE4){ icnname = "TWNSUP_3.ICN"; }else{ icnname = "TWNSDW_3.ICN"; }
    INTERFACEACTION action;

    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // спрайт
    FillSPRITE(&sprite, icnname, 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);

    if(actionHead){
	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_PRESENT;
	action.pf = ActionOverDwelling4;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionClickDwelling4;
	AddActionEvent(actionHead, &action);
    }
}

void DrawSCRSDwelling5(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    INTERFACEACTION action;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // спрайт
    FillSPRITE(&sprite, "TWNSDW_4.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);

    if(actionHead){
	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_PRESENT;
	action.pf = ActionOverDwelling5;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionClickDwelling5;
	AddActionEvent(actionHead, &action);
    }
}

void DrawSCRSDwelling6(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    INTERFACEACTION action;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // спрайт
    FillSPRITE(&sprite, "TWNSDW_5.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);

    if(actionHead){
	cur.w = 90;
	cur.h = 120;
	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_PRESENT;
	action.pf = ActionOverDwelling6;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionClickDwelling6;
	AddActionEvent(actionHead, &action);
    }
}

void DrawSCRSMageGuild(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    INTERFACEACTION action;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    const S_CASTLE *castle = GetCurrentCastle();

    if(castle->mageGuild.level > 5 || castle->mageGuild.level < 1) return;

    // спрайт
    FillSPRITE(&sprite, "TWNSMAGE.ICN", castle->mageGuild.level);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);

    if(1 == castle->mageGuild.level){
	cur.x +=10;
	cur.w = 60;
    }else if(2 == castle->mageGuild.level){
	cur.x +=10;
	cur.w = 60;
    }else if(3 == castle->mageGuild.level){
	cur.x +=10;
	cur.w = 60;
    }else if(4 == castle->mageGuild.level){
	cur.x +=10;
	cur.w = 60;
    }else if(5 == castle->mageGuild.level){
	cur.x +=10;
	cur.w = 60;
    }

    if(actionHead){
	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_PRESENT;
	action.pf = ActionOverMageGuild;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionClickMageGuild;
	AddActionEvent(actionHead, &action);
    }
}

