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

#include "game.h"
#include "dialog.h"
#include "agg.h"
#include "settings.h"
#include "client.h"
#include "server.h"

#ifdef WITH_NET

extern u8 SelectCountPlayers(void);

void SendPacketToAllClients(std::list<ClientSocket> & clients, Network::Message & msg, u32 owner)
{
    Network::Message ready(MSG_READY);

    std::list<ClientSocket>::iterator it = clients.begin();
    for(; it != clients.end(); ++it) if((*it).IsConnected())
	(*it).GetThreadID() != owner ? msg.Send(*it) : ready.Send(*it);
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

bool FH2Server::Bind(u16 port)
{
    IPaddress ip;

    return Network::ResolveHost(ip, NULL, port) && Open(ip);
}

int FH2Server::ConnectionChat(void)
{
    Settings & conf = Settings::Get();
    std::list<ClientSocket>::iterator it;
    Network::Message packet(MSG_UNKNOWN);

    std::string str;
    bool exit = false;

    // wait players
    while(! exit)
    {
	// allow accept
        if(conf.PreferablyCountPlayers() > std::count_if(clients.begin(), clients.end(), std::mem_fun_ref(&ClientSocket::IsConnected)))
        {
    	    TCPsocket csd = Accept();

    	    if(csd)
    	    {
        	// find free socket
        	it = std::find_if(clients.begin(), clients.end(), std::not1(std::mem_fun_ref(&ClientSocket::IsConnected)));
        	if(it == clients.end())
        	{
            	    clients.push_back(ClientSocket());
            	    it = clients.end();
            	    --it;
        	}

		// first player: set admin mode
		if(1 == std::count_if(clients.begin(), clients.end(), std::mem_fun_ref(&ClientSocket::IsConnected)))
		{
		    admin_id = (*it).GetThreadID();
		    (*it).SetModes(ST_ADMIN);
		}

    		// join
        	(*it).Join(csd);
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

            it = std::find_if(clients.begin(), clients.end(), std::bind2nd(std::mem_fun_ref(&ClientSocket::IsThreadID), id));

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
		    if(2 < conf.Debug()) Error::Verbose("FH2Server::ConnectionChat: admin shutdown");
                    packet.Reset();
                    packet.SetID(MSG_MESSAGE);
                    str = "FH2Server::ConnectionChat: logout player: " + (*it).GetName();
                    packet.Push(str);
		    SendPacketToAllClients(clients, packet, id);
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

    std::for_each(clients.begin(), clients.end(), std::mem_fun_ref(&ClientSocket::Shutdown));
    Close();

    return 0;
}

Game::menu_t Game::NetworkHost(void)
{
    Settings & conf = Settings::Get();
    Display & display = Display::Get();

    // select count players
    conf.SetPreferablyCountPlayers(SelectCountPlayers());

    // clear background
    const Sprite &back = AGG::GetICN(ICN::HEROES, 0);
    display.Blit(back);
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
    FH2Client client;

    // connect to server
    if(client.Connect(localhost, conf.GetPort()))
    {
	client.ConnectionChat();
    }
    else
        Dialog::Message(_("Error"), Network::GetError(), Font::BIG, Dialog::OK);

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
