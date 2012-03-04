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

#include <algorithm>
#include "dir.h"
#include "game.h"
#include "dialog.h"
#include "settings.h"
#include "network.h"
#include "remoteclient.h"
#include "localclient.h"
#include "cursor.h"
#include "button.h"
#include "world.h"
#include "agg.h"
#include "race.h"
#include "zzlib.h"
#include "dialog_selectscenario.h"
#include "server.h"

#ifdef WITH_NET

// game_scenarioinfo.cpp
void RedrawScenarioStaticInfo(const Rect &);



FH2LocalClient & FH2LocalClient::Get(void)
{
    static FH2LocalClient fh2localclient;

    return fh2localclient;
}

FH2LocalClient::FH2LocalClient() : admin_id(0)
{
}

u8 GetPlayersColors(const std::vector<Player> & v)
{
    u8 res = 0;
    std::vector<Player>::const_iterator it1 = v.begin();
    std::vector<Player>::const_iterator it2 = v.end();
    for(; it1 != it2; ++it1) if((*it1).id && (*it1).color) res |= (*it1).color;

    return res;
}

bool FH2LocalClient::Connect(const std::string & srv, u16 port)
{
    server = srv;
    IPaddress ip;
    if(Network::ResolveHost(ip, srv.c_str(), port) && Open(ip))
    {
	Settings::Get().SetNetworkLocalClient(true);
	return true;
    }
    return false;
}

void FH2LocalClient::PopPlayersInfo(QueueMessage & msg)
{
    Player cur;
    u8 size, admin;
    players.clear();
    admin_id = 0;
    msg.Pop(size);
    for(u8 ii = 0; ii < size; ++ii)
    {
        msg.Pop(cur.color);
        msg.Pop(cur.race);
        msg.Pop(cur.name);
        msg.Pop(cur.id);
        msg.Pop(admin);
	if(admin) admin_id = cur.id;
        if(cur.id) players.push_back(cur);
    }
}

int FH2LocalClient::Main(void)
{
    const Settings & conf = Settings::Get();
    std::string err;

    if(ConnectionChat())
    {
	if(conf.GameType(Game::TYPE_STANDARD))
	{
	    if(ScenarioInfoDialog())
	    {
		Cursor & cursor = Cursor::Get();
		Display & display = Display::Get();

		cursor.Hide();
    		display.Fill(0, 0, 0);
        	TextBox(_("Please wait..."), Font::BIG, Rect(0, display.h()/2, display.w(), display.h()/2));
        	display.Flip();

/*
		if(Game::IO::LoadBIN(packet))
		{
			conf.SetMyColor(Color::Get(player_color));
			cursor.Hide();
			return true;
		}
*/

//    		StartGame();
		err = "under construction";
	    }
	    else
		err = "close scenario dialog";
	}
	else
	if(conf.GameType(Game::TYPE_BATTLEONLY))
	{
	    if(Prepare4BattleOnly())
	    {
		GetLoadMaps(false, false);
		Game::StartBattleOnly();
	    }
	}
	else
	    err = "unknown game type";
    }

    Logout(err);

    return 1;
}

void FH2LocalClient::Logout(const std::string & err)
{
    QueueMessage packet(MSG_LOGOUT);
    packet.Push(err);
    Send(packet);
    DELAY(100);
    Close();
    modes = 0;
}

bool FH2LocalClient::SendWait(QueueMessage & msg, u16 id, const char* str)
{
    if(msg.GetID() != id)
	msg.SetID(id);

    DEBUG(DBG_NETWORK, DBG_INFO, str << " send: " << Network::GetMsgString(id));
    if(!Send(msg)) return false;

    DEBUG(DBG_NETWORK, DBG_INFO, str << " wait: " << Network::GetMsgString(id));
    if(!Wait(msg, id)) return false;

    return true;
}

bool FH2LocalClient::WaitSend(QueueMessage & msg, u16 id, const char* str)
{
    DEBUG(DBG_NETWORK, DBG_INFO, str << " wait: " << Network::GetMsgString(id));
    if(!Wait(msg, id)) return false;

    DEBUG(DBG_NETWORK, DBG_INFO, str << " send: " << Network::GetMsgString(id));
    if(!Send(msg)) return false;

    return true;
}

bool FH2LocalClient::ConnectionChat(void)
{
    Settings & conf = Settings::Get();
    QueueMessage packet;

    color = 0;
    race = 0;
    name.clear();
    id = 0;

    // recv ready, banner
    DEBUG(DBG_NETWORK, DBG_INFO, "wait ready");
    if(!Wait(packet, MSG_READY)) return false;

    // get banner
    std::string str;
    packet.Pop(str);

    // dialog: input name
    if(!Dialog::InputString("Connected to " + server + "\n \n" + str + "\n \nEnter player name:", name))
	return false;

    u8 game_type = conf.GameType();
    DEBUG(DBG_NETWORK, DBG_INFO, "game type: 0x" << std::hex << static_cast<int>(game_type));
    
    // send hello
    packet.Reset();
    packet.SetID(MSG_HELLO);
    packet.Push(game_type);
    packet.Push(name);

    if(!SendWait(packet, MSG_HELLO, "ConnectionChat: "))
	return false;


    packet.Pop(modes);
    packet.Pop(game_type);
    packet.Pop(id);
    DEBUG(DBG_NETWORK, DBG_INFO, (Modes(ST_ADMIN) ? "admin" : "client") << " mode");

    if(conf.GameType() != game_type)
	conf.SetGameType(game_type);
    DEBUG(DBG_NETWORK, DBG_INFO, "game type: 0x" << std::hex << static_cast<int>(game_type));

    // ready
    DEBUG(DBG_NETWORK, DBG_INFO, "wait: " << Network::GetMsgString(MSG_READY));
    if(!Wait(packet, MSG_READY)) return false;
    packet.Pop(str);

    if(str.size())
    {
	Dialog::Message("Error", str, Font::BIG, Dialog::OK);
	return false;
    }

    return true;
}

bool FH2LocalClient::GetCurrentMapInfo(void)
{
    Settings & conf = Settings::Get();

    // send get_current_map
    QueueMessage packet(MSG_GET_CURRENT_MAP);
    if(!SendWait(packet, MSG_GET_CURRENT_MAP, "GetCurrentMapInfo: "))
	return false;

    Network::PacketPopMapsFileInfo(packet, conf.CurrentFileInfo());

    return true;
}

bool FH2LocalClient::Prepare4BattleOnly(void)
{
    // get color
    if(! GetPlayersInfo()) return false;

    if(2 > players.size())
    {
	GetLoadMaps(true, false);

	LocalEvent & le = LocalEvent::Get();
	Cursor & cursor = Cursor::Get();
	Display & display = Display::Get();

	cursor.Hide();
	TextBox textbox("wait other player", Font::BIG, BOXAREA_WIDTH);
	Dialog::Box box(10 + textbox.h(), Dialog::CANCEL);
	const Rect & pos = box.GetArea();                                                                                          
	textbox.Blit(pos.x, pos.y + 20);

	ButtonGroups btnGroups(box.GetArea(), Dialog::CANCEL);
	btnGroups.Draw();

	cursor.Show();
	display.Flip();

	QueueMessage packet;

	DEBUG(DBG_NETWORK, DBG_INFO, "wait: " << Network::GetMsgString(MSG_UPDATE_PLAYERS));

	while(le.HandleEvents())
	{
	    if(Dialog::ZERO != btnGroups.QueueEventProcessing())
		return false;

	    if(Ready())
    	    {
        	if(!Network::RecvMessage(*this, packet))
            	    return false;

        	if(MSG_UPDATE_PLAYERS == packet.GetID())
		    break;
    	    }
    	    DELAY(10);
	}

	MsgUpdatePlayers(packet);
    }

    return true;
}

bool FH2LocalClient::GetLoadMaps(bool forceLoad, bool sendAll)
{
    QueueMessage packet(MSG_MAPS_LOAD);
    packet.Push(static_cast<u8>(forceLoad));
    packet.Push(static_cast<u8>(sendAll));
    if(!SendWait(packet, MSG_MAPS_LOAD, "LoadMaps: "))
	return false;

    if(Game::IO::LoadBIN(packet))
	return true;

    DEBUG(DBG_NETWORK, DBG_WARN, "error");
    return false;
}

bool FH2LocalClient::SendStartBattleOnly(void)
{
    QueueMessage packet(MSG_START_BATTLEONLY);

    DEBUG(DBG_NETWORK, DBG_INFO, "");
    if(!Send(packet)) return false;
    return true;
}

bool FH2LocalClient::GetPlayersInfo(void)
{
    // get my color
    QueueMessage packet(MSG_GET_CURRENT_COLOR);
    if(!SendWait(packet, MSG_GET_CURRENT_COLOR, "GetPlayersInfo: "))
	return false;

    packet.Pop(color);

    if(0 == color)
    {
	DEBUG(DBG_NETWORK, DBG_WARN, "unknown color");
	return false;
    }

    // get players
    MsgUpdatePlayers(packet);

    return true;
}

bool FH2LocalClient::ScenarioInfoDialog(void)
{
    Settings & conf = Settings::Get();
    Cursor & cursor = Cursor::Get();
    Display & display = Display::Get();
    LocalEvent & le = LocalEvent::Get();

    if(!GetCurrentMapInfo() ||
	!GetPlayersInfo()) return false;

    const Sprite & panel = AGG::GetICN(ICN::NGHSBKG, 0);
    const Rect  rectPanel(204, 32, panel.w(), panel.h());
    const Point pointOpponentInfo(rectPanel.x + 24, rectPanel.y + 202);
    const Point pointClassInfo(rectPanel.x + 24, rectPanel.y + 282);
    const Rect  box(pointOpponentInfo, 360, 180);

    Rects::const_iterator itr;

    Interface::PlayersInfo playersInfo(true, false, false);

    playersInfo.UpdateInfo(conf.GetPlayers(), pointOpponentInfo, pointClassInfo);

    RedrawScenarioStaticInfo(rectPanel);
    playersInfo.RedrawInfo();

    Button buttonSelectMaps(rectPanel.x + 309, rectPanel.y + 45, ICN::NGEXTRA, 64, 65);
    Button buttonOk(rectPanel.x + 31, rectPanel.y + 380, ICN::NGEXTRA, 66, 67);
    Button buttonCancel(rectPanel.x + 287, rectPanel.y + 380, ICN::NGEXTRA, 68, 69);

    SpriteCursor sp;
    sp.SetSprite(AGG::GetICN(ICN::NGEXTRA, 80));

    if(! Modes(ST_ADMIN))
    {
	buttonOk.SetDisable(true);
	buttonSelectMaps.SetDisable(true);
    }

    buttonSelectMaps.Draw();
    buttonOk.Draw();
    buttonCancel.Draw();
    cursor.Show();
    display.Flip();

    bool exit = false;
    bool update_info = false;
    u8 change_color = Color::NONE;
    DEBUG(DBG_NETWORK, DBG_INFO, "start queue");

    while(!exit && le.HandleEvents())
    {
        if(Ready())
	{
	    QueueMessage packet;

	    if(!Recv(packet)) return false;
	    DEBUG(DBG_NETWORK, DBG_INFO, "recv: " << Network::GetMsgString(packet.GetID()));

	    switch(packet.GetID())
    	    {
		case MSG_READY:
		    break;

		case MSG_CHANGE_RACE:
		    Network::UnpackRaceColors(packet);
		    update_info = true;
		    break;

		case MSG_UPDATE_PLAYERS:
		{
		    MsgUpdatePlayers(packet);
		    if(Modes(ST_ADMIN))
		    {
			buttonOk.Release();
			buttonOk.SetDisable(false);
			buttonSelectMaps.Release();
			buttonSelectMaps.SetDisable(false);
		    }
		    else
		    {
			buttonOk.Press();
			buttonOk.SetDisable(true);
			buttonSelectMaps.Press();
			buttonSelectMaps.SetDisable(true);
		    }
		    update_info = true;
		    break;
		}

		case MSG_SET_CURRENT_MAP:
		    Network::PacketPopMapsFileInfo(packet, conf.CurrentFileInfo());
		    playersInfo.UpdateInfo(conf.GetPlayers(), pointOpponentInfo, pointClassInfo);
		    update_info = true;
		    break;

		case MSG_MESSAGE:
		    break;

		case MSG_SHUTDOWN:
		    exit = true;
		    break;


		default: break;
	    }
	}

	if(update_info)
	{
	    cursor.Hide();
	    RedrawScenarioStaticInfo(rectPanel);
	    playersInfo.RedrawInfo();
	    buttonSelectMaps.Draw();
	    buttonOk.Draw();
	    buttonCancel.Draw();
	    cursor.Show();
	    display.Flip();
	    update_info = false;
	}

	// press button
        if(buttonSelectMaps.isEnable()) le.MousePressLeft(buttonSelectMaps) ? buttonSelectMaps.PressDraw() : buttonSelectMaps.ReleaseDraw();
        if(buttonOk.isEnable()) le.MousePressLeft(buttonOk) ? buttonOk.PressDraw() : buttonOk.ReleaseDraw();
        le.MousePressLeft(buttonCancel) ? buttonCancel.PressDraw() : buttonCancel.ReleaseDraw();

	// click select
	if(Game::HotKeyPress(Game::EVENT_BUTTON_SELECT) ||
	    (buttonSelectMaps.isEnable() && le.MouseClickLeft(buttonSelectMaps)))
	{
	    cursor.Hide();
	    sp.Hide();
	    cursor.Show();

	    // recv maps_info_list
	    QueueMessage packet(MSG_GET_MAPS_LIST);
	    if(SendWait(packet, MSG_GET_MAPS_LIST, "ScenarioInfoDialog: "))
	    {
		MapsFileInfoList lists;
		Network::PacketPopMapsFileInfoList(packet, lists);
		if(const Maps::FileInfo *fi = Dialog::SelectScenario(lists))
        	{
		    // send set_maps_info
	    	    packet.Reset();
		    packet.SetID(MSG_SET_CURRENT_MAP);
	    	    packet.Push(fi->file);
		    DEBUG(DBG_NETWORK, DBG_INFO, "send: " << Network::GetMsgString(packet.GetID()));
	    	    if(!Send(packet)) return false;
		}
		update_info = true;
	    }
	}
	else
	// click cancel
        if(le.MouseClickLeft(buttonCancel) || Game::HotKeyPress(Game::EVENT_DEFAULT_EXIT))
    	    return false;
        else
        // click ok
        if(Game::HotKeyPress(Game::EVENT_DEFAULT_READY) || (buttonOk.isEnable() && le.MouseClickLeft(buttonOk)))
    	{
/*
	    packet.Reset();
	    packet.SetID(MSG_START_GAME);
	    DEBUG(DBG_NETWORK, DBG_INFO, "send: " << Network::GetMsgString(packet.GetID()));
	    if(!Send(packet)) return false;
	    cursor.Hide();
	    return true;
*/
	}
	else
	if(Modes(ST_ADMIN) && le.MouseClickLeft(box))
	{
	    // click colors
	    if(Player* player = playersInfo.GetFromOpponentClick(le.GetMouseCursor()))
	    {
		u8 humans = conf.GetPlayers().GetColors(CONTROL_HUMAN);

		if((humans & player->color) && Color::NONE == change_color)
		{
		    cursor.Hide();
		    sp.Move((*itr).x - 3, (*itr).y - 3);
		    cursor.Show();
		    display.Flip();
		    change_color = player->color;
		}
		else
		if(conf.CurrentFileInfo().AllowHumanColors() & player->color)
		{
		    cursor.Hide();
		    sp.Hide();
		    cursor.Show();
		    display.Flip();
		    if(Color::NONE != change_color)
		    {
			QueueMessage packet(MSG_CHANGE_COLORS);
			packet.Push(change_color);
			packet.Push(player->color);
		        DEBUG(DBG_NETWORK, DBG_INFO, "send: " << Network::GetMsgString(packet.GetID()));
			if(!Send(packet)) return false;
			change_color = Color::NONE;
		    }
		}
	    }
	    else
	    // click races
	    if(Player* player = playersInfo.GetFromClassClick(le.GetMouseCursor()))
	    {
		if(conf.AllowChangeRace(player->color))
		{
                    switch(player->race)
                    {
                        case Race::KNGT: player->race = Race::BARB; break;
                        case Race::BARB: player->race = Race::SORC; break;
                        case Race::SORC: player->race = Race::WRLK; break;
                        case Race::WRLK: player->race = Race::WZRD; break;
                        case Race::WZRD: player->race = Race::NECR; break;
                        case Race::NECR: player->race = Race::RAND; break;
                        case Race::RAND: player->race = Race::KNGT; break;
                        default: break;
                    }
		    if(change_color)
		    {
			cursor.Hide();
			sp.Hide();
			cursor.Show();
			display.Flip();
			change_color = Color::NONE;
		    }
		    if((player->race & Race::ALL) || player->race == Race::RAND)
		    {
			QueueMessage packet(MSG_CHANGE_RACE);
			packet.Push(player->color);
			packet.Push(player->race);
		        DEBUG(DBG_NETWORK, DBG_INFO, "send: " << Network::GetMsgString(packet.GetID()));
			if(!Send(packet)) return false;
		    }
		}
	    }
	}

        DELAY(10);
    }

    return false;
}

void FH2LocalClient::MsgUpdatePlayers(QueueMessage & packet)
{
    PopPlayersInfo(packet);
    Settings::Get().GetPlayers().SetHumanColors(GetPlayersColors(players));
    std::vector<Player>::iterator itp = std::find_if(players.begin(), players.end(), std::bind2nd(std::mem_fun_ref(&Player::isID), id));
    if(itp != players.end())
    {
	color = (*itp).color;
	race = (*itp).race;
	(admin_id == id ? SetModes(ST_ADMIN) : ResetModes(ST_ADMIN));
    }
}

const Player* FH2LocalClient::FindPlayer(u8 color) const
{
    std::vector<Player>::const_iterator itp = std::find_if(players.begin(), players.end(),
					    std::bind2nd(std::mem_fun_ref(&Player::isColor), color));

    return itp != players.end() ? &(*itp) : NULL;
}

Game::menu_t Game::NetworkGuest(void)
{
    Settings & conf = Settings::Get();
    Display & display = Display::Get();
    Cursor & cursor = Cursor::Get();
    
    // clear background
    const Sprite &back = AGG::GetICN(ICN::HEROES, 0);
    cursor.Hide();
    back.Blit(display);
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

    // main procedure
    client.Main();

    return QUITGAME;
}

#else
Game::menu_t Game::NetworkGuest(void)
{
    Dialog::Message(_("Error"), _("This release is compiled without network support."), Font::BIG, Dialog::OK);
    return MAINMENU;
}
#endif
