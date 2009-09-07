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

#ifdef WITH_NET

#include <sstream>
#include <algorithm>
#include <functional>
#include "world.h"
#include "settings.h"
#include "server.h"
#include "client.h"
#include "remoteclient.h"
#include "zzlib.h"

int FH2RemoteClient::callbackCreateThread(void *data)
{
    if(data)
    {
	reinterpret_cast<FH2RemoteClient *>(data)->ConnectionChat();
	reinterpret_cast<FH2RemoteClient *>(data)->thread.Wait();
	return 0;
    }
    return -1;
}

FH2RemoteClient::FH2RemoteClient()
{
}

void FH2RemoteClient::RunThread(void)
{
    if(!thread.IsRun())
	thread.Create(callbackCreateThread, this);
}

void FH2RemoteClient::ShutdownThread(void)
{
    SetModes(ST_SHUTDOWN);
    DELAY(100);

    if(thread.IsRun()) thread.Kill();

    if(sd)
    {
        packet.Reset();
	packet.SetID(MSG_SHUTDOWN);
	packet.Send(*this);
        Close();
    }
    SetModes(0);
}

int FH2RemoteClient::Logout(void)
{
    packet.Reset();
    packet.SetID(MSG_LOGOUT);
    packet.Push(std::string("logout: lost connection"));

    FH2Server & server = FH2Server::Get();
    server.mutex.Lock();
    server.queue.push_back(std::make_pair(packet, player_id));
    server.mutex.Unlock();

    Close();
    modes = 0;
    
    return -1;
}

int FH2RemoteClient::ConnectionChat(void)
{
    Settings & conf = Settings::Get();
    bool extdebug = 2 < conf.Debug();

    player_color = 0;
    player_race = 0;
    player_name.clear();

    // wait thread id
    if(extdebug) std::cerr << "FH2RemoteClient::ConnectionChat: wait start thread...";
    while(0 == thread.GetID()){ DELAY(10); };
    player_id = thread.GetID();
    if(extdebug) std::cerr << "ok" << std::endl;

    SetModes(ST_CONNECT);

    // send banner
    std::ostringstream banner;
    banner << "Free Heroes II Server, version: " << static_cast<int>(conf.MajorVersion()) << "." << static_cast<int>(conf.MinorVersion()) << std::endl;

    packet.Reset();
    packet.SetID(MSG_READY);
    packet.Push(banner.str());

    // send ready
    if(extdebug) std::cerr << "FH2RemoteClient::ConnectionChat: id: 0x" << std::hex << player_id << ", send ready...";
    if(!Send(packet, extdebug)) return Logout();
    if(extdebug) std::cerr << "ok" << std::endl;

    // recv hello
    if(extdebug) std::cerr << "FH2RemoteClient::ConnectionChat: id: 0x" << std::hex << player_id << ", recv hello...";
    if(!Wait(packet, MSG_HELLO, extdebug)) return Logout();
    if(extdebug) std::cerr << "ok" << std::endl;

    packet.Pop(player_name);
    if(conf.Debug())
        std::cerr << "FH2RemoteClient::ConnectionChat: id: 0x" << std::hex << player_id << ", connected " << " player: " << player_name << ", host 0x" << std::hex << Host() << ":0x" << Port() << std::endl;

    FH2Server & server = FH2Server::Get();
    std::string str;

    // check color
    server.mutex.Lock();
    player_color = Color::GetFirst(conf.CurrentFileInfo().allow_colors & (~conf.PlayersColors()));
    conf.SetPlayersColors(Network::GetPlayersColors(server.clients));
    server.mutex.Unlock();
    if(0 == player_color)
    {
	if(extdebug) std::cerr << "FH2RemoteClient::ConnectionChat: id: 0x" << std::hex << player_id << ", player_color = 0, logout" << std::endl;
	return Logout();
    }
    // send update players
    packet.Reset();
    packet.SetID(MSG_PLAYERS);
    server.mutex.Lock();
    Network::PacketPushPlayersInfo(packet, server.clients);
    server.queue.push_back(std::make_pair(packet, player_id));
    server.mutex.Unlock();

    // send hello, modes, id, color
    packet.Reset();
    packet.SetID(MSG_HELLO);
    packet.Push(modes);
    packet.Push(player_id);
    packet.Push(player_color);
    if(extdebug) std::cerr << "FH2RemoteClient::ConnectionChat: id: 0x" << std::hex << player_id << ", send hello...";
    if(!Send(packet, extdebug)) return Logout();
    if(extdebug) std::cerr << "ok" << std::endl;
    if(extdebug) std::cerr << "FH2RemoteClient::ConnectionChat: " << (Modes(ST_ADMIN) ? "admin" : "client") << " mode" << std::endl;

    if(Modes(ST_ADMIN)) SetModes(ST_ALLOWPLAYERS);

    if(extdebug) std::cerr << "FH2RemoteClient::ConnectionChat start queue" << std::endl;
    while(1)
    {
        if(Modes(ST_SHUTDOWN)) break;

	if(Ready())
	{
            if(extdebug) std::cerr << "FH2RemoteClient::ConnectionChat: recv: ";
	    if(!Recv(packet, extdebug)) return Logout();
            if(extdebug) std::cerr << Network::GetMsgString(packet.GetID()) << std::endl;

	    // msg put to queue
	    if(MSG_UNKNOWN != Network::GetMsg(packet.GetID()) && Network::MsgIsBroadcast(packet.GetID()))
	    {
		server.mutex.Lock();
		server.queue.push_back(std::make_pair(packet, player_id));
	        server.mutex.Unlock();
	    }

    	    // msg processing
    	    switch(Network::GetMsg(packet.GetID()))
    	    {
    		case MSG_PING:
		    if(extdebug) Error::Verbose("FH2RemoteClient::ConnectionChat: ping");
		    packet.Reset();
		    packet.SetID(MSG_PING);
		    packet.Send(*this);
		    break;

        	case MSG_LOGOUT:
                    // send message
                    packet.Reset();
                    packet.SetID(MSG_MESSAGE);
                    str = "logout player: " + player_name;
                    packet.Push(str);
		    server.mutex.Lock();
		    server.queue.push_back(std::make_pair(packet, player_id));
		    //
		    conf.SetPlayersColors(Network::GetPlayersColors(server.clients) & (~player_color));
                    // send players
        	    packet.Reset();
                    packet.SetID(MSG_PLAYERS);
                    Network::PacketPushPlayersInfo(packet, server.clients, player_id);
		    server.queue.push_back(std::make_pair(packet, player_id));
	    	    server.mutex.Unlock();
		    if(extdebug) Error::Verbose("FH2RemoteClient::ConnectionChat: " + str);
	    	    return Logout();

    		case MSG_MAPS_INFO_SET:
		    packet.Pop(str);
		    if(Modes(ST_ADMIN) && str.size())
		    {
        		packet.Reset();
                	packet.SetID(MSG_MAPS_INFO);
			server.mutex.Lock();
			Settings::Get().LoadFileMaps(str);
			Network::PacketPushMapsFileInfo(packet, conf.CurrentFileInfo());
			server.queue.push_back(std::make_pair(packet, player_id));
	    		server.mutex.Unlock();
			if(extdebug) std::cerr << "FH2RemoteClient::ConnectionChat send maps_info...";
			if(!Send(packet, extdebug)) return Logout();
			if(extdebug) std::cerr << "ok" << std::endl;
		    }
		    break;

    		case MSG_MAPS_INFO_GET:
		    packet.Reset();
		    packet.SetID(MSG_MAPS_INFO);
		    Network::PacketPushMapsFileInfo(packet, conf.CurrentFileInfo());
		    if(extdebug) std::cerr << "FH2RemoteClient::ConnectionChat send maps_info...";
		    if(!Send(packet, extdebug)) return Logout();
		    if(extdebug) std::cerr << "ok" << std::endl;
		    break;

    		case MSG_MAPS_LIST_GET:
		    packet.Reset();
		    packet.SetID(MSG_MAPS_LIST);
		    server.mutex.Lock();
		    Network::PacketPushMapsFileInfoList(packet, server.finfo_list);
		    server.mutex.Unlock();
		    if(extdebug) std::cerr << "FH2RemoteClient::ConnectionChat send list maps_info...";
		    if(!Send(packet, extdebug)) return Logout();
		    if(extdebug) std::cerr << "ok" << std::endl;
		    break;

    		case MSG_MAPS_LOAD:
		    if(Modes(ST_ADMIN))
		    {
			packet.Reset();
			packet.SetID(MSG_MAPS_LOAD);
			server.mutex.Lock();
			//
			conf.FixKingdomRandomRace();
			std::for_each(server.clients.begin(), server.clients.end(), Player::FixRandomRace);
			conf.SetPlayersColors(Network::GetPlayersColors(server.clients));
			//
			World::Get().LoadMaps(conf.MapsFile());
			std::vector<char> bufz;
			Game::SaveZXML(bufz);
			std::vector<char>::const_iterator it1 = bufz.begin();
			std::vector<char>::const_iterator it2 = bufz.end();
			for(; it1 != it2; ++it1) packet.Push(static_cast<u8>(*it1));
/*
			{
			        std::fstream fs("map.snd.z", std::ios::out | std::ios::binary);
                    		if(fs.good()) fs.write(packet.DtPt(), packet.DtSz());
                                fs.close();
                        }
                        server.packet_maps = packet;
*/
			//
			server.queue.push_back(std::make_pair(packet, player_id));
			server.mutex.Unlock();
			if(extdebug) std::cerr << "FH2RemoteClient::ConnectionChat send all maps_load...";
			if(!Send(packet, extdebug)) return Logout();
			if(extdebug) std::cerr << "ok" << std::endl;
			packet.Reset();
		    }
    		    break;

    		case MSG_MAPS_LOAD_ERR:
/*
			server.mutex.Lock();
			if(extdebug) std::cerr << "FH2RemoteClient::ConnectionChat send maps_load_err...";
			if(!Send(server.packet_maps, extdebug)) return Logout();
			if(extdebug) std::cerr << "ok" << std::endl;
			server.mutex.Unlock();
*/
    		    break;

    		case MSG_PLAYERS_GET:
		    packet.Reset();
		    packet.SetID(MSG_PLAYERS);
		    server.mutex.Lock();
                    Network::PacketPushPlayersInfo(packet, server.clients);
		    server.mutex.Unlock();
		    if(extdebug) std::cerr << "FH2RemoteClient::ConnectionChat send players...";
		    if(!Send(packet, extdebug)) return Logout();
		    if(extdebug) std::cerr << "ok" << std::endl;
		    break;
/*
    		case MSG_PLAYERS:
		{
		    std::vector<Player> players;
		    Network::PacketPopPlayersInfo(packet, players);
		    server.mutex.Lock();
		    conf.SetPlayersColors(Network::GetPlayersColors(players));
		    server.mutex.Unlock();
		    std::vector<Player>::const_iterator itp = std::find_if(players.begin(), players.end(), std::bind2nd(std::mem_fun_ref(&Player::isID), player_id));
		    if(itp != players.end())
		    {
			player_name = (*itp).player_name;
		        player_color = (*itp).player_color;
		        player_race = (*itp).player_race;
		    }
    		    break;
    		}
*/

    		case MSG_MAPS:
		{
		    packet.Reset();
		    break;
		}

    		default:
    		    break;
    	    }
	}

        DELAY(10);
    }
    
    Close();
    modes = 0;

    return 0;
}

#endif
