/***************************************************************************
 *   Copyright (C) 2006 by Andrey Afletdinov                               *
 *   afletdinov@mail.dc.baikal.ru                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef H2GAME_H
#define H2GAME_H

#include "rect.h"
#include "types.h"

#define RADARWIDTH	144
#define BORDERWIDTH	16

class Heroes;
class Castle;
class Surface;
class Kingdom;
class Player;

namespace Game
{
    typedef enum
    {
	CANCEL = 0,
	QUITGAME,
	MAINMENU,
        NEWGAME,
        LOADGAME,
        HIGHSCORES,
        CREDITS,
        NEWSTANDARD,
        NEWCAMPAIN,
        NEWMULTI,
        NEWHOTSEAT,
        NEWNETWORK,
        LOADSTANDARD,
        LOADCAMPAIN,
        LOADMULTI,
        SCENARIOINFO,
        SELECTSCENARIO,
	STARTGAME,
	SAVEGAME,
	EDITMAINMENU,
	EDITNEWMAP,
	EDITLOADMAP,
	EDITSAVEMAP,
	EDITSTART,
	ENDTURN,
	TESTING,
    } menu_t;

    typedef enum { UNKNOWN = 0, STANDARD = 1, CAMPAIGN = 2, HOTSEAT = 4, NETWORK = 8, MULTI = HOTSEAT | NETWORK } type_t;
    typedef enum { NONE = 0, LOCAL = 1, REMOTE = 2, AI = 4 } control_t;

    control_t GetControl(u8);
    type_t GetType(u8);

    menu_t MainMenu(void);
    menu_t NewGame(void);
    menu_t LoadGame(void);
    menu_t HighScores(void);
    menu_t Credits(void);
    menu_t NewStandard(void);
    menu_t NewCampain(void);
    menu_t NewMulti(void);
    menu_t NewHotSeat(void);
    menu_t NewNetwork(void);
    menu_t LoadStandard(void);
    menu_t LoadCampain(void);
    menu_t LoadMulti(void);
    menu_t ScenarioInfo(void);
    menu_t SelectScenario(void);
    menu_t StartGame(void);

    menu_t NetworkHost(void);
    menu_t NetworkGuest(void);

    menu_t Testing(u8);

    void DrawInterface(void);
    
    bool ShouldAnimate(u32 ticket);
    bool ShouldAnimateInfrequent(u32 ticket, u32 modifier);
    void SetFixVideoMode(void);
    
    void PreloadLOOPSounds(void);
    void EnvironmentSoundMixer(void);

    u8  GetRating(void);
    u16 GetGameOverScores(void);

    void KeyboardGlobalFilter(u16, u16);
    void ShowLoadMapsText(void);

    namespace Scenario
    {
	void RedrawStaticInfo(const Point &);
	void RedrawOpponentsInfo(const Point &, const std::vector<Player> *players = NULL);
	void RedrawClassInfo(const Point &);
    };

    namespace Editor
    {
	Game::menu_t MainMenu(void);
	Game::menu_t NewMaps(void);
	Game::menu_t LoadMaps(void);
	Game::menu_t StartGame(void);
	Game::menu_t StartGame(void);
    };
};

#endif
