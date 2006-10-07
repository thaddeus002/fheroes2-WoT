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
#include "draw.h"  
#include "tools.h"      
#include "cursor.h"      
#include "castle.h"      
#include "castleaction.h"     
#include "castle_wzrd.h"

void DrawWZRDCastle(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;

    INTERFACEACTION action;
    const S_CASTLE *castle = GetCurrentCastle();
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    if(castle->building & BUILD_CASTLE){
	// анимация
	if(animHead){
	    cur.x = cx;
	    cur.y = cy;
	    cur.w = 0;
	    cur.h = 0;
	    FillSPRITE(&sprite, "TWNZCSTL.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(animHead, &cur, header, 5);
	}
	// спрайт замка
	FillSPRITE(&sprite, "TWNZCSTL.ICN", 0);
    }else FillSPRITE(&sprite, "TWNZTENT.ICN", 0);

    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    GetFlagUpdateBuilding() ? AlphaBlendingObject(header->surface, &cur, GetCastleAlphaBlending()) : SDL_BlitSurface(header->surface, NULL, video, &cur);

    if(actionHead){
	cur.w -= 10;
	cur.h -= 10;
	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_PRESENT;
	action.pf = ActionOverCastle;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionClickCastle;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_RCLICK;
	action.pf = ActionClickRCastle;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);
    }	
}

void DrawWZRDCapitan(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    INTERFACEACTION action;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // спрайт
    FillSPRITE(&sprite, "TWNZCAPT.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    GetFlagUpdateBuilding() ? AlphaBlendingObject(header->surface, &cur, GetCastleAlphaBlending()) : SDL_BlitSurface(header->surface, NULL, video, &cur);

    if(actionHead){
	cur.x += 9;
	cur.w = 30;
	cur.h -= 5;
	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_PRESENT;
	action.pf = ActionOverCaptain;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionClickCaptain;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_RCLICK;
	action.pf = ActionClickRCaptain;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);
    }
}

void DrawWZRDStatue(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    INTERFACEACTION action;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // спрайт
    FillSPRITE(&sprite, "TWNZSTAT.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    GetFlagUpdateBuilding() ? AlphaBlendingObject(header->surface, &cur, GetCastleAlphaBlending()) : SDL_BlitSurface(header->surface, NULL, video, &cur);

    if(actionHead){
	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_PRESENT;
	action.pf = ActionOverStatue;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionClickStatue;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_RCLICK;
	action.pf = ActionClickRStatue;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);
    }
}

void DrawWZRDWell(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    INTERFACEACTION action;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // спрайт
    FillSPRITE(&sprite, "TWNZWELL.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    GetFlagUpdateBuilding() ? AlphaBlendingObject(header->surface, &cur, GetCastleAlphaBlending()) : SDL_BlitSurface(header->surface, NULL, video, &cur);

    if(actionHead){
	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_PRESENT;
	action.pf = ActionOverWell;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionClickWell;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_RCLICK;
	action.pf = ActionClickRWell;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);
    }
}

void DrawWZRDThievesGuild(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

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
	FillSPRITE(&sprite, "TWNZTHIE.ICN", 1);
	header = GetICNHeader(&sprite);
	AddAnimationEvent(animHead, &cur, header, 5);
    }

    // спрайт
    FillSPRITE(&sprite, "TWNZTHIE.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    GetFlagUpdateBuilding() ? AlphaBlendingObject(header->surface, &cur, GetCastleAlphaBlending()) : SDL_BlitSurface(header->surface, NULL, video, &cur);

    if(actionHead){
	cur.x += 10;
	cur.w = 55;
	cur.h = 45;
	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_PRESENT;
	action.pf = ActionOverThievesGuild;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionClickThievesGuild;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_RCLICK;
	action.pf = ActionClickRThievesGuild;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);
    }
}

void DrawWZRDTavern(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

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
	FillSPRITE(&sprite, "TWNZTVRN.ICN", 1);
	header = GetICNHeader(&sprite);
	AddAnimationEvent(animHead, &cur, header, 6);
    }
    
    // спрайт
    FillSPRITE(&sprite, "TWNZTVRN.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    GetFlagUpdateBuilding() ? AlphaBlendingObject(header->surface, &cur, GetCastleAlphaBlending()) : SDL_BlitSurface(header->surface, NULL, video, &cur);

    if(actionHead){
	cur.w = 70;
	cur.h = 70;
	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_PRESENT;
	action.pf = ActionOverTavern;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionClickTavern;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_RCLICK;
	action.pf = ActionClickRTavern;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);
    }
}

void DrawWZRDShipyard(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

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
	if(CastleNearBoat()){
	    FillSPRITE(&sprite, "TWNZBOAT.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(animHead, &cur, header, 9);
	}else{
	    FillSPRITE(&sprite, "TWNZDOCK.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(animHead, &cur, header, 5);
	}
    }

    // спрайт
    if(CastleNearBoat()) FillSPRITE(&sprite, "TWNZBOAT.ICN", 0); else FillSPRITE(&sprite, "TWNZDOCK.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    GetFlagUpdateBuilding() ? AlphaBlendingObject(header->surface, &cur, GetCastleAlphaBlending()) : SDL_BlitSurface(header->surface, NULL, video, &cur);

    if(actionHead){
	cur.y += 13;
	cur.h = 40;
	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_PRESENT;
	action.pf = ActionOverDock;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionClickDock;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_RCLICK;
	action.pf = ActionClickRDock;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);
    }
}

void DrawWZRDLTurret(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // спрайт
    FillSPRITE(&sprite, "TWNZLTUR.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    GetFlagUpdateBuilding() ? AlphaBlendingObject(header->surface, &cur, GetCastleAlphaBlending()) : SDL_BlitSurface(header->surface, NULL, video, &cur);
}

void DrawWZRDRTurret(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // спрайт
    FillSPRITE(&sprite, "TWNZRTUR.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    GetFlagUpdateBuilding() ? AlphaBlendingObject(header->surface, &cur, GetCastleAlphaBlending()) : SDL_BlitSurface(header->surface, NULL, video, &cur);
}

void DrawWZRDMarketplace(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    INTERFACEACTION action;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // спрайт
    FillSPRITE(&sprite, "TWNZMARK.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    GetFlagUpdateBuilding() ? AlphaBlendingObject(header->surface, &cur, GetCastleAlphaBlending()) : SDL_BlitSurface(header->surface, NULL, video, &cur);

    if(actionHead){
	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_PRESENT;
	action.pf = ActionOverMarketplace;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionClickMarketplace;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_RCLICK;
	action.pf = ActionClickRMarketplace;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);
    }
}

void DrawWZRDMoat(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    INTERFACEACTION action;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // спрайт
    FillSPRITE(&sprite, "TWNZMOAT.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    GetFlagUpdateBuilding() ? AlphaBlendingObject(header->surface, &cur, GetCastleAlphaBlending()) : SDL_BlitSurface(header->surface, NULL, video, &cur);

    if(actionHead){
	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_PRESENT;
	action.pf = ActionOverMoat;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionClickMoat;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_RCLICK;
	action.pf = ActionClickRMoat;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);
    }
}

void DrawWZRDSpec(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    INTERFACEACTION action;

    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // спрайт
    FillSPRITE(&sprite, "TWNZSPEC.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    GetFlagUpdateBuilding() ? AlphaBlendingObject(header->surface, &cur, GetCastleAlphaBlending()) : SDL_BlitSurface(header->surface, NULL, video, &cur);

    if(actionHead){
	cur.x += 40;
	cur.w = 100;
	cur.h = 70;
	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_PRESENT;
	action.pf = ActionOverSpec;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionClickSpec;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_RCLICK;
	action.pf = ActionClickRSpec;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);
    }
}

void DrawWZRDWel2(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    INTERFACEACTION action;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // спрайт
    FillSPRITE(&sprite, "TWNZWEL2.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    GetFlagUpdateBuilding() ? AlphaBlendingObject(header->surface, &cur, GetCastleAlphaBlending()) : SDL_BlitSurface(header->surface, NULL, video, &cur);

    if(actionHead){
	cur.y += 10;
	cur.h = 40;
	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_PRESENT;
	action.pf = ActionOverWel2;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionClickWel2;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_RCLICK;
	action.pf = ActionClickRWel2;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);
    }
}

void DrawWZRDExt0(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

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
	FillSPRITE(&sprite, "TWNZEXT0.ICN", 1);
	header = GetICNHeader(&sprite);
	AddAnimationEvent(animHead, &cur, header, 5);
    }

    // спрайт
    FillSPRITE(&sprite, "TWNZEXT0.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);
}

void DrawWZRDDwelling1(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

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
	FillSPRITE(&sprite, "TWNZDW_0.ICN", 1);
	header = GetICNHeader(&sprite);
	AddAnimationEvent(animHead, &cur, header, 5);
    }

    // спрайт
    FillSPRITE(&sprite, "TWNZDW_0.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    GetFlagUpdateBuilding() ? AlphaBlendingObject(header->surface, &cur, GetCastleAlphaBlending()) : SDL_BlitSurface(header->surface, NULL, video, &cur);

    if(actionHead){
	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_PRESENT;
	action.pf = ActionOverDwelling1;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionClickDwelling1;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_RCLICK;
	action.pf = ActionClickRDwelling1;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);
    }
}

void DrawWZRDDwelling2(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    INTERFACEACTION action;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    FillSPRITE(&sprite, "TWNZDW_1.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    GetFlagUpdateBuilding() ? AlphaBlendingObject(header->surface, &cur, GetCastleAlphaBlending()) : SDL_BlitSurface(header->surface, NULL, video, &cur);

    if(actionHead){
	cur.w = 90;
	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_PRESENT;
	action.pf = ActionOverDwelling2;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionClickDwelling2;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_RCLICK;
	action.pf = ActionClickRDwelling2;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);
    }
}

void DrawWZRDDwelling3(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    const S_CASTLE *castle = GetCurrentCastle();
    const char *icnname = NULL;
    if(castle->building & DWELLING_UPGRADE3){ icnname = "TWNZUP_2.ICN"; }else{ icnname = "TWNZDW_2.ICN"; }
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
    GetFlagUpdateBuilding() ? AlphaBlendingObject(header->surface, &cur, GetCastleAlphaBlending()) : SDL_BlitSurface(header->surface, NULL, video, &cur);

    if(actionHead){
	cur.x += 24;
	cur.w = 100;
	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_PRESENT;
	action.pf = ActionOverDwelling3;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionClickDwelling3;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_RCLICK;
	action.pf = ActionClickRDwelling3;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);
    }
}

void DrawWZRDDwelling4(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    INTERFACEACTION action;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // спрайт
    FillSPRITE(&sprite, "TWNZDW_3.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    GetFlagUpdateBuilding() ? AlphaBlendingObject(header->surface, &cur, GetCastleAlphaBlending()) : SDL_BlitSurface(header->surface, NULL, video, &cur);

    if(actionHead){
	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_PRESENT;
	action.pf = ActionOverDwelling4;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionClickDwelling4;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_RCLICK;
	action.pf = ActionClickRDwelling4;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);
    }
}

void DrawWZRDDwelling5(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    const S_CASTLE *castle = GetCurrentCastle();
    const char *icnname = NULL;
    if(castle->building & DWELLING_UPGRADE5){ icnname = "TWNZUP_4.ICN"; }else{ icnname = "TWNZDW_4.ICN"; }
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
    GetFlagUpdateBuilding() ? AlphaBlendingObject(header->surface, &cur, GetCastleAlphaBlending()) : SDL_BlitSurface(header->surface, NULL, video, &cur);

    if(actionHead){
    	cur.x += 47;
	cur.w = 50;
	cur.h = 170;
	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_PRESENT;
	action.pf = ActionOverDwelling5;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionClickDwelling5;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_RCLICK;
	action.pf = ActionClickRDwelling5;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);
    }
}

void DrawWZRDDwelling6(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    const S_CASTLE *castle = GetCurrentCastle();
    const char *icnname = NULL;
    if(castle->building & DWELLING_UPGRADE6){ icnname = "TWNZUP_5.ICN"; }else{ icnname = "TWNZDW_5.ICN"; }
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
    GetFlagUpdateBuilding() ? AlphaBlendingObject(header->surface, &cur, GetCastleAlphaBlending()) : SDL_BlitSurface(header->surface, NULL, video, &cur);

    if(actionHead){
	cur.h = 45;
	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_PRESENT;
	action.pf = ActionOverDwelling6;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionClickDwelling6;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_RCLICK;
	action.pf = ActionClickRDwelling6;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);
    }
}

void DrawWZRDMageGuild(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    INTERFACEACTION action;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    const S_CASTLE *castle = GetCurrentCastle();
    
    if(GetMageGuildLevel(castle) == MAGIC_NONE) return;

    // спрайт
    FillSPRITE(&sprite, "TWNZMAGE.ICN", GetMageGuildLevel(castle) - 1);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    GetFlagUpdateBuilding() ? AlphaBlendingObject(header->surface, &cur, GetCastleAlphaBlending()) : SDL_BlitSurface(header->surface, NULL, video, &cur);

    switch(GetMageGuildLevel(castle)){
	case MAGIC_LEVEL1:
	    cur.x += 52;
	    cur.w = 60;
	    break;
	case MAGIC_LEVEL2:
	    cur.x += 52;
	    cur.w = 60;
	    break;
	case MAGIC_LEVEL3:
	    cur.x += 52;
	    cur.w = 60;
	    break;
	case MAGIC_LEVEL4:
	    cur.x += 52;
	    cur.w = 60;
	    break;
	case MAGIC_LEVEL5:
	    cur.x += 52;
	    cur.w = 60;
	default:
	    break;
    }

    if(actionHead){
	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_PRESENT;
	action.pf = ActionOverMageGuild;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionClickMageGuild;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);

	ZeroINTERFACEACTION(&action);
	action.rect = cur;
	action.mouseEvent = MOUSE_RCLICK;
	action.pf = ActionClickRMageGuild;
	action.level = LEVELEVENT_CASTLEUPDATEBUILD;
	AddActionEvent(actionHead, &action);
    }
}
