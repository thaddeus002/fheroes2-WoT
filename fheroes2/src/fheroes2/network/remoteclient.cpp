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

#ifdef WITH_NET

#include <sstream>
#include <algorithm>
#include <functional>
#include "world.h"
#include "settings.h"
#include "server.h"
#include "client.h"
#include "kingdom.h"
#include "race.h"
#include "battle2.h"
#include "battle_stats.h"
#include "remoteclient.h"
#include "zzlib.h"

FH2RemoteClients::FH2RemoteClients()
{
    mutexClients.Create();
    clients.reserve(8);
}

size_t FH2RemoteClients::GetConnected(void) const
{
    mutexClients.Lock();
    size_t res =  std::count_if(clients.begin(), clients.end(),
                        std::mem_fun_ref(&FH2RemoteClient::IsConnected));
    mutexClients.Unlock();
    return res;
}

FH2RemoteClient* FH2RemoteClients::GetAdmin(void)
{
    mutexClients.Lock();
    std::vector<FH2RemoteClient>::iterator it = std::find_if(clients.begin(), clients.end(),
				std::bind2nd(std::mem_fun_ref(&FH2RemoteClient::Modes), ST_ADMIN));
    mutexClients.Unlock();
    return it != clients.end() ? &(*it) : NULL;
}

FH2RemoteClient* FH2RemoteClients::GetNewClient(void)
{
    mutexClients.Lock();
    std::vector<FH2RemoteClient>::iterator it = std::find_if(clients.begin(), clients.end(),
				std::not1(std::mem_fun_ref(&FH2RemoteClient::IsConnected)));
    if(it == clients.end())
    {
	clients.push_back(FH2RemoteClient());
	clients.back().InitMutex();
	it = clients.end();
	--it;
    }
    mutexClients.Unlock();
    return &(*it);
}

void FH2RemoteClients::Shutdown(void)
{
    std::for_each(clients.begin(), clients.end(), std::mem_fun_ref(&FH2RemoteClient::ShutdownThread));
}

u8 FH2RemoteClients::GetPlayersColors(void) const
{
    u8 res = 0;
    mutexClients.Lock();
    for(std::vector<FH2RemoteClient>::const_iterator
	it = clients.begin(); it != clients.end(); ++it)
	    if((*it).id && (*it).color) res |= (*it).color;
    mutexClients.Unlock();
    return res;
}

FH2RemoteClient* FH2RemoteClients::GetClient(u8 color)
{
    mutexClients.Lock();
    std::vector<FH2RemoteClient>::iterator it = std::find_if(clients.begin(), clients.end(),
                                std::bind2nd(std::mem_fun_ref(&FH2RemoteClient::isColor), color));
    mutexClients.Unlock();
    return it != clients.end() ? &(*it) : NULL;
}

void FH2RemoteClients::PushPlayersInfo(QueueMessage & msg) const
{
    mutexClients.Lock();
    u8 count = std::count_if(clients.begin(), clients.end(),
	    std::not1(std::bind2nd(std::mem_fun_ref(&Player::isID), 0)));
    msg.Push(count);
    if(count)
    {
        for(std::vector<FH2RemoteClient>::const_iterator
	    it = clients.begin(); it != clients.end(); ++it) if((*it).id)
        {
            msg.Push((*it).color);
            msg.Push((*it).race);
            msg.Push((*it).name);
            msg.Push((*it).id);
            msg.Push(static_cast<u8>((*it).Modes(ST_ADMIN)));
        }
    }
    mutexClients.Unlock();
}

void FH2RemoteClients::Send2All(const QueueMessage & msg, u32 exclude)
{
    mutexClients.Lock();
    for(std::vector<FH2RemoteClient>::iterator
	it = clients.begin(); it != clients.end(); ++it)
	    if((*it).IsConnected() && (*it).id != exclude) Network::SendMessage(*it, msg);
    mutexClients.Unlock();
}

u8 FH2RemoteClients::ResetPlayersColors(void)
{
    const Settings & conf = Settings::Get();

    mutexClients.Lock();

    // reset all
    for(std::vector<FH2RemoteClient>::iterator
	it = clients.begin(); it != clients.end(); ++it)
    {
        (*it).color = 0;
        (*it).race = 0;
    }

    u8 colors = 0;

    // set first admin
    std::vector<FH2RemoteClient>::iterator it = std::find_if(clients.begin(), clients.end(),
				std::bind2nd(std::mem_fun_ref(&FH2RemoteClient::Modes), ST_ADMIN));
    if(it != clients.end())
    {
        (*it).color = Color::GetFirst(conf.CurrentFileInfo().AllowHumanColors());
        colors |= (*it).color;
    }

    // set other
    for(std::vector<FH2RemoteClient>::iterator
	it = clients.begin(); it != clients.end(); ++it)
	if(0 == (*it).color)
    {
        const u8 color = Color::GetFirst(conf.CurrentFileInfo().AllowHumanColors() & (~colors));
        if(color)
        {
            (*it).color = color;
            colors |= color;
        }
        else
        // no free colors, shutdown client
        {
            (*it).SetModes(ST_SHUTDOWN);
        }
    }

    mutexClients.Unlock();

    return colors;
}

bool FH2RemoteClients::ChangeColors(u8 from, u8 to)
{
    std::vector<FH2RemoteClient>::iterator it1, it2;

    mutexClients.Lock();
        
    it1 = std::find_if(clients.begin(), clients.end(),
	std::bind2nd(std::mem_fun_ref(&FH2RemoteClient::isColor), from));

    it2 = std::find_if(clients.begin(), clients.end(),
	std::bind2nd(std::mem_fun_ref(&FH2RemoteClient::isColor), to));

    if(it2 == clients.end())
    {
        (*it1).color = to;
	mutexClients.Unlock();
	return false;
    }
    else
    if(it1 != clients.end())
    {
        std::swap((*it1).color, (*it2).color);
    }

    mutexClients.Unlock();
    return true;
}

void FH2RemoteClients::ChangeRace(u8 color, u8 race)
{
    mutexClients.Lock();
    std::vector<FH2RemoteClient>::iterator it = std::find_if(clients.begin(), clients.end(),
				std::bind2nd(std::mem_fun_ref(&FH2RemoteClient::isColor), color));
    if(it != clients.end())
        (*it).race = race;
    mutexClients.Unlock();
}

void FH2RemoteClients::SetNewAdmin(u32 old_rid)
{
    std::vector<FH2RemoteClient>::iterator it;

    mutexClients.Lock();
    it = std::find_if(clients.begin(), clients.end(),
	std::bind2nd(std::mem_fun_ref(&FH2RemoteClient::isID), old_rid));

    if(it != clients.end()) (*it).ResetModes(ST_ADMIN);

    it = std::find_if(clients.begin(), clients.end(),
	std::not1(std::bind2nd(std::mem_fun_ref(&FH2RemoteClient::isID), old_rid)));
    if(it != clients.end()) (*it).SetModes(ST_ADMIN);
    mutexClients.Unlock();
}

void FH2RemoteClients::Dump(void) const
{
    mutexClients.Lock();
    for(std::vector<FH2RemoteClient>::const_iterator
	it = clients.begin(); it != clients.end(); ++it)
	VERBOSE((*it).String());
    mutexClients.Unlock();
}



void FH2RemoteClient::SetModes(u32 f)
{
    mutexModes.Lock();
    BitModes::SetModes(f);
    mutexModes.Unlock();
}

bool FH2RemoteClient::Modes(u32 f) const
{
    mutexModes.Lock();
    bool res = BitModes::Modes(f);
    mutexModes.Unlock();
    return res;
}

int FH2RemoteClient::callbackCreateThread(void* data)
{
    return data ? reinterpret_cast<FH2RemoteClient *>(data)->Main() : -1;
}

FH2RemoteClient::FH2RemoteClient()
{
}

void FH2RemoteClient::InitMutex(void)
{
    mutexModes.Create();
}

void FH2RemoteClient::RunThread(void)
{
    if(thread.IsRun())
    {
	DEBUG(DBG_NETWORK, DBG_INFO, "wait thread");
	thread.Wait();
    }
    DEBUG(DBG_NETWORK, DBG_INFO, "run thread");
    thread.Create(callbackCreateThread, this);
}

int FH2RemoteClient::Main(void)
{
    // wait thread id
    if(0 == thread.GetID())
    {
	DEBUG(DBG_NETWORK, DBG_INFO, "wait start thread");
	while(0 == thread.GetID()){ DELAY(30); };
    }

    if(ConnectionChat())
    {
	FH2Server & server = FH2Server::Get();

	// send update players
	{
	    QueueMessage msg;
	    server.SendUpdatePlayers(msg, id);
	}

	DEBUG(DBG_NETWORK, DBG_INFO, String());
	DEBUG(DBG_NETWORK, DBG_INFO, "start queue");

	while(! Modes(ST_SHUTDOWN))
	{
	    if(Ready())
	    {
		RemoteMessage & msg = server.GetNewMessage(*this);
		QueueMessage & packet = msg.packet;

		if(!Recv(packet)) return -1;

        	DEBUG(DBG_NETWORK, DBG_INFO, "recv: " <<
			Network::GetMsgString(packet.GetID()));

		msg.SetReady();
	    }

	    DELAY(10);
	}
    }

    CloseConnection();

    return 1;
}

void FH2RemoteClient::ShutdownThread(void)
{
    SetModes(ST_SHUTDOWN);

    DELAY(100);

    if(thread.IsRun()) thread.Kill();

    if(sd)
    {
        QueueMessage packet(MSG_SHUTDOWN);
	Network::SendMessage(*this, packet);
    }

    DEBUG(DBG_NETWORK, DBG_INFO, "shutdown thread");

    Reset();
}

void FH2RemoteClient::CloseConnection(void)
{
    if(sd) Close();

    Reset();

    DEBUG(DBG_NETWORK, DBG_INFO, "close connection");
}

bool FH2RemoteClient::ConnectionChat(void)
{
    const Settings & conf = Settings::Get();

    QueueMessage packet;

    id = thread.GetID();

    SetModes(ST_CONNECT);

    // send banner
    std::ostringstream banner;
    banner << "Free Heroes II Server, version: " << static_cast<int>(conf.MajorVersion()) <<
					"." << static_cast<int>(conf.MinorVersion()) << std::endl;
    packet.Reset();
    packet.SetID(MSG_READY);
    packet.Push(banner.str());

    // send ready
    DEBUG(DBG_NETWORK, DBG_INFO, "id: 0x" << std::hex << id << ", send ready");
    if(!Send(packet)) return false;

    // recv hello
    DEBUG(DBG_NETWORK, DBG_INFO, "id: 0x" << std::hex << id << ", wait hello");
    if(!Wait(packet, MSG_HELLO)) return false;

    u8 game_type;
    packet.Pop(game_type);
    packet.Pop(name);
    DEBUG(DBG_NETWORK, DBG_INFO, "id: 0x" << std::hex << id << ", connected " << 
			" player: " << name << ", host 0x" << std::hex << Host() << ":0x" << Port());

    if(Modes(ST_ADMIN) && conf.GameType() != game_type) SetGameType(game_type);

    // wait set game type
    DELAY(100);

    // send hello, modes, id
    packet.Reset();
    packet.SetID(MSG_HELLO);
    packet.Push(modes);
    packet.Push(conf.GameType());
    packet.Push(id);
    // send
    DEBUG(DBG_NETWORK, DBG_INFO, "id: 0x" << std::hex << id << ", send hello");
    if(!Send(packet)) return false;

    DEBUG(DBG_NETWORK, DBG_INFO, (Modes(ST_ADMIN) ? "admin" : "client") << " mode");


    // update colors
    return UpdateColors();
}

void FH2RemoteClient::SetGameType(u8 type)
{
    FH2Server & server = FH2Server::Get();

    RemoteMessage & msg = server.GetNewMessage(*this);
    QueueMessage & packet = msg.packet;

    DEBUG(DBG_NETWORK, DBG_INFO, "0x" << std::hex << static_cast<int>(type));

    packet.SetID(MSG_SET_GAMETYPE);
    packet.Push(type);
    msg.SetReady();
}

bool FH2RemoteClient::UpdateColors(void)
{
    FH2Server & server = FH2Server::Get();

    color = server.GetFreeColor(Modes(ST_ADMIN));
    std::string err;
    QueueMessage packet;

    if(0 == color)
    {
	DEBUG(DBG_NETWORK, DBG_INFO, "0x" << std::hex << id << " invalid color");
	err = "full house";
    }

    packet.SetID(MSG_READY);
    packet.Push(err);
    if(!Send(packet) || err.size()) return false;

    DEBUG(DBG_NETWORK, DBG_INFO, "select " << Color::String(color));
    server.UpdateColors();

    return true;
}

bool FH2RemoteClient::SendMapsInfoList(QueueMessage & msg)
{
    FH2Server & server = FH2Server::Get();

    msg.Reset();
    msg.SetID(MSG_GET_MAPS_LIST);
    server.PushMapsFileInfoList(msg);

    DEBUG(DBG_NETWORK, DBG_INFO, "size: " << std::dec << msg.DtSz() << " bytes");
    return Send(msg);
}


bool FH2RemoteClient::SendCurrentColor(QueueMessage & msg)
{
    FH2Server & server = FH2Server::Get();

    msg.Reset();
    msg.SetID(MSG_GET_CURRENT_COLOR);
    // color
    msg.Push(color);
    // players info
    server.PushPlayersInfo(msg);

    DEBUG(DBG_NETWORK, DBG_INFO, Color::String(color) << ", " <<
			    "size: " << std::dec << msg.DtSz() << " bytes");
    return Send(msg);
}

bool FH2RemoteClient::SendCurrentMapInfo(QueueMessage & msg)
{
    Settings & conf = Settings::Get();

    msg.Reset();
    msg.SetID(MSG_GET_CURRENT_MAP);

    // added cur maps info
    Network::PacketPushMapsFileInfo(msg, conf.CurrentFileInfo());

    DEBUG(DBG_NETWORK, DBG_INFO, "size: " << std::dec << msg.DtSz() << " bytes");

    return Send(msg);
}

void FH2RemoteClient::Reset(void)
{
    color = 0;
    race = Race::RAND;
    name.clear();
    modes = ST_SHUTDOWN;

    if(Network::Socket::IsValid())
	Network::Socket::Close();

    modes = 0;
    DEBUG(DBG_NETWORK, DBG_TRACE, "reset modes");
}

std::string FH2RemoteClient::String(void) const
{
    std::ostringstream os;

    os << "Player(" <<
	"id: " << "0x" << std::hex << id << ", " <<
	"color: " << std::dec << static_cast<int>(color) << ", " <<
	"race: " << static_cast<int>(race) << ", " <<
	"name: " << name << "), " <<
	"Modes(" << "0x" << std::hex << modes << "), " <<
	"Socket(";

    if(Network::Socket::IsValid())
    {
    	os << "host: " << "0x" << Host() << ", " << "port: " << "0x" << Port();
    }
    else
    {
    	os << "false";
    }

    os << "), " << "Thread(0x" << thread.GetID() << ")" << std::dec << std::endl;
    return os.str();
}

#endif
