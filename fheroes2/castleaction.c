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
#include "castle.h"
#include "castleaction.h"

ACTION ActionOverCastle(void){

    const char *message = NULL;
    const S_CASTLE *castle = GetCurrentCastle();

    if(castle->castle){ message = "Castle"; }else{ message = "Tent"; }

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    cur.x = video->w / 2;
    cur.y = video->h / 2 + 240 - BORDERWIDTH;

    cur.x = cur.x - strlen(message) * FONT_WIDTHSMALL / 2;
    cur.y = cur.y - 3;
    cur.w = FONT_WIDTHBIG * strlen(message);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, message, FONT_BIG);

    return NONE;
}

ACTION ActionOverCaptain(void){

    const char *message = "Captains Quarters";

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    cur.x = video->w / 2;
    cur.y = video->h / 2 + 240 - BORDERWIDTH;

    cur.x = cur.x - strlen(message) * FONT_WIDTHSMALL / 2;
    cur.y = cur.y - 3;
    cur.w = FONT_WIDTHBIG * strlen(message);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, message, FONT_BIG);

    return NONE;
}

ACTION ActionOverStatue(void){

    const char *message = "Statue";

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    cur.x = video->w / 2;
    cur.y = video->h / 2 + 240 - BORDERWIDTH;

    cur.x = cur.x - strlen(message) * FONT_WIDTHSMALL / 2;
    cur.y = cur.y - 3;
    cur.w = FONT_WIDTHBIG * strlen(message);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, message, FONT_BIG);

    return NONE;
}

ACTION ActionOverMarketplace(void){

    const char *message = "Marketplace";

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    cur.x = video->w / 2;
    cur.y = video->h / 2 + 240 - BORDERWIDTH;

    cur.x = cur.x - strlen(message) * FONT_WIDTHSMALL / 2;
    cur.y = cur.y - 3;
    cur.w = FONT_WIDTHBIG * strlen(message);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, message, FONT_BIG);

    return NONE;
}

ACTION ActionOverMoat(void){

    const char *message = "Moat";

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    cur.x = video->w / 2;
    cur.y = video->h / 2 + 240 - BORDERWIDTH;

    cur.x = cur.x - strlen(message) * FONT_WIDTHSMALL / 2;
    cur.y = cur.y - 3;
    cur.w = FONT_WIDTHBIG * strlen(message);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, message, FONT_BIG);

    return NONE;
}

ACTION ActionOverDock(void){

    const char *message = "Dock";

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    cur.x = video->w / 2;
    cur.y = video->h / 2 + 240 - BORDERWIDTH;

    cur.x = cur.x - strlen(message) * FONT_WIDTHSMALL / 2;
    cur.y = cur.y - 3;
    cur.w = FONT_WIDTHBIG * strlen(message);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, message, FONT_BIG);

    return NONE;
}

ACTION ActionOverWell(void){

    const char *message = "Well";

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    cur.x = video->w / 2;
    cur.y = video->h / 2 + 240 - BORDERWIDTH;

    cur.x = cur.x - strlen(message) * FONT_WIDTHSMALL / 2;
    cur.y = cur.y - 3;
    cur.w = FONT_WIDTHBIG * strlen(message);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, message, FONT_BIG);

    return NONE;
}

ACTION ActionOverTavern(void){

    const char *message = "Tavern";

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    cur.x = video->w / 2;
    cur.y = video->h / 2 + 240 - BORDERWIDTH;

    cur.x = cur.x - strlen(message) * FONT_WIDTHSMALL / 2;
    cur.y = cur.y - 3;
    cur.w = FONT_WIDTHBIG * strlen(message);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, message, FONT_BIG);

    return NONE;
}

ACTION ActionOverThievesGuild(void){

    const char *message = "Thieves Guild";

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    cur.x = video->w / 2;
    cur.y = video->h / 2 + 240 - BORDERWIDTH;

    cur.x = cur.x - strlen(message) * FONT_WIDTHSMALL / 2;
    cur.y = cur.y - 3;
    cur.w = FONT_WIDTHBIG * strlen(message);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, message, FONT_BIG);

    return NONE;
}

ACTION ActionOverMageGuild(void){

    const char *message = "Mage Guild";

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    cur.x = video->w / 2;
    cur.y = video->h / 2 + 240 - BORDERWIDTH;

    cur.x = cur.x - strlen(message) * FONT_WIDTHSMALL / 2;
    cur.y = cur.y - 3;
    cur.w = FONT_WIDTHBIG * strlen(message);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, message, FONT_BIG);

    return NONE;
}

ACTION ActionClickCastleMonster(void){
/*
    Uint8               select;
    SDL_Rect            rect;
    SDL_Surface         *surface;
    BOOL                use;
    } backMonsterCursor;
*/
    Sint32 mx, my;
    SDL_Surface *video = SDL_GetVideoSurface();

    // верхний левый угол начала
    Uint16 cx = video->w / 2 - 208;
    //Uint16 cy = video->h / 2 + 22;

    SDL_GetMouseState(&mx, &my);

    Uint8 index = (Uint8) ((mx - cx) / 88 + 1);
    printf("%d\n", index);

    return NONE;
}

ACTION ActionClickHeroesMonster(void){

    Sint32 mx, my;
    SDL_Surface *video = SDL_GetVideoSurface();

    // верхний левый угол начала
    Uint16 cx = video->w / 2 - 208;
    //Uint16 cy = video->h / 2 + 120;

    SDL_GetMouseState(&mx, &my);

    Uint8 index = (Uint8) ((mx - cx) / 88 + 1);
    printf("%d\n", index);

    return NONE;
}

ACTION ActionOverHeroes(void){

    const char *message = "View Hero";

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    cur.x = video->w / 2;
    cur.y = video->h / 2 + 240 - BORDERWIDTH;

    cur.x = cur.x - strlen(message) * FONT_WIDTHSMALL / 2;
    cur.y = cur.y - 3;
    cur.w = FONT_WIDTHBIG * strlen(message);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, message, FONT_BIG);

    return NONE;
}

ACTION ActionOverExit(void){

    const char *message = NULL;
    const S_CASTLE *castle = GetCurrentCastle();

    if(castle->castle){ message = "Exit Castle"; }else{ message = "Exit Town"; }

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    cur.x = video->w / 2;
    cur.y = video->h / 2 + 240 - BORDERWIDTH;

    cur.x = cur.x - strlen(message) * FONT_WIDTHSMALL / 2;
    cur.y = cur.y - 3;
    cur.w = FONT_WIDTHBIG * strlen(message);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, message, FONT_BIG);

    return NONE;
}

ACTION ActionViewHeroes(void){

    return NONE;
}
