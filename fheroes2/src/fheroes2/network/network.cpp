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

#include "sdlnet.h"
#include "settings.h"
#include "network.h"

#ifdef WITH_NET

const char* Network::GetMsgString(u16 msg)
{
    switch(msg)
    {
        case MSG_PING:          return "MSG_PING";
        case MSG_HELLO:         return "MSG_HELLO";
        case MSG_CONNECT:       return "MSG_CONNECT";
        case MSG_MESSAGE:       return "MSG_MESSAGE";
        case MSG_READY:         return "MSG_READY";
        case MSG_LOGOUT:        return "MSG_LOGOUT";
        case MSG_SHUTDOWN:      return "MSG_SHUTDOWN";

        case MSG_LOADMAPS:      return "MSG_LOADMAPS";
        case MSG_TURNS:         return "MSG_TURNS";
        case MSG_HEROES:        return "MSG_HEROES";
        case MSG_CASTLE:        return "MSG_CASTLE";
        case MSG_SPELL:         return "MSG_SPELL";
        case MSG_MAPS:          return "MSG_MAPS";
        case MSG_KINGDOM:       return "MSG_KINGDOM";
        case MSG_WORLD:         return "MSG_WORLD";

        default: break;
    }

    return "MSG_UNKNOWN";
}

bool Network::PacketAsk(Socket & csd, Message & msg)
{
    const Settings & conf = Settings::Get();

    if(! msg.Send(csd))
    {
        csd.Close();
        if(1 < conf.Debug()) Error::Warning("Network::AskHello: close socket");
        return false;
    }

    return true;
}

bool Network::PacketWait(Socket & csd, Message & msg, u16 id)
{
    const Settings & conf = Settings::Get();

    while(1)
    {
	if(msg.Recv(csd, (1 < conf.Debug())))
	{
	    if(id == msg.GetID()) return true;
	    else
	    if(MSG_LOGOUT == msg.GetID() || MSG_SHUTDOWN == msg.GetID()) return false;
	    else
	    if(1 < conf.Debug()) Error::Warning("Network::WaitHello: receive packet: " + std::string(GetMsgString(msg.GetID())));
	}
	else
	{
    	    csd.Close();
    	    if(1 < conf.Debug()) Error::Warning("Network::WaitHello: close socket");
    	    return false;
	}

	DELAY(1);
    }

    return true;
}


#endif
