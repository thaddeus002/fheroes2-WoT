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

#include <algorithm>
#include "dir.h"
#include "game.h"
#include "dialog.h"
#include "settings.h"
#include "remoteclient.h"
#include "localclient.h"
#include "cursor.h"
#include "button.h"
#include "world.h"
#include "agg.h"
#include "zzlib.h"

extern bool DialogSelectMapsFileList(MapsFileInfoList &, std::string &);

#ifdef WITH_NET

FH2LocalClient & FH2LocalClient::Get(void)
{
    static FH2LocalClient fh2localclient;

    return fh2localclient;
}

FH2LocalClient::FH2LocalClient()
{
    players.reserve(6);
}
    
bool FH2LocalClient::Connect(const std::string & srv, u16 port)
{
    server = srv;
    IPaddress ip;
    return Network::ResolveHost(ip, srv.c_str(), port) && Open(ip);
}

int FH2LocalClient::Logout(void)
{
    packet.Reset();
    packet.SetID(MSG_LOGOUT);
    packet.Push(player_name);
    Send(packet);
    DELAY(100);
    Close();
    modes = 0;
    return -1;
}

int FH2LocalClient::ConnectionChat(void)
{
    Settings & conf = Settings::Get();
    bool extdebug = 2 < conf.Debug();

    player_color = 0;
    player_race = 0;
    player_name.clear();
    player_id = 0;

    // recv ready, banner
    if(extdebug) std::cerr << "FH2LocalClient::ConnectionChat: recv ready...";
    if(!Wait(packet, MSG_READY, extdebug)) return -1;
    if(extdebug) std::cerr << "ok" << std::endl;

    // get banner
    std::string str;
    packet.Pop(str);

    // dialog: input name
    if(!Dialog::InputString("Connected to " + server + "\n \n" + str + "\n \nEnter player name:", player_name))
	return -1;

    // send hello
    packet.Reset();
    packet.SetID(MSG_HELLO);
    packet.Push(player_name);
    if(extdebug) std::cerr << "FH2LocalClient::ConnectionChat send hello...";
    if(!Send(packet, extdebug)) return -1;
    if(extdebug) std::cerr << "ok" << std::endl;

    // recv hello, modes, player_id
    if(extdebug) std::cerr << "FH2LocalClient::ConnectionChat: recv hello...";
    if(!Wait(packet, MSG_HELLO, extdebug)) return -1;
    if(extdebug) std::cerr << "ok" << std::endl;
    packet.Pop(modes);
    packet.Pop(player_id);
    packet.Pop(player_color);
    if(0 == player_id || 0 == player_color) Error::Warning("FH2LocalClient::ConnectionChat: player zero values");
    if(extdebug) std::cerr << "FH2LocalClient::ConnectionChat: " << (Modes(ST_ADMIN) ? "admin" : "client") << " mode" << std::endl;
    
    // send get maps info
    packet.Reset();
    packet.SetID(MSG_MAPS_INFO_GET);
    if(extdebug) std::cerr << "FH2LocalClient::ConnectionChat send get_maps_info...";
    if(!Send(packet, extdebug)) return -1;
    if(extdebug) std::cerr << "ok" << std::endl;

    // recv maps
    if(extdebug) std::cerr << "FH2LocalClient::ConnectionChat: recv maps_info...";
    if(!Wait(packet, MSG_MAPS_INFO, extdebug)) return -1;
    if(extdebug) std::cerr << "ok" << std::endl;

    Network::PacketPopMapsFileInfo(packet, conf.CurrentFileInfo());

    // send get players
    packet.Reset();
    packet.SetID(MSG_PLAYERS_GET);
    if(extdebug) std::cerr << "FH2LocalClient::ConnectionChat send get_players...";
    if(!Send(packet, extdebug)) return -1;
    if(extdebug) std::cerr << "ok" << std::endl;

    // recv players
    if(extdebug) std::cerr << "FH2LocalClient::ConnectionChat: recv players...";
    if(!Wait(packet, MSG_PLAYERS, extdebug)) return -1;
    if(extdebug) std::cerr << "ok" << std::endl;
    Network::PacketPopPlayersInfo(packet, players);

    // dialog scenario info
    if(Dialog::OK != ScenarioInfoDialog()) return -1;

    Cursor::Get().Hide();

    return 1;
}

int FH2LocalClient::ScenarioInfoDialog(void)
{
    Settings & conf = Settings::Get();
    bool extdebug = 2 < conf.Debug();
    bool change_players = false;
    bool update_info    = false;

    // draw info dialog
    Cursor & cursor = Cursor::Get();
    Display & display = Display::Get();
    LocalEvent & le = LocalEvent::Get();

    const Point pointPanel(204, 32);
    const Point pointOpponentInfo(pointPanel.x + 24, pointPanel.y + 202);
    const Point pointClassInfo(pointPanel.x + 24, pointPanel.y + 282);

    Game::Scenario::RedrawStaticInfo(pointPanel);
    Game::Scenario::RedrawOpponentsInfo(pointOpponentInfo, &players);
    Game::Scenario::RedrawClassInfo(pointClassInfo);

    Button buttonSelectMaps(pointPanel.x + 309, pointPanel.y + 45, ICN::NGEXTRA, 64, 65);
    Button buttonOk(pointPanel.x + 31, pointPanel.y + 380, ICN::NGEXTRA, 66, 67);
    Button buttonCancel(pointPanel.x + 287, pointPanel.y + 380, ICN::NGEXTRA, 68, 69);

    if(! Modes(ST_ADMIN & modes))
    {
	buttonOk.Press();
	buttonOk.SetDisable(true);
	buttonSelectMaps.Press();
	buttonSelectMaps.SetDisable(true);
    }

    buttonSelectMaps.Draw();
    buttonOk.Draw();
    buttonCancel.Draw();
    cursor.Show();
    display.Flip();

    bool exit = false;
    if(extdebug) std::cerr << "FH2LocalClient::ScenarioInfoDialog: start queue" << std::endl;

    while(!exit && le.HandleEvents())
    {
        if(Ready())
	{
	    if(extdebug) std::cerr << "FH2LocalClient::ScenarioInfoDialog: recv: ";
	    if(!Recv(packet, extdebug)) return Dialog::CANCEL;
	    if(extdebug) std::cerr << Network::GetMsgString(packet.GetID()) << std::endl;

	    switch(packet.GetID())
    	    {
		case MSG_READY:
		    break;

		case MSG_PLAYERS:
		{
		    Network::PacketPopPlayersInfo(packet, players);
		    conf.SetPlayersColors(Network::GetPlayersColors(players));
		    std::vector<Player>::iterator itp = std::find_if(players.begin(), players.end(), std::bind2nd(std::mem_fun_ref(&Player::isID), player_id));
		    if(itp != players.end())
		    {
			player_color = (*itp).player_color;
			player_race = (*itp).player_race;
		    }
		    update_info = true;
		    break;
		}

		case MSG_MAPS_INFO:
		    Network::PacketPopMapsFileInfo(packet, conf.CurrentFileInfo());
		    update_info = true;
		    break;

		case MSG_MESSAGE:
		    break;

		case MSG_SHUTDOWN:
		    exit = true;
		    break;

		case MSG_MAPS_LOAD:
		    cursor.Hide();
        	    display.Fill(0, 0, 0);
            	    TextBox(_("Maps Loading..."), Font::BIG, Rect(0, display.h()/2, display.w(), display.h()/2));
                    display.Flip();

		    if(Game::IO::LoadBIN(packet))
		    {
			conf.SetMyColor(Color::Get(player_color));
			return Dialog::OK;
		    }
		    else
		    {
			if(extdebug) std::cerr << "FH2LocalClient::ScenarioInfoDialog: send maps_load_err...";
			return Dialog::CANCEL;
		    }
		    break;

		default: break;
	    }
	}

	if(update_info)
	{
	    cursor.Hide();
	    Game::Scenario::RedrawStaticInfo(pointPanel);
	    Game::Scenario::RedrawOpponentsInfo(pointOpponentInfo, &players);
	    Game::Scenario::RedrawClassInfo(pointClassInfo);
	    buttonSelectMaps.Draw();
	    buttonOk.Draw();
	    buttonCancel.Draw();
	    cursor.Show();
	    display.Flip();
	    update_info = false;
	}

	if(change_players)
	{
	/*
	    packet.Reset();
	    packet.SetID(MSG_PLAYERS);
	    Network::PacketPushPlayersInfo(packet, players);

	    if(extdebug) std::cerr << "FH2LocalClient::ScenarioInfoDialog: send players...";
	    if(!Send(packet, extdebug)) return Dialog::CANCEL;
	    if(extdebug) std::cerr << "ok" << std::endl;

	    conf.SetPlayersColors(Network::GetPlayersColors(players));
	*/
	    change_players = false;
	    update_info = true;
	}

	// press button
        if(buttonSelectMaps.isEnable()) le.MousePressLeft(buttonSelectMaps) ? buttonSelectMaps.PressDraw() : buttonSelectMaps.ReleaseDraw();
        if(buttonOk.isEnable()) le.MousePressLeft(buttonOk) ? buttonOk.PressDraw() : buttonOk.ReleaseDraw();
        le.MousePressLeft(buttonCancel) ? buttonCancel.PressDraw() : buttonCancel.ReleaseDraw();

	// click select
	if(le.KeyPress(KEY_s) || (buttonSelectMaps.isEnable() && le.MouseClickLeft(buttonSelectMaps)))
	{
	    // recv maps_info_list
	    packet.Reset();
	    packet.SetID(MSG_MAPS_LIST_GET);
	    if(extdebug) std::cerr << "FH2LocalClient::ScenarioInfoDialog: send get_maps_list...";
	    if(!Send(packet, extdebug)) return Dialog::CANCEL;
	    if(extdebug) std::cerr << "ok" << std::endl;

	    if(extdebug) std::cerr << "FH2LocalClient::ScenarioInfoDialog: recv maps_list...";
	    if(!Wait(packet, MSG_MAPS_LIST, extdebug)) return Dialog::CANCEL;
	    if(extdebug) std::cerr << "ok" << std::endl;

	    MapsFileInfoList lists;
	    PacketPopMapsFileInfoList(packet, lists);

            std::string filemaps;
            if(DialogSelectMapsFileList(lists, filemaps) && filemaps.size())
            {
		// send set_maps_info
	        packet.Reset();
		packet.SetID(MSG_MAPS_INFO_SET);
	        packet.Push(filemaps);
		if(extdebug) std::cerr << "FH2LocalClient::ScenarioInfoDialog: send maps_info...";
	        if(!Send(packet, extdebug)) return Dialog::CANCEL;
		if(extdebug) std::cerr << "ok" << std::endl;
	    }
	    update_info = true;
	}
	else
	// click cancel
        if(le.MouseClickLeft(buttonCancel) || le.KeyPress(KEY_ESCAPE))
    	    return Dialog::CANCEL;
        else
        // click ok
        if(le.KeyPress(KEY_RETURN) || (buttonOk.isEnable() && le.MouseClickLeft(buttonOk)))
    	{
           packet.Reset();
           packet.SetID(MSG_MAPS_LOAD);
           if(extdebug) std::cerr << "FH2LocalClient::ScenarioInfoDialog: send maps_load...";
           if(!Send(packet, extdebug)) return Dialog::CANCEL;
           if(extdebug) std::cerr << "ok" << std::endl;
	}

        DELAY(10);
    }

    return Dialog::CANCEL;
}

Game::menu_t Game::NetworkGuest(void)
{
    Settings & conf = Settings::Get();
    Display & display = Display::Get();
    Cursor & cursor = Cursor::Get();
    
    // clear background
    const Sprite &back = AGG::GetICN(ICN::HEROES, 0);
    cursor.Hide();
    display.Blit(back);
    cursor.Show();
    display.Flip();

    std::string server;
    if(!Dialog::InputString("Server Name", server)) return MAINMENU;

    FH2LocalClient & client = FH2LocalClient::Get();

    if(! client.Connect(server, conf.GetPort()))
    {
        Dialog::Message(_("Error"), Network::GetError(), Font::BIG, Dialog::OK);
	return Game::MAINMENU;
    }
    conf.SetGameType(Game::NETWORK);

    if(0 < client.ConnectionChat())
    {
	StartGame();
    }

    client.Logout();

    return QUITGAME;
}

#else
Game::menu_t Game::NetworkGuest(void)
{
    Dialog::Message(_("Error"), _("This release is compiled without network support."), Font::BIG, Dialog::OK);
    return MAINMENU;
}
#endif
