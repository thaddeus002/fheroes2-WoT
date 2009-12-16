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

#ifndef H2NETWORK_H
#define H2NETWORK_H

#include "gamedefs.h"

#ifdef WITH_NET

#include "sdlnet.h"
#include "client.h"
#include "maps_fileinfo.h"

typedef std::pair<Network::Message, u32> MessageID;
class FH2RemoteClient;
class Kingdom;
class Heroes;
class Castle;
namespace Maps { class Tiles; };

enum msg_t
{
        MSG_RAW,

        MSG_PING,
	MSG_READY,
        MSG_MESSAGE,

        MSG_HELLO,
        MSG_LOGOUT,
        MSG_SHUTDOWN,

        MSG_MAPS_INFO,
        MSG_MAPS_INFO_GET,
        MSG_MAPS_INFO_SET,

        MSG_MAPS_LOAD,
        MSG_MAPS_LOAD_ERR,

        MSG_MAPS_LIST,
        MSG_MAPS_LIST_GET,

        MSG_PLAYERS,
        MSG_PLAYERS_GET,

        MSG_YOUR_TURN,
        MSG_END_TURN,

        MSG_TILES,
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
    bool		MsgIsBroadcast(u16);

    void		PacketPopMapsFileInfoList(Network::Message &, MapsFileInfoList &);
    void		PacketPushMapsFileInfo(Network::Message &, const Maps::FileInfo &);
    void		PacketPopMapsFileInfo(Network::Message &, Maps::FileInfo &);
    void		PacketPushPlayersInfo(Network::Message &, const std::vector<FH2RemoteClient> &, u32 exclude = 0);

    u8			GetPlayersColors(const std::vector<FH2RemoteClient> &);
    
    void		PackKingdom(Network::Message &, const Kingdom &);
    void		UnpackKingdom(Network::Message &);

    void		PackTile(Network::Message &, const Maps::Tiles &);
    void		UnpackTile(Network::Message &);

    void		PackHero(Network::Message &, const Heroes &);
    void		UnpackHero(Network::Message &);

    void		PackCastle(Network::Message &, const Castle &);
    void		UnpackCastle(Network::Message &);
};

#endif

#endif
