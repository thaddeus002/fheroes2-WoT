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
#include <cstdlib>

#include "gamedefs.h"
#include "settings.h"
#include "game_focus.h"
#include "maps_tiles.h"
#include "ground.h"
#include "world.h"
#include "mp2.h"
#include "agg.h"
#include "game.h"

Game::menu_t Game::NewStandard(void){ Settings::Get().SetGameType(Game::STANDARD); return Game::SCENARIOINFO; }
Game::menu_t Game::NewHotSeat(void){ Settings::Get().SetGameType(Game::HOTSEAT); return Game::SCENARIOINFO; }
Game::menu_t Game::Credits(void){ Error::Verbose("Credits: under construction."); return Game::MAINMENU; }
Game::menu_t Game::NewCampain(void){ Settings::Get().SetGameType(Game::CAMPAIGN); Error::Verbose("New Campain Game: under construction."); return Game::NEWGAME; }
Game::menu_t Game::NewNetwork(void){ Settings::Get().SetGameType(Game::NETWORK); Error::Verbose("New Network Game: under construction."); return Game::NEWMULTI; }
Game::menu_t Game::LoadCampain(void){ Settings::Get().SetGameType(Game::CAMPAIGN); Error::Verbose("Load Campain Game: under construction."); return Game::LOADGAME; }
Game::menu_t Game::LoadMulti(void){ Error::Verbose("Load Multi Game: under construction."); return Game::LOADGAME; }

void Game::SetFixVideoMode(void)
{
    const Settings & conf = Settings::Get();

    Size fixsize(conf.VideoMode());
    u32 max = 0;

    switch(conf.FileInfo().SizeMaps())
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
void Game::EnvironmentSoundMixer(bool forced)
{
    static u16 previous_index = MAXU16;

    const Focus & focus = Focus::Get();
    const Point & abs_pt = focus.Center();
    const Settings & conf = Settings::Get();

    const u16 new_index = Maps::GetIndexFromAbsPoint(abs_pt);

    if(conf.Sound() && (previous_index != new_index || forced))
    {
	previous_index = new_index;

	std::vector<u8> vols(CHANNEL_RESERVED, 0);

        Mixer::PauseLoops();

        AGG::PlayMusic(MUS::FromGround(world.GetTiles(new_index).GetGround()));
        
        // scan 4x4 square from focus
        for(s16 yy = abs_pt.y - 3; yy <= abs_pt.y + 3; ++yy)
    	{
    	    for(s16 xx = abs_pt.x - 3; xx <= abs_pt.x + 3; ++xx)
	    {
		if(Maps::isValidAbsPoint(xx, yy))
		{
		    const Maps::Tiles & tile = world.GetTiles(xx, yy);
		    
    		    // calculation volume
    		    const u8 length = std::max(std::abs(xx - abs_pt.x), std::abs(yy - abs_pt.y));
		    const u16 volume = (2 < length ? 2 : (1 < length ? 5 : (0 < length ? 7 : 10))) * MAXVOLUME * conf.SoundVolume() / 100;
		    u8 channel = 0xFF;
		    MP2::object_t object = tile.GetObject();
		    
		    if(MP2::OBJ_HEROES == object)
		    {
			const Heroes *hero = world.GetHeroes(xx, yy);
			
			if(hero) object = hero->GetUnderObject();
		    }

		    // ext: stream
		    if(tile.isStream() && volume > vols[11]) vols[11] = volume;

		    switch(object)
		    {
			case MP2::OBJ_BUOY:		channel = 0; break;
			case MP2::OBJ_SHIPWRECK:
			case MP2::OBJ_DERELICTSHIP:	channel = 1; break;
			case MP2::OBJ_COAST:		channel = 2; break;
			case MP2::OBJ_ORACLE:		channel = 3; break;
			case MP2::OBJ_STONELIGHTS:	channel = 4; break;
			case MP2::OBJ_LAVAPOOL:		channel = 5; break;
			case MP2::OBJ_ALCHEMYLAB:	channel = 6; break;
			case MP2::OBJ_WATERWHEEL:	channel = 7; break;
			case MP2::OBJ_CAMPFIRE:		channel = 8; break;
			case MP2::OBJ_WINDMILL:		channel = 9; break;
			case MP2::OBJ_ARTESIANSPRING:
			case MP2::OBJ_FOUNTAIN:		channel = 10; break;
			case MP2::OBJ_WATERLAKE:
			case MP2::OBJ_WATERINGHOLE:	channel = 11; break;
			case MP2::OBJ_MINES:		channel = 12; break;
			case MP2::OBJ_SAWMILL:		channel = 13; break;
			case MP2::OBJ_DAEMONCAVE:	channel = 14; break;
			case MP2::OBJ_SHRINE1:
			case MP2::OBJ_SHRINE2:
			case MP2::OBJ_SHRINE3:		channel = 15; break;
			case MP2::OBJ_TARPIT:		channel = 16; break;
			case MP2::OBJ_TRADINGPOST:	channel = 17; break;
			case MP2::OBJ_RUINS:		channel = 18; break;
			case MP2::OBJ_PEASANTHUT:
			case MP2::OBJ_DWARFCOTT:
			case MP2::OBJ_ARCHERHOUSE:	channel = 19; break;
			case MP2::OBJ_VOLCANO:		channel = 20; break;

			default: continue;
		    }

		    if(volume > vols[channel]) vols[channel] = volume;
		}
	    }
	}

	for(u8 ch = 0; ch < vols.size(); ++ch)
	    Mixer::Volume(ch, vols[ch]);
        
        Mixer::ResumeLoops();
    }
}

void Game::PreloadLOOPSounds(void)
{
    if(! Settings::Get().Sound()) return;

    Mixer::LoadRAW(AGG::Cache::Get().GetWAV(M82::LOOP0000), true, 0);
    Mixer::LoadRAW(AGG::Cache::Get().GetWAV(M82::LOOP0001), true, 1);
    Mixer::LoadRAW(AGG::Cache::Get().GetWAV(M82::LOOP0002), true, 2);
    Mixer::LoadRAW(AGG::Cache::Get().GetWAV(M82::LOOP0003), true, 3);
    Mixer::LoadRAW(AGG::Cache::Get().GetWAV(M82::LOOP0004), true, 4);
    Mixer::LoadRAW(AGG::Cache::Get().GetWAV(M82::LOOP0006), true, 5);
    Mixer::LoadRAW(AGG::Cache::Get().GetWAV(M82::LOOP0007), true, 6);
    Mixer::LoadRAW(AGG::Cache::Get().GetWAV(M82::LOOP0009), true, 7);
    Mixer::LoadRAW(AGG::Cache::Get().GetWAV(M82::LOOP0010), true, 8);
    Mixer::LoadRAW(AGG::Cache::Get().GetWAV(M82::LOOP0011), true, 9);
    Mixer::LoadRAW(AGG::Cache::Get().GetWAV(M82::LOOP0012), true, 10);
    Mixer::LoadRAW(AGG::Cache::Get().GetWAV(M82::LOOP0013), true, 11);
    Mixer::LoadRAW(AGG::Cache::Get().GetWAV(M82::LOOP0015), true, 12);
    Mixer::LoadRAW(AGG::Cache::Get().GetWAV(M82::LOOP0016), true, 13);
    Mixer::LoadRAW(AGG::Cache::Get().GetWAV(M82::LOOP0017), true, 14);
    Mixer::LoadRAW(AGG::Cache::Get().GetWAV(M82::LOOP0018), true, 15);
    Mixer::LoadRAW(AGG::Cache::Get().GetWAV(M82::LOOP0021), true, 16);
    Mixer::LoadRAW(AGG::Cache::Get().GetWAV(M82::LOOP0022), true, 17);
    Mixer::LoadRAW(AGG::Cache::Get().GetWAV(M82::LOOP0024), true, 18);
    Mixer::LoadRAW(AGG::Cache::Get().GetWAV(M82::LOOP0025), true, 19);
    Mixer::LoadRAW(AGG::Cache::Get().GetWAV(M82::LOOP0027), true, 20);

    if(Settings::Get().Debug()) Error::Verbose("Game::PreloadLOOPSoundToMixer: done.");
}
