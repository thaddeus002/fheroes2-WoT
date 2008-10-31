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

#include "maps.h"

#define RADARWIDTH	144
#define BORDERWIDTH	16

namespace Game
{
    typedef enum
    {
	CANCEL,
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
	ENDTURN
    } menu_t;

    typedef enum { UNKNOWN = 0, STANDARD = 1, CAMPAIGN = 2, HOTSEAT = 4, NETWORK = 8, MULTI = HOTSEAT | NETWORK } type_t;
    typedef enum { LOCAL, REMOTE, AI } control_t;

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
    menu_t StartGame(void);

    void DrawInterface(void);
    
    bool ShouldAnimate(u32 ticket);
    bool ShouldAnimateInfrequent(u32 ticket, u32 modifier);
    void SetFixVideoMode(void);
    
    void PreloadLOOPSounds(void);
    void EnvironmentSoundMixer(bool forced);

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
