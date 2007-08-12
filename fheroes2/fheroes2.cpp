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
#include "localevent.h"
#include "rand.h"
#include "agg.h"
#include "cursor.h"
#include "game.h"
#include "image_logo.h"
#include "image_icons.h"

int main(int argc, char **argv)
{
	std::string caption("Free Heroes II, version: ");
	String::AddInt(caption, MAJOR_VERSION);
	caption += ".";
	String::AddInt(caption, MINOR_VERSION);

#ifndef BUILD_RELEASE
	caption += ", build: ";
	String::AddInt(caption, BUILD_DATE);
#endif

	std::cout << caption << std::endl;

	// load defaults
	H2Config::Defaults();
	std::cout << "config: defaults load" << std::endl;

	// load fheroes2.cfg
	const std::string & fheroes2_cfg = "fheroes2.cfg";
	std::cout << "config: " << fheroes2_cfg << (H2Config::Load(fheroes2_cfg) ? " load" : " not found") << std::endl;

	{
	    // parse cmd params
	    CmdLine cmd((const char **) argv);

	    if(cmd.Exists('h'))
	    {
		std::cout << "Usage: " << argv[0] << " [OPTIONS]\n" \
		    << "  -e\teditors mode\n" \
		    << "  -d\tdebug mode\n" \
		    << "  -c\tpath to config file (default fheroes2.cfg)\n" \
		    << "  -h\tprint this help and exit" << std::endl;

		return EXIT_SUCCESS;
	    }

	    // load cmd config
	    if(cmd.Exists('c'))
	    {
		const std::string & cmd_config = cmd.GetValue('c');
		std::cout << "config: " << cmd_config << (H2Config::Load(cmd_config) ? " load" : " not found") << std::endl;
	    }

	    // set debug
	    if(cmd.Exists('d')) H2Config::SetDebug();

	    // editor mode
	    if(cmd.Exists('e'))
	    {
		H2Config::SetEditor();

		if(H2Config::Debug()) std::cout << "start: editing mode." << std::endl;
	    }
	}

	// random init
	Rand::Init();

	try {
	    // init SDL
	    if(0 > SDL_Init(SDL_INIT_VIDEO)) Error::Except(std::string(SDL_GetError()));
    	    atexit(SDL_Quit);

	    Display::SetVideoMode(Display::SMALL);
	    Display::HideCursor();
	    Display::SetCaption(caption);

    	    Surface icons(image_icons.pixel_data, image_icons.width, image_icons.height, image_icons.bytes_per_pixel, true);
	    Display::SetIcons(icons);

	    // init AGG
	    AGG::Init(H2Config::GetAGGFile());

	    // load palette
	    AGG::LoadPalette();

#ifdef BUILD_RELEASE
	    // SDL logo
	    if(H2Config::Logo())
	    {
    		Surface logo(image_logo.pixel_data, image_logo.width, image_logo.height, image_logo.bytes_per_pixel, false);

		const u32 black = logo.MapRGB(0, 0, 0);

		u8 ii = 0;

		while(ii < 250)
		{
		    logo.SetAlpha(ii);
		    display.Blit(logo);
		    display.Flip();
		    display.Fill(black);
		    ii += 10;
		}

		while(ii > 0)
		{
		    logo.SetAlpha(ii);
		    display.Blit(logo);
		    display.Flip();
		    display.Fill(black);
		    ii -= 10;
		}
	    }
#endif

	    // init cursor
	    Cursor::Init(Cursor::POINTER);

	    // enable events
	    LocalEvent::SetState(SDL_USEREVENT, true);
	
	    LocalEvent::SetState(SDL_KEYDOWN, true);
	    LocalEvent::SetState(SDL_KEYUP, true);
	    LocalEvent::SetState(SDL_MOUSEMOTION, true);
	    LocalEvent::SetState(SDL_MOUSEBUTTONDOWN, true);
	    LocalEvent::SetState(SDL_MOUSEBUTTONUP, true);
	    LocalEvent::SetState(SDL_QUIT, true);

	    // ignore events
	    LocalEvent::SetState(SDL_ACTIVEEVENT, false);
	    LocalEvent::SetState(SDL_JOYAXISMOTION, false);
	    LocalEvent::SetState(SDL_JOYBALLMOTION, false);
	    LocalEvent::SetState(SDL_JOYHATMOTION, false);
	    LocalEvent::SetState(SDL_JOYBUTTONUP, false);
	    LocalEvent::SetState(SDL_JOYBUTTONDOWN, false);
	    LocalEvent::SetState(SDL_SYSWMEVENT, false);
	    LocalEvent::SetState(SDL_VIDEORESIZE, false);
	    LocalEvent::SetState(SDL_VIDEOEXPOSE, false);

	    // set global events
	    SDL_SetEventFilter(Game::GlobalFilterEvents);

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

	} catch(std::bad_alloc)
	{
	} catch(Error::Exception)
	{
	    H2Config::PrintCurrentValues();
	}

	std::cout << "Bye." << std::endl;
	
	return EXIT_SUCCESS;
}
