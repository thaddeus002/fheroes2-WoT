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

#include "error.h"
#include "settings.h"
#include "audio.h"
#include "game_focus.h"
#include "maps_tiles.h"
#include "ground.h"
#include "world.h"
#include "m82.h"
#include "mp2.h"
#include "agg.h"
#include "game.h"

Game::menu_t Game::NewStandard(void){ return Game::SCENARIOINFO; }

Game::menu_t Game::Credits(void){ Error::Verbose("Credits: under construction."); return Game::MAINMENU; }
Game::menu_t Game::NewCampain(void){ Error::Verbose("New Campain Game: under construction."); return Game::NEWGAME; }
Game::menu_t Game::NewMulti(void){ Error::Verbose("New Multi Game: under construction."); return Game::NEWGAME; }
Game::menu_t Game::LoadCampain(void){ Error::Verbose("Load Campain Game: under construction."); return Game::LOADGAME; }
Game::menu_t Game::LoadMulti(void){ Error::Verbose("Load Multi Game: under construction."); return Game::LOADGAME; }

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

	Audio::Mixer & mixer = Audio::Mixer::Get();

	std::map<M82::m82_t, u8> maps_sound;

        // reset all loop sound
	mixer.StopRepeate();

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
		    const u8 volume = (2 < length ? 2 : (1 < length ? 5 : (0 < length ? 7 : 10))) * MIX_MAXVOLUME / 10 * conf.SoundVolume() / 10;

		    M82::m82_t m82 = M82::UNKNOWN;
		    MP2::object_t object = tile.GetObject();
		    
		    if(MP2::OBJ_HEROES == object)
		    {
			const Heroes *hero = world.GetHeroes(xx, yy);
			
			if(hero) object = hero->GetUnderObject();
		    }

		    switch(object)
		    {
			case MP2::OBJ_BUOY:		m82 = M82::LOOP0000; break;
			case MP2::OBJ_DERELICTSHIP:
			case MP2::OBJ_SHIPWRECK:	m82 = M82::LOOP0001; break;
			case MP2::OBJ_COAST:		m82 = M82::LOOP0002; break;
			case MP2::OBJ_ORACLE:		m82 = M82::LOOP0003; break;
			case MP2::OBJ_STONELIGHTS:	m82 = M82::LOOP0004; break;
			//case MP2:::	m82 = M82::LOOP0005; break;
			case MP2::OBJ_LAVAPOOL:		m82 = M82::LOOP0006; break;
			case MP2::OBJ_ALCHEMYTOWER:	m82 = M82::LOOP0007; break;
			//case MP2:::	m82 = M82::LOOP0008; break;
			case MP2::OBJ_WATERMILL:	m82 = M82::LOOP0009; break;
			case MP2::OBJ_CAMPFIRE:		m82 = M82::LOOP0010; break;
			case MP2::OBJ_WINDMILL:		m82 = M82::LOOP0011; break;
			case MP2::OBJ_ARTESIANSPRING:
			case MP2::OBJ_FOUNTAIN:		m82 = M82::LOOP0012; break;
			case MP2::OBJ_WATERLAKE:
			case MP2::OBJ_CRAKEDLAKE:	m82 = M82::LOOP0013; break;
			//case MP2:::	m82 = M82::LOOP0014; break;
			case MP2::OBJ_MINES:		m82 = M82::LOOP0015; break;
			case MP2::OBJ_SAWMILL:		m82 = M82::LOOP0016; break;
			case MP2::OBJ_DAEMONCAVE:	m82 = M82::LOOP0017; break;
			case MP2::OBJ_SHRINE1:
			case MP2::OBJ_SHRINE2:
			case MP2::OBJ_SHRINE3:		m82 = M82::LOOP0018; break;
			//case MP2::OBJ_STONES:		if(Maps::Ground::WATER == tile.GetGround()) m82 = M82::LOOP0019; break;
			//case MP2:::	m82 = M82::LOOP0020; break;
			case MP2::OBJ_OILLAKE:		m82 = M82::LOOP0021; break;
			case MP2::OBJ_TRADINGPOST:	m82 = M82::LOOP0022; break;
			//case MP2:::	m82 = M82::LOOP0023; break;
			case MP2::OBJ_RUINS:		m82 = M82::LOOP0024; break;
			case MP2::OBJ_PEASANTHUNT:
			case MP2::OBJ_DWARFCOTT:
			case MP2::OBJ_ARCHERHOUSE:	m82 = M82::LOOP0025; break;
			//case MP2:::	m82 = M82::LOOP0026; break;
			case MP2::OBJ_VOLCANO:		m82 = M82::LOOP0027; break;

			default: break;
		    }

		    if(M82::UNKNOWN != m82 && volume > maps_sound[m82]) maps_sound[m82] = volume;
		}
	    }
	}


        // load sound
	std::map<M82::m82_t, u8>::const_iterator it1 = maps_sound.begin();
	std::map<M82::m82_t, u8>::const_iterator it2 = maps_sound.end();
	
	for(; it1 != it2; ++it1)
	    mixer.Play(AGG::Cache::Get().GetWAV((*it1).first), (*it1).second, Audio::Mixer::PLAY | Audio::Mixer::REPEATE | Audio::Mixer::ENHANCE);
    }
}
