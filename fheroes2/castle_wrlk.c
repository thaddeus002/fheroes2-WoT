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
#include "castle_wrlk.h" 

void DrawWRLKCastle(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

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
	FillSPRITE(&sprite, "TWNWCSTL.ICN", 1);
	header = GetICNHeader(&sprite);
	AddAnimationEvent(animHead, &cur, header, 5);
	// спрайт замка
	FillSPRITE(&sprite, "TWNWCSTL.ICN", 0);
    }else FillSPRITE(&sprite, "TWNWTENT.ICN", 0);

    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);

    cur.x += 30;
    cur.w = 210;
    cur.h -= 20;
    ZeroINTERFACEACTION(&action);
    action.rect = cur;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionOverCastle;
    AddActionEvent(actionHead, &action);
}

void DrawWRLKCapitan(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

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
    FillSPRITE(&sprite, "TWNWCAPT.ICN", 1);
    header = GetICNHeader(&sprite);
    AddAnimationEvent(animHead, &cur, header, 5);

    // спрайт
    FillSPRITE(&sprite, "TWNWCAPT.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);

    cur.x += 10;
    cur.y += 10;
    cur.w = 80;
    cur.h = 100;
    ZeroINTERFACEACTION(&action);
    action.rect = cur;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionOverCaptain;
    AddActionEvent(actionHead, &action);
}

void DrawWRLKStatue(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    INTERFACEACTION action;

    // спрайт
    FillSPRITE(&sprite, "TWNWSTAT.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);

    cur.x += 65;
    cur.w = 44;
    ZeroINTERFACEACTION(&action);
    action.rect = cur;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionOverStatue;
    AddActionEvent(actionHead, &action);
}

void DrawWRLKWell(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // спрайт
    FillSPRITE(&sprite, "TWNWWELL.ICN", 0);
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

void DrawWRLKThievesGuild(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    INTERFACEACTION action;

    // спрайт
    FillSPRITE(&sprite, "TWNWTHIE.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);

    cur.x += 8;
    cur.w = 70;
    cur.h = 60;
    ZeroINTERFACEACTION(&action);
    action.rect = cur;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionOverThievesGuild;
    AddActionEvent(actionHead, &action);
}

void DrawWRLKTavern(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    INTERFACEACTION action;

    // спрайт
    FillSPRITE(&sprite, "TWNWTVRN.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);

    cur.x += 10;
    cur.w = 78;
    cur.h = 54;
    ZeroINTERFACEACTION(&action);
    action.rect = cur;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionOverTavern;
    AddActionEvent(actionHead, &action);
}

void DrawWRLKShipyard(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

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
    FillSPRITE(&sprite, "TWNWDOCK.ICN", 1);
    header = GetICNHeader(&sprite);
    AddAnimationEvent(animHead, &cur, header, 5);

    // спрайт
    FillSPRITE(&sprite, "TWNWDOCK.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);

    cur.x += 20;
    cur.w = 130;
    cur.y += 20;
    cur.h = 53;
    ZeroINTERFACEACTION(&action);
    action.rect = cur;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionOverDock;
    AddActionEvent(actionHead, &action);
}

void DrawWRLKLTurret(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // спрайт
    FillSPRITE(&sprite, "TWNWLTUR.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);
}

void DrawWRLKRTurret(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // спрайт
    FillSPRITE(&sprite, "TWNWRTUR.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);
}

void DrawWRLKMarketplace(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // спрайт
    FillSPRITE(&sprite, "TWNWMARK.ICN", 0);
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

void DrawWRLKMoat(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

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
    FillSPRITE(&sprite, "TWNWMOAT.ICN", 1);
    header = GetICNHeader(&sprite);
    AddAnimationEvent(animHead, &cur, header, 5);
    // спрайт
    FillSPRITE(&sprite, "TWNWMOAT.ICN", 0);
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

void DrawWRLKSpec(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    INTERFACEACTION action;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // спрайт
    FillSPRITE(&sprite, "TWNWSPEC.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);

    cur.w = 70;
    ZeroINTERFACEACTION(&action);
    action.rect = cur;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionOverSpec;
    AddActionEvent(actionHead, &action);
}

void DrawWRLKWel2(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    INTERFACEACTION action;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // анимация
    cur.x = cx;
    cur.y = cy;
    cur.w = 0;
    cur.h = 0;
    FillSPRITE(&sprite, "TWNWWEL2.ICN", 1);
    header = GetICNHeader(&sprite);
    AddAnimationEvent(animHead, &cur, header, 6);
    // спрайт
    FillSPRITE(&sprite, "TWNWWEL2.ICN", 0);

    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);

    cur.x += 50;
    cur.w = 70;
    ZeroINTERFACEACTION(&action);
    action.rect = cur;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionOverWel2;
    AddActionEvent(actionHead, &action);
}

void DrawWRLKExt0(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

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
    FillSPRITE(&sprite, "TWNWEXT0.ICN", 1);
    header = GetICNHeader(&sprite);
    AddAnimationEvent(animHead, &cur, header, 5);

    // спрайт
    FillSPRITE(&sprite, "TWNWEXT0.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);
}

void DrawWRLKDwelling1(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

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
    FillSPRITE(&sprite, "TWNWDW_0.ICN", 1);
    header = GetICNHeader(&sprite);
    AddAnimationEvent(animHead, &cur, header, 6);

    // спрайт
    FillSPRITE(&sprite, "TWNWDW_0.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);

    cur.w = 50;
    INTERFACEACTION action;
    ZeroINTERFACEACTION(&action);
    action.rect = cur;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionOverDwelling1;
    AddActionEvent(actionHead, &action);
}

void DrawWRLKDwelling2(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // спрайт
    FillSPRITE(&sprite, "TWNWDW_1.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);

    cur.x += 9;
    cur.w = 90;
    INTERFACEACTION action;
    ZeroINTERFACEACTION(&action);
    action.rect = cur;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionOverDwelling2;
    AddActionEvent(actionHead, &action);
}

void DrawWRLKDwelling3(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // спрайт
    FillSPRITE(&sprite, "TWNWDW_2.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);

    cur.x += 105;
    cur.w = 60;
    cur.h = 60;
    INTERFACEACTION action;
    ZeroINTERFACEACTION(&action);
    action.rect = cur;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionOverDwelling3;
    AddActionEvent(actionHead, &action);
}

void DrawWRLKDwelling4(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    const S_CASTLE *castle = GetCurrentCastle();
    const char *icnname = NULL;
    if(castle->dwelling & DWELLING_UPGRADE4){ icnname = "TWNWUP_3.ICN"; }else{ icnname = "TWNWDW_3.ICN"; }
    
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
    
    cur.w = 110;
    INTERFACEACTION action;
    ZeroINTERFACEACTION(&action);
    action.rect = cur;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionOverDwelling4;
    AddActionEvent(actionHead, &action);
}

void DrawWRLKDwelling5(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 320;
    Uint16 cy = video->h / 2 - 240;

    // спрайт
    FillSPRITE(&sprite, "TWNWDW_4.ICN", 0);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);

    cur.x += 90;
    cur.w = 90;
    cur.h = 80;
    INTERFACEACTION action;
    ZeroINTERFACEACTION(&action);
    action.rect = cur;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionOverDwelling5;
    AddActionEvent(actionHead, &action);
}

void DrawWRLKDwelling6(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    const S_CASTLE *castle = GetCurrentCastle();
    const char *icnname = NULL;
    if(castle->dwelling & DWELLING_UPGRADE7){ icnname = "TWNWUP5B.ICN"; }
    else if(castle->dwelling & DWELLING_UPGRADE6){ icnname = "TWNWUP_5.ICN"; }
    else{ icnname = "TWNWDW_5.ICN"; }
    
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

    cur.x += 10;
    cur.w = 64;
    INTERFACEACTION action;
    ZeroINTERFACEACTION(&action);
    action.rect = cur;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionOverDwelling6;
    AddActionEvent(actionHead, &action);
}

void DrawWRLKMageGuild(S_ANIMATION **animHead, INTERFACEACTION **actionHead){

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
    FillSPRITE(&sprite, "TWNWMAGE.ICN", castle->magicTower);
    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);

    if(1 == castle->magicTower){
	cur.x += 32;
	cur.w = 50;
	cur.h = 50;
    }else if(2 == castle->magicTower){
	cur.x += 49;
	cur.w = 50;
	cur.h = 80;
    }else if(3 == castle->magicTower){
	cur.x += 49;
	cur.w = 50;
	cur.h = 110;
    }else if(4 == castle->magicTower){
	cur.x += 49;
	cur.w = 50;
	cur.h = 150;
    }else if(5 == castle->magicTower){
	cur.x += 49;
	cur.w = 50;
	cur.h = 170;
    }

    ZeroINTERFACEACTION(&action);
    action.rect = cur;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionOverMageGuild;
    AddActionEvent(actionHead, &action);
}
