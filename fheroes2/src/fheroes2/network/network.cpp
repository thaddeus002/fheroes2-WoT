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

#include <cstdlib>

#include "sdlnet.h"
#include "settings.h"
#include "server.h"
#include "error.h"
#include "network.h"

#ifdef WITH_NET

const char* Network::GetMsgString(u16 msg)
{
    switch(msg)
    {
        case MSG_PING:          return "MSG_PING";
        case MSG_READY:         return "MSG_READY";
        case MSG_MESSAGE:       return "MSG_MESSAGE";

        case MSG_HELLO:         return "MSG_HELLO";
        case MSG_PLAYERS:       return "MSG_PLAYERS";
        case MSG_LOGOUT:        return "MSG_LOGOUT";
        case MSG_SHUTDOWN:      return "MSG_SHUTDOWN";

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

int Network::RunDedicatedServer(void)
{
    Settings & conf = Settings::Get();

    Network::SetProtocolVersion(static_cast<u16>((conf.MajorVersion() << 8)) | conf.MinorVersion());
    
    if(SDL::Init(INIT_TIMER))
    try
    {
        std::atexit(SDL::Quit);

        FH2Server & server = FH2Server::Get();

        if(! server.Bind(conf.GetPort()))
        {
            Error::Warning(Network::GetError());
            return -1;
        }

	conf.SetModes(Settings::DEDICATEDSERVER);
	conf.SetPreferablyCountPlayers(1);

        return FH2Server::callbackCreateThread(&server);
    }
    catch(std::bad_alloc)
    {
    }
    catch(Error::Exception)
    {
        conf.Dump();
    }

    return 0;
}

void Network::PacketPushMapsFileInfo(Network::Message & packet, const Maps::FileInfo & fi)
{
    packet.Push(fi.file);
    packet.Push(fi.name);
    packet.Push(fi.description);
    packet.Push(fi.size_w);
    packet.Push(fi.size_h);
    packet.Push(fi.difficulty);

    packet.Push(static_cast<u8>(KINGDOMMAX));
    for(u8 ii = 0; ii < KINGDOMMAX; ++ii) packet.Push(fi.races[ii]);

    packet.Push(fi.kingdom_colors);
    packet.Push(fi.allow_colors);
    packet.Push(fi.rnd_colors);
    packet.Push(fi.rnd_races);
    packet.Push(fi.conditions_wins);
    packet.Push(fi.wins1);
    packet.Push(fi.wins2);
    packet.Push(fi.wins3);
    packet.Push(fi.wins4);
    packet.Push(fi.conditions_loss);
    packet.Push(fi.loss1);
    packet.Push(fi.loss2);
    packet.Push(static_cast<u32>(fi.localtime));
    packet.Push(static_cast<u8>(fi.with_heroes));
}

void Network::PacketPopMapsFileInfo(Network::Message & packet, Maps::FileInfo & fi)
{
    u8 byte8, race;
    u32 byte32;

    packet.Pop(fi.file);
    packet.Pop(fi.name);
    packet.Pop(fi.description);
    packet.Pop(fi.size_w);
    packet.Pop(fi.size_h);
    packet.Pop(fi.difficulty);

    packet.Pop(byte8);
    for(u8 ii = 0; ii < byte8; ++ii){ packet.Pop(race); fi.races[ii] = race; }

    packet.Pop(fi.kingdom_colors);
    packet.Pop(fi.allow_colors);
    packet.Pop(fi.rnd_colors);
    packet.Pop(fi.rnd_races);
    packet.Pop(fi.conditions_wins);
    packet.Pop(fi.wins1);
    packet.Pop(fi.wins2);
    packet.Pop(fi.wins3);
    packet.Pop(fi.wins4);
    packet.Pop(fi.conditions_loss);
    packet.Pop(fi.loss1);
    packet.Pop(fi.loss2);
    packet.Pop(byte32);
    fi.localtime = byte32;
    packet.Pop(byte8);
    fi.with_heroes = byte8;
}

void Network::PacketPushPlayersInfo(Network::Message & m, const std::vector<Player> & v)
{
    m.Push(static_cast<u8>(v.size()));
    std::vector<Player>::const_iterator itp1 = v.begin();
    std::vector<Player>::const_iterator itp2 = v.end();
    for(; itp1 != itp2; ++itp1)
    {
	m.Push((*itp1).player_color);
	m.Push((*itp1).player_race);
        m.Push((*itp1).player_name);
        m.Push((*itp1).player_id);
    }
}

void Network::PacketPopPlayersInfo(Network::Message & m, std::vector<Player> & v)
{
    Player cur;
    u8 size;
    v.clear();
    m.Pop(size);
    for(u8 ii = 0; ii < size; ++ii)
    {
	m.Pop(cur.player_color);
	m.Pop(cur.player_race);
	m.Pop(cur.player_name);
        m.Pop(cur.player_id);
        if(cur.player_id) v.push_back(cur);
    }
}

u8 Network::GetPlayersColors(std::vector<Player> & v)
{
    u8 res = 0;
    std::vector<Player>::const_iterator it1 = v.begin();
    std::vector<Player>::const_iterator it2 = v.end();
    for(; it1 != it2; ++it1) if((*it1).player_id && (*it1).player_color) res |= (*it1).player_color;
                
    return res;
};
                    
#endif
