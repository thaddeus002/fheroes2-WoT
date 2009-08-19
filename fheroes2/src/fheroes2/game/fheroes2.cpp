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
#include <climits>
#include <cstdlib>

#include "gamedefs.h"
#include "engine.h"
#include "settings.h"
#include "dir.h"
#include "agg.h"
#include "cursor.h"
#include "game.h"
#include "test.h"
#include "sdlnet.h"
#include "image_logo.h"
#include "image_icons.h"
#include "network.h"
#include "string_util.h"

int PrintHelp(const char *basename)
{
    std::cout << "Usage: " << basename << " [OPTIONS]" << std::endl \
	    << "  -e\teditors mode" << std::endl;
#ifndef BUILD_RELEASE
    std::cout << "  -d\tdebug mode" << std::endl;
#endif
#ifdef WITH_NET
    std::cout << "  -s\tdedicated server" << std::endl;
#endif
    std::cout << "  -c\tpath to config file (default fheroes2.cfg)" << std::endl \
	    << "  -h\tprint this help and exit" << std::endl;

    return EXIT_SUCCESS;
}

int main(int argc, char **argv)
{
	chdir(dirname_internal(argv[0]));

	Settings & conf = Settings::Get();
	int test = 0;

	std::string caption("Free Heroes II, version: ");
	String::AddInt(caption, conf.MajorVersion());
	caption += ".";
	String::AddInt(caption, conf.MinorVersion());

#ifndef BUILD_RELEASE
	caption += ", build: ";
	String::AddInt(caption, conf.DateBuild());
#endif

	std::cout << caption << std::endl;

	// load fheroes2.cfg
	const std::string & fheroes2_cfg = "fheroes2.cfg";
	std::cout << "config: " << fheroes2_cfg << (conf.Read(fheroes2_cfg) ? " load" : " not found") << std::endl;

	{
	    int opt;
	    while((opt = getopt(argc, argv, "hest:d:c:")) != -1)
    		switch(opt)
                {
                    case 'e':
			conf.SetModes(Settings::EDITOR);
			conf.SetDebug(3);

			std::cout << "start: editor mode." << std::endl;

			caption = "Free Heroes II (Editor), version: ";
			String::AddInt(caption, conf.MajorVersion());
			caption += ".";
			String::AddInt(caption, conf.MinorVersion());

#ifdef BUILD_RELEASE
			break;
#else
			caption += ", build: ";
			String::AddInt(caption, conf.DateBuild());
			break;
			
                    case 't':
			test = String::ToInt(optarg);
			break;
#endif

                    case 'd':
#ifndef BUILD_RELEASE
                	conf.SetDebug(optarg ? String::ToInt(optarg) : 0);
#endif
                	break;

                    case 'c':
                	std::cout << "config: " << optarg << (conf.Read(optarg) ? " load" : " not found") << std::endl;
                	break;

                    case 's':
#ifdef WITH_NET
                	      return Network::RunDedicatedServer();
#endif
                    case '?':
                    case 'h': return PrintHelp(argv[0]);

                    default:  break;
		}

	}

#ifdef WITH_TTF
	if(conf.Unicode())
	{
	    const std::string localesdir(conf.LocalDataPrefix() + SEPARATOR + "lang");
	    setlocale(LC_ALL, "");
	    bindtextdomain(GETTEXT_PACKAGE, localesdir.c_str());
	    bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
	    textdomain(GETTEXT_PACKAGE);
	}
#endif

	// random init
	Rand::Init();

	u32 subsystem = INIT_VIDEO;
        if(conf.Sound() || conf.Music())
            subsystem |= INIT_AUDIO;
        if(conf.Modes(Settings::MUSIC_CD))
            subsystem |= INIT_CDROM | INIT_AUDIO;
#ifdef WITH_NET
        subsystem |= INIT_TIMER;
        Network::SetProtocolVersion(static_cast<u16>((conf.MajorVersion() << 8)) | conf.MinorVersion());
#endif

	if(SDL::Init(subsystem))
	try
	{
	    std::atexit(SDL::Quit);

	    if(Mixer::isValid())
	    {
                Mixer::Volume(-1, conf.SoundVolume());
                Music::Volume(conf.MusicVolume());
	    }
	    else
	    if(conf.Sound() || conf.Music())
	    {
		conf.ResetModes(Settings::SOUND);
		conf.ResetModes(Settings::MUSIC);
	    }

	    Display::SetVideoMode(640, 480);
	    Display::HideCursor();
	    Display::SetCaption(caption);

    	    Surface icons(image_icons.pixel_data, image_icons.width, image_icons.height, image_icons.bytes_per_pixel, true);
	    Display::SetIcons(icons);

	    AGG::Cache & cache = AGG::Cache::Get();

	    // read data directory
    	    Dir dir;
    	
            dir.Read(conf.DataDirectory(), ".agg", false);

	    // not found agg, exit
	    if(0 == dir.size()) Error::Except("AGG data files not found.");

    	    // attach agg files
    	    for(Dir::const_iterator itd = dir.begin(); itd != dir.end(); ++itd) cache.AttachFile(*itd);

            if(conf.Debug()) conf.Dump();
            
            // load palette
	    cache.LoadPAL();

	    // load font
	    cache.LoadFNT();

#ifdef BUILD_RELEASE
	    // SDL logo
	    if(conf.Logo())
	    {
		Display & display = Display::Get();

    		Surface logo(image_logo.pixel_data, image_logo.width, image_logo.height, image_logo.bytes_per_pixel, false);
    		
    		logo.SetDisplayFormat();

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
		
		DELAY(500);

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
	    AGG::PreloadObject(ICN::ADVMCO);
	    Cursor::Get().SetThemes(Cursor::POINTER);

	    // default events
	    LocalEvent::SetStateDefaults();

	    // set global events
	    LocalEvent::SetGlobalFilterEvents(Cursor::Redraw);

	    // set prefix for store screenshot
	    caption = conf.LocalDataPrefix() + SEPARATOR + "save" + SEPARATOR + "screenshot_";
	    LocalEvent::SetScreenshotPrefix(caption);

	    // goto main menu
	    Game::menu_t rs = (test ? Game::TESTING : (conf.Editor() ? Game::EDITMAINMENU : Game::MAINMENU));

	    while(rs != Game::QUITGAME)
	    {
		switch(rs)
		{
	    		case Game::EDITMAINMENU:   rs = Game::Editor::MainMenu();	break;
	    		case Game::EDITNEWMAP:     rs = Game::Editor::NewMaps();	break;
	    		case Game::EDITLOADMAP:    rs = Game::Editor::LoadMaps();       break;
	    		case Game::EDITSTART:      rs = Game::Editor::StartGame();      break;
	    		case Game::MAINMENU:       rs = Game::MainMenu();		break;
	    		case Game::NEWGAME:        rs = Game::NewGame();		break;
	    		case Game::LOADGAME:       rs = Game::LoadGame();		break;
	    		case Game::HIGHSCORES:     rs = Game::HighScores();		break;
	    		case Game::CREDITS:        rs = Game::Credits();		break;
	    		case Game::NEWSTANDARD:    rs = Game::NewStandard();		break;
	    		case Game::NEWCAMPAIN:     rs = Game::NewCampain();		break;
	    		case Game::NEWMULTI:       rs = Game::NewMulti();		break;
			case Game::NEWHOTSEAT:     rs = Game::NewHotSeat();		break;
		        case Game::NEWNETWORK:     rs = Game::NewNetwork();		break;
	    		case Game::LOADSTANDARD:   rs = Game::LoadStandard();		break;
	    		case Game::LOADCAMPAIN:    rs = Game::LoadCampain();		break;
	    		case Game::LOADMULTI:      rs = Game::LoadMulti();		break;
	    		case Game::SCENARIOINFO:   rs = Game::ScenarioInfo();		break;
			case Game::STARTGAME:      rs = Game::StartGame();      	break;
		        case Game::TESTING:        rs = Game::Testing(test);		break;

	    		default: break;
		}
	    }

	    Display::ShowCursor();
	    if(Settings::Get().Modes(Settings::FADE)) Display::Fade();

	} catch(std::bad_alloc)
	{
	} catch(Error::Exception)
	{
	    conf.Dump();
	}

	std::cout << "Bye." << std::endl;
	
	return EXIT_SUCCESS;
}
