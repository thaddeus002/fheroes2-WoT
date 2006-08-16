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
#include "castle_kngt.h"

void DrawKNGTCastle(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

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
	    FillSPRITE(&sprite, "TWNKCSTL.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(animHead, &cur, header, 5);
	}
	// спрайт замка
	FillSPRITE(&sprite, "TWNKCSTL.ICN", 0);
    }else FillSPRITE(&sprite, "TWNKTENT.ICN", 0);

    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);

    if(actionHead){
	if(castle->castle){
	    cur.w -= 80;
	    cur.h -= 25;
	}else{
	    cur.w = 50;
	    cur.h = 50;
	}
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

void DrawKNGTCapitan(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    INTERFACEACTION action;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // спрайт
    FillSPRITE(&sprite, "TWNBCAPT.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);

    if(actionHead){
	cur.w = 55;
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

void DrawKNGTStatue(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    INTERFACEACTION action;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // спрайт
    FillSPRITE(&sprite, "TWNKSTAT.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);

    if(actionHead){
	cur.w = 50;
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

void DrawKNGTWell(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    INTERFACEACTION action;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // спрайт
    FillSPRITE(&sprite, "TWNKWELL.ICN", 0);

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

void DrawKNGTThievesGuild(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

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
	FillSPRITE(&sprite, "TWNKTHIE.ICN", 1);
	header = GetICNHeader(&sprite);
	AddAnimationEvent(animHead, &cur, header, 5);
    }

    // спрайт
    FillSPRITE(&sprite, "TWNKTHIE.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);

    if(actionHead){
	cur.w = 60;
	cur.h = 60;
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

void DrawKNGTTavern(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

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
	FillSPRITE(&sprite, "TWNKTVRN.ICN", 1);
	header = GetICNHeader(&sprite);
	AddAnimationEvent(animHead, &cur, header, 5);
    }

    // спрайт
    FillSPRITE(&sprite, "TWNKTVRN.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);

    if(actionHead){
	cur.x += 190;
	cur.w = 70;
	cur.h = 45;
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

void DrawKNGTShipyard(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

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
	FillSPRITE(&sprite, "TWNKDOCK.ICN", 1);
	header = GetICNHeader(&sprite);
	AddAnimationEvent(animHead, &cur, header, 5);
    }

    // спрайт
    FillSPRITE(&sprite, "TWNKDOCK.ICN", 0);
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
	action.pf = ActionOverDock;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionClickDock;
	AddActionEvent(actionHead, &action);
    }
}

void DrawKNGTLTurret(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

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
	FillSPRITE(&sprite, "TWNKLTUR.ICN", 1);
	header = GetICNHeader(&sprite);
        AddAnimationEvent(animHead, &cur, header, 5);
    }

    // спрайт
    FillSPRITE(&sprite, "TWNKLTUR.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);
}

void DrawKNGTRTurret(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

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
	FillSPRITE(&sprite, "TWNKRTUR.ICN", 1);
	header = GetICNHeader(&sprite);
	AddAnimationEvent(animHead, &cur, header, 5);
    }

    // спрайт
    FillSPRITE(&sprite, "TWNKRTUR.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);
}

void DrawKNGTMarketplace(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    INTERFACEACTION action;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // спрайт
    FillSPRITE(&sprite, "TWNKMARK.ICN", 0);
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

void DrawKNGTMoat(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    INTERFACEACTION action;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // спрайт
    FillSPRITE(&sprite, "TWNKMOAT.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);

    if(actionHead){
	cur.h = 20;
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

void DrawKNGTSpec(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    INTERFACEACTION action;
        
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    FillSPRITE(&sprite, "TWNKSPEC.ICN", 0);
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
	action.pf = ActionOverSpec;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionClickSpec;
	AddActionEvent(actionHead, &action);
    }
}

void DrawKNGTWel2(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    INTERFACEACTION action;
        
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // спрайт
    FillSPRITE(&sprite, "TWNKWEL2.ICN", 0);
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
	action.pf = ActionOverWel2;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionClickWel2;
	AddActionEvent(actionHead, &action);
    }
}

void DrawKNGTExt1(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
        
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // спрайт
    FillSPRITE(&sprite, "TWNKEXT1.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);
}

void DrawKNGTExt2(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
        
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // спрайт
    FillSPRITE(&sprite, "TWNKEXT2.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);
}

void DrawKNGTExt0(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

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
	FillSPRITE(&sprite, "TWNKEXT0.ICN", 1);
	header = GetICNHeader(&sprite);
	AddAnimationEvent(animHead, &cur, header, 5);
    }

    // спрайт
    FillSPRITE(&sprite, "TWNKEXT0.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);
}

void DrawKNGTDwelling1(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

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
	FillSPRITE(&sprite, "TWNKDW_0.ICN", 1);
	header = GetICNHeader(&sprite);
	AddAnimationEvent(animHead, &cur, header, 5);
    }

    // спрайт
    FillSPRITE(&sprite, "TWNKDW_0.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);

    if(actionHead){
	cur.x += 10;
	cur.w = 80;
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

void DrawKNGTDwelling2(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    const S_CASTLE *castle = GetCurrentCastle();
    const char *icnname = NULL;
    if(castle->dwelling & DWELLING_UPGRADE2){ icnname = "TWNKUP_1.ICN"; }else{ icnname = "TWNKDW_1.ICN"; }
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
	action.pf = ActionOverDwelling2;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionClickDwelling2;
	AddActionEvent(actionHead, &action);
    }
}

void DrawKNGTDwelling3(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    const S_CASTLE *castle = GetCurrentCastle();
    const char *icnname = NULL;
    if(castle->dwelling & DWELLING_UPGRADE3){ icnname = "TWNKUP_2.ICN"; }else{ icnname = "TWNKDW_2.ICN"; }
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
	AddAnimationEvent(animHead, &cur, header, 6);
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
	cur.x += 40;
	cur.w = 110;
	cur.h = 50;
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

void DrawKNGTDwelling4(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    const S_CASTLE *castle = GetCurrentCastle();
    const char *icnname = NULL;
    if(castle->dwelling & DWELLING_UPGRADE4){ icnname = "TWNKUP_3.ICN"; }else{ icnname = "TWNKDW_3.ICN"; }
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
	cur.x += 120;
	cur.w = 85;
	cur.y += 15;
	cur.h = 70;
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

void DrawKNGTDwelling5(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    const S_CASTLE *castle = GetCurrentCastle();
    const char *icnname = NULL;
    if(castle->dwelling & DWELLING_UPGRADE5){ icnname = "TWNKUP_4.ICN"; }else{ icnname = "TWNKDW_4.ICN"; }
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
	AddAnimationEvent(animHead, &cur, header, 7);
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
	cur.y += 22;
	cur.w = 160;
        cur.h = 60;
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

void DrawKNGTDwelling6(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    const S_CASTLE *castle = GetCurrentCastle();
    const char *icnname = NULL;
    if(castle->dwelling & DWELLING_UPGRADE6){ icnname = "TWNKUP_5.ICN"; }else{ icnname = "TWNKDW_5.ICN"; }
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
	cur.x += 330;
	cur.w = 170;
	cur.h = 190;
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

void DrawKNGTMageGuild(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    INTERFACEACTION action;

    const S_CASTLE *castle = GetCurrentCastle();

    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    if(castle->mageGuild.level > 5 || castle->mageGuild.level < 1) return;

    FillSPRITE(&sprite, "TWNKMAGE.ICN", castle->mageGuild.level - 1);

    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);

    if(1 == castle->mageGuild.level){
	cur.x += 260;
	cur.y += 10;
	cur.w = 55;
	cur.h = 60;
    }else if(2 == castle->mageGuild.level){
	cur.x += 260;
	cur.y += 10;
	cur.w = 55;
	cur.h = 80;
    }else if(3 == castle->mageGuild.level){
	cur.x += 260;
	cur.y += 10;
	cur.w = 55;
	cur.h = 100;
    }else if(4 == castle->mageGuild.level){
	cur.x += 260;
	cur.y += 10;
	cur.w = 55;
	cur.h = 120;
    }else if(5 == castle->mageGuild.level){
	cur.x += 260;
	cur.y += 10;
	cur.w = 55;
	cur.h = 140;
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
