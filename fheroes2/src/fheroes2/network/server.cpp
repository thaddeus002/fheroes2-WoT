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

#include "game.h"
#include "dialog.h"
#include "agg.h"
#include "settings.h"
#include "cursor.h"
#include "localclient.h"
#include "server.h"

#ifdef WITH_NET

extern u8 SelectCountPlayers(void);

void SendPacketToAllClients(std::list<FH2RemoteClient> & clients, Network::Message & msg, u32 owner)
{
    Network::Message ready(MSG_READY);

    std::list<FH2RemoteClient>::iterator it = clients.begin();
    for(; it != clients.end(); ++it) if((*it).IsConnected())
	(*it).player_id != owner ? msg.Send(*it) : ready.Send(*it);
}

FH2Server::FH2Server()
{
}

FH2Server::~FH2Server()
{
}

FH2Server & FH2Server::Get(void)
{
    static FH2Server fh2server;

    return fh2server;
}

int FH2Server::callbackCreateThread(void *ptr)
{
    return ptr ? reinterpret_cast<FH2Server *>(ptr)->ConnectionChat() : -1;
}

void FH2Server::Exit(void)
{
    exit = true;
}

bool FH2Server::Bind(u16 port)
{
    IPaddress ip;

    return Network::ResolveHost(ip, NULL, port) && Open(ip);
}

int FH2Server::ConnectionChat(void)
{
    Settings & conf = Settings::Get();
    std::list<FH2RemoteClient>::iterator it;
    Network::Message packet(MSG_UNKNOWN);

    std::string str;
    exit = false;

    // wait players
    while(! exit)
    {
    	if(TCPsocket csd = Accept())
    	{
	    const u8 players = std::count_if(clients.begin(), clients.end(), std::mem_fun_ref(&FH2RemoteClient::IsConnected));

    	    // request admin
	    it = std::find_if(clients.begin(), clients.end(), std::bind2nd(std::mem_fun_ref(&FH2RemoteClient::Modes), ST_ADMIN));

	    // check count players
    	    if((conf.PreferablyCountPlayers() <= players) ||
    	    // check admin allow connect
    		((clients.end() != it && (*it).Modes(ST_CONNECT)) && !(*it).Modes(ST_ALLOWPLAYERS)))
	    {
		Socket sct(csd);
		// send message
		std::cout << "false: close socket" << std::endl;
		sct.Close();
	    }
	    else
	    // connect
    	    {
    		// find free socket
    		it = std::find_if(clients.begin(), clients.end(), std::not1(std::mem_fun_ref(&FH2RemoteClient::IsConnected)));
    		if(it == clients.end())
    		{
            	    clients.push_back(FH2RemoteClient());
            	    it = clients.end();
            	    --it;
        	}

		// first player: set admin mode
		if(0 == players)
		{
		    admin_id = (*it).player_id;
		    (*it).SetModes(ST_ADMIN);
		}

        	(*it).Assign(csd);
    		(*it).RunThread();
	    }
	}

        // scan queue
        if(queue.size())
        {
    	    mutex.Lock();
	    MessageID & msgid = queue.front();
    	    Network::Message & msg = msgid.first;
	    u32 id = msgid.second;

            it = std::find_if(clients.begin(), clients.end(), std::bind2nd(std::mem_fun_ref(&FH2RemoteClient::isID), id));

            if(clients.end() != it) switch(msg.GetID())
            {
    		case MSG_PING:
    		    if(2 < conf.Debug()) Error::Verbose("FH2Server::ConnectionChat: ping");
                    packet.Reset();
                    packet.SetID(MSG_PING);
                    packet.Send(*it);
                    break;

    		case MSG_SHUTDOWN:
		    if(admin_id == id)
		    {
    			if(2 < conf.Debug()) Error::Verbose("FH2Server::ConnectionChat: admin shutdown");
    			exit = true;
                	packet.Reset();
                	packet.SetID(MSG_SHUTDOWN);
			SendPacketToAllClients(clients, packet, id);
		    }
		    break;

    		case MSG_LOGOUT:
		{
		    if(2 < conf.Debug()) Error::Verbose("FH2Server::ConnectionChat: client logout");
		    // send message
                    packet.Reset();
                    packet.SetID(MSG_MESSAGE);
                    str = "FH2Server::ConnectionChat: logout player: " + (*it).player_name;
                    packet.Push(str);
		    SendPacketToAllClients(clients, packet, id);
                    // update players struct
                    packet.Reset();
                    packet.SetID(MSG_PLAYERS);
                    std::vector<Player> players;
		    StorePlayersInfo(players);
		    std::vector<Player>::iterator itp = std::find_if(players.begin(), players.end(), std::bind2nd(std::mem_fun_ref(&Player::isID), id));
		    if(itp != players.end()) players.erase(itp);
		    Network::PacketPushPlayersInfo(packet, players);
		    SendPacketToAllClients(clients, packet, id);
		}
		    break;

		case MSG_PLAYERS:
		    if(2 < conf.Debug()) Error::Verbose("FH2Server::ConnectionChat: send all msg players");
		    SendPacketToAllClients(clients, msg, id);
		    break;

		case MSG_MESSAGE:
                    msg.Pop(str);
    		    if(2 < conf.Debug()) Error::Verbose("FH2Server::ConnectionChat: message: " + str);
                    packet.Reset();
                    packet.SetID(MSG_MESSAGE);
                    packet.Push(str);
		    SendPacketToAllClients(clients, packet, id);
		    break;

        	default: break;
            }

            queue.pop_front();
            mutex.Unlock();
	}

	DELAY(1);
    }

    std::for_each(clients.begin(), clients.end(), std::mem_fun_ref(&FH2RemoteClient::ShutdownThread));
    Close();

    return 0;
}

void FH2Server::StorePlayersInfo(std::vector<Player> & players)
{
    players.clear();

    std::list<FH2RemoteClient>::const_iterator itc1 = clients.begin();
    std::list<FH2RemoteClient>::const_iterator itc2 = clients.end();
    for(; itc1 != itc2; ++itc1) if((*itc1).player_id) players.push_back(*itc1);
}

Game::menu_t Game::NetworkHost(void)
{
    Settings & conf = Settings::Get();
    Display & display = Display::Get();
    Cursor & cursor = Cursor::Get();

    // select count players
    const u8 max_players = SelectCountPlayers();

    if(2 > max_players) return Game::MAINMENU;
    conf.SetPreferablyCountPlayers(max_players);

    // clear background
    const Sprite &back = AGG::GetICN(ICN::HEROES, 0);
    cursor.Hide();
    display.Blit(back);
    cursor.Show();
    display.Flip();

    // create local server
    FH2Server & server = FH2Server::Get();

    if(! server.Bind(conf.GetPort()))
    {
	Dialog::Message(_("Error"), Network::GetError(), Font::BIG, Dialog::OK);
	return Game::MAINMENU;
    }

    Thread thread;
    thread.Create(FH2Server::callbackCreateThread, &server);

    // create local client
    const std::string localhost("127.0.0.1");
    FH2LocalClient client;

    // connect to server
    if(client.Connect(localhost, conf.GetPort()))
    {
	client.ConnectionChat();
    }
    else
        Dialog::Message(_("Error"), Network::GetError(), Font::BIG, Dialog::OK);

    server.Exit();

    if(0 > thread.Wait())
    {
	Dialog::Message(_("Error"), Network::GetError(), Font::BIG, Dialog::OK);
	return Game::MAINMENU;
    }
    server.Close();

    return QUITGAME;
}

#else
Game::menu_t Game::NetworkHost(void)
{
    Dialog::Message(_("Error"), _("This release is compiled without network support."), Font::BIG, Dialog::OK);
    return MAINMENU;
}
#endif
