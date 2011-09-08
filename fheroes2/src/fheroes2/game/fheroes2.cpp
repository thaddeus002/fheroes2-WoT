/***************************************************************************
 *   Copyright (C) 2009 by Andrey Afletdinov <fheroes2@gmail.com>          *
 *                                                                         *
 *   Part of the Free Heroes2 Engine:                                      *
 *   http://sourceforge.net/projects/fheroes2                              *
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

#include <unistd.h>
#include <iostream>
#include <string>

#include "gamedefs.h"
#include "engine.h"
#include "settings.h"
#include "dir.h"
#include "agg.h"
#include "cursor.h"
#include "game.h"
#include "test.h"
#include "sdlnet.h"
#include "images_pack.h"
#include "localclient.h"

#include "zzlib.h"

void LoadZLogo(void);
void SetVideoDriver(const std::string &);
void SetTimidityEnvPath(const Settings &);
void SetLangEnvPath(const Settings &);
void ReadConfigFile(Settings &);
void LoadConfigFiles(Settings &, const std::string &);
void ShowAGGError(void);

int PrintHelp(const char *basename)
{
    VERBOSE("Usage: " << basename << " [OPTIONS]");
#ifdef WITH_EDITOR
    VERBOSE("  -e\teditors mode");
#endif
#ifndef BUILD_RELEASE
    VERBOSE("  -d\tdebug mode");
#endif
#ifdef WITH_NET
    VERBOSE("  -s\tdedicated server");
#endif
    VERBOSE("  -h\tprint this help and exit");

    return EXIT_SUCCESS;
}

std::string GetCaption(void)
{
    return std::string("Free Heroes II, version: " + Settings::GetVersion());
}

bool RunEditor(const char* name)
{
    const char* feditor2 = "feditor2";
    return 0 == String::Lower(GetBasename(name)).compare(0, strlen(feditor2), feditor2);
}

int main(int argc, char **argv)
{
	Settings & conf = Settings::Get();
	int test = 0;

	DEBUG(DBG_ALL, DBG_INFO, "Free Heroes II, " + conf.GetVersion());

	LoadConfigFiles(conf, GetDirname(argv[0]));


#ifdef WITH_EDITOR
	if(RunEditor(argv[0])) conf.SetEditor();
#endif

	// getopt
	{
	    int opt;
	    while((opt = getopt(argc, argv, "hest:d:")) != -1)
    		switch(opt)
                {
#ifdef WITH_EDITOR
                    case 'e':
			conf.SetEditor();
			break;
#endif			
#ifndef BUILD_RELEASE
                    case 't':
			test = String::ToInt(optarg);
			break;

                    case 'd':
                	conf.SetDebug(optarg ? String::ToInt(optarg) : 0);
                	break;
#endif

#ifdef WITH_NET
                    case 's':
                	      return Network::RunDedicatedServer();
#endif
                    case '?':
                    case 'h': return PrintHelp(argv[0]);

                    default:  break;
		}
	}

	if(conf.SelectVideoDriver().size()) SetVideoDriver(conf.SelectVideoDriver());

	// random init
	Rand::Init();
        if(conf.Music()) SetTimidityEnvPath(conf);

	u32 subsystem = INIT_VIDEO | INIT_TIMER;

        if(conf.Sound() || conf.Music())
            subsystem |= INIT_AUDIO;
#ifdef WITH_AUDIOCD
        if(conf.MusicCD())
            subsystem |= INIT_CDROM | INIT_AUDIO;
#endif
#ifdef WITH_NET
        Network::SetProtocolVersion(static_cast<u16>(MAJOR_VERSION << 8) | MINOR_VERSION);
#endif

	if(SDL::Init(subsystem))
#ifndef ANDROID
	try
#endif
	{
	    std::atexit(SDL::Quit);

	    if(conf.Unicode()) SetLangEnvPath(conf);

	    if(Mixer::isValid())
	    {
		Mixer::SetChannels(8);
                Mixer::Volume(-1, Mixer::MaxVolume() * conf.SoundVolume() / 10);
                Music::Volume(Mixer::MaxVolume() * conf.MusicVolume() / 10);
                if(conf.Music())
		{
		    Music::SetFadeIn(3000);
		}
	    }
	    else
	    if(conf.Sound() || conf.Music())
	    {
		conf.ResetSound();
		conf.ResetMusic();
	    }

	    if(0 == conf.VideoMode().w || 0 == conf.VideoMode().h)
	    	conf.SetAutoVideoMode();

            Display::SetVideoMode(conf.VideoMode().w, conf.VideoMode().h, conf.DisplayFlags());

	    Display::HideCursor();
	    Display::SetCaption(GetCaption());

    	    //Ensure the mouse position is updated to prevent bad initial values.
    	    LocalEvent::Get().GetMouseCursor();

#ifdef WITH_ZLIB
    	    ZSurface zicons;
	    if(zicons.Load(_ptr_08067830.width, _ptr_08067830.height, _ptr_08067830.bpp, _ptr_08067830.pitch,
    		_ptr_08067830.rmask, _ptr_08067830.gmask, _ptr_08067830.bmask, _ptr_08067830.amask, _ptr_08067830.zdata, sizeof(_ptr_08067830.zdata)))
	    Display::SetIcons(zicons);
#endif
	    AGG::Cache & cache = AGG::Cache::Get();

	    // read data dir
	    if(! cache.ReadDataDir())
	    {
		DEBUG(DBG_GAME, DBG_WARN, "data files not found");
		ShowAGGError();
		return EXIT_FAILURE;
	    }

            DEBUG(DBG_GAME, DBG_INFO, conf.String());

            // load palette
	    cache.LoadPAL();

	    // load font
	    cache.LoadFNT();

#ifdef WITH_ZLIB
	    LoadZLogo();
#endif

	    // init cursor
	    Cursor::Get().SetThemes(Cursor::POINTER);
	    AGG::ICNRegistryEnable(true);

	    // init game data
	    Game::Init(argv);

	    // goto main menu
#ifdef WITH_EDITOR
	    Game::menu_t rs = (test ? Game::TESTING : (conf.Editor() ? Game::EDITMAINMENU : Game::MAINMENU));
#else
	    Game::menu_t rs = (test ? Game::TESTING : Game::MAINMENU);
#endif

	    while(rs != Game::QUITGAME)
	    {
		switch(rs)
		{
#ifdef WITH_EDITOR
	    		case Game::EDITMAINMENU:   rs = Game::Editor::MainMenu();	break;
	    		case Game::EDITNEWMAP:     rs = Game::Editor::NewMaps();	break;
	    		case Game::EDITLOADMAP:    rs = Game::Editor::LoadMaps();       break;
	    		case Game::EDITSTART:      rs = Game::Editor::StartGame();      break;
#endif
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
		        case Game::NEWBATTLEONLY:  rs = Game::NewBattleOnly();		break;
	    		case Game::LOADSTANDARD:   rs = Game::LoadStandard();		break;
	    		case Game::LOADCAMPAIN:    rs = Game::LoadCampain();		break;
	    		case Game::LOADMULTI:      rs = Game::LoadMulti();		break;
	    		case Game::SCENARIOINFO:   rs = Game::ScenarioInfo();		break;
	    		case Game::SELECTSCENARIO: rs = Game::SelectScenario();		break;
			case Game::STARTGAME:      rs = Game::StartGame();      	break;
		        case Game::TESTING:        rs = Game::Testing(test);		break;

	    		default: break;
		}
	    }

	    //Display::ShowCursor();
	    if(Settings::Get().ExtUseFade()) Display::Fade();

	}
#ifndef ANDROID
/*
	catch(std::bad_alloc)
	{
    	    AGG::Cache::Get().Dump();
	    DEBUG(DBG_GAME, DBG_WARN, "std::bad_alloc");
	}
*/
	catch(Error::Exception)
	{
    	    AGG::Cache::Get().Dump();
#ifdef WITH_NET
            if(conf.GameType(Game::TYPE_NETWORK)) FH2LocalClient::Get().Logout("internal error");
#endif
	    VERBOSE(std::endl << conf.String());
	}
#endif
	return EXIT_SUCCESS;
}

void LoadZLogo(void)
{
#ifdef BUILD_RELEASE
    // SDL logo
    if(Settings::Get().ExtShowSDL())
    {
	Display & display = Display::Get();

    	ZSurface* zlogo = new ZSurface();
	if(zlogo->Load(_ptr_0806f690.width, _ptr_0806f690.height, _ptr_0806f690.bpp, _ptr_0806f690.pitch,
    		_ptr_0806f690.rmask, _ptr_0806f690.gmask, _ptr_0806f690.bmask, _ptr_0806f690.amask, _ptr_0806f690.zdata, sizeof(_ptr_0806f690.zdata)))
	{
	    Surface* logo = zlogo;

	    // scale logo
	    if(Settings::Get().QVGA())
	    {
    		Surface* small = new Surface();
		Surface::ScaleMinifyByTwo(*small, *zlogo);
		delete zlogo;
		zlogo = NULL;
		logo = small;
	    }

    	    logo->SetDisplayFormat();

	    const u32 black = logo->MapRGB(0, 0, 0);
	    const Point offset((display.w() - logo->w()) / 2, (display.h() - logo->h()) / 2);

	    u8 ii = 0;

	    while(ii < 250)
	    {
		logo->SetAlpha(ii);
		display.Blit(*logo, offset);
		display.Flip();
		display.Fill(black);
		ii += 10;
	    }
		
	    DELAY(500);

	    while(ii > 0)
	    {
		logo->SetAlpha(ii);
		display.Blit(*logo, offset);
		display.Flip();
		display.Fill(black);
		ii -= 10;
	    }
	}
	if(zlogo) delete zlogo;
    }
#endif
}

void SetVideoDriver(const std::string & driver)
{
    setenv("SDL_VIDEODRIVER", driver.c_str(), 1);
}

void SetTimidityEnvPath(const Settings & conf)
{
    const std::string timidity = conf.LocalPrefix() + SEPARATOR + "files" + SEPARATOR + "timidity";
    if(FilePresent(timidity + SEPARATOR + "timidity.cfg"))
    {
	setenv("TIMIDITY_PATH", timidity.c_str(), 1);
    }
}

void SetLangEnvPath(const Settings & conf)
{
#ifdef WITH_TTF
    if(conf.ForceLang().size())
    {
	setenv("LANGUAGE", conf.ForceLang().c_str(), 1);
	setenv("LANG", conf.ForceLang().c_str(), 1);
    }

    const std::string strtmp = conf.LocalPrefix() + SEPARATOR + "files" + SEPARATOR + "lang";
    setlocale(LC_ALL, "en_US.UTF8");
    bindtextdomain(GETTEXT_PACKAGE, strtmp.c_str());
    bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
    textdomain(GETTEXT_PACKAGE);
#endif
}

void ReadConfigFile(Settings & conf)
{
    std::string strtmp = conf.LocalPrefix() + SEPARATOR + "fheroes2.cfg";
    if(FilePresent(strtmp))
    {
	VERBOSE("config: " << strtmp << " load.");
	conf.Read(strtmp);
    }
}

void LoadConfigFiles(Settings & conf, const std::string & dirname)
{
    // prefix from build
#ifdef CONFIGURE_FHEROES2_DATA
    conf.SetLocalPrefix(CONFIGURE_FHEROES2_DATA);
    if(conf.LocalPrefix().size()) ReadConfigFile(conf);
#endif

    // prefix from env
    if(getenv("FHEROES2_DATA"))
    {
	conf.SetLocalPrefix(getenv("FHEROES2_DATA"));
	ReadConfigFile(conf);
    }

    // prefix from dirname
    if(conf.LocalPrefix().empty() && dirname.size())
    {
	conf.SetLocalPrefix(dirname.c_str());
	ReadConfigFile(conf);
    }
}

void ShowAGGError(void)
{
    ZSurface zerr;
    if(zerr.Load(_ptr_080721d0.width, _ptr_080721d0.height, _ptr_080721d0.bpp, _ptr_080721d0.pitch,
    	    _ptr_080721d0.rmask, _ptr_080721d0.gmask, _ptr_080721d0.bmask, _ptr_080721d0.amask, _ptr_080721d0.zdata, sizeof(_ptr_080721d0.zdata)))
    {
	Display & display = Display::Get();
	LocalEvent & le = LocalEvent::Get();

	display.Blit(zerr, (display.w() - zerr.w()) / 2, (display.h() - zerr.h()) / 2);
	display.Flip();

	while(le.HandleEvents() && !le.KeyPress() && !le.MouseClickLeft());
    }
}
