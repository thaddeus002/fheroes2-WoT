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
#include "mp2maps.h"
#include "kingdom.h"
#include "castle.h"
#include "castleinfo.h"
#include "marketplace.h"
#include "wellinfo.h"
#include "taverninfo.h"
#include "castleaction.h"
#include "castledialog.h"
#include "recrutmonster.h"

#define MESSAGELEN	64

ACTION ActionOverCastle(void){

    const char *message = NULL;
    const S_CASTLE *castle = GetCurrentCastle();

    message = (castle->building & BUILD_CASTLE ? "Castle" : "Tent");

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
	    monster = (castle->building & DWELLING_UPGRADE2 ? GetStatMonster(CHIEF_ORC) : GetStatMonster(ORC));
	    break;
	case KNIGHT:
	    monster = (castle->building & DWELLING_UPGRADE2 ? GetStatMonster(ARCHER) : GetStatMonster(RANGER));
	    break;
	case WARLOCK:
	    monster = GetStatMonster(GARGOYLE);
	    break;
	case SORCERESS:
	    monster = (castle->building & DWELLING_UPGRADE2 ? GetStatMonster(DWARF) : GetStatMonster(BATTLE_DWARF));
	    break;
	case NECROMANCER:
	    monster = (castle->building & DWELLING_UPGRADE2 ? GetStatMonster(ZOMBIE) : GetStatMonster(MUTANT_ZOMBIE));
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
	    monster = (castle->building & DWELLING_UPGRADE3 ? GetStatMonster(PIKEMAN) : GetStatMonster(VETERAN_PIKEMAN));
	    break;
	case WARLOCK:
	    monster = GetStatMonster(GRIFFIN);
	    break;
	case SORCERESS:
	    monster = (castle->building & DWELLING_UPGRADE3 ? GetStatMonster(ELF) : GetStatMonster(GRAND_ELF));
	    break;
	case NECROMANCER:
	    monster = (castle->building & DWELLING_UPGRADE3 ? GetStatMonster(MUMMY) : GetStatMonster(ROYAL_MUMMY));
	    break;
	case WIZARD:
	    monster = (castle->building & DWELLING_UPGRADE3 ? GetStatMonster(IRON_GOLEM) : GetStatMonster(STEEL_GOLEM));
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
	    monster = (castle->building & DWELLING_UPGRADE4 ? GetStatMonster(OGRE) : GetStatMonster(LORD_OGRE));
	    break;
	case KNIGHT:
	    monster = (castle->building & DWELLING_UPGRADE4 ? GetStatMonster(SWORDSMAN) : GetStatMonster(MASTER_SWORDSMAN));
	    break;
	case WARLOCK:
	    monster = (castle->building & DWELLING_UPGRADE4 ? GetStatMonster(MINOTAUR) : GetStatMonster(KNIGHT_MINOTAUR));
	    break;
	case SORCERESS:
	    monster = (castle->building & DWELLING_UPGRADE4 ? GetStatMonster(DRUID) : GetStatMonster(GREATER_DRUID));
	    break;
	case NECROMANCER:
	    monster = (castle->building & DWELLING_UPGRADE4 ? GetStatMonster(VAMPIRE) : GetStatMonster(LORD_VAMPIRE));
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
	    monster = (castle->building & DWELLING_UPGRADE5 ? GetStatMonster(TROLL) : GetStatMonster(WAR_TROLL));
	    break;
	case KNIGHT:
	    monster = (castle->building & DWELLING_UPGRADE5 ? GetStatMonster(CAVALIRY) : GetStatMonster(CHAMPION));
	    break;
	case WARLOCK:
	    monster = GetStatMonster(HIDRA);
	    break;
	case SORCERESS:
	    monster = GetStatMonster(UNICORN);
	    break;
	case NECROMANCER:
	    monster = (castle->building & DWELLING_UPGRADE5 ? GetStatMonster(LICH) : GetStatMonster(POWER_LICH));
	    break;
	case WIZARD:
	    monster = (castle->building & DWELLING_UPGRADE5 ? GetStatMonster(MAGE) : GetStatMonster(ARCHMAGE));
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
	    monster = (castle->building & DWELLING_UPGRADE6 ? GetStatMonster(PALADIN) : GetStatMonster(CRUSADER));
	    break;
	case WARLOCK:
	    if(castle->building & DWELLING_UPGRADE7)
		monster = GetStatMonster(BLACK_DRAGON);
	    else if(castle->building & DWELLING_UPGRADE6)
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
	    monster = (castle->building & DWELLING_UPGRADE6 ? GetStatMonster(GIANT) : GetStatMonster(TITAN));
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

    message = (castle->building & BUILD_CASTLE ? "Exit Castle" : "Exit Town");
    
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

    ResetCastleSelector();

    const S_CASTLE *castle = GetCurrentCastle();
    
    if(castle->building & BUILD_CASTLE)

	ShowCastleInfo();
    
    else if(! castle->allowCastle)
	return MessageBox(NULL, "This town may not be upgraded to a castle.", FONT_BIG, OK);

    else if(!KingdomAllowPayment(castle->color, PaymentConditionsBuilding(castle->race, BUILD_CASTLE)))
	BuyBuildingBox(castle->race, BUILD_CASTLE, CANCEL);

    else if(OK == BuyBuildingBox(castle->race, BUILD_CASTLE, OK|CANCEL)){

	BuildCastle(castle);
	EnableCastleUpdateBuilding();
	MapsUpgradeTown2Castle(castle->ax, castle->ay);

	return NONE;

    }

    return NONE;
}

ACTION ActionClickCaptain(void){

    ResetCastleSelector();

    const S_CASTLE *castle = GetCurrentCastle();

    if(EXIT == MessageBox(
			GetStringBuilding(castle->race, BUILD_CAPTAIN),
			GetStringDescriptionsBuilding(castle->race, BUILD_CAPTAIN),
			FONT_BIG,
			OK )) return EXIT;
    
    return NONE;
}

ACTION ActionClickMageGuild(void){

    ResetCastleSelector();

    ACTION result = ShowMageGuildInfo();

    if(EXIT != result) result = NONE;
    
    return result;
}

ACTION ActionClickStatue(void){

    ResetCastleSelector();

    const S_CASTLE *castle = GetCurrentCastle();

    if(EXIT == MessageBox(
			GetStringBuilding(castle->race, BUILD_STATUE),
			GetStringDescriptionsBuilding(castle->race, BUILD_STATUE),
			FONT_BIG,
			OK )) return EXIT;
    
    return NONE;
}

ACTION ActionClickWell(void){

    ResetCastleSelector();
    ACTION result = ShowWellInfo();
    RedrawCastleMonster();
    return result;
}

ACTION ActionClickThievesGuild(void){

    ResetCastleSelector();

    return ShowThievesGuildInfo();
}

ACTION ActionClickTavern(void){

    ResetCastleSelector();

    return ShowTavernInfo("The ultimate artifact may be found in a murky swamp..");
}

ACTION ActionClickDock(void){

    ResetCastleSelector();

    const S_CASTLE *castle = GetCurrentCastle();

    S_PAYMENT payment = *(PaymentConditionsBuilding(castle->race, BUILD_BOAT));

    if(OK == BuyBoat() && KingdomAllowPayment(castle->color, &payment)){

	KingdomWasteResource(castle->color, &payment);

	fprintf(stderr, "Buy Boat OK.!");
    }

    return NONE;
}

ACTION ActionClickMarketplace(void){

    ResetCastleSelector();

    const S_CASTLE *castle = GetCurrentCastle();

    if(! castle) return NONE;

    ACTION result = ShowMarketplace(castle->color);
    RedrawCastleInfoResource();
    
    return result;
}

ACTION ActionClickMoat(void){

    ResetCastleSelector();

    const S_CASTLE *castle = GetCurrentCastle();

    if(EXIT == MessageBox(
			GetStringBuilding(castle->race, BUILD_MOAT),
			GetStringDescriptionsBuilding(castle->race, BUILD_MOAT),
			FONT_BIG,
			OK )) return EXIT;
    
    return NONE;
}

ACTION ActionClickSpec(void){

    ResetCastleSelector();

    const S_CASTLE *castle = GetCurrentCastle();

    if(EXIT == MessageBox(
			GetStringBuilding(castle->race, BUILD_SPEC),
			GetStringDescriptionsBuilding(castle->race, BUILD_SPEC),
			FONT_BIG,
			OK )) return EXIT;
    
    return NONE;
}

ACTION ActionClickWel2(void){

    ResetCastleSelector();

    const S_CASTLE *castle = GetCurrentCastle();

    if(EXIT == MessageBox(
			GetStringBuilding(castle->race, BUILD_WEL2),
			GetStringDescriptionsBuilding(castle->race, BUILD_WEL2),
			FONT_BIG,
			OK )) return EXIT;
    
    return NONE;
}

ACTION ActionClickDwelling1(void){

    ResetCastleSelector();

    const Uint8 levelMonster = 1;

    const S_CASTLE *castle = GetCurrentCastle();
    if(! castle) return NONE;

    if(	BuyMonsterFromCastle(castle, levelMonster, 
	DialogRecrutMonster(GetMonsterFromCastle(castle, levelMonster), levelMonster, castle->monster[levelMonster - 1])) ){
	    RedrawCastleMonster();
	    RedrawCastleInfoResource();
    }

    return NONE;
}

ACTION ActionClickDwelling2(void){

    ResetCastleSelector();

    const Uint8 levelMonster = 2;

    const S_CASTLE *castle = GetCurrentCastle();
    if(! castle) return NONE;

    if(	BuyMonsterFromCastle(castle, levelMonster, 
	DialogRecrutMonster(GetMonsterFromCastle(castle, levelMonster), levelMonster, castle->monster[levelMonster - 1])) ){
	    RedrawCastleMonster();
	    RedrawCastleInfoResource();
    }

    return NONE;
}

ACTION ActionClickDwelling3(void){

    ResetCastleSelector();

    const Uint8 levelMonster = 3;

    const S_CASTLE *castle = GetCurrentCastle();
    if(! castle) return NONE;

    if(	BuyMonsterFromCastle(castle, levelMonster, 
	DialogRecrutMonster(GetMonsterFromCastle(castle, levelMonster), levelMonster, castle->monster[levelMonster - 1])) ){
	    RedrawCastleMonster();
	    RedrawCastleInfoResource();
    }

    return NONE;
}

ACTION ActionClickDwelling4(void){

    ResetCastleSelector();

    const Uint8 levelMonster = 4;

    const S_CASTLE *castle = GetCurrentCastle();
    if(! castle) return NONE;

    if(	BuyMonsterFromCastle(castle, levelMonster, 
	DialogRecrutMonster(GetMonsterFromCastle(castle, levelMonster), levelMonster, castle->monster[levelMonster - 1])) ){
	    RedrawCastleMonster();
	    RedrawCastleInfoResource();
    }

    return NONE;
}

ACTION ActionClickDwelling5(void){

    ResetCastleSelector();

    const Uint8 levelMonster = 5;

    const S_CASTLE *castle = GetCurrentCastle();
    if(! castle) return NONE;

    if(	BuyMonsterFromCastle(castle, levelMonster, 
	DialogRecrutMonster(GetMonsterFromCastle(castle, levelMonster), levelMonster, castle->monster[levelMonster - 1])) ){
	    RedrawCastleMonster();
	    RedrawCastleInfoResource();
    }

    return NONE;
}

ACTION ActionClickDwelling6(void){

    ResetCastleSelector();

    const Uint8 levelMonster = 6;

    const S_CASTLE *castle = GetCurrentCastle();
    if(! castle) return NONE;

    if(	BuyMonsterFromCastle(castle, levelMonster, 
	DialogRecrutMonster(GetMonsterFromCastle(castle, levelMonster), levelMonster, castle->monster[levelMonster - 1])) ){
	    RedrawCastleMonster();
	    RedrawCastleInfoResource();
    }

    return NONE;
}

ACTION ActionClickRCastle(void){

    ResetCastleSelector();

    const S_CASTLE *castle = GetCurrentCastle();
    
    if(castle->building & BUILD_CASTLE)
	return MessageBox(
			    GetStringBuilding(castle->race, BUILD_CASTLE),
			    GetStringDescriptionsBuilding(castle->race, BUILD_CASTLE),
			    FONT_BIG,
			    NONE );

    return MessageBox(
			GetStringBuilding(castle->race, BUILD_TENT),
			GetStringDescriptionsBuilding(castle->race, BUILD_TENT),
			FONT_BIG,
			NONE );
}

ACTION ActionClickRCaptain(void){

    ResetCastleSelector();

    const S_CASTLE *castle = GetCurrentCastle();

    return MessageBox(
			GetStringBuilding(castle->race, BUILD_CAPTAIN),
			GetStringDescriptionsBuilding(castle->race, BUILD_CAPTAIN),
			FONT_BIG,
			NONE );
}

ACTION ActionClickRMageGuild(void){

    ResetCastleSelector();

    const S_CASTLE *castle = GetCurrentCastle();

    return MessageBox(
			GetStringBuilding(castle->race, BUILD_MAGEGUILD1),
			GetStringDescriptionsBuilding(castle->race, BUILD_MAGEGUILD1),
			FONT_BIG,
			NONE );
}

ACTION ActionClickRStatue(void){

    ResetCastleSelector();

    const S_CASTLE *castle = GetCurrentCastle();

    return MessageBox(
			GetStringBuilding(castle->race, BUILD_STATUE),
			GetStringDescriptionsBuilding(castle->race, BUILD_STATUE),
			FONT_BIG,
			NONE );
}

ACTION ActionClickRWell(void){

    ResetCastleSelector();

    const S_CASTLE *castle = GetCurrentCastle();

    return MessageBox(
			GetStringBuilding(castle->race, BUILD_WELL),
			GetStringDescriptionsBuilding(castle->race, BUILD_WELL),
			FONT_BIG,
			NONE );
}

ACTION ActionClickRThievesGuild(void){

    ResetCastleSelector();

    const S_CASTLE *castle = GetCurrentCastle();

    return MessageBox(
			GetStringBuilding(castle->race, BUILD_THIEVESGUILD),
			GetStringDescriptionsBuilding(castle->race, BUILD_THIEVESGUILD),
			FONT_BIG,
			NONE );
}

ACTION ActionClickRTavern(void){

    ResetCastleSelector();

    const S_CASTLE *castle = GetCurrentCastle();

    return MessageBox(
			GetStringBuilding(castle->race, BUILD_TAVERN),
			GetStringDescriptionsBuilding(castle->race, BUILD_TAVERN),
			FONT_BIG,
			NONE );
}

ACTION ActionClickRDock(void){

    ResetCastleSelector();

    const S_CASTLE *castle = GetCurrentCastle();

    return MessageBox(
			GetStringBuilding(castle->race, BUILD_SHIPYARD),
			GetStringDescriptionsBuilding(castle->race, BUILD_SHIPYARD),
			FONT_BIG,
			NONE );
}

ACTION ActionClickRMarketplace(void){

    ResetCastleSelector();

    const S_CASTLE *castle = GetCurrentCastle();

    return MessageBox(
			GetStringBuilding(castle->race, BUILD_MARKETPLACE),
			GetStringDescriptionsBuilding(castle->race, BUILD_MARKETPLACE),
			FONT_BIG,
			NONE );
}

ACTION ActionClickRMoat(void){

    ResetCastleSelector();

    const S_CASTLE *castle = GetCurrentCastle();

    return MessageBox(
			GetStringBuilding(castle->race, BUILD_MOAT),
			GetStringDescriptionsBuilding(castle->race, BUILD_MOAT),
			FONT_BIG,
			NONE );
}

ACTION ActionClickRSpec(void){

    ResetCastleSelector();

    const S_CASTLE *castle = GetCurrentCastle();

    return MessageBox(
			GetStringBuilding(castle->race, BUILD_SPEC),
			GetStringDescriptionsBuilding(castle->race, BUILD_SPEC),
			FONT_BIG,
			NONE );
}

ACTION ActionClickRWel2(void){

    ResetCastleSelector();

    const S_CASTLE *castle = GetCurrentCastle();

    return MessageBox(
			GetStringBuilding(castle->race, BUILD_WEL2),
			GetStringDescriptionsBuilding(castle->race, BUILD_WEL2),
			FONT_BIG,
			NONE );
}

ACTION ActionClickRDwelling1(void){

    ResetCastleSelector();

    Uint8 level = 1;

    const S_CASTLE *castle = GetCurrentCastle();
    
    DialogRecrutMonsterInfo(GetMonsterFromCastle(castle, level), castle->monster[level - 1]);

    return NONE;
}

ACTION ActionClickRDwelling2(void){

    ResetCastleSelector();

    Uint8 level = 2;

    const S_CASTLE *castle = GetCurrentCastle();
    
    DialogRecrutMonsterInfo(GetMonsterFromCastle(castle, level), castle->monster[level - 1]);

    return NONE;
}

ACTION ActionClickRDwelling3(void){

    ResetCastleSelector();

    Uint8 level = 3;

    const S_CASTLE *castle = GetCurrentCastle();
    
    DialogRecrutMonsterInfo(GetMonsterFromCastle(castle, level), castle->monster[level - 1]);

    return NONE;
}

ACTION ActionClickRDwelling4(void){

    ResetCastleSelector();

    Uint8 level = 4;

    const S_CASTLE *castle = GetCurrentCastle();
    
    DialogRecrutMonsterInfo(GetMonsterFromCastle(castle, level), castle->monster[level - 1]);

    return NONE;
}

ACTION ActionClickRDwelling5(void){

    ResetCastleSelector();

    Uint8 level = 5;

    const S_CASTLE *castle = GetCurrentCastle();
    
    DialogRecrutMonsterInfo(GetMonsterFromCastle(castle, level), castle->monster[level - 1]);

    return NONE;
}

ACTION ActionClickRDwelling6(void){

    ResetCastleSelector();

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
    
    char message[MESSAGELEN + 1];
    const S_CASTLE *castle = GetCurrentCastle();
    
    switch(AllowBuildWel2(castle)){

	case BUILD_OK:
	    snprintf(message, MESSAGELEN, "Build %s", GetStringBuilding(castle->race, BUILD_WEL2));
	    break;
	
	case CANNOT_BUILD:
	    snprintf(message, MESSAGELEN, "Cannot build %s", GetStringBuilding(castle->race, BUILD_WEL2));
	    break;
	
	case ALREADY_BUILD:
	    snprintf(message, MESSAGELEN, "%s is already build", GetStringBuilding(castle->race, BUILD_WEL2));
	    break;
	
	case END_TUR:
	    snprintf(message, MESSAGELEN, "Cannot build. Already build here this turn.");
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
    
    char message[MESSAGELEN + 1];
    const S_CASTLE *castle = GetCurrentCastle();
    
    switch(AllowBuildSpec(castle)){

	case BUILD_OK:
	    snprintf(message, MESSAGELEN, "Build %s", GetStringBuilding(castle->race, BUILD_SPEC));
	    break;
	
	case CANNOT_BUILD:
	    snprintf(message, MESSAGELEN, "Cannot build %s", GetStringBuilding(castle->race, BUILD_SPEC));
	    break;
	
	case ALREADY_BUILD:
	    snprintf(message, MESSAGELEN, "%s is already build", GetStringBuilding(castle->race, BUILD_SPEC));
	    break;
	
	case END_TUR:
	    snprintf(message, MESSAGELEN, "Cannot build. Already build here this turn.");
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
    
    char message[MESSAGELEN + 1];
    const S_CASTLE *castle = GetCurrentCastle();
    
    switch(AllowBuildDwelling1(castle)){

	case BUILD_OK:
	    snprintf(message, MESSAGELEN, "Build %s", GetStringBuilding(castle->race, DWELLING_MONSTER1));
	    break;
	
	case CANNOT_BUILD:
	    snprintf(message, MESSAGELEN, "Cannot build %s", GetStringBuilding(castle->race, DWELLING_MONSTER1));
	    break;
	
	case ALREADY_BUILD:
	    snprintf(message, MESSAGELEN, "%s is already build", GetStringBuilding(castle->race, DWELLING_MONSTER1));
	    break;
	
	case END_TUR:
	    snprintf(message, MESSAGELEN, "Cannot build. Already build here this turn.");
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
    
    char message[MESSAGELEN + 1];
    const S_CASTLE *castle = GetCurrentCastle();
    
    
    switch(AllowBuildDwelling2(castle)){

	case BUILD_OK:
	    castle->building & DWELLING_MONSTER2 ? snprintf(message, MESSAGELEN, "Build %s", GetStringBuilding(castle->race, DWELLING_UPGRADE2)) :
		snprintf(message, MESSAGELEN, "Build %s", GetStringBuilding(castle->race, DWELLING_MONSTER2));
	    break;

	case CANNOT_BUILD:
	    castle->building & DWELLING_MONSTER2 ? snprintf(message, MESSAGELEN, "Cannot build %s", GetStringBuilding(castle->race, DWELLING_UPGRADE2)) :
		snprintf(message, MESSAGELEN, "Cannot build %s", GetStringBuilding(castle->race, DWELLING_MONSTER2));
	    break;

	case ALREADY_BUILD:
	    if(castle->building & DWELLING_UPGRADE2)
		snprintf(message, MESSAGELEN, "%s is already build", GetStringBuilding(castle->race, DWELLING_UPGRADE2));
	    else
		snprintf(message, MESSAGELEN, "%s is already build", GetStringBuilding(castle->race, DWELLING_MONSTER2));
	    break;

	case END_TUR:
	    snprintf(message, MESSAGELEN, "Cannot build. Already build here this turn.");
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
    
    char message[MESSAGELEN + 1];
    const S_CASTLE *castle = GetCurrentCastle();
    
    switch(AllowBuildDwelling3(castle)){

	case BUILD_OK:
	    castle->building & DWELLING_MONSTER3 ? snprintf(message, MESSAGELEN, "Build %s", GetStringBuilding(castle->race, DWELLING_UPGRADE3)) :
		snprintf(message, MESSAGELEN, "Build %s", GetStringBuilding(castle->race, DWELLING_MONSTER3));
	    break;
	
	case CANNOT_BUILD:
	    castle->building & DWELLING_MONSTER3 ? snprintf(message, MESSAGELEN, "Cannot build %s", GetStringBuilding(castle->race, DWELLING_UPGRADE3)) :
		snprintf(message, MESSAGELEN, "Cannot build %s", GetStringBuilding(castle->race, DWELLING_MONSTER3));
	    break;
	
	case ALREADY_BUILD:
	    if(castle->building & DWELLING_UPGRADE3)
		snprintf(message, MESSAGELEN, "%s is already build", GetStringBuilding(castle->race, DWELLING_UPGRADE3));
	    else
		snprintf(message, MESSAGELEN, "%s is already build", GetStringBuilding(castle->race, DWELLING_MONSTER3));
	    break;
	
	case END_TUR:
	    snprintf(message, MESSAGELEN, "Cannot build. Already build here this turn.");
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
    
    char message[MESSAGELEN + 1];
    const S_CASTLE *castle = GetCurrentCastle();
    
    switch(AllowBuildDwelling4(castle)){

	case BUILD_OK:
	    castle->building & DWELLING_MONSTER4 ? snprintf(message, MESSAGELEN, "Build %s", GetStringBuilding(castle->race, DWELLING_UPGRADE4)) :
		snprintf(message, MESSAGELEN, "Build %s", GetStringBuilding(castle->race, DWELLING_MONSTER4));
	    break;
	
	case CANNOT_BUILD:
	    castle->building & DWELLING_MONSTER4 ? snprintf(message, MESSAGELEN, "Cannot build %s", GetStringBuilding(castle->race, DWELLING_UPGRADE4)) :
		snprintf(message, MESSAGELEN, "Cannot build %s", GetStringBuilding(castle->race, DWELLING_MONSTER4));
	    break;
	
	case ALREADY_BUILD:
	    if(castle->building & DWELLING_UPGRADE4)
		snprintf(message, MESSAGELEN, "%s is already build", GetStringBuilding(castle->race, DWELLING_UPGRADE4));
	    else
		snprintf(message, MESSAGELEN, "%s is already build", GetStringBuilding(castle->race, DWELLING_MONSTER4));
	    break;
	
	case END_TUR:
	    snprintf(message, MESSAGELEN, "Cannot build. Already build here this turn.");
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
    
    char message[MESSAGELEN + 1];
    const S_CASTLE *castle = GetCurrentCastle();
    
    switch(AllowBuildDwelling5(castle)){

	case BUILD_OK:
	    castle->building & DWELLING_MONSTER5 ? snprintf(message, MESSAGELEN, "Build %s", GetStringBuilding(castle->race, DWELLING_UPGRADE5)) :
		snprintf(message, MESSAGELEN, "Build %s", GetStringBuilding(castle->race, DWELLING_MONSTER5));
	    break;
	
	case CANNOT_BUILD:
	    castle->building & DWELLING_MONSTER5 ? snprintf(message, MESSAGELEN, "Cannot build %s", GetStringBuilding(castle->race, DWELLING_UPGRADE5)) :
		snprintf(message, MESSAGELEN, "Cannot build %s", GetStringBuilding(castle->race, DWELLING_MONSTER5));
	    break;
	
	case ALREADY_BUILD:
	    if(castle->building & DWELLING_UPGRADE5)
		snprintf(message, MESSAGELEN, "%s is already build", GetStringBuilding(castle->race, DWELLING_UPGRADE5));
	    else
		snprintf(message, MESSAGELEN, "%s is already build", GetStringBuilding(castle->race, DWELLING_MONSTER5));
	    break;
	
	case END_TUR:
	    snprintf(message, MESSAGELEN, "Cannot build. Already build here this turn.");
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
    
    char message[MESSAGELEN + 1];
    const S_CASTLE *castle = GetCurrentCastle();
    
    switch(AllowBuildDwelling6(castle)){

	case BUILD_OK:
	    if(castle->race == WARLOCK && castle->building & DWELLING_UPGRADE6)
		snprintf(message, MESSAGELEN, "Build %s", GetStringBuilding(castle->race, DWELLING_UPGRADE7));
	    else if(castle->building & DWELLING_MONSTER6)
		snprintf(message, MESSAGELEN, "Build %s", GetStringBuilding(castle->race, DWELLING_UPGRADE6));
	    else
		snprintf(message, MESSAGELEN, "Build %s", GetStringBuilding(castle->race, DWELLING_MONSTER6));
	    break;
	
	case CANNOT_BUILD:
	    if(castle->race == WARLOCK && castle->building & DWELLING_UPGRADE6)
		snprintf(message, MESSAGELEN, "Cannot build %s", GetStringBuilding(castle->race, DWELLING_UPGRADE7));
	    else if(castle->building & DWELLING_MONSTER6)
		snprintf(message, MESSAGELEN, "Cannot build %s", GetStringBuilding(castle->race, DWELLING_UPGRADE6));
	    else
		snprintf(message, MESSAGELEN, "Cannot build %s", GetStringBuilding(castle->race, DWELLING_MONSTER6));
	    break;
	
	case ALREADY_BUILD:
	    if(castle->building & DWELLING_UPGRADE7)
		snprintf(message, MESSAGELEN, "%s is already build", GetStringBuilding(castle->race, DWELLING_UPGRADE7));
	    else if(castle->building & DWELLING_UPGRADE6)
		snprintf(message, MESSAGELEN, "%s is already build", GetStringBuilding(castle->race, DWELLING_UPGRADE6));
	    else if(castle->building & DWELLING_MONSTER6)
		snprintf(message, MESSAGELEN, "%s is already build", GetStringBuilding(castle->race, DWELLING_MONSTER6));
	    break;
	
	case END_TUR:
	    snprintf(message, MESSAGELEN, "Cannot build. Already build here this turn.");
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

    char message[MESSAGELEN + 1];

    const S_HEROES *heroes = GetStatHeroes(GetRecrutPrimaryHeroes());
    const S_CASTLE *castle = GetCurrentCastle();
    
    switch(AllowRecrutHeroes(castle)){

	case BUILD_OK:
	    snprintf(message, MESSAGELEN,  "Recrut %s The %s.", heroes->name, GetStringRace(heroes->race));
	    break;
	
	default:
	case CANNOT_BUILD:
	    snprintf(message, MESSAGELEN, "Cannot afford a Hero.");
	    break;
	
	case ALREADY_BUILD:
	    snprintf(message, MESSAGELEN, "Cannot recrut - you already have a Hero in this town.");
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

    char message[MESSAGELEN + 1];

    const S_HEROES *heroes = GetStatHeroes(GetRecrutSecondaryHeroes());
    const S_CASTLE *castle = GetCurrentCastle();
    
    switch(AllowRecrutHeroes(castle)){

	case BUILD_OK:
	    snprintf(message, MESSAGELEN,  "Recrut %s The %s.", heroes->name, GetStringRace(heroes->race));
	    break;
	
	default:
	case CANNOT_BUILD:
	    snprintf(message, MESSAGELEN, "Cannot afford a Hero.");
	    break;
	
	case ALREADY_BUILD:
	    snprintf(message, MESSAGELEN, "Cannot recrut - you already have a Hero in this town.");
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

    if( OK == BuyBuildingBox(castle->race, BUILD_TAVERN, OK|CANCEL)){

	BuildTavern(castle);
	EnableCastleUpdateBuilding();

	return CANCEL;
    }

    return NONE;
}

ACTION ActionCastleClickStatue(void){

    const S_CASTLE *castle = GetCurrentCastle();

    if( OK == BuyBuildingBox(castle->race, BUILD_STATUE, OK|CANCEL)){

	BuildStatue(castle);
	EnableCastleUpdateBuilding();

	return CANCEL;
    }

    return NONE;
}

ACTION ActionCastleClickWell(void){

    const S_CASTLE *castle = GetCurrentCastle();

    if( OK == BuyBuildingBox(castle->race, BUILD_WELL, OK|CANCEL)){

	BuildWell(castle);
	EnableCastleUpdateBuilding();

	return CANCEL;
    }

    return NONE;
}

ACTION ActionCastleClickMoat(void){

    const S_CASTLE *castle = GetCurrentCastle();

    if( OK == BuyBuildingBox(castle->race, BUILD_MOAT, OK|CANCEL)){

	BuildMoat(castle);
	EnableCastleUpdateBuilding();

	return CANCEL;
    }

    return NONE;
}

ACTION ActionCastleClickMarketplace(void){

    const S_CASTLE *castle = GetCurrentCastle();

    if( OK == BuyBuildingBox(castle->race, BUILD_MARKETPLACE, OK|CANCEL)){

	BuildMarketplace(castle);
	EnableCastleUpdateBuilding();

	return CANCEL;
    }

    return NONE;
}

ACTION ActionCastleClickThievesGuild(void){

    const S_CASTLE *castle = GetCurrentCastle();

    if( OK == BuyBuildingBox(castle->race, BUILD_THIEVESGUILD, OK|CANCEL)){

	BuildThievesGuild(castle);
	EnableCastleUpdateBuilding();

	return CANCEL;
    }

    return NONE;
}

ACTION ActionCastleClickLeftTurret(void){

    const S_CASTLE *castle = GetCurrentCastle();

    if( OK == BuyBuildingBox(castle->race, BUILD_LEFTTURRET, OK|CANCEL)){

	BuildLeftTurret(castle);
	EnableCastleUpdateBuilding();

	return CANCEL;
    }

    return NONE;
}

ACTION ActionCastleClickRightTurret(void){

    const S_CASTLE *castle = GetCurrentCastle();

    if( OK == BuyBuildingBox(castle->race, BUILD_RIGHTTURRET, OK|CANCEL)){

	BuildRightTurret(castle);
	EnableCastleUpdateBuilding();

	return CANCEL;
    }

    return NONE;
}

ACTION ActionCastleClickMageGuild(void){

    const S_CASTLE *castle = GetCurrentCastle();
    ACTION result = NONE;
    
    switch(GetMageGuildLevel(castle)){
	case MAGIC_NONE:
	    result = BuyBuildingBox(castle->race, BUILD_MAGEGUILD1, OK|CANCEL);
	    break;
	case MAGIC_LEVEL1:
	    result = BuyBuildingBox(castle->race, BUILD_MAGEGUILD2, OK|CANCEL);
	    break;
	case MAGIC_LEVEL2:
	    result = BuyBuildingBox(castle->race, BUILD_MAGEGUILD3, OK|CANCEL);
	    break;
	case MAGIC_LEVEL3:
	    result = BuyBuildingBox(castle->race, BUILD_MAGEGUILD4, OK|CANCEL);
	    break;
	case MAGIC_LEVEL4:
	    result = BuyBuildingBox(castle->race, BUILD_MAGEGUILD5, OK|CANCEL);
	    break;
	case MAGIC_LEVEL5:
	    break;
    }

    if( OK == result){

	BuildMageGuild(castle);
	EnableCastleUpdateBuilding();

	return CANCEL;
    }
    
    return NONE;
}

ACTION ActionCastleClickShipyard(void){

    const S_CASTLE *castle = GetCurrentCastle();

    if( OK == BuyBuildingBox(castle->race, BUILD_SHIPYARD, OK|CANCEL)){

	BuildShipyard(castle);
	EnableCastleUpdateBuilding();

	return CANCEL;
    }

    return NONE;
}

ACTION ActionCastleClickWel2(void){

    const S_CASTLE *castle = GetCurrentCastle();

    if( OK == BuyBuildingBox(castle->race, BUILD_WEL2, OK|CANCEL)){

	BuildWel2(castle);
	EnableCastleUpdateBuilding();

	return CANCEL;
    }

    return NONE;
}

ACTION ActionCastleClickSpec(void){

    const S_CASTLE *castle = GetCurrentCastle();

    if( OK == BuyBuildingBox(castle->race, BUILD_SPEC, OK|CANCEL)){

	BuildSpec(castle);
	EnableCastleUpdateBuilding();

	return CANCEL;
    }

    return NONE;
}

ACTION ActionCastleClickDwelling1(void){

    const S_CASTLE *castle = GetCurrentCastle();

    if(OK == BuyBuildingBox(castle->race, DWELLING_MONSTER1, OK|CANCEL)){

	BuildDwelling1(castle);
	CastleIncreaseArmy(castle, DWELLING_MONSTER1, GetMonsterGrown(GetMonsterFromCastle(castle, 1)));
	EnableCastleUpdateBuilding();

	return CANCEL;
    }

    return NONE;
}

ACTION ActionCastleClickDwelling2(void){

    const S_CASTLE *castle = GetCurrentCastle();
    ACTION result;

    if(castle->building & DWELLING_MONSTER2 && CastleDwellingUpgradable(castle, DWELLING_UPGRADE2))
	result = BuyBuildingBox(castle->race, DWELLING_UPGRADE2, OK|CANCEL);
    else
	result = BuyBuildingBox(castle->race, DWELLING_MONSTER2, OK|CANCEL);

    if(OK == result){

	if(castle->building & DWELLING_MONSTER2 && CastleDwellingUpgradable(castle, DWELLING_UPGRADE2))
	    BuildUpgrade2(castle);
	else
	    BuildDwelling2(castle);

	if(! castle->building & DWELLING_UPGRADE2) CastleIncreaseArmy(castle, DWELLING_MONSTER2, GetMonsterGrown(GetMonsterFromCastle(castle, 2)));
	EnableCastleUpdateBuilding();

	return CANCEL;
    }

    return NONE;
}

ACTION ActionCastleClickDwelling3(void){

    const S_CASTLE *castle = GetCurrentCastle();
    ACTION result;

    if(castle->building & DWELLING_MONSTER3 && CastleDwellingUpgradable(castle, DWELLING_UPGRADE3))
	result = BuyBuildingBox(castle->race, DWELLING_UPGRADE3, OK|CANCEL);
    else
	result = BuyBuildingBox(castle->race, DWELLING_MONSTER3, OK|CANCEL);

    if(OK == result){

	if(castle->building & DWELLING_MONSTER3 && CastleDwellingUpgradable(castle, DWELLING_UPGRADE3))
	    BuildUpgrade3(castle);
	else
	    BuildDwelling3(castle);

	if(! castle->building & DWELLING_UPGRADE3) CastleIncreaseArmy(castle, DWELLING_MONSTER3, GetMonsterGrown(GetMonsterFromCastle(castle, 3)));
	EnableCastleUpdateBuilding();

	return CANCEL;
    }

    return NONE;
}

ACTION ActionCastleClickDwelling4(void){

    const S_CASTLE *castle = GetCurrentCastle();
    ACTION result;

    if(castle->building & DWELLING_MONSTER4 && CastleDwellingUpgradable(castle, DWELLING_UPGRADE4))
	result = BuyBuildingBox(castle->race, DWELLING_UPGRADE4, OK|CANCEL);
    else
	result = BuyBuildingBox(castle->race, DWELLING_MONSTER4, OK|CANCEL);

    if(OK == result){

	if(castle->building & DWELLING_MONSTER4 && CastleDwellingUpgradable(castle, DWELLING_UPGRADE4))
	    BuildUpgrade4(castle);
	else
	    BuildDwelling4(castle);

	if(! castle->building & DWELLING_UPGRADE4) CastleIncreaseArmy(castle, DWELLING_MONSTER4, GetMonsterGrown(GetMonsterFromCastle(castle, 4)));
	EnableCastleUpdateBuilding();

	return CANCEL;
    }

    return NONE;
}

ACTION ActionCastleClickDwelling5(void){

    const S_CASTLE *castle = GetCurrentCastle();
    ACTION result;

    if(castle->building & DWELLING_MONSTER5 && CastleDwellingUpgradable(castle, DWELLING_UPGRADE5))
	result = BuyBuildingBox(castle->race, DWELLING_UPGRADE5, OK|CANCEL);
    else
	result = BuyBuildingBox(castle->race, DWELLING_MONSTER5, OK|CANCEL);

    if(OK == result){

	if(castle->building & DWELLING_MONSTER5 && CastleDwellingUpgradable(castle, DWELLING_UPGRADE5))
	    BuildUpgrade5(castle);
	else
	    BuildDwelling5(castle);

	if(! castle->building & DWELLING_UPGRADE5) CastleIncreaseArmy(castle, DWELLING_MONSTER5, GetMonsterGrown(GetMonsterFromCastle(castle, 5)));
	EnableCastleUpdateBuilding();

	return CANCEL;
    }

    return NONE;
}

ACTION ActionCastleClickDwelling6(void){

    const S_CASTLE *castle = GetCurrentCastle();
    ACTION result;

    if(castle->building & DWELLING_UPGRADE6 && CastleDwellingUpgradable(castle, DWELLING_UPGRADE7))
	result = BuyBuildingBox(castle->race, DWELLING_UPGRADE7, OK|CANCEL);
    else if(castle->building & DWELLING_MONSTER6 && CastleDwellingUpgradable(castle, DWELLING_UPGRADE6))
	result = BuyBuildingBox(castle->race, DWELLING_UPGRADE6, OK|CANCEL);
    else
	result = BuyBuildingBox(castle->race, DWELLING_MONSTER6, OK|CANCEL);

    if(OK == result){

	if(castle->building & DWELLING_UPGRADE6 && CastleDwellingUpgradable(castle, DWELLING_UPGRADE7))
	    BuildUpgrade7(castle);
	else if(castle->building & DWELLING_MONSTER6 && CastleDwellingUpgradable(castle, DWELLING_UPGRADE6))
	    BuildUpgrade6(castle);
	else
	    BuildDwelling6(castle);

	if(!castle->building & DWELLING_UPGRADE6 && !castle->building & DWELLING_UPGRADE7) CastleIncreaseArmy(castle, DWELLING_MONSTER6, GetMonsterGrown(GetMonsterFromCastle(castle, 6)));
	EnableCastleUpdateBuilding();

	return CANCEL;
    }
    return NONE;
}

ACTION ActionCastleClickCaptain(void){

    const S_CASTLE *castle = GetCurrentCastle();

    if( OK == BuyBuildingBox(castle->race, BUILD_CAPTAIN, OK|CANCEL)){

	BuildCaptain(castle);
	EnableCastleUpdateBuilding();

	return CANCEL;
    }

    return NONE;
}

ACTION ActionCastleRClickRecrutPrimaryHeroes(void){

    return ShowHeroesInfo(GetRecrutPrimaryHeroes(), TRUE);
}

ACTION ActionCastleRClickRecrutSecondaryHeroes(void){

    return ShowHeroesInfo(GetRecrutSecondaryHeroes(), TRUE);
}

ACTION ActionCastleLClickRecrutPrimaryHeroes(void){

    return NONE;
}

ACTION ActionCastleLClickRecrutSecondaryHeroes(void){

    return NONE;
}

ACTION ActionCastleRClickTavern(void){

    const S_CASTLE *castle = GetCurrentCastle();

    return BuyBuildingBox(castle->race, BUILD_TAVERN, NONE);
}

ACTION ActionCastleRClickStatue(void){

    const S_CASTLE *castle = GetCurrentCastle();

    return BuyBuildingBox(castle->race, BUILD_STATUE, NONE);
}

ACTION ActionCastleRClickWell(void){

    const S_CASTLE *castle = GetCurrentCastle();

    return BuyBuildingBox(castle->race, BUILD_WELL, NONE);
}

ACTION ActionCastleRClickMoat(void){

    const S_CASTLE *castle = GetCurrentCastle();

    return BuyBuildingBox(castle->race, BUILD_MOAT, NONE);
}

ACTION ActionCastleRClickMarketplace(void){

    const S_CASTLE *castle = GetCurrentCastle();

    return BuyBuildingBox(castle->race, BUILD_MARKETPLACE, NONE);
}

ACTION ActionCastleRClickThievesGuild(void){

    const S_CASTLE *castle = GetCurrentCastle();

    return BuyBuildingBox(castle->race, BUILD_THIEVESGUILD, NONE);
}

ACTION ActionCastleRClickLeftTurret(void){

    const S_CASTLE *castle = GetCurrentCastle();

    return BuyBuildingBox(castle->race, BUILD_LEFTTURRET, NONE);
}

ACTION ActionCastleRClickRightTurret(void){

    const S_CASTLE *castle = GetCurrentCastle();

    return BuyBuildingBox(castle->race, BUILD_RIGHTTURRET, NONE);
}

ACTION ActionCastleRClickMageGuild(void){

    const S_CASTLE *castle = GetCurrentCastle();

    switch(GetMageGuildLevel(castle)){
	case MAGIC_NONE:
	    BuyBuildingBox(castle->race, BUILD_MAGEGUILD1, NONE);
	    break;
	case MAGIC_LEVEL1:
	    BuyBuildingBox(castle->race, BUILD_MAGEGUILD2, NONE);
	    break;
	case MAGIC_LEVEL2:
	    BuyBuildingBox(castle->race, BUILD_MAGEGUILD3, NONE);
	    break;
	case MAGIC_LEVEL3:
	    BuyBuildingBox(castle->race, BUILD_MAGEGUILD4, NONE);
	    break;
	case MAGIC_LEVEL4:
	case MAGIC_LEVEL5:
	    BuyBuildingBox(castle->race, BUILD_MAGEGUILD5, NONE);
	    break;
    }
    
    return NONE;
}

ACTION ActionCastleRClickShipyard(void){

    const S_CASTLE *castle = GetCurrentCastle();

    return BuyBuildingBox(castle->race, BUILD_SHIPYARD, NONE);
}

ACTION ActionCastleRClickWel2(void){

    const S_CASTLE *castle = GetCurrentCastle();

    return BuyBuildingBox(castle->race, BUILD_WEL2, NONE);
}

ACTION ActionCastleRClickSpec(void){

    const S_CASTLE *castle = GetCurrentCastle();

    return BuyBuildingBox(castle->race, BUILD_SPEC, NONE);
}

ACTION ActionCastleRClickCaptain(void){

    const S_CASTLE *castle = GetCurrentCastle();

    return BuyBuildingBox(castle->race, BUILD_CAPTAIN, NONE);
}

ACTION ActionCastleRClickDwelling1(void){

    const S_CASTLE *castle = GetCurrentCastle();

    BuyBuildingBox(castle->race, DWELLING_MONSTER1, NONE);

    return NONE;
}

ACTION ActionCastleRClickDwelling2(void){

    const S_CASTLE *castle = GetCurrentCastle();

    if(castle->building & DWELLING_MONSTER2 && CastleDwellingUpgradable(castle, DWELLING_UPGRADE2))
	BuyBuildingBox(castle->race, DWELLING_UPGRADE2, NONE);
    else
	BuyBuildingBox(castle->race, DWELLING_MONSTER2, NONE);

    return NONE;
}

ACTION ActionCastleRClickDwelling3(void){

    const S_CASTLE *castle = GetCurrentCastle();

    if(castle->building & DWELLING_MONSTER3 && CastleDwellingUpgradable(castle, DWELLING_UPGRADE3))
	BuyBuildingBox(castle->race, DWELLING_UPGRADE3, NONE);
    else
	BuyBuildingBox(castle->race, DWELLING_MONSTER3, NONE);

    return NONE;
}

ACTION ActionCastleRClickDwelling4(void){

    const S_CASTLE *castle = GetCurrentCastle();

    if(castle->building & DWELLING_MONSTER4 && CastleDwellingUpgradable(castle, DWELLING_UPGRADE4))
	BuyBuildingBox(castle->race, DWELLING_UPGRADE4, NONE);
    else
	BuyBuildingBox(castle->race, DWELLING_MONSTER4, NONE);

    return NONE;
}

ACTION ActionCastleRClickDwelling5(void){

    const S_CASTLE *castle = GetCurrentCastle();

    if(castle->building & DWELLING_MONSTER5 && CastleDwellingUpgradable(castle, DWELLING_UPGRADE5))
	BuyBuildingBox(castle->race, DWELLING_UPGRADE5, NONE);
    else
	BuyBuildingBox(castle->race, DWELLING_MONSTER5, NONE);

    return NONE;
}

ACTION ActionCastleRClickDwelling6(void){

    const S_CASTLE *castle = GetCurrentCastle();

    if(castle->building & DWELLING_UPGRADE6 && CastleDwellingUpgradable(castle, DWELLING_UPGRADE7))
	BuyBuildingBox(castle->race, DWELLING_UPGRADE7, NONE);
    else if(castle->building & DWELLING_MONSTER6 && CastleDwellingUpgradable(castle, DWELLING_UPGRADE6))
	BuyBuildingBox(castle->race, DWELLING_UPGRADE6, NONE);
    else
	BuyBuildingBox(castle->race, DWELLING_MONSTER6, NONE);

    return NONE;
}
