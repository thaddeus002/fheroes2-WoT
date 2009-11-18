/***************************************************************************
 *   Copyright (C) 2009 by Andrey Afletdinov                               *
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

#ifdef WITH_NET

#include <vector>
#include <algorithm>

#include "agg.h"
#include "cursor.h"
#include "settings.h"
#include "dialog.h"
#include "world.h"
#include "castle.h"
#include "heroes.h"
#include "kingdom.h"
#include "game_interface.h"
#include "game_focus.h"
#include "localclient.h"

namespace Game
{
    menu_t HumanTurn(void);
};

bool FH2LocalClient::StartGame(void)
{
    Game::SetFixVideoMode();

    // cursor
    Cursor & cursor = Cursor::Get();
    Settings & conf = Settings::Get();
    Display & display = Display::Get();

    GameOver::Result::Get().Reset();

    cursor.Hide();

    AGG::FreeObject(ICN::HEROES);
    AGG::FreeObject(ICN::BTNSHNGL);
    AGG::FreeObject(ICN::SHNGANIM);
    AGG::FreeObject(ICN::BTNNEWGM);
    AGG::FreeObject(ICN::REDBACK);
    AGG::FreeObject(ICN::NGEXTRA);
    AGG::FreeObject(ICN::NGHSBKG);
    AGG::FreeObject(ICN::REQSBKG);
    AGG::FreeObject(ICN::REQUEST);
    AGG::FreeObject(ICN::REQUESTS);
    AGG::FreeObject(ICN::ESCROLL);
    AGG::FreeObject(ICN::HSBKG);
    AGG::FreeObject(ICN::HISCORE);

    if(Settings::Get().LowMemory())
    {
        AGG::ICNRegistryEnable(false);
        AGG::ICNRegistryFreeObjects();
    }

    // preload sounds
    Mixer::Reset();

    // draw interface
    Interface::Basic & I = Interface::Basic::Get();

    Interface::GameArea & areaMaps = I.gameArea;
    areaMaps.Build();

    Game::Focus & global_focus = Game::Focus::Get();
    global_focus.Reset();

    Interface::Radar & radar = I.radar;
    Interface::HeroesIcons & heroesBar = I.iconsPanel.GetHeroesBar();
    Interface::CastleIcons & castleBar = I.iconsPanel.GetCastleBar();
    Interface::StatusWindow& statusWin = I.statusWindow;
    heroesBar.Reset();
    castleBar.Reset();

    radar.Build();

    I.Redraw(REDRAW_ICONS | REDRAW_BUTTONS | REDRAW_BORDER);
    castleBar.Hide();
    heroesBar.Hide();

    cursor.Show();
    display.Flip();
    
    Kingdom & kingdom = world.GetMyKingdom();

    cursor.Hide();
    cursor.SetThemes(Cursor::WAIT);
    cursor.Show();
    display.Flip();

    Network::Message packet;

    while(LocalEvent::Get().HandleEvents())
    {
	if(Ready())
        {
	    if(!Recv(packet))
	    {
		Dialog::Message("Error", "FH2LocalClient::StartGame: recv: error", Font::BIG, Dialog::OK);
		return false;
            }
	    DEBUG(DBG_NETWORK , DBG_INFO, "FH2LocalClient::StartGame: recv: " << Network::GetMsgString(packet.GetID()));

            switch(packet.GetID())
            {
		case MSG_MAPS_LOAD:
		{
		    if(Game::IO::LoadBIN(packet))
		    {
			conf.SetMyColor(Color::Get(player_color));
			heroesBar.Reset();
			castleBar.Reset();
		    }
            	    else
            		DEBUG(DBG_NETWORK , DBG_WARN, "FH2LocalClient::StartGame: MSG_MAPS_LOAD error");
		}
		break;

		case MSG_YOUR_TURN:
		{
		    u8 color, percent;
		    packet.Pop(color);
		    packet.Pop(percent);

		    radar.HideArea();
		    conf.SetCurrentColor(Color::Get(color));
		    //Interface::Basic::SetTurnProgress(percent);

		    DEBUG(DBG_NETWORK , DBG_INFO, "FH2LocalClient::StartGame: player: " << Color::String(color));
		    world.ClearFog(color);

		    if(conf.MyColor() == color)
            	    {
			if(Game::ENDTURN == Game::HumanTurn())
			{
			    /*
			    Network::PackKingdom(packet, kingdom);
			    DEBUG(DBG_NETWORK, DBG_INFO, "FH2LocalClient::StartGame: send MSG_KINGDOM");
			    if(!Send(packet)) return false;
			    
			    // send all heroes
			    const std::vector<Heroes *> & heroes = kingdom.GetHeroes();
			    std::vector<Heroes *>::const_iterator ith1 = heroes.begin();
			    std::vector<Heroes *>::const_iterator ith2 = heroes.end();
			    for(; ith1 != ith2; ++ith1) if(*ith1)
			    {
				Network::PackHero(packet, **ith1);
				DEBUG(DBG_NETWORK, DBG_INFO, "FH2LocalClient::StartGame: send MSG_HEROES");
				if(!Send(packet)) return false;
			    }

			    // send all castles
			    const std::vector<Castle *> & castles = kingdom.GetCastles();
			    std::vector<Castle *>::const_iterator itc1 = castles.begin();
			    std::vector<Castle *>::const_iterator itc2 = castles.end();
			    for(; itc1 != itc2; ++itc1) if(*itc1)
			    {
				Network::PackCastle(packet, **itc1);
				DEBUG(DBG_NETWORK, DBG_INFO, "FH2LocalClient::StartGame: send MSG_CASTLE");
				if(!Send(packet)) return false;
			    }
			    */

			    packet.Reset();
			    packet.SetID(MSG_END_TURN);
			    DEBUG(DBG_NETWORK, DBG_INFO, "FH2LocalClient::StartGame: send MSG_END_TURN");
			    if(!Send(packet)) return false;
			}
			else
			    return true;
		    }
		    else
		    {
			if(STATUS_AITURN != statusWin.GetState())
			{
                    	    // for pocketpc: show status window
                    	    if(conf.PocketPC() && !conf.ShowStatus())
                    	    {
                        	conf.SetModes(Settings::SHOWSTATUS);
                    	    }

                    	    cursor.Hide();

			    statusWin.Reset();
                    	    statusWin.SetState(STATUS_AITURN);

                    	    I.SetRedraw(REDRAW_GAMEAREA | REDRAW_STATUS);

                    	    cursor.SetThemes(Cursor::WAIT);
                    	    I.Redraw();
                    	    cursor.Show();
                    	    display.Flip();
			}

		    }
		}
		break;

		//case MSG_KINGDOM:
		//{
		//    Network::UnpackKingdom(packet);
		//}
		//break;
		//
		//case MSG_TILES:
		//{
		//    Network::UnpackTile(packet);
		//}
		//break;
		//
		//case MSG_HEROES:
		//{
		//    Network::unpackHero(packet);
		//}
		//break;

		default: break;
	    }
	}
    }

    return true;
}

#endif
