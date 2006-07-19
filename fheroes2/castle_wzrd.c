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

    if(castle->castle){
	// анимация
	cur.x = cx;
	cur.y = cy;
	cur.w = 0;
	cur.h = 0;
	FillSPRITE(&sprite, "TWNZCSTL.ICN", 1);
	header = GetICNHeader(&sprite);
	AddAnimationEvent(animHead, &cur, header, 5);

	// спрайт замка
	FillSPRITE(&sprite, "TWNZCSTL.ICN", 0);
    }else FillSPRITE(&sprite, "TWNZTENT.ICN", 0);

    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);

    cur.w -= 10;
    cur.h -= 10;
    ZeroINTERFACEACTION(&action);
    action.rect = cur;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionOverCastle;
    AddActionEvent(actionHead, &action);
}

void DrawWZRDCapitan(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    INTERFACEACTION action;

    // спрайт
    FillSPRITE(&sprite, "TWNZCAPT.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);

    cur.x += 9;
    cur.w = 30;
    cur.h -= 5;
    ZeroINTERFACEACTION(&action);
    action.rect = cur;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionOverCaptain;
    AddActionEvent(actionHead, &action);
}

void DrawWZRDStatue(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    INTERFACEACTION action;

    // спрайт
    FillSPRITE(&sprite, "TWNZSTAT.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);

    ZeroINTERFACEACTION(&action);
    action.rect = cur;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionOverStatue;
    AddActionEvent(actionHead, &action);
}

void DrawWZRDWell(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // спрайт
    FillSPRITE(&sprite, "TWNZWELL.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);

    INTERFACEACTION action;
    ZeroINTERFACEACTION(&action);
    action.rect = cur;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionOverWell;
    AddActionEvent(actionHead, &action);
}

void DrawWZRDThievesGuild(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    INTERFACEACTION action;

    // анимация
    cur.x = cx;
    cur.y = cy;
    cur.w = 0;
    cur.h = 0;
    FillSPRITE(&sprite, "TWNZTHIE.ICN", 1);
    header = GetICNHeader(&sprite);
    AddAnimationEvent(animHead, &cur, header, 5);

    // спрайт
    FillSPRITE(&sprite, "TWNZTHIE.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);

    cur.x += 10;
    cur.w = 55;
    cur.h = 45;
    ZeroINTERFACEACTION(&action);
    action.rect = cur;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionOverThievesGuild;
    AddActionEvent(actionHead, &action);
}

void DrawWZRDTavern(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    INTERFACEACTION action;

    // анимация
    cur.x = cx;
    cur.y = cy;
    cur.w = 0;
    cur.h = 0;
    FillSPRITE(&sprite, "TWNZTVRN.ICN", 1);
    header = GetICNHeader(&sprite);
    AddAnimationEvent(animHead, &cur, header, 6);

    // спрайт
    FillSPRITE(&sprite, "TWNZTVRN.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);

    cur.w = 70;
    cur.h = 70;
    ZeroINTERFACEACTION(&action);
    action.rect = cur;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionOverTavern;
    AddActionEvent(actionHead, &action);
}

void DrawWZRDShipyard(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    INTERFACEACTION action;

    // анимация
    cur.x = cx;
    cur.y = cy;
    cur.w = 0;
    cur.h = 0;
    FillSPRITE(&sprite, "TWNZDOCK.ICN", 1);
    header = GetICNHeader(&sprite);
    AddAnimationEvent(animHead, &cur, header, 5);

    // спрайт
    FillSPRITE(&sprite, "TWNZDOCK.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);

    cur.y += 13;
    cur.h = 40;
    ZeroINTERFACEACTION(&action);
    action.rect = cur;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionOverDock;
    AddActionEvent(actionHead, &action);
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
    SDL_BlitSurface(header->surface, NULL, video, &cur);
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
    SDL_BlitSurface(header->surface, NULL, video, &cur);
}

void DrawWZRDMarketplace(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // спрайт
    FillSPRITE(&sprite, "TWNZMARK.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);

    INTERFACEACTION action;
    ZeroINTERFACEACTION(&action);
    action.rect = cur;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionOverMarketplace;
    AddActionEvent(actionHead, &action);
}

void DrawWZRDMoat(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // спрайт
    FillSPRITE(&sprite, "TWNZMOAT.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);

    INTERFACEACTION action;
    ZeroINTERFACEACTION(&action);
    action.rect = cur;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionOverMoat;
    AddActionEvent(actionHead, &action);
}

void DrawWZRDSpec(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // спрайт
    FillSPRITE(&sprite, "TWNZSPEC.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);
}

void DrawWZRDExt1(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // спрайт
    FillSPRITE(&sprite, "TWNZWEL2.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);
}

void DrawWZRDExt0(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // анимация
    cur.x = cx;
    cur.y = cy;
    cur.w = 0;
    cur.h = 0;
    FillSPRITE(&sprite, "TWNZEXT0.ICN", 1);
    header = GetICNHeader(&sprite);
    AddAnimationEvent(animHead, &cur, header, 5);

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
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // анимация
    cur.x = cx;
    cur.y = cy;
    cur.w = 0;
    cur.h = 0;
    FillSPRITE(&sprite, "TWNZDW_0.ICN", 1);
    header = GetICNHeader(&sprite);
    AddAnimationEvent(animHead, &cur, header, 5);

    // спрайт
    FillSPRITE(&sprite, "TWNZDW_0.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);
}

void DrawWZRDDwelling2(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    FillSPRITE(&sprite, "TWNZDW_1.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);
}

void DrawWZRDDwelling3(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // анимация
    cur.x = cx;
    cur.y = cy;
    cur.w = 0;
    cur.h = 0;
    FillSPRITE(&sprite, "TWNZDW_2.ICN", 1);
    header = GetICNHeader(&sprite);
    AddAnimationEvent(animHead, &cur, header, 5);

    // спрайт
    FillSPRITE(&sprite, "TWNZDW_2.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);
}

void DrawWZRDDwelling4(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // спрайт
    FillSPRITE(&sprite, "TWNZDW_3.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);
}

void DrawWZRDDwelling5(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // спрайт
    FillSPRITE(&sprite, "TWNZDW_4.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);
}

void DrawWZRDDwelling6(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // спрайт
    FillSPRITE(&sprite, "TWNZDW_5.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);
}

void DrawWZRDMageGuild(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    INTERFACEACTION action;
    const S_CASTLE *castle = GetCurrentCastle();
    
    if(castle->magicTower > 5 || castle->magicTower < 1) return;

    // спрайт
    FillSPRITE(&sprite, "TWNZMAGE.ICN", castle->magicTower);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);

    if(1 == castle->magicTower){
	cur.x += 52;
	cur.w = 60;
    }else if(2 == castle->magicTower){
	cur.x += 52;
	cur.w = 60;
    }else if(3 == castle->magicTower){
	cur.x += 52;
	cur.w = 60;
    }else if(4 == castle->magicTower){
	cur.x += 52;
	cur.w = 60;
    }else if(5 == castle->magicTower){
	cur.x += 52;
	cur.w = 60;
    }

    ZeroINTERFACEACTION(&action);
    action.rect = cur;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionOverMageGuild;
    AddActionEvent(actionHead, &action);
}

void DrawWZRDUpgrade2(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    return;
}

void DrawWZRDUpgrade3(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // анимация
    cur.x = cx;
    cur.y = cy;
    cur.w = 0;
    cur.h = 0;
    FillSPRITE(&sprite, "TWNZUP_2.ICN", 1);
    header = GetICNHeader(&sprite);
    AddAnimationEvent(animHead, &cur, header, 5);

    // спрайт
    FillSPRITE(&sprite, "TWNZUP_2.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);
}

void DrawWZRDUpgrade4(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    return;
}

void DrawWZRDUpgrade5(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // спрайт
    FillSPRITE(&sprite, "TWNZUP_4.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);
}

void DrawWZRDUpgrade6(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // спрайт
    FillSPRITE(&sprite, "TWNZUP_5.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);
}

void DrawWZRDUpgrade7(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    return;
}
