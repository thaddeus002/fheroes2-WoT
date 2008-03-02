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

#include "error.h"
#include "game.h"

Game::menu_t Game::NewStandard(void){ return Game::SCENARIOINFO; }

Game::menu_t Game::Credits(void){ Error::Verbose("Credits: under construction."); return Game::MAINMENU; }
Game::menu_t Game::NewCampain(void){ Error::Verbose("New Campain Game: under construction."); return Game::NEWGAME; }
Game::menu_t Game::NewMulti(void){ Error::Verbose("New Multi Game: under construction."); return Game::NEWGAME; }
Game::menu_t Game::LoadCampain(void){ Error::Verbose("Load Campain Game: under construction."); return Game::LOADGAME; }
Game::menu_t Game::LoadMulti(void){ Error::Verbose("Load Multi Game: under construction."); return Game::LOADGAME; }

