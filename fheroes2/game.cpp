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

#include "display.h"
#include "cursor.h"
#include "error.h"
#include "game.h"

/* global event filter */
int Game::GlobalFilterEvents(const SDL_Event *event)
{
    // motion
    if(SDL_MOUSEMOTION == event->type){
    
	// redraw cursor
	if(Cursor::Visible()){ Cursor::Redraw(); display.Flip(); }

        return 1;
    }

    // key
    if(SDL_KEYDOWN == event->type)
	switch(event->key.keysym.sym){

	    case SDLK_F4:
		display.FullScreen();
    		return 0;

	    case SDLK_PRINT:
		if(display.SaveBMP("screenshot.bmp")) Error::Verbose("save: screenshot.bmp");
		return 0;
	    
	    default:
    		break;
	}

    return 1;
}

Game::menu_t Game::NewStandard(void){ return Game::SCENARIOINFO; }

Game::menu_t Game::Credits(void){ Error::Verbose("Credits: under construction."); return Game::MAINMENU; }
Game::menu_t Game::NewCampain(void){ Error::Verbose("New Campain Game: under construction."); return Game::NEWGAME; }
Game::menu_t Game::NewMulti(void){ Error::Verbose("New Multi Game: under construction."); return Game::NEWGAME; }
Game::menu_t Game::LoadCampain(void){ Error::Verbose("Load Campain Game: under construction."); return Game::LOADGAME; }
Game::menu_t Game::LoadMulti(void){ Error::Verbose("Load Multi Game: under construction."); return Game::LOADGAME; }

const Game::gamefocus_t & Game::GetFocus(void){ return Game::globalfocus; }
