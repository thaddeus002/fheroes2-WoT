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

#include <map>
#include <sstream>
#include <ctime>
#include <cstdlib>
#include <algorithm>

#include "gamedefs.h"
#include "settings.h"
#include "game_focus.h"
#include "maps_tiles.h"
#include "ground.h"
#include "world.h"
#include "mp2.h"
#include "agg.h"
#include "test.h"
#include "game.h"

namespace Game
{
    u8 GetMixerChannelFromObject(const Maps::Tiles &);

    static std::vector<u8> reserved_vols(LOOPXX_COUNT, 0);
    static u8 ai_turn_progress;
};

Game::menu_t Game::Testing(u8 t)
{
#ifndef BUILD_RELEASE
    Test::Run(t);
    return Game::QUITGAME;
#else
    return Game::MAINMENU;
#endif
}

Game::menu_t Game::Credits(void)
{
    const Settings & conf = Settings::Get();

    std::string str = conf.BuildVersion();

    str.append("\n \n");
    str.append("Site project: https://sourceforge.net/projects/fheroes2");

    Dialog::Message("Free Heroes II Engine", str, Font::SMALL, Dialog::OK);

    //Error::Verbose("Credits: under construction.");

    return Game::MAINMENU;
}

void Game::SetFixVideoMode(void)
{
    const Settings & conf = Settings::Get();

    Size fixsize(conf.VideoMode());
    u32 max = 0;

    switch(conf.MapsWidth())
    {
	default:
	case Maps::SMALL:
	    max = (6 + 36) * TILEWIDTH;
	    break;

	case Maps::MEDIUM:
	    max = (6 + 72) * TILEWIDTH;
	    break;

	case Maps::LARGE:
	    max = (6 + 108) * TILEWIDTH;
	    break;

	case Maps::XLARGE:
	    max = (6 + 144) * TILEWIDTH;
	    break;
    }

    if(conf.VideoMode().w > max) fixsize.w = max;
    if(conf.VideoMode().h > max) fixsize.h = max;

    Display::Get().SetVideoMode(fixsize, conf.FullScreen());
}

/* play all sound from focus area game */
void Game::EnvironmentSoundMixer(void)
{
    const Focus & focus = Focus::Get();
    const Point & abs_pt = focus.Center();
    const Settings & conf = Settings::Get();

    if(conf.Sound())
    {
	std::vector<u8> & vols = reserved_vols;
	std::fill(vols.begin(), vols.end(), 0);

        // scan 4x4 square from focus
        for(s16 yy = abs_pt.y - 3; yy <= abs_pt.y + 3; ++yy)
    	{
    	    for(s16 xx = abs_pt.x - 3; xx <= abs_pt.x + 3; ++xx)
	    {
		if(Maps::isValidAbsPoint(xx, yy))
		{
		    const u8 channel = GetMixerChannelFromObject(world.GetTiles(xx, yy));
    		    if(channel < vols.size())
		    {
			// calculation volume
    			const u8 length = std::max(std::abs(xx - abs_pt.x), std::abs(yy - abs_pt.y));
			const u8 volume = (2 < length ? 2 : (1 < length ? 5 : (0 < length ? 7 : 10))) * conf.SoundVolume() / 10;

			if(volume > vols[channel]) vols[channel] = volume;
		    }
		}
	    }
	}

	AGG::Cache::Get().LoadLOOPXXSounds(vols);
    }
}

u8 Game::GetMixerChannelFromObject(const Maps::Tiles & tile)
{
    // force: check stream
    if(tile.isStream()) return 13;

    MP2::object_t object = tile.GetObject();
    if(MP2::OBJ_HEROES == object)
    {
	const Heroes* hero = world.GetHeroes(tile.GetIndex());
	object = hero->GetUnderObject();
    }

    return M82::GetIndexLOOP00XXFromObject(object);
}

Game::control_t Game::GetControl(u8 index)
{
    switch(index)
    {
	case LOCAL:	return LOCAL;
	case REMOTE:	return REMOTE;
	case AI:	return AI;
	default: break;
    }
    return NONE;
}

Game::type_t Game::GetType(u8 index)
{
    switch(index)
    {
        case STANDARD:  return STANDARD;
        case CAMPAIGN:  return CAMPAIGN;
        case HOTSEAT:   return HOTSEAT;
        case NETWORK:   return NETWORK;
	default: break;
    }
    return UNKNOWN;
}

u8 Game::GetRating(void)
{
    Settings & conf = Settings::Get();
    u8 rating = 50;

    switch(conf.MapsDifficulty())
    {
        case Difficulty::NORMAL:     rating += 20; break;
        case Difficulty::HARD:       rating += 40; break;
        case Difficulty::EXPERT:
        case Difficulty::IMPOSSIBLE:	rating += 80; break;
	default: break;
    }

    switch(conf.GameDifficulty())
    {
        case Difficulty::NORMAL:     rating += 30; break;
        case Difficulty::HARD:       rating += 50; break;
        case Difficulty::EXPERT:	rating += 70; break;
        case Difficulty::IMPOSSIBLE:	rating += 90; break;
	default: break;
    }

    return rating;
}

u16 Game::GetGameOverScores(void)
{
    Settings & conf = Settings::Get();

    u8 k_size = 0;

    switch(conf.MapsWidth())
    {
	case Maps::SMALL:	k_size = 140; break;
	case Maps::MEDIUM:	k_size = 100; break;
	case Maps::LARGE:	k_size =  80; break;
	case Maps::XLARGE:	k_size =  60; break;
	default: break;
    }

    u8 flag = 0;
    u8 nk = 0;
    u16 end_days = world.CountDay();

    for(u16 ii = 1; ii <= end_days; ++ii)
    {
	nk = ii * k_size / 100;

	if(0 == flag && nk > 60){ end_days = ii + (world.CountDay() - ii) / 2; flag = 1; }
	else
	if(1 == flag && nk > 120) end_days = ii + (world.CountDay() - ii) / 2;
	else
	if(nk > 180) break;
    }

    return GetRating() * (200 - nk) / 100;
}

void Game::KeyboardGlobalFilter(u16 sym, u16 mod)
{
    Display & display = Display::Get();

    switch(sym)
    {
	case SDLK_F4:
    	    display.FullScreen();
            break;

	case SDLK_PRINT:
        {
            std::ostringstream stream;
            stream << Settings::Get().LocalPrefix() << SEPARATOR << "files" << SEPARATOR << "save" << SEPARATOR << "screenshot_" << std::time(0);
#ifndef WITH_IMAGE
                stream << ".bmp";
#else
                stream << ".png";
#endif
	    if(display.Save(stream.str().c_str())) Error::Verbose("save: " + stream.str());
        }
	    break;

	default:
	    break;
    }
}

void Game::ShowLoadMapsText(void)
{
    Display & display = Display::Get();
    display.Fill(0, 0, 0);
    TextBox(_("Maps Loading..."), Font::BIG, Rect(0, display.h()/2, display.w(), display.h()/2));
    display.Flip();
}

void Game::SetAIProgress(u8 v)
{
    ai_turn_progress = v;
}

u8 Game::GetAIProgress(void)
{
    return ai_turn_progress;
}
