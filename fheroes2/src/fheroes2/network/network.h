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

#ifndef H2NETWORK_H
#define H2NETWORK_H

#include "gamedefs.h"

#ifdef WITH_NET

#include "sdlnet.h"
#include "client.h"
#include "maps_fileinfo.h"

typedef std::pair<Network::Message, u32> MessageID;

enum msg_t
{
        MSG_RAW,

        MSG_PING,
	MSG_READY,
        MSG_MESSAGE,

        MSG_HELLO,
        MSG_PLAYERS,
        MSG_LOGOUT,
        MSG_SHUTDOWN,

        MSG_TURNS,
        MSG_HEROES,
        MSG_CASTLE,
        MSG_SPELL,
        MSG_MAPS,
        MSG_KINGDOM,
        MSG_WORLD,

        MSG_UNKNOWN,
};

namespace Network
{
    int			RunDedicatedServer(void);
    const char*         GetMsgString(u16);
    msg_t		GetMsg(u16);

    void		PacketPushMapsFileInfo(Network::Message &, const Maps::FileInfo &);
    void		PacketPopMapsFileInfo(Network::Message &, Maps::FileInfo &);
    void		PacketPushPlayersInfo(Network::Message &, const std::vector<Player> &);
    void		PacketPopPlayersInfo(Network::Message &, std::vector<Player> &);

    u8			GetPlayersColors(std::vector<Player> &);
};

#endif

#endif
