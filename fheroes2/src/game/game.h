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
        LOADSTANDARD,
        LOADCAMPAIN,
        LOADMULTI,
        SCENARIOINFO,
        SELECTSCENARIO,
	STARTGAME,
	SAVEGAME,
	EDITNEWMAP,
	EDITLOADMAP,
	EDITSAVEMAP,
	EDITSTART
    } menu_t;

    menu_t MainMenu(void);
    menu_t NewGame(void);
    menu_t LoadGame(void);
    menu_t HighScores(void);
    menu_t Credits(void);
    menu_t NewStandard(void);
    menu_t NewCampain(void);
    menu_t NewMulti(void);
    menu_t LoadStandard(void);
    menu_t LoadCampain(void);
    menu_t LoadMulti(void);
    menu_t ScenarioInfo(void);
    menu_t StartGame(void);

    void DrawInterface(void);
    
    void EnvironmentSoundMixer(bool forced = false);

    namespace Editor
    {
	Game::menu_t MainMenu(void);
	Game::menu_t NewMaps(void);
	Game::menu_t LoadMaps(void);
	Game::menu_t StartGame(const Maps::mapsize_t sizemap = Maps::SMALL);
    };
};

#endif
