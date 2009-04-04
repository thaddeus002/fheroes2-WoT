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
#include "sdlnet.h"
#include "network.h"

#ifdef WITH_NET

extern u8 SelectCountPlayers(void);

Game::menu_t Game::NetworkServer(void)
{
    Settings & conf = Settings::Get();

    // select count players
    conf.SetPreferablyCountPlayers(SelectCountPlayers());

    const u8 max_players = conf.PreferablyCountPlayers();
    if(2 > max_players) return MAINMENU;

    IPaddress ip;
    if(! Network::ResolveHost(ip, NULL, conf.GetPort()))
    {
	Dialog::Message(_("Error"), Network::GetError(), Font::BIG, Dialog::OK);
	return Game::MAINMENU;
    }

    Network::Server server;
    if(! server.Open(ip))
    {
	Dialog::Message(_("Error"), Network::GetError(), Font::BIG, Dialog::OK);
	return Game::MAINMENU;
    }

    // prepare banner
    std::string str = "Free Heroes II Server, version: ";
    String::AddInt(str, conf.MajorVersion());
    str += ".";
    String::AddInt(str, conf.MinorVersion());
    str += ", build: ";
    String::AddInt(str, conf.DateBuild());
    str += "\n";

    Network::Message banner;
    banner.Push(str);

    bool exit = false;
    LocalEvent & le = LocalEvent::GetLocalEvent();

    // wait connection
    while(!exit && le.HandleEvents())
    {
	TCPsocket csd = server.Accept();
        Network::Socket client(csd);
        banner.Send(client, true);
        client.Close();

        DELAY(1);
    }

    // all ready: load maps

    // game loop turns


    server.Close();

    return QUITGAME;
}

#else
Game::menu_t Game::NetworkServer(void)
{
    Dialog::Message(_("Error"), _("This release is compiled without network support."), Font::BIG, Dialog::OK);
    return MAINMENU;
}
#endif
