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
#include "heroesinfo.h"
#include "monster.h"
#include "magictower.h"
#include "thievesguild.h"
#include "kingdom.h"
#include "castle.h"
#include "castleinfo.h"
#include "marketplace.h"
#include "wellinfo.h"
#include "taverninfo.h"
#include "castleaction.h"
#include "castledialog.h"
#include "recrutmonster.h"

ACTION ActionOverCastle(void){

    const char *message = NULL;
    const S_CASTLE *castle = GetCurrentCastle();

    if(castle->building & BUILD_CASTLE){ message = "Castle"; }else{ message = "Tent"; }

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

    const char *message = GetStringBuilding(KNIGHT, BUILD_CAPTAIN);

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

    const char *message = GetStringBuilding(KNIGHT, BUILD_STATUE);

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

    const char *message = GetStringBuilding(KNIGHT, BUILD_MARKETPLACE);

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

    const char *message = GetStringBuilding(KNIGHT, BUILD_MOAT);

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

    const char *message = GetStringBuilding(KNIGHT, BUILD_SHIPYARD);

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

    const char *message = GetStringBuilding(KNIGHT, BUILD_WELL);

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

    const char *message = GetStringBuilding(KNIGHT, BUILD_TAVERN);

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

    const char *message = GetStringBuilding(KNIGHT, BUILD_THIEVESGUILD);

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

    const char *message = GetStringBuilding(KNIGHT, BUILD_MAGEGUILD1);

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

    message = GetStringBuilding(castle->race, BUILD_SPEC);

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

    message = GetStringBuilding(castle->race, BUILD_WEL2);

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
                
    if(castle->building & BUILD_CASTLE){ message = "Exit Castle"; }else{ message = "Exit Town"; }
    
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
    
    if(castle->building & BUILD_CASTLE)
	ShowCastleInfo();
    else if(castle->allowCastle)
	fprintf(stderr, "Allow build Castle\n");
    else
	fprintf(stderr, "Not allow build Castle\n");

    return NONE;
}

ACTION ActionClickCaptain(void){

    return BuildingInfoYESBox(BUILD_CAPTAIN);
}

ACTION ActionClickMageGuild(void){

    ACTION result = ShowMageGuildInfo();

    if(EXIT != result) result = NONE;
    
    return result;
}

ACTION ActionClickStatue(void){

    return BuildingInfoYESBox(BUILD_STATUE);
}

ACTION ActionClickWell(void){
    
    return ShowWellInfo();
}

ACTION ActionClickThievesGuild(void){

    return ShowThievesGuildInfo();
}

ACTION ActionClickTavern(void){

    return ShowTavernInfo("The ultimate artifact may be found in a murky swamp..");
}

ACTION ActionClickDock(void){
    fprintf(stderr, "ClickDock\n");
    return NONE;
}

ACTION ActionClickMarketplace(void){

    const S_CASTLE *castle = GetCurrentCastle();
    if(! castle) return NONE;

    return ShowMarketplace(castle->color);
}

ACTION ActionClickMoat(void){

    return BuildingInfoYESBox(BUILD_MOAT);
}

ACTION ActionClickSpec(void){

    return BuildingInfoYESBox(BUILD_SPEC);
}

ACTION ActionClickWel2(void){

    return BuildingInfoYESBox(BUILD_WEL2);
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

ACTION ActionClickRCastle(void){

    const S_CASTLE *castle = GetCurrentCastle();
    
    if(castle->building & BUILD_CASTLE)
	return BuildingInfoBox(BUILD_CASTLE);

    return BuildingInfoBox(BUILD_TENT);
}

ACTION ActionClickRCaptain(void){

    return BuildingInfoBox(BUILD_CAPTAIN);
}

ACTION ActionClickRMageGuild(void){

    return BuildingInfoBox(BUILD_MAGEGUILD1);
}

ACTION ActionClickRStatue(void){

    return BuildingInfoBox(BUILD_STATUE);
}

ACTION ActionClickRWell(void){

    return BuildingInfoBox(BUILD_WELL);
}

ACTION ActionClickRThievesGuild(void){

    return BuildingInfoBox(BUILD_THIEVESGUILD);
}

ACTION ActionClickRTavern(void){

    return BuildingInfoBox(BUILD_TAVERN);
}

ACTION ActionClickRDock(void){

    return BuildingInfoBox(BUILD_SHIPYARD);
}

ACTION ActionClickRMarketplace(void){

    return BuildingInfoBox(BUILD_MARKETPLACE);
}

ACTION ActionClickRMoat(void){

    return BuildingInfoBox(BUILD_MOAT);
}

ACTION ActionClickRSpec(void){

    return BuildingInfoBox(BUILD_SPEC);
}

ACTION ActionClickRWel2(void){

    return BuildingInfoBox(BUILD_WEL2);
}

ACTION ActionClickRDwelling1(void){

    Uint8 level = 1;

    const S_CASTLE *castle = GetCurrentCastle();
    
    DialogRecrutMonsterInfo(GetMonsterFromCastle(castle, level), castle->monster[level - 1]);

    return NONE;
}

ACTION ActionClickRDwelling2(void){

    Uint8 level = 2;

    const S_CASTLE *castle = GetCurrentCastle();
    
    DialogRecrutMonsterInfo(GetMonsterFromCastle(castle, level), castle->monster[level - 1]);

    return NONE;
}

ACTION ActionClickRDwelling3(void){

    Uint8 level = 3;

    const S_CASTLE *castle = GetCurrentCastle();
    
    DialogRecrutMonsterInfo(GetMonsterFromCastle(castle, level), castle->monster[level - 1]);

    return NONE;
}

ACTION ActionClickRDwelling4(void){

    Uint8 level = 4;

    const S_CASTLE *castle = GetCurrentCastle();
    
    DialogRecrutMonsterInfo(GetMonsterFromCastle(castle, level), castle->monster[level - 1]);

    return NONE;
}

ACTION ActionClickRDwelling5(void){

    Uint8 level = 5;

    const S_CASTLE *castle = GetCurrentCastle();
    
    DialogRecrutMonsterInfo(GetMonsterFromCastle(castle, level), castle->monster[level - 1]);

    return NONE;
}

ACTION ActionClickRDwelling6(void){

    Uint8 level = 6;

    const S_CASTLE *castle = GetCurrentCastle();
    
    DialogRecrutMonsterInfo(GetMonsterFromCastle(castle, level), castle->monster[level - 1]);

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
	    sprintf(message, "Build %s", GetStringBuilding(castle->race, BUILD_WEL2));
	    break;
	
	case CANNOT_BUILD:
	    sprintf(message, "Cannot build %s", GetStringBuilding(castle->race, BUILD_WEL2));
	    break;
	
	case ALREADY_BUILD:
	    sprintf(message, "%s is already build", GetStringBuilding(castle->race, BUILD_WEL2));
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
	    sprintf(message, "Build %s", GetStringBuilding(castle->race, BUILD_SPEC));
	    break;
	
	case CANNOT_BUILD:
	    sprintf(message, "Cannot build %s", GetStringBuilding(castle->race, BUILD_SPEC));
	    break;
	
	case ALREADY_BUILD:
	    sprintf(message, "%s is already build", GetStringBuilding(castle->race, BUILD_SPEC));
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

ACTION ActionCastleOverRecrutPrimaryHeroes(void){

    char message[64];

    const S_HEROES *heroes = GetStatHeroes(GetRecrutPrimaryHeroes());
    const S_CASTLE *castle = GetCurrentCastle();
    
    switch(AllowRecrutHeroes(castle)){

	case BUILD_OK:
	    sprintf(message,  "Recrut %s The %s.", heroes->name, GetStringRace(heroes->race));
	    break;
	
	default:
	case CANNOT_BUILD:
	    sprintf(message, "Cannot afford a Hero.");
	    break;
	
	case ALREADY_BUILD:
	    sprintf(message, "Cannot recrut - you already have a Hero in this town.");
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

ACTION ActionCastleOverRecrutSecondaryHeroes(void){

    char message[64];

    const S_HEROES *heroes = GetStatHeroes(GetRecrutSecondaryHeroes());
    const S_CASTLE *castle = GetCurrentCastle();
    
    switch(AllowRecrutHeroes(castle)){

	case BUILD_OK:
	    sprintf(message,  "Recrut %s The %s.", heroes->name, GetStringRace(heroes->race));
	    break;
	
	default:
	case CANNOT_BUILD:
	    sprintf(message, "Cannot afford a Hero.");
	    break;
	
	case ALREADY_BUILD:
	    sprintf(message, "Cannot recrut - you already have a Hero in this town.");
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
    sprintf(message, "Build Mage Guild Level %d ?", GetMageGuildLevel(castle) + 1);
    
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
    sprintf(message, "Build %s ?", GetStringBuilding(castle->race, BUILD_WEL2));
    
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
    sprintf(message, "Build %s ?", GetStringBuilding(castle->race, BUILD_SPEC));
    
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
	CastleIncreaseArmy(castle, DWELLING_MONSTER1, GetMonsterGrown(GetMonsterFromCastle(castle, 1)));
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

	CastleIncreaseArmy(castle, DWELLING_MONSTER2, GetMonsterGrown(GetMonsterFromCastle(castle, 2)));
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

	CastleIncreaseArmy(castle, DWELLING_MONSTER3, GetMonsterGrown(GetMonsterFromCastle(castle, 3)));
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

	CastleIncreaseArmy(castle, DWELLING_MONSTER4, GetMonsterGrown(GetMonsterFromCastle(castle, 4)));
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

	CastleIncreaseArmy(castle, DWELLING_MONSTER5, GetMonsterGrown(GetMonsterFromCastle(castle, 5)));
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

	CastleIncreaseArmy(castle, DWELLING_MONSTER6, GetMonsterGrown(GetMonsterFromCastle(castle, 6)));
	EnableCastleUpdateBuilding();

	return CANCEL;
    }
    return NONE;
}

ACTION ActionCastleClickCaptain(void){

    const S_CASTLE *castle = GetCurrentCastle();

    char message[32];
    sprintf(message, "Build %s ?", GetStringBuilding(KNIGHT, BUILD_CAPTAIN));
    
    if(YES == MessageBox(message, FONT_BIG)){

	BuildCaptain(castle);
	EnableCastleUpdateBuilding();

	return CANCEL;
    }

    return NONE;
}

ACTION ActionCastleRClickRecrutPrimaryHeroes(void){

    return ShowHeroesInfo(GetRecrutPrimaryHeroes());
}

ACTION ActionCastleRClickRecrutSecondaryHeroes(void){

    return ShowHeroesInfo(GetRecrutSecondaryHeroes());
}

ACTION ActionCastleLClickRecrutPrimaryHeroes(void){

    return NONE;
}

ACTION ActionCastleLClickRecrutSecondaryHeroes(void){

    return NONE;
}

