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

typedef std::pair<Network::Message, u32> MessageID;

enum msg_t
{
        MSG_RAW,

        MSG_PING,
	MSG_READY,
        MSG_MESSAGE,

        MSG_HELLO,
        MSG_LOGOUT,
        MSG_SHUTDOWN,

        MSG_LOADMAPS,
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

    //bool PacketAsk(Socket &, Message &);
    //bool PacketWait(Socket &, Message &, u16);
};

#endif

#endif
