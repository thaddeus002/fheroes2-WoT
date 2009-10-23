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
#include "world.h"
#include "kingdom.h"
#include "server.h"

#ifdef WITH_NET

extern u8 SelectCountPlayers(void);

void SendPacketToAllClients(std::list<FH2RemoteClient> & clients, Network::Message & msg, u32 owner)
{
    static Network::Message ready(MSG_READY);

    std::list<FH2RemoteClient>::iterator it = clients.begin();
    for(; it != clients.end(); ++it) if((*it).IsConnected())
	(*it).player_id != owner ? msg.Send(*it) : ready.Send(*it);
}

FH2Server::FH2Server()
{
    AGG::Cache & cache = AGG::Cache::Get();
    if(! cache.ReadDataDir()) Error::Except("AGG data files not found.");

    if(!PrepareMapsFileInfoList(finfo_list) ||
       !Settings::Get().LoadFileMapsMP2(finfo_list.front().file)) Error::Warning("No maps available!");
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
		Error::Verbose("count player: ", conf.PreferablyCountPlayers());
		Socket sct(csd);
		// send message
		std::cout << "FH2Server::ConnectionChat: accept: close socket" << std::endl;
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

            switch(msg.GetID())
            {
    		case MSG_SHUTDOWN:
		    if(admin_id == msgid.second)
		    {
    			if(2 < conf.Debug()) Error::Verbose("FH2Server::ConnectionChat: admin shutdown");
    			exit = true;
                	packet.Reset();
                	packet.SetID(MSG_SHUTDOWN);
			SendPacketToAllClients(clients, packet, msgid.second);
		    }
		    break;

		default:
		    SendPacketToAllClients(clients, msg, msgid.second);
		    break;
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

void FH2Server::StartGame(void)
{
    Settings & conf = Settings::Get();

    conf.SetGameOverResult(GameOver::COND_NONE);
    conf.SetCurrentKingdomColors(conf.KingdomColors());

    for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color) if(color & conf.PlayersColors())
    {
	world.GetKingdom(color).SetControl(Game::REMOTE);
        world.GetKingdom(color).UpdateStartingResource();
    }
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
    conf.SetGameType(Game::NETWORK);

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
    FH2LocalClient & client = FH2LocalClient::Get();

    // connect to server
    if(client.Connect(localhost, conf.GetPort()))
    {
	client.SetModes(ST_LOCALSERVER);
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
