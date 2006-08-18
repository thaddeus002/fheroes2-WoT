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
#include "element.h"
#include "config.h"
#include "actionevent.h"
#include "monster.h"
#include "magictower.h"
#include "kingdom.h"
#include "castle.h"
#include "castleinfo.h"
#include "wellinfo.h"
#include "castleaction.h"
#include "recrutmonster.h"

ACTION ActionOverCastle(void){

    const char *message = NULL;
    const S_CASTLE *castle = GetCurrentCastle();

    if(castle->castle){ message = "Castle"; }else{ message = "Tent"; }

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    cur.x = video->w / 2;
    cur.y = video->h / 2 + 240 - BORDERWIDTH;

    cur.x = cur.x - GetLengthText(message, FONT_BIG) / 2;
    cur.y = cur.y - 3;
    cur.w = GetLengthText(message, FONT_BIG);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, message, FONT_BIG);

    return NONE;
}

ACTION ActionOverCaptain(void){

    const char *message = GetStringCapitan();

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    cur.x = video->w / 2;
    cur.y = video->h / 2 + 240 - BORDERWIDTH;

    cur.x = cur.x - GetLengthText(message, FONT_BIG) / 2;
    cur.y = cur.y - 3;
    cur.w = GetLengthText(message, FONT_BIG);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, message, FONT_BIG);

    return NONE;
}

ACTION ActionOverStatue(void){

    const char *message = GetStringStatue();

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    cur.x = video->w / 2;
    cur.y = video->h / 2 + 240 - BORDERWIDTH;

    cur.x = cur.x - GetLengthText(message, FONT_BIG) / 2;
    cur.y = cur.y - 3;
    cur.w = GetLengthText(message, FONT_BIG);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, message, FONT_BIG);

    return NONE;
}

ACTION ActionOverMarketplace(void){

    const char *message = GetStringMarketplace();

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    cur.x = video->w / 2;
    cur.y = video->h / 2 + 240 - BORDERWIDTH;

    cur.x = cur.x - GetLengthText(message, FONT_BIG) / 2;
    cur.y = cur.y - 3;
    cur.w = GetLengthText(message, FONT_BIG);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, message, FONT_BIG);

    return NONE;
}

ACTION ActionOverMoat(void){

    const char *message = GetStringMoat();

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    cur.x = video->w / 2;
    cur.y = video->h / 2 + 240 - BORDERWIDTH;

    cur.x = cur.x - GetLengthText(message, FONT_BIG) / 2;
    cur.y = cur.y - 3;
    cur.w = GetLengthText(message, FONT_BIG);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, message, FONT_BIG);

    return NONE;
}

ACTION ActionOverDock(void){

    const char *message = GetStringShipyard();

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    cur.x = video->w / 2;
    cur.y = video->h / 2 + 240 - BORDERWIDTH;

    cur.x = cur.x - GetLengthText(message, FONT_BIG) / 2;
    cur.y = cur.y - 3;
    cur.w = GetLengthText(message, FONT_BIG);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, message, FONT_BIG);

    return NONE;
}

ACTION ActionOverWell(void){

    const char *message = GetStringWell();

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    cur.x = video->w / 2;
    cur.y = video->h / 2 + 240 - BORDERWIDTH;

    cur.x = cur.x - GetLengthText(message, FONT_BIG) / 2;
    cur.y = cur.y - 3;
    cur.w = GetLengthText(message, FONT_BIG);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, message, FONT_BIG);

    return NONE;
}

ACTION ActionOverTavern(void){

    const char *message = GetStringTavern();

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    cur.x = video->w / 2;
    cur.y = video->h / 2 + 240 - BORDERWIDTH;

    cur.x = cur.x - GetLengthText(message, FONT_BIG) / 2;
    cur.y = cur.y - 3;
    cur.w = GetLengthText(message, FONT_BIG);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, message, FONT_BIG);

    return NONE;
}

ACTION ActionOverThievesGuild(void){

    const char *message = GetStringThievesGuild();

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    cur.x = video->w / 2;
    cur.y = video->h / 2 + 240 - BORDERWIDTH;

    cur.x = cur.x - GetLengthText(message, FONT_BIG) / 2;
    cur.y = cur.y - 3;
    cur.w = GetLengthText(message, FONT_BIG);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, message, FONT_BIG);

    return NONE;
}

ACTION ActionOverMageGuild(void){

    const char *message = GetStringMageGuild();

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    cur.x = video->w / 2;
    cur.y = video->h / 2 + 240 - BORDERWIDTH;

    cur.x = cur.x - GetLengthText(message, FONT_BIG) / 2;
    cur.y = cur.y - 3;
    cur.w = GetLengthText(message, FONT_BIG);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, message, FONT_BIG);

    return NONE;
}

ACTION ActionOverSpec(void){

    const char *message = NULL;
    const S_CASTLE *castle = GetCurrentCastle();

    message = GetStringSpec(castle->race);

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    cur.x = video->w / 2;
    cur.y = video->h / 2 + 240 - BORDERWIDTH;

    cur.x = cur.x - GetLengthText(message, FONT_BIG) / 2;
    cur.y = cur.y - 3;
    cur.w = GetLengthText(message, FONT_BIG);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, message, FONT_BIG);

    return NONE;
}


ACTION ActionOverWel2(void){

    const char *message = NULL;
    const S_CASTLE *castle = GetCurrentCastle();

    message = GetStringWel2(castle->race);

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    cur.x = video->w / 2;
    cur.y = video->h / 2 + 240 - BORDERWIDTH;

    cur.x = cur.x - GetLengthText(message, FONT_BIG) / 2;
    cur.y = cur.y - 3;
    cur.w = GetLengthText(message, FONT_BIG);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, message, FONT_BIG);

    return NONE;
}

ACTION ActionOverDwelling1(void){

    const S_CASTLE *castle = GetCurrentCastle();
    const S_MONSTER *monster = NULL;

    switch(castle->race){
	case BARBARIAN:
	    monster = GetStatMonster(GOBLIN);
	    break;
	case KNIGHT:
	    monster = GetStatMonster(PEASANT);
	    break;
	case WARLOCK:
	    monster = GetStatMonster(CENTAUR);
	    break;
	case SORCERESS:
	    monster = GetStatMonster(SPRITE);
	    break;
	case NECROMANCER:
	    monster = GetStatMonster(SKELETON);
	    break;
	case WIZARD:
	    monster = GetStatMonster(HALFLING);
	    break;
	default:
	    return NONE;
	    break;
    }

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    cur.x = video->w / 2;
    cur.y = video->h / 2 + 240 - BORDERWIDTH;

    cur.x = cur.x - GetLengthText(monster->descriptions, FONT_BIG) / 2;
    cur.y = cur.y - 3;
    cur.w = GetLengthText(monster->descriptions, FONT_BIG);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, monster->descriptions, FONT_BIG);

    return NONE;
}

ACTION ActionOverDwelling2(void){

    const S_CASTLE *castle = GetCurrentCastle();
    const S_MONSTER *monster = NULL;

    switch(castle->race){
	case BARBARIAN:
	    if(castle->dwelling & DWELLING_UPGRADE2)
		monster = GetStatMonster(CHIEF_ORC);
	    else
		monster = GetStatMonster(ORC);
	    break;
	case KNIGHT:
	    if(castle->dwelling & DWELLING_UPGRADE2)
		monster = GetStatMonster(ARCHER);
	    else
		monster = GetStatMonster(RANGER);
	    break;
	case WARLOCK:
	    monster = GetStatMonster(GARGOYLE);
	    break;
	case SORCERESS:
	    if(castle->dwelling & DWELLING_UPGRADE2)
		monster = GetStatMonster(DWARF);
	    else
		monster = GetStatMonster(BATTLE_DWARF);
	    break;
	case NECROMANCER:
	    if(castle->dwelling & DWELLING_UPGRADE2)
		monster = GetStatMonster(ZOMBIE);
	    else
		monster = GetStatMonster(MUTANT_ZOMBIE);
	    break;
	case WIZARD:
	    monster = GetStatMonster(BOAR);
	    break;
	default:
	    return NONE;
	    break;
    }

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    cur.x = video->w / 2;
    cur.y = video->h / 2 + 240 - BORDERWIDTH;

    cur.x = cur.x - GetLengthText(monster->descriptions, FONT_BIG) / 2;
    cur.y = cur.y - 3;
    cur.w = GetLengthText(monster->descriptions, FONT_BIG);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, monster->descriptions, FONT_BIG);

    return NONE;
}

ACTION ActionOverDwelling3(void){

    const S_CASTLE *castle = GetCurrentCastle();
    const S_MONSTER *monster = NULL;

    switch(castle->race){
	case BARBARIAN:
	    monster = GetStatMonster(WOLF);
	    break;
	case KNIGHT:
	    if(castle->dwelling & DWELLING_UPGRADE3)
		monster = GetStatMonster(PIKEMAN);
	    else
		monster = GetStatMonster(VETERAN_PIKEMAN);
	    break;
	case WARLOCK:
	    monster = GetStatMonster(GRIFFIN);
	    break;
	case SORCERESS:
	    if(castle->dwelling & DWELLING_UPGRADE3)
		monster = GetStatMonster(ELF);
	    else
		monster = GetStatMonster(GRAND_ELF);
	    break;
	case NECROMANCER:
	    if(castle->dwelling & DWELLING_UPGRADE3)
		monster = GetStatMonster(MUMMY);
	    else
		monster = GetStatMonster(ROYAL_MUMMY);
	    break;
	case WIZARD:
	    if(castle->dwelling & DWELLING_UPGRADE3)
		monster = GetStatMonster(IRON_GOLEM);
	    else
		monster = GetStatMonster(STEEL_GOLEM);
	    break;
	default:
	    return NONE;
	    break;
    }

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    cur.x = video->w / 2;
    cur.y = video->h / 2 + 240 - BORDERWIDTH;

    cur.x = cur.x - GetLengthText(monster->descriptions, FONT_BIG) / 2;
    cur.y = cur.y - 3;
    cur.w = GetLengthText(monster->descriptions, FONT_BIG);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, monster->descriptions, FONT_BIG);

    return NONE;
}

ACTION ActionOverDwelling4(void){

    const S_CASTLE *castle = GetCurrentCastle();
    const S_MONSTER *monster = NULL;

    switch(castle->race){
	case BARBARIAN:
	    if(castle->dwelling & DWELLING_UPGRADE4)
		monster = GetStatMonster(OGRE);
	    else
		monster = GetStatMonster(LORD_OGRE);
	    break;
	case KNIGHT:
	    if(castle->dwelling & DWELLING_UPGRADE4)
		monster = GetStatMonster(SWORDSMAN);
	    else
		monster = GetStatMonster(MASTER_SWORDSMAN);
	    break;
	case WARLOCK:
	    if(castle->dwelling & DWELLING_UPGRADE4)
		monster = GetStatMonster(MINOTAUR);
	    else
		monster = GetStatMonster(KNIGHT_MINOTAUR);
	    break;
	case SORCERESS:
	    if(castle->dwelling & DWELLING_UPGRADE4)
		monster = GetStatMonster(DRUID);
	    else
		monster = GetStatMonster(GREATER_DRUID);
	    break;
	case NECROMANCER:
	    if(castle->dwelling & DWELLING_UPGRADE4)
		monster = GetStatMonster(VAMPIRE);
	    else
		monster = GetStatMonster(LORD_VAMPIRE);
	    break;
	case WIZARD:
	    monster = GetStatMonster(ROC);
	    break;
	default:
	    return NONE;
	    break;
    }

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    cur.x = video->w / 2;
    cur.y = video->h / 2 + 240 - BORDERWIDTH;

    cur.x = cur.x - GetLengthText(monster->descriptions, FONT_BIG) / 2;
    cur.y = cur.y - 3;
    cur.w = GetLengthText(monster->descriptions, FONT_BIG);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, monster->descriptions, FONT_BIG);

    return NONE;
}

ACTION ActionOverDwelling5(void){

    const S_CASTLE *castle = GetCurrentCastle();
    const S_MONSTER *monster = NULL;

    switch(castle->race){
	case BARBARIAN:
	    if(castle->dwelling & DWELLING_UPGRADE5)
		monster = GetStatMonster(TROLL);
	    else
		monster = GetStatMonster(WAR_TROLL);
	    break;
	case KNIGHT:
	    if(castle->dwelling & DWELLING_UPGRADE5)
		monster = GetStatMonster(CAVALIRY);
	    else
		monster = GetStatMonster(CHAMPION);
	    break;
	case WARLOCK:
	    monster = GetStatMonster(HIDRA);
	    break;
	case SORCERESS:
	    monster = GetStatMonster(UNICORN);
	    break;
	case NECROMANCER:
	    if(castle->dwelling & DWELLING_UPGRADE5)
		monster = GetStatMonster(LICH);
	    else
		monster = GetStatMonster(POWER_LICH);
	    break;
	case WIZARD:
	    if(castle->dwelling & DWELLING_UPGRADE5)
		monster = GetStatMonster(MAGE);
	    else
		monster = GetStatMonster(ARCHMAGE);
	    break;
	default:
	    return NONE;
	    break;
    }

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    cur.x = video->w / 2;
    cur.y = video->h / 2 + 240 - BORDERWIDTH;

    cur.x = cur.x - GetLengthText(monster->descriptions, FONT_BIG) / 2;
    cur.y = cur.y - 3;
    cur.w = GetLengthText(monster->descriptions, FONT_BIG);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, monster->descriptions, FONT_BIG);

    return NONE;
}

ACTION ActionOverDwelling6(void){

    const S_CASTLE *castle = GetCurrentCastle();
    const S_MONSTER *monster = NULL;

    switch(castle->race){
	case BARBARIAN:
	    monster = GetStatMonster(CYCLOPS);
	    break;
	case KNIGHT:
	    if(castle->dwelling & DWELLING_UPGRADE6)
		monster = GetStatMonster(PALADIN);
	    else
		monster = GetStatMonster(CRUSADER);
	    break;
	case WARLOCK:
	    if(castle->dwelling & DWELLING_UPGRADE7)
		monster = GetStatMonster(BLACK_DRAGON);
	    else if(castle->dwelling & DWELLING_UPGRADE6)
		monster = GetStatMonster(RED_DRAGON);
	    else
		monster = GetStatMonster(GREEN_DRAGON);
	    break;
	case SORCERESS:
	    monster = GetStatMonster(PHOENIX);
	    break;
	case NECROMANCER:
	    monster = GetStatMonster(BONE_DRAGON);
	    break;
	case WIZARD:
	    if(castle->dwelling & DWELLING_UPGRADE6)
		monster = GetStatMonster(GIANT);
	    else
		monster = GetStatMonster(TITAN);
	    break;
	default:
	    return NONE;
	    break;
    }

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    cur.x = video->w / 2;
    cur.y = video->h / 2 + 240 - BORDERWIDTH;

    cur.x = cur.x - GetLengthText(monster->descriptions, FONT_BIG) / 2;
    cur.y = cur.y - 3;
    cur.w = GetLengthText(monster->descriptions, FONT_BIG);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, monster->descriptions, FONT_BIG);

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
    
    cur.x = cur.x - GetLengthText(message, FONT_BIG) / 2;
    cur.y = cur.y - 3;
    cur.w = GetLengthText(message, FONT_BIG);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, message, FONT_BIG);

    return NONE;
}               
                    
ACTION ActionOverHeroes(void){

    const char *message = "View Hero";

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;

    cur.x = video->w / 2;
    cur.y = video->h / 2 + 240 - BORDERWIDTH;

    cur.x = cur.x - GetLengthText(message, FONT_BIG) / 2;
    cur.y = cur.y - 3;
    cur.w = GetLengthText(message, FONT_BIG);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, message, FONT_BIG);

    return NONE;
}

ACTION ActionClickCastle(void){

    const S_CASTLE *castle = GetCurrentCastle();
    
    if(castle->castle)
	ShowCastleInfo();
    else if(castle->allowCastle)
	fprintf(stderr, "Allow build Castle\n");
    else
	fprintf(stderr, "Not allow build Castle\n");

    return NONE;
}

ACTION ActionClickCaptain(void){
    fprintf(stderr, "ClickCaptain\n");
    return NONE;
}

ACTION ActionClickMageGuild(void){

    ACTION result = ShowMageGuildInfo();

    if(EXIT != result) result = NONE;
    
    return result;
}

ACTION ActionClickStatue(void){
    fprintf(stderr, "ClickStatue\n");
    return NONE;
}

ACTION ActionClickWell(void){
    
    return ShowWellInfo();
}

ACTION ActionClickThievesGuild(void){
    fprintf(stderr, "ClickThievesGuild\n");
    return NONE;
}

ACTION ActionClickTavern(void){
    fprintf(stderr, "ClickTavern\n");
    return NONE;
}

ACTION ActionClickDock(void){
    fprintf(stderr, "ClickDock\n");
    return NONE;
}

ACTION ActionClickMarketplace(void){
    fprintf(stderr, "ClickMarketplace\n");
    return NONE;
}

ACTION ActionClickMoat(void){
    fprintf(stderr, "ClickMoat\n");
    return NONE;
}

ACTION ActionClickSpec(void){
    fprintf(stderr, "ClickSpec\n");
    return NONE;
}

ACTION ActionClickWel2(void){
    fprintf(stderr, "ClickWel2\n");
    return NONE;
}

ACTION ActionClickDwelling1(void){

    const Uint8 levelMonster = 1;

    const S_CASTLE *castle = GetCurrentCastle();
    if(! castle) return NONE;

    if(	BuyMonsterFromCastle(castle, levelMonster, 
	DialogRecrutMonster(GetMonsterFromCastle(castle, levelMonster), levelMonster, castle->monster[levelMonster - 1])) ){
	    RedrawCastleMonster();
	    RedrawCastleInfoResource();
    }else
	ErrorDialogRecrutMonster(castle);

    return NONE;
}

ACTION ActionClickDwelling2(void){

    const Uint8 levelMonster = 2;

    const S_CASTLE *castle = GetCurrentCastle();
    if(! castle) return NONE;

    if(	BuyMonsterFromCastle(castle, levelMonster, 
	DialogRecrutMonster(GetMonsterFromCastle(castle, levelMonster), levelMonster, castle->monster[levelMonster - 1])) ){
	    RedrawCastleMonster();
	    RedrawCastleInfoResource();
    }else
	ErrorDialogRecrutMonster(castle);

    return NONE;
}

ACTION ActionClickDwelling3(void){

    const Uint8 levelMonster = 3;

    const S_CASTLE *castle = GetCurrentCastle();
    if(! castle) return NONE;

    if(	BuyMonsterFromCastle(castle, levelMonster, 
	DialogRecrutMonster(GetMonsterFromCastle(castle, levelMonster), levelMonster, castle->monster[levelMonster - 1])) ){
	    RedrawCastleMonster();
	    RedrawCastleInfoResource();
    }else
	ErrorDialogRecrutMonster(castle);

    return NONE;
}

ACTION ActionClickDwelling4(void){

    const Uint8 levelMonster = 4;

    const S_CASTLE *castle = GetCurrentCastle();
    if(! castle) return NONE;

    if(	BuyMonsterFromCastle(castle, levelMonster, 
	DialogRecrutMonster(GetMonsterFromCastle(castle, levelMonster), levelMonster, castle->monster[levelMonster - 1])) ){
	    RedrawCastleMonster();
	    RedrawCastleInfoResource();
    }else
	ErrorDialogRecrutMonster(castle);

    return NONE;
}

ACTION ActionClickDwelling5(void){

    const Uint8 levelMonster = 5;

    const S_CASTLE *castle = GetCurrentCastle();
    if(! castle) return NONE;

    if(	BuyMonsterFromCastle(castle, levelMonster, 
	DialogRecrutMonster(GetMonsterFromCastle(castle, levelMonster), levelMonster, castle->monster[levelMonster - 1])) ){
	    RedrawCastleMonster();
	    RedrawCastleInfoResource();
    }else
	ErrorDialogRecrutMonster(castle);

    return NONE;
}

ACTION ActionClickDwelling6(void){

    const Uint8 levelMonster = 6;

    const S_CASTLE *castle = GetCurrentCastle();
    if(! castle) return NONE;

    if(	BuyMonsterFromCastle(castle, levelMonster, 
	DialogRecrutMonster(GetMonsterFromCastle(castle, levelMonster), levelMonster, castle->monster[levelMonster - 1])) ){
	    RedrawCastleMonster();
	    RedrawCastleInfoResource();
    }else
	ErrorDialogRecrutMonster(castle);

    return NONE;
}

ACTION ActionCastleOverExit(void){
    
    const char *message = "Exit Castle Options";

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;

    cur.x = video->w / 2;
    cur.y = video->h / 2 + 240 - BORDERWIDTH;
    
    cur.x = cur.x - GetLengthText(message, FONT_BIG) / 2;
    cur.y = cur.y - 3;
    cur.w = GetLengthText(message, FONT_BIG);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, message, FONT_BIG);
                
    return NONE;
}               

ACTION ActionCastleClickExit(void){
            
    return CANCEL;      
}

ACTION ActionCastleOverTavern(void){
    
    const char *message = NULL;
    const S_CASTLE *castle = GetCurrentCastle();
    
    switch(AllowBuildTavern(castle)){

	case BUILD_OK:
	    message = "Build Tavern";
	    break;
	
	case CANNOT_BUILD:
	    message = "Cannot build Tavern";
	    break;
	
	case ALREADY_BUILD:
	    message = "Tavern is already build";
	    break;
	
	case END_TUR:
	    message = "Cannot build. Already build here this turn.";
	    break;
    }

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    cur.x = video->w / 2;
    cur.y = video->h / 2 + 240 - BORDERWIDTH;
    
    cur.x = cur.x - GetLengthText(message, FONT_BIG) / 2;
    cur.y = cur.y - 3;
    cur.w = GetLengthText(message, FONT_BIG);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, message, FONT_BIG);

    return NONE;
}

ACTION ActionCastleOverStatue(void){
    
    const char *message = NULL;
    const S_CASTLE *castle = GetCurrentCastle();
    
    switch(AllowBuildStatue(castle)){

	case BUILD_OK:
	    message = "Build Statue";
	    break;
	
	case CANNOT_BUILD:
	    message = "Cannot build Statue";
	    break;
	
	case ALREADY_BUILD:
	    message = "Statue is already build";
	    break;
	
	case END_TUR:
	    message = "Cannot build. Already build here this turn.";
	    break;
    }

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    cur.x = video->w / 2;
    cur.y = video->h / 2 + 240 - BORDERWIDTH;
    
    cur.x = cur.x - GetLengthText(message, FONT_BIG) / 2;
    cur.y = cur.y - 3;
    cur.w = GetLengthText(message, FONT_BIG);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, message, FONT_BIG);

    return NONE;
}

ACTION ActionCastleOverWell(void){
    
    const char *message = NULL;
    const S_CASTLE *castle = GetCurrentCastle();
    
    switch(AllowBuildWell(castle)){

	case BUILD_OK:
	    message = "Build Well";
	    break;
	
	case CANNOT_BUILD:
	    message = "Cannot build Well";
	    break;
	
	case ALREADY_BUILD:
	    message = "Well is already build";
	    break;
	
	case END_TUR:
	    message = "Cannot build. Already build here this turn.";
	    break;
    }

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    cur.x = video->w / 2;
    cur.y = video->h / 2 + 240 - BORDERWIDTH;
    
    cur.x = cur.x - GetLengthText(message, FONT_BIG) / 2;
    cur.y = cur.y - 3;
    cur.w = GetLengthText(message, FONT_BIG);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, message, FONT_BIG);

    return NONE;
}

ACTION ActionCastleOverMoat(void){
    
    const char *message = NULL;
    const S_CASTLE *castle = GetCurrentCastle();
    
    switch(AllowBuildMoat(castle)){

	case BUILD_OK:
	    message = "Build Moat";
	    break;
	
	case CANNOT_BUILD:
	    message = "Cannot build Moat";
	    break;
	
	case ALREADY_BUILD:
	    message = "Moat is already build";
	    break;
	
	case END_TUR:
	    message = "Cannot build. Already build here this turn.";
	    break;
    }

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    cur.x = video->w / 2;
    cur.y = video->h / 2 + 240 - BORDERWIDTH;
    
    cur.x = cur.x - GetLengthText(message, FONT_BIG) / 2;
    cur.y = cur.y - 3;
    cur.w = GetLengthText(message, FONT_BIG);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, message, FONT_BIG);

    return NONE;
}

ACTION ActionCastleOverMarketplace(void){
    
    const char *message = NULL;
    const S_CASTLE *castle = GetCurrentCastle();
    
    switch(AllowBuildMarketplace(castle)){

	case BUILD_OK:
	    message = "Build Marketplace";
	    break;
	
	case CANNOT_BUILD:
	    message = "Cannot build Marketplace";
	    break;
	
	case ALREADY_BUILD:
	    message = "Marketplace is already build";
	    break;
	
	case END_TUR:
	    message = "Cannot build. Already build here this turn.";
	    break;
    }

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    cur.x = video->w / 2;
    cur.y = video->h / 2 + 240 - BORDERWIDTH;
    
    cur.x = cur.x - GetLengthText(message, FONT_BIG) / 2;
    cur.y = cur.y - 3;
    cur.w = GetLengthText(message, FONT_BIG);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, message, FONT_BIG);

    return NONE;
}

ACTION ActionCastleOverThievesGuild(void){
    
    const char *message = NULL;
    const S_CASTLE *castle = GetCurrentCastle();
    
    switch(AllowBuildThievesGuild(castle)){

	case BUILD_OK:
	    message = "Build Thieves Guild";
	    break;
	
	case CANNOT_BUILD:
	    message = "Cannot build Thieves Guild";
	    break;
	
	case ALREADY_BUILD:
	    message = "Thieves Guild is already  build";
	    break;
	
	case END_TUR:
	    message = "Cannot build. Already build here this turn.";
	    break;
    }

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    cur.x = video->w / 2;
    cur.y = video->h / 2 + 240 - BORDERWIDTH;
    
    cur.x = cur.x - GetLengthText(message, FONT_BIG) / 2;
    cur.y = cur.y - 3;
    cur.w = GetLengthText(message, FONT_BIG);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, message, FONT_BIG);

    return NONE;
}

ACTION ActionCastleOverLeftTurret(void){
    
    const char *message = NULL;
    const S_CASTLE *castle = GetCurrentCastle();
    
    switch(AllowBuildLeftTurret(castle)){

	case BUILD_OK:
	    message = "Build Left Turret";
	    break;
	
	case CANNOT_BUILD:
	    message = "Cannot build Left Turret";
	    break;
	
	case ALREADY_BUILD:
	    message = "Left Turret is already build";
	    break;
	
	case END_TUR:
	    message = "Cannot build. Already build here this turn.";
	    break;
    }

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    cur.x = video->w / 2;
    cur.y = video->h / 2 + 240 - BORDERWIDTH;
    
    cur.x = cur.x - GetLengthText(message, FONT_BIG) / 2;
    cur.y = cur.y - 3;
    cur.w = GetLengthText(message, FONT_BIG);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, message, FONT_BIG);

    return NONE;
}

ACTION ActionCastleOverRightTurret(void){
    
    const char *message = NULL;
    const S_CASTLE *castle = GetCurrentCastle();
    
    switch(AllowBuildRightTurret(castle)){

	case BUILD_OK:
	    message = "Build Right Turret";
	    break;
	
	case CANNOT_BUILD:
	    message = "Cannot build Right Turret";
	    break;
	
	case ALREADY_BUILD:
	    message = "Right Turret is already build";
	    break;
	
	case END_TUR:
	    message = "Cannot build. Already build here this turn.";
	    break;
    }

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    cur.x = video->w / 2;
    cur.y = video->h / 2 + 240 - BORDERWIDTH;
    
    cur.x = cur.x - GetLengthText(message, FONT_BIG) / 2;
    cur.y = cur.y - 3;
    cur.w = GetLengthText(message, FONT_BIG);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, message, FONT_BIG);

    return NONE;
}

ACTION ActionCastleOverMageGuild(void){
    
    const char *message = NULL;
    const S_CASTLE *castle = GetCurrentCastle();
    
    switch(AllowBuildMageGuild(castle)){

	case BUILD_OK:
	    message = "Build Mage Guild";
	    break;
	
	case CANNOT_BUILD:
	    message = "Cannot build Mage Guild";
	    break;
	
	case ALREADY_BUILD:
	    message = "Mage Guild is already build";
	    break;
	
	case END_TUR:
	    message = "Cannot build. Already build here this turn.";
	    break;
    }

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    cur.x = video->w / 2;
    cur.y = video->h / 2 + 240 - BORDERWIDTH;
    
    cur.x = cur.x - GetLengthText(message, FONT_BIG) / 2;
    cur.y = cur.y - 3;
    cur.w = GetLengthText(message, FONT_BIG);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, message, FONT_BIG);

    return NONE;
}

ACTION ActionCastleOverShipyard(void){
    
    const char *message = NULL;
    const S_CASTLE *castle = GetCurrentCastle();
    
    switch(AllowBuildShipyard(castle)){

	case BUILD_OK:
	    message = "Build Shipyard";
	    break;
	
	case CANNOT_BUILD:
	    message = "Cannot build Shipyard";
	    break;
	
	case ALREADY_BUILD:
	    message = "Shipyard is already build";
	    break;
	
	case END_TUR:
	    message = "Cannot build. Already build here this turn.";
	    break;
    }

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    cur.x = video->w / 2;
    cur.y = video->h / 2 + 240 - BORDERWIDTH;
    
    cur.x = cur.x - GetLengthText(message, FONT_BIG) / 2;
    cur.y = cur.y - 3;
    cur.w = GetLengthText(message, FONT_BIG);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, message, FONT_BIG);

    return NONE;
}

ACTION ActionCastleOverWel2(void){
    
    char message[64];
    const S_CASTLE *castle = GetCurrentCastle();
    
    switch(AllowBuildWel2(castle)){

	case BUILD_OK:
	    sprintf(message, "Build %s", GetStringWel2(castle->race));
	    break;
	
	case CANNOT_BUILD:
	    sprintf(message, "Cannot build %s", GetStringWel2(castle->race));
	    break;
	
	case ALREADY_BUILD:
	    sprintf(message, "%s is already build", GetStringWel2(castle->race));
	    break;
	
	case END_TUR:
	    sprintf(message, "Cannot build. Already build here this turn.");
	    break;
    }

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    cur.x = video->w / 2;
    cur.y = video->h / 2 + 240 - BORDERWIDTH;
    
    cur.x = cur.x - GetLengthText(message, FONT_BIG) / 2;
    cur.y = cur.y - 3;
    cur.w = GetLengthText(message, FONT_BIG);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, message, FONT_BIG);

    return NONE;
}

ACTION ActionCastleOverSpec(void){
    
    char message[64];
    const S_CASTLE *castle = GetCurrentCastle();
    
    switch(AllowBuildSpec(castle)){

	case BUILD_OK:
	    sprintf(message, "Build %s", GetStringSpec(castle->race));
	    break;
	
	case CANNOT_BUILD:
	    sprintf(message, "Cannot build %s", GetStringSpec(castle->race));
	    break;
	
	case ALREADY_BUILD:
	    sprintf(message, "%s is already build", GetStringSpec(castle->race));
	    break;
	
	case END_TUR:
	    sprintf(message, "Cannot build. Already build here this turn.");
	    break;
    }

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    cur.x = video->w / 2;
    cur.y = video->h / 2 + 240 - BORDERWIDTH;
    
    cur.x = cur.x - GetLengthText(message, FONT_BIG) / 2;
    cur.y = cur.y - 3;
    cur.w = GetLengthText(message, FONT_BIG);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, message, FONT_BIG);

    return NONE;
}

ACTION ActionCastleOverDwelling1(void){
    
    char message[64];
    const S_CASTLE *castle = GetCurrentCastle();
    
    switch(AllowBuildDwelling1(castle)){

	case BUILD_OK:
	    sprintf(message, "Build %s", GetStringDwelling(castle->race, DWELLING_MONSTER1));
	    break;
	
	case CANNOT_BUILD:
	    sprintf(message, "Cannot build %s", GetStringDwelling(castle->race, DWELLING_MONSTER1));
	    break;
	
	case ALREADY_BUILD:
	    sprintf(message, "%s is already build", GetStringDwelling(castle->race, DWELLING_MONSTER1));
	    break;
	
	case END_TUR:
	    sprintf(message, "Cannot build. Already build here this turn.");
	    break;
    }

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    cur.x = video->w / 2;
    cur.y = video->h / 2 + 240 - BORDERWIDTH;
    
    cur.x = cur.x - GetLengthText(message, FONT_BIG) / 2;
    cur.y = cur.y - 3;
    cur.w = GetLengthText(message, FONT_BIG);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, message, FONT_BIG);

    return NONE;
}

ACTION ActionCastleOverDwelling2(void){
    
    char message[64];
    const S_CASTLE *castle = GetCurrentCastle();
    
    
    switch(AllowBuildDwelling2(castle)){

	case BUILD_OK:
	    castle->dwelling & DWELLING_MONSTER2 ? sprintf(message, "Build %s", GetStringDwelling(castle->race, DWELLING_UPGRADE2)) :
		sprintf(message, "Build %s", GetStringDwelling(castle->race, DWELLING_MONSTER2));
	    break;

	case CANNOT_BUILD:
	    castle->dwelling & DWELLING_MONSTER2 ? sprintf(message, "Cannot build %s", GetStringDwelling(castle->race, DWELLING_UPGRADE2)) :
		sprintf(message, "Cannot build %s", GetStringDwelling(castle->race, DWELLING_MONSTER2));
	    break;

	case ALREADY_BUILD:
	    if(castle->dwelling & DWELLING_UPGRADE2)
		sprintf(message, "%s is already build", GetStringDwelling(castle->race, DWELLING_UPGRADE2));
	    else
		sprintf(message, "%s is already build", GetStringDwelling(castle->race, DWELLING_MONSTER2));
	    break;

	case END_TUR:
	    sprintf(message, "Cannot build. Already build here this turn.");
	    break;
    }

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    cur.x = video->w / 2;
    cur.y = video->h / 2 + 240 - BORDERWIDTH;
    
    cur.x = cur.x - GetLengthText(message, FONT_BIG) / 2;
    cur.y = cur.y - 3;
    cur.w = GetLengthText(message, FONT_BIG);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, message, FONT_BIG);

    return NONE;
}

ACTION ActionCastleOverDwelling3(void){
    
    char message[64];
    const S_CASTLE *castle = GetCurrentCastle();
    
    switch(AllowBuildDwelling3(castle)){

	case BUILD_OK:
	    castle->dwelling & DWELLING_MONSTER3 ? sprintf(message, "Build %s", GetStringDwelling(castle->race, DWELLING_UPGRADE3)) :
		sprintf(message, "Build %s", GetStringDwelling(castle->race, DWELLING_MONSTER3));
	    break;
	
	case CANNOT_BUILD:
	    castle->dwelling & DWELLING_MONSTER3 ? sprintf(message, "Cannot build %s", GetStringDwelling(castle->race, DWELLING_UPGRADE3)) :
		sprintf(message, "Cannot build %s", GetStringDwelling(castle->race, DWELLING_MONSTER3));
	    break;
	
	case ALREADY_BUILD:
	    if(castle->dwelling & DWELLING_UPGRADE3)
		sprintf(message, "%s is already build", GetStringDwelling(castle->race, DWELLING_UPGRADE3));
	    else
		sprintf(message, "%s is already build", GetStringDwelling(castle->race, DWELLING_MONSTER3));
	    break;
	
	case END_TUR:
	    sprintf(message, "Cannot build. Already build here this turn.");
	    break;
    }

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    cur.x = video->w / 2;
    cur.y = video->h / 2 + 240 - BORDERWIDTH;
    
    cur.x = cur.x - GetLengthText(message, FONT_BIG) / 2;
    cur.y = cur.y - 3;
    cur.w = GetLengthText(message, FONT_BIG);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, message, FONT_BIG);

    return NONE;
}

ACTION ActionCastleOverDwelling4(void){
    
    char message[64];
    const S_CASTLE *castle = GetCurrentCastle();
    
    switch(AllowBuildDwelling4(castle)){

	case BUILD_OK:
	    castle->dwelling & DWELLING_MONSTER4 ? sprintf(message, "Build %s", GetStringDwelling(castle->race, DWELLING_UPGRADE4)) :
		sprintf(message, "Build %s", GetStringDwelling(castle->race, DWELLING_MONSTER4));
	    break;
	
	case CANNOT_BUILD:
	    castle->dwelling & DWELLING_MONSTER4 ? sprintf(message, "Cannot build %s", GetStringDwelling(castle->race, DWELLING_UPGRADE4)) :
		sprintf(message, "Cannot build %s", GetStringDwelling(castle->race, DWELLING_MONSTER4));
	    break;
	
	case ALREADY_BUILD:
	    if(castle->dwelling & DWELLING_UPGRADE4)
		sprintf(message, "%s is already build", GetStringDwelling(castle->race, DWELLING_UPGRADE4));
	    else
		sprintf(message, "%s is already build", GetStringDwelling(castle->race, DWELLING_MONSTER4));
	    break;
	
	case END_TUR:
	    sprintf(message, "Cannot build. Already build here this turn.");
	    break;
    }

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    cur.x = video->w / 2;
    cur.y = video->h / 2 + 240 - BORDERWIDTH;
    
    cur.x = cur.x - GetLengthText(message, FONT_BIG) / 2;
    cur.y = cur.y - 3;
    cur.w = GetLengthText(message, FONT_BIG);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, message, FONT_BIG);

    return NONE;
}

ACTION ActionCastleOverDwelling5(void){
    
    char message[64];
    const S_CASTLE *castle = GetCurrentCastle();
    
    switch(AllowBuildDwelling5(castle)){

	case BUILD_OK:
	    castle->dwelling & DWELLING_MONSTER5 ? sprintf(message, "Build %s", GetStringDwelling(castle->race, DWELLING_UPGRADE5)) :
		sprintf(message, "Build %s", GetStringDwelling(castle->race, DWELLING_MONSTER5));
	    break;
	
	case CANNOT_BUILD:
	    castle->dwelling & DWELLING_MONSTER5 ? sprintf(message, "Cannot build %s", GetStringDwelling(castle->race, DWELLING_UPGRADE5)) :
		sprintf(message, "Cannot build %s", GetStringDwelling(castle->race, DWELLING_MONSTER5));
	    break;
	
	case ALREADY_BUILD:
	    if(castle->dwelling & DWELLING_UPGRADE5)
		sprintf(message, "%s is already build", GetStringDwelling(castle->race, DWELLING_UPGRADE5));
	    else
		sprintf(message, "%s is already build", GetStringDwelling(castle->race, DWELLING_MONSTER5));
	    break;
	
	case END_TUR:
	    sprintf(message, "Cannot build. Already build here this turn.");
	    break;
    }

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    cur.x = video->w / 2;
    cur.y = video->h / 2 + 240 - BORDERWIDTH;
    
    cur.x = cur.x - GetLengthText(message, FONT_BIG) / 2;
    cur.y = cur.y - 3;
    cur.w = GetLengthText(message, FONT_BIG);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, message, FONT_BIG);

    return NONE;
}

ACTION ActionCastleOverDwelling6(void){
    
    char message[64];
    const S_CASTLE *castle = GetCurrentCastle();
    
    switch(AllowBuildDwelling6(castle)){

	case BUILD_OK:
	    if(castle->race == WARLOCK && castle->dwelling & DWELLING_UPGRADE6)
		sprintf(message, "Build %s", GetStringDwelling(castle->race, DWELLING_UPGRADE7));
	    else if(castle->dwelling & DWELLING_MONSTER6)
		sprintf(message, "Build %s", GetStringDwelling(castle->race, DWELLING_UPGRADE6));
	    else
		sprintf(message, "Build %s", GetStringDwelling(castle->race, DWELLING_MONSTER6));
	    break;
	
	case CANNOT_BUILD:
	    if(castle->race == WARLOCK && castle->dwelling & DWELLING_UPGRADE6)
		sprintf(message, "Cannot build %s", GetStringDwelling(castle->race, DWELLING_UPGRADE7));
	    else if(castle->dwelling & DWELLING_MONSTER6)
		sprintf(message, "Cannot build %s", GetStringDwelling(castle->race, DWELLING_UPGRADE6));
	    else
		sprintf(message, "Cannot build %s", GetStringDwelling(castle->race, DWELLING_MONSTER6));
	    break;
	
	case ALREADY_BUILD:
	    if(castle->dwelling & DWELLING_UPGRADE7)
		sprintf(message, "%s is already build", GetStringDwelling(castle->race, DWELLING_UPGRADE7));
	    else if(castle->dwelling & DWELLING_UPGRADE6)
		sprintf(message, "%s is already build", GetStringDwelling(castle->race, DWELLING_UPGRADE6));
	    else if(castle->dwelling & DWELLING_MONSTER6)
		sprintf(message, "%s is already build", GetStringDwelling(castle->race, DWELLING_MONSTER6));
	    break;
	
	case END_TUR:
	    sprintf(message, "Cannot build. Already build here this turn.");
	    break;
    }

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    cur.x = video->w / 2;
    cur.y = video->h / 2 + 240 - BORDERWIDTH;
    
    cur.x = cur.x - GetLengthText(message, FONT_BIG) / 2;
    cur.y = cur.y - 3;
    cur.w = GetLengthText(message, FONT_BIG);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, message, FONT_BIG);

    return NONE;
}

ACTION ActionCastleOverCaptain(void){
    
    const char *message = NULL;
    const S_CASTLE *castle = GetCurrentCastle();

    switch(AllowBuildCaptain(castle)){

	case BUILD_OK:
	    message = "Build Captains Quarters";
	    break;
	
	case CANNOT_BUILD:
	    message = "Cannot build Captains Quarters";
	    break;
	
	case ALREADY_BUILD:
	    message = "Captains Quarters is already build";
	    break;
	
	case END_TUR:
	    message = "Cannot build. Already build here this turn.";
	    break;
    }

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    cur.x = video->w / 2;
    cur.y = video->h / 2 + 240 - BORDERWIDTH;
    
    cur.x = cur.x - GetLengthText(message, FONT_BIG) / 2;
    cur.y = cur.y - 3;
    cur.w = GetLengthText(message, FONT_BIG);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, message, FONT_BIG);

    return NONE;
}

ACTION ActionCastleClickTavern(void){

    const S_CASTLE *castle = GetCurrentCastle();

    if(YES == MessageBox("Build Tavern ?", FONT_BIG)){

	BuildTavern(castle);
	EnableCastleUpdateBuilding();

	return CANCEL;
    }

    return NONE;
}

ACTION ActionCastleClickStatue(void){

    const S_CASTLE *castle = GetCurrentCastle();

    if(YES == MessageBox("Build Statue ?", FONT_BIG)){

	BuildStatue(castle);
	EnableCastleUpdateBuilding();

	return CANCEL;
    }

    return NONE;
}

ACTION ActionCastleClickWell(void){

    const S_CASTLE *castle = GetCurrentCastle();

    if(YES == MessageBox("Build Well ?", FONT_BIG)){

	BuildWell(castle);
	EnableCastleUpdateBuilding();

	return CANCEL;
    }

    return NONE;
}

ACTION ActionCastleClickMoat(void){

    const S_CASTLE *castle = GetCurrentCastle();

    if(YES == MessageBox("Build Moat ?", FONT_BIG)){

	BuildMoat(castle);
	EnableCastleUpdateBuilding();

	return CANCEL;
    }

    return NONE;
}

ACTION ActionCastleClickMarketplace(void){

    const S_CASTLE *castle = GetCurrentCastle();

    if(YES == MessageBox("Build Marketplace ?", FONT_BIG)){

	BuildMarketplace(castle);
	EnableCastleUpdateBuilding();

	return CANCEL;
    }

    return NONE;
}

ACTION ActionCastleClickThievesGuild(void){

    const S_CASTLE *castle = GetCurrentCastle();

    if(YES == MessageBox("Build Thieves Guild ?", FONT_BIG)){

	BuildThievesGuild(castle);
	EnableCastleUpdateBuilding();

	return CANCEL;
    }

    return NONE;
}

ACTION ActionCastleClickLeftTurret(void){

    const S_CASTLE *castle = GetCurrentCastle();

    if(YES == MessageBox("Build Left Turret ?", FONT_BIG)){

	BuildLeftTurret(castle);
	EnableCastleUpdateBuilding();

	return CANCEL;
    }

    return NONE;
}

ACTION ActionCastleClickRightTurret(void){

    const S_CASTLE *castle = GetCurrentCastle();

    if(YES == MessageBox("Build Right Turret ?", FONT_BIG)){

	BuildRightTurret(castle);
	EnableCastleUpdateBuilding();

	return CANCEL;
    }

    return NONE;
}

ACTION ActionCastleClickMageGuild(void){

    const S_CASTLE *castle = GetCurrentCastle();

    char message[32];
    sprintf(message, "Build Mage Guild Level %d ?", castle->mageGuild.level + 1);
    
    if(YES == MessageBox(message, FONT_BIG)){

	BuildMageGuild(castle);
	EnableCastleUpdateBuilding();

	return CANCEL;
    }

    return NONE;
}

ACTION ActionCastleClickShipyard(void){

    const S_CASTLE *castle = GetCurrentCastle();

    if(YES == MessageBox("Build Shipyard ?", FONT_BIG)){

	BuildShipyard(castle);
	EnableCastleUpdateBuilding();

	return CANCEL;
    }

    return NONE;
}

ACTION ActionCastleClickWel2(void){

    const S_CASTLE *castle = GetCurrentCastle();

    char message[32];
    sprintf(message, "Build %s ?", GetStringWel2(castle->race));
    
    if(YES == MessageBox(message, FONT_BIG)){

	BuildWel2(castle);
	EnableCastleUpdateBuilding();

	return CANCEL;
    }

    return NONE;
}

ACTION ActionCastleClickSpec(void){

    const S_CASTLE *castle = GetCurrentCastle();

    char message[32];
    sprintf(message, "Build %s ?", GetStringSpec(castle->race));
    
    if(YES == MessageBox(message, FONT_BIG)){

	BuildSpec(castle);
	EnableCastleUpdateBuilding();

	return CANCEL;
    }

    return NONE;
}

ACTION ActionCastleClickDwelling1(void){

    const S_CASTLE *castle = GetCurrentCastle();

    char message[32];

    sprintf(message, "Build %s ?", GetStringDwelling(castle->race, DWELLING_MONSTER1));

    if(YES == MessageBox(message, FONT_BIG)){

	BuildDwelling1(castle);
	EnableCastleUpdateBuilding();

	return CANCEL;
    }

    return NONE;
}

ACTION ActionCastleClickDwelling2(void){

    const S_CASTLE *castle = GetCurrentCastle();

    char message[32];

    if(castle->dwelling & DWELLING_MONSTER2 && CastleDwellingUpgradable(castle, DWELLING_UPGRADE2))
	sprintf(message, "Build %s ?", GetStringDwelling(castle->race, DWELLING_UPGRADE2));
    else
	sprintf(message, "Build %s ?", GetStringDwelling(castle->race, DWELLING_MONSTER2));

    if(YES == MessageBox(message, FONT_BIG)){

	if(castle->dwelling & DWELLING_MONSTER2 && CastleDwellingUpgradable(castle, DWELLING_UPGRADE2))
	    BuildUpgrade2(castle);
	else
	    BuildDwelling2(castle);

	EnableCastleUpdateBuilding();

	return CANCEL;
    }

    return NONE;
}

ACTION ActionCastleClickDwelling3(void){

    const S_CASTLE *castle = GetCurrentCastle();

    char message[32];

    if(castle->dwelling & DWELLING_MONSTER3 && CastleDwellingUpgradable(castle, DWELLING_UPGRADE3))
	sprintf(message, "Build %s ?", GetStringDwelling(castle->race, DWELLING_UPGRADE3));
    else
	sprintf(message, "Build %s ?", GetStringDwelling(castle->race, DWELLING_MONSTER3));

    if(YES == MessageBox(message, FONT_BIG)){

	if(castle->dwelling & DWELLING_MONSTER3 && CastleDwellingUpgradable(castle, DWELLING_UPGRADE3))
	    BuildUpgrade3(castle);
	else
	    BuildDwelling3(castle);

	EnableCastleUpdateBuilding();

	return CANCEL;
    }

    return NONE;
}

ACTION ActionCastleClickDwelling4(void){

    const S_CASTLE *castle = GetCurrentCastle();

    char message[32];

    if(castle->dwelling & DWELLING_MONSTER4 && CastleDwellingUpgradable(castle, DWELLING_UPGRADE4))
	sprintf(message, "Build %s ?", GetStringDwelling(castle->race, DWELLING_UPGRADE4));
    else
	sprintf(message, "Build %s ?", GetStringDwelling(castle->race, DWELLING_MONSTER4));

    if(YES == MessageBox(message, FONT_BIG)){

	if(castle->dwelling & DWELLING_MONSTER4 && CastleDwellingUpgradable(castle, DWELLING_UPGRADE4))
	    BuildUpgrade4(castle);
	else
	    BuildDwelling4(castle);

	EnableCastleUpdateBuilding();

	return CANCEL;
    }

    return NONE;
}

ACTION ActionCastleClickDwelling5(void){

    const S_CASTLE *castle = GetCurrentCastle();

    char message[32];

    if(castle->dwelling & DWELLING_MONSTER5 && CastleDwellingUpgradable(castle, DWELLING_UPGRADE5))
	sprintf(message, "Build %s ?", GetStringDwelling(castle->race, DWELLING_UPGRADE5));
    else
	sprintf(message, "Build %s ?", GetStringDwelling(castle->race, DWELLING_MONSTER5));

    if(YES == MessageBox(message, FONT_BIG)){

	if(castle->dwelling & DWELLING_MONSTER5 && CastleDwellingUpgradable(castle, DWELLING_UPGRADE5))
	    BuildUpgrade5(castle);
	else
	    BuildDwelling5(castle);

	EnableCastleUpdateBuilding();

	return CANCEL;
    }

    return NONE;
}

ACTION ActionCastleClickDwelling6(void){

    const S_CASTLE *castle = GetCurrentCastle();

    char message[32];

    if(castle->dwelling & DWELLING_UPGRADE6 && CastleDwellingUpgradable(castle, DWELLING_UPGRADE7))
	sprintf(message, "Build %s ?", GetStringDwelling(castle->race, DWELLING_UPGRADE7));
    else if(castle->dwelling & DWELLING_MONSTER6 && CastleDwellingUpgradable(castle, DWELLING_UPGRADE6))
	sprintf(message, "Build %s ?", GetStringDwelling(castle->race, DWELLING_UPGRADE6));
    else
	sprintf(message, "Build %s ?", GetStringDwelling(castle->race, DWELLING_MONSTER6));

    if(YES == MessageBox(message, FONT_BIG)){

	if(castle->dwelling & DWELLING_UPGRADE6 && CastleDwellingUpgradable(castle, DWELLING_UPGRADE7))
	    BuildUpgrade7(castle);
	else if(castle->dwelling & DWELLING_MONSTER6 && CastleDwellingUpgradable(castle, DWELLING_UPGRADE6))
	    BuildUpgrade6(castle);
	else
	    BuildDwelling6(castle);

	EnableCastleUpdateBuilding();

	return CANCEL;
    }
    return NONE;
}

ACTION ActionCastleClickCaptain(void){

    //const S_CASTLE *castle = GetCurrentCastle();
    //BuildingMessageBox(castle->race, BUILD_CAPTAIN, "The Captain's Quarters provides a captain to assist in the castle's defense when no hero is present.", BUILD_CAPTAIN_GOLD, 0, 0, 0, 0, 0, 0);
    return NONE;
}

ACTION BuildingMessageBox(E_RACE race, E_BUILDINGCASTLE build, const char *text, Uint16 gold, Uint8 ore, Uint8 wood, Uint8 mercury, Uint8 crystal, Uint8 sulfur, Uint8 gems){

    CursorOff();
    SetIntValue(ANIM2, FALSE);
    SetIntValue(ANIM3, FALSE);
    
    Uint32 cursor = GetCursor();
    SDL_Surface *back, *image, *video;
    SDL_Rect rectBack, rectCur;
    ACTION result;
    //INTERFACEACTION action;
    INTERFACEACTION *dialog = NULL;        
    AGGSPRITE sprite;
    Uint8 i;
    
    const char *buybuild = "BUYBUILD.ICN";
    const char *system = "SYSTEM.ICN";
    if(GetIntValue(EVILINTERFACE)){
	buybuild = "BUYBUILE.ICN";
	system = "SYSTEME.ICN";
    }
    
    Uint16 height = 0;
    Uint16 width = 0;
    Uint8 count = 0;
    Uint16 max = 0;
    // получаем левый верхний спрайт
    FillSPRITE(&sprite, buybuild, 4);
    image = GetICNSprite(&sprite);
    width += image->w;
    // получаем правый верхний спрайт
    FillSPRITE(&sprite, buybuild, 0);
    width += image->w;
    // вычисляем динамически высоту диалога
	max += 57;	// высота картинки
	max += 10;	// отступ
	max += FONT_HEIGHTBIG * (GetLengthText(text, FONT_BIG) / (width - 30)) + 1;
	max += 10;	// отступ
    if(gold) count++;
    if(ore) count++;
    if(wood) count++;
    if(mercury) count++;
    if(crystal) count++;
    if(sulfur) count++;
    if(gems) count++;
    if(count > 4) max += 80;	// две строки с ресурсом
    else max += 40;

    // получаем левый средний спрайт
    FillSPRITE(&sprite, buybuild, 5);
    image = GetICNSprite(&sprite);
    // количество средних блоков
    count = (max - height) / image->h + 1;
    height = height + count * image->h;

    // отрисовка диалога по центру экрана
    video = SDL_GetVideoSurface();
    rectBack.w = width;
    rectBack.h = height;
    // поправка на верхний и нижний
    FillSPRITE(&sprite, buybuild, 4);
    image = GetICNSprite(&sprite);
    rectBack.h += image->h;
    FillSPRITE(&sprite, buybuild, 6);
    image = GetICNSprite(&sprite);
    rectBack.h += image->h;
    rectBack.x = (video->w - rectBack.w) / 2;
    rectBack.y = (video->h - rectBack.h) / 2;

    // сохраняем бакгроунд
    if(NULL == (back = SDL_CreateRGBSurface(SDL_SWSURFACE, rectBack.w, rectBack.h, 16, 0, 0, 0, 0))){
	fprintf(stderr, "MessageBox: CreateRGBSurface failed: %s\n", SDL_GetError());
	return EXIT;
    }    

    SDL_BlitSurface(video, &rectBack, back, NULL);

    // получаем левый верхний спрайт
    FillSPRITE(&sprite, buybuild, 4);
    image = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + 1;
    rectCur.y = rectBack.y;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // получаем левый средний спрайт
    FillSPRITE(&sprite, buybuild, 5);
    image = GetICNSprite(&sprite);
    rectCur.x = rectBack.x;
    rectCur.w = image->w;
    rectCur.y += rectCur.h;
    rectCur.h = image->h;
    for(i = 0; i < count; ++i){
	SDL_BlitSurface(image, NULL, video, &rectCur);
	rectCur.y += rectCur.h;
    }

    // получаем левый нижний спрайт
    FillSPRITE(&sprite, buybuild, 6);
    image = GetICNSprite(&sprite);
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // получаем правый верхний спрайт
    FillSPRITE(&sprite, buybuild, 0);
    image = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + rectCur.w;
    rectCur.y = rectBack.y;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // получаем правый средний спрайт
    FillSPRITE(&sprite, buybuild, 1);
    image = GetICNSprite(&sprite);
    rectCur.w = image->w;
    rectCur.y += rectCur.h;
    rectCur.h = image->h;
    for(i = 0; i < count; ++i){
	SDL_BlitSurface(image, NULL, video, &rectCur);
	rectCur.y += rectCur.h;
    }

    // получаем правый нижний спрайт
    FillSPRITE(&sprite, buybuild, 2);
    image = GetICNSprite(&sprite);
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);

/*
    // кнопка YES
    FillSPRITE(&sprite, system, 5);
    image = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + 40;
    rectCur.y = rectBack.y + rectBack.h - 70;
    if(GetIntValue(EVILINTERFACE))
        rectCur.y = rectBack.y + rectBack.h - 85;
    rectCur.w = image->w;
    rectCur.h = image->h;
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, system, 5);
    FillSPRITE(&action.objectPush, system, 6);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = DialogPressYES;
    AddActionEvent(&dialog, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // кнопка NO
    FillSPRITE(&sprite, system, 7);
    image = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + rectBack.w - image->w - 25;
    if(GetIntValue(EVILINTERFACE))
	rectCur.y = rectBack.y + rectBack.h - 85;
    rectCur.w = image->w;
    rectCur.h = image->h;
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, system, 7);
    FillSPRITE(&action.objectPush, system, 8);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = DialogPressNO;
    AddActionEvent(&dialog, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);
*/
    // Отрисовка диалога
    SDL_Flip(video);

    SetCursor(CURSOR_POINTER);

    CursorOn();

    // цикл событий
    result = ActionCycle(dialog);
                                    
    // востанавливаем бакгроунд
    CursorOff();
    SDL_BlitSurface(back, NULL, video, &rectBack);
    //SDL_Flip(video);

    FreeActionEvent(dialog);
    SDL_FreeSurface(back);

    SetCursor(cursor);

    SetIntValue(ANIM2, TRUE);
    SetIntValue(ANIM3, TRUE);
    CursorOn();

    return result;
}
