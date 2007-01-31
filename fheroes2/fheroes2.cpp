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


#include <iostream>
#include <string>

#include "gamedefs.h"
#include "cmdline.h"
#include "tools.h"
#include "config.h"
#include "error.h"
#include "rand.h"
#include "agg.h"
#include "cursor.h"
#include "game.h"

int main(int argc, char **argv)
{
	std::string caption("Free Heroes II, version: ");
	String::AddInt(caption, GAME_VERSION);

	std::cout << caption << std::endl;

	{
	    // parse cmd params
	    CmdLine cmd((const char **) argv);

	    if(cmd.Exists('h')){

		std::cout << "Usage: " << argv[0] << " [OPTIONS]\n" \
		    << "  -d\tdebug mode\n" \
		    << "  -c\tpath to config file (default fheroes2.cfg)\n" \
		    << "  -h\tprint this help and exit" << std::endl;

		return EXIT_SUCCESS;
	    }
	
	    // init config
	    cmd.Exists('c') ? H2Config::Init(cmd.GetValue('c')) : H2Config::Init("fheroes2.cfg");

	    // set debug
	    if(cmd.Exists('d')) H2Config::SetDebug();
	}

	// random init
	Rand::Init();

	try {
	    // init Display
	    if(!SDLmm::Display::Init()) Error::Except(SDLmm::GetError());
    	    atexit(SDLmm::Display::Quit);

	    Display::SetVideoMode(Display::SMALL);
	    Display::HideCursor();
	    Display::SetCaption(caption);

	    // init AGG
	    AGG::Init(H2Config::GetAGGFile());

	    // load palette
	    AGG::LoadPalette();

	    // init cursor
	    Cursor::Init(Cursor::POINTER);

	    // enable events
	    SDLmm::Event::EventState(SDL_USEREVENT, SDL_ENABLE);
	    SDLmm::Event::EventState(SDL_KEYDOWN, SDL_ENABLE);
	    SDLmm::Event::EventState(SDL_KEYUP, SDL_ENABLE);
	    SDLmm::Event::EventState(SDL_MOUSEMOTION, SDL_ENABLE);
	    SDLmm::Event::EventState(SDL_MOUSEBUTTONDOWN, SDL_ENABLE);
	    SDLmm::Event::EventState(SDL_MOUSEBUTTONUP, SDL_ENABLE);
	    SDLmm::Event::EventState(SDL_QUIT, SDL_ENABLE);

	    // ignore events
	    SDLmm::Event::EventState(SDL_ACTIVEEVENT, SDL_IGNORE);
	    SDLmm::Event::EventState(SDL_JOYAXISMOTION, SDL_IGNORE);
	    SDLmm::Event::EventState(SDL_JOYBALLMOTION, SDL_IGNORE);
	    SDLmm::Event::EventState(SDL_JOYHATMOTION, SDL_IGNORE);
	    SDLmm::Event::EventState(SDL_JOYBUTTONUP, SDL_IGNORE);
	    SDLmm::Event::EventState(SDL_JOYBUTTONDOWN, SDL_IGNORE);
	    SDLmm::Event::EventState(SDL_SYSWMEVENT, SDL_IGNORE);
	    SDLmm::Event::EventState(SDL_VIDEORESIZE, SDL_IGNORE);
	    SDLmm::Event::EventState(SDL_VIDEOEXPOSE, SDL_IGNORE);

	    // set global events
	    SDLmm::Event::SetEventFilter(Game::GlobalFilterEvents);

	    // goto main menu
	    Game::menu_t rs = Game::MAINMENU;

	    while(rs != Game::QUITGAME)

		switch(rs){
	    	    case Game::MAINMENU:       rs = Game::MainMenu();		break;
	    	    case Game::NEWGAME:        rs = Game::NewGame();		break;
	    	    case Game::LOADGAME:       rs = Game::LoadGame();		break;
	    	    case Game::HIGHSCORES:     rs = Game::HighScores();		break;
	    	    case Game::CREDITS:        rs = Game::Credits();		break;
	    	    case Game::NEWSTANDARD:    rs = Game::NewStandard();	break;
	    	    case Game::NEWCAMPAIN:     rs = Game::NewCampain();		break;
	    	    case Game::NEWMULTI:       rs = Game::NewMulti();		break;
	    	    case Game::LOADSTANDARD:   rs = Game::LoadStandard();	break;
	    	    case Game::LOADCAMPAIN:    rs = Game::LoadCampain();	break;
	    	    case Game::LOADMULTI:      rs = Game::LoadMulti();		break;
	    	    case Game::SCENARIOINFO:   rs = Game::ScenarioInfo();	break;
		    case Game::STARTGAME:      rs = Game::StartGame();          break;
	    	    default: break;
		}

	    Display::ShowCursor();

	} catch(std::bad_alloc){

	} catch(Error::Exception){

	}

	std::cout << "Bye." << std::endl;
	
	return EXIT_SUCCESS;
}
