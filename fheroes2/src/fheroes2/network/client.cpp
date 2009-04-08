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
#include "settings.h"
#include "client.h"
#include "agg.h"

#ifdef WITH_NET

FH2Client::FH2Client()
{
}
    
bool FH2Client::Connect(const std::string & server, u16 port)
{
    IPaddress ip;
    return Network::ResolveHost(ip, server.c_str(), port) && Open(ip);
}

int FH2Client::ConnectionChat(void)
{
    Settings & conf = Settings::Get();
    bool extdebug = 2 < conf.Debug();
    Network::Message packet;
    std::string str;

    // recv ready, banner
    if(extdebug) std::cerr << "FH2Client::ConnectionChat: recv ready...";
    if(!packet.Recv(*this))
    {
        Close();
        if(extdebug) std::cerr << "error" << std::endl;
        if(conf.Debug()) std::cerr << "FH2Client::ConnectionChat: close socket" << std::endl;
	return -1;
    }
    if(MSG_READY != packet.GetID())
    {
	Close();
        if(extdebug) std::cerr << "error" << std::endl;
        if(conf.Debug()) std::cerr << "FH2Client::ConnectionChat: close socket" << std::endl;
	return -1;
    }
    if(extdebug) std::cerr << "ok" << std::endl;

    // get banner
    packet.Pop(str);
    if(!Dialog::InputString("Connected to " + str + ", enter player name:", name))
    {
	Close();
        if(conf.Debug()) std::cerr << "FH2Client::ConnectionChat: close socket" << std::endl;
	return -1;
    }

    // send hello
    packet.Reset();
    packet.SetID(MSG_HELLO);
    packet.Push(name);
    if(extdebug) std::cerr << "FH2Client::ConnectionChat send hello...";
    if(!packet.Send(*this))
    {
	Close();
        if(conf.Debug()) std::cerr << "FH2Client::ConnectionChat: close socket" << std::endl;
	return -1;
    }
    if(extdebug) std::cerr << "ok" << std::endl;

    // recv hello, color
    if(extdebug) std::cerr << "FH2Client::ConnectionChat: recv hello...";
    if(!packet.Recv(*this))
    {
        Close();
        if(extdebug) std::cerr << "error" << std::endl;
        if(conf.Debug()) std::cerr << "FH2Client::ConnectionChat: close socket" << std::endl;
	return -1;
    }
    if(MSG_HELLO != packet.GetID())
    {
	Close();
        if(extdebug) std::cerr << "error" << std::endl;
        if(conf.Debug()) std::cerr << "FH2Client::ConnectionChat: close socket" << std::endl;
	return -1;
    }
    if(extdebug) std::cerr << "ok" << std::endl;

    // get allow colors
    packet.Pop(color);

    // select first color
    if(Color::BLUE & color) color = Color::BLUE;
    else
    if(Color::GREEN & color) color = Color::GREEN;
    else
    if(Color::RED & color) color = Color::RED;
    else
    if(Color::YELLOW & color) color = Color::YELLOW;
    else
    if(Color::ORANGE & color) color = Color::ORANGE;
    else
    if(Color::PURPLE & color) color = Color::PURPLE;
    else
    {
	Close();
        if(conf.Debug()) std::cerr << "FH2Client::ConnectionChat: unknown colors" << std::endl;
	return -1;
    }

    // send ready
    packet.Reset();
    packet.SetID(MSG_READY);
    packet.Push(color);
    if(extdebug) std::cerr << "FH2Client::ConnectionChat send ready...";
    if(!packet.Send(*this))
    {
	Close();
        if(conf.Debug()) std::cerr << "FH2Client::ConnectionChat: close socket" << std::endl;
	return -1;
    }
    if(extdebug) std::cerr << "ok" << std::endl;

    bool exit = false;

    while(!exit)
    {
        if(!packet.Recv(*this, extdebug))
        {
            Close();
            if(conf.Debug()) std::cerr << "ClientSocket::ConnectionChat: close socket" << std::endl;
            return -1;
        }

	switch(packet.GetID())
        {
	    case MSG_READY:
		break;

	    case MSG_MESSAGE:
		break;

	    case MSG_SHUTDOWN:
		exit = true;
		break;

	    default: break;
	}

        DELAY(300);
    }

    return 0;
}

Game::menu_t Game::NetworkGuest(void)
{
    Settings & conf = Settings::Get();
    Display & display = Display::Get();

    // clear background
    const Sprite &back = AGG::GetICN(ICN::HEROES, 0);
    display.Blit(back);
    display.Flip();

    std::string server;
    if(!Dialog::InputString("Server Name", server)) return MAINMENU;

    FH2Client client;

    if(! client.Connect(server, conf.GetPort()))
    {
        Dialog::Message(_("Error"), Network::GetError(), Font::BIG, Dialog::OK);
	return Game::MAINMENU;
    }

    client.ConnectionChat();
    client.Close();

    return QUITGAME;
}

#else
Game::menu_t Game::NetworkGuest(void)
{
    Dialog::Message(_("Error"), _("This release is compiled without network support."), Font::BIG, Dialog::OK);
    return MAINMENU;
}
#endif
