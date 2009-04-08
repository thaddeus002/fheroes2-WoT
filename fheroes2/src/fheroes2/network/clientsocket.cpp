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
#include "settings.h"
#include "server.h"
#include "clientsocket.h"

int ClientSocket::callbackCreateThread(void *data)
{
    if(data)
    {
	reinterpret_cast<ClientSocket *>(data)->ConnectionChat();
	reinterpret_cast<ClientSocket *>(data)->thread.Wait();
	return 0;
    }
    return -1;
}

ClientSocket::ClientSocket()
{
}

bool ClientSocket::IsConnected(void) const
{
    return Modes(ST_CONNECT) && sd;
}

void ClientSocket::Join(TCPsocket csd)
{
    if(sd) Close();
    sd = csd;
}

void ClientSocket::RunThread(void)
{
    if(!thread.IsRun())
	thread.Create(callbackCreateThread, this);
}

void ClientSocket::Shutdown(void)
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

const std::string & ClientSocket::GetName(void) const
{
    return name;
}

u32 ClientSocket::GetThreadID(void) const
{
    return thread.GetID();
}

bool ClientSocket::IsThreadID(u32 id) const
{
    return thread.IsRun() && id == thread.GetID();
}

int ClientSocket::ConnectionChat(void)
{
    Settings & conf = Settings::Get();
    bool extdebug = 2 < conf.Debug();

    SetModes(ST_CONNECT);

    // send banner
    std::ostringstream banner;
    banner << "Free Heroes II Server, version: " << static_cast<int>(conf.MajorVersion()) << "." << static_cast<int>(conf.MinorVersion()) << std::endl;

    packet.Reset();
    packet.SetID(MSG_READY);
    packet.Push(banner.str());

    // send ready
    if(2 < conf.Debug()) std::cerr << "ClientSocket::ConnectionChat: id: 0x" << std::hex << GetThreadID() << "send ready...";
    if(!packet.Send(*this))
    {
	Close();
        SetModes(0);
	if(extdebug) std::cerr << "error" << std::endl;
        if(conf.Debug()) std::cerr << "ClientSocket::ConnectionChat: id: 0x" << std::hex << GetThreadID() << " close socket" << std::endl;
	return -1;
    }
    if(extdebug) std::cerr << "ok" << std::endl;

    // recv hello
    if(extdebug) std::cerr << "ClientSocket::ConnectionChat: id: 0x" << std::hex << GetThreadID() << " recv hello...";
    if(!packet.Recv(*this))
    {
	Close();
        SetModes(0);
	if(extdebug) std::cerr << "error" << std::endl;
        if(conf.Debug()) std::cerr << "ClientSocket::ConnectionChat: id: 0x" << std::hex << GetThreadID() << " close socket" << std::endl;
        return -1;
    }
    if(MSG_HELLO != packet.GetID())
    {
        Close();
        SetModes(0);
	if(extdebug) std::cerr << "error" << std::endl;
	if(conf.Debug()) std::cerr << "ClientSocket::ConnectionChat: id: 0x" << std::hex << GetThreadID() << " unknown client" << std::endl;
        return -1;
    }
    if(extdebug) std::cerr << "ok" << std::endl;

    packet.Pop(name);
    if(conf.Debug())
        std::cerr << "ClientSocket::ConnectionChat: id: 0x" << std::hex << GetThreadID() << ", connected " << " player: " << name << ", host 0x" << std::hex << Host() << ":0x" << Port() << std::endl;

    FH2Server & server = FH2Server::Get();

    // send hello, allow_colors
    packet.Reset();
    packet.SetID(MSG_HELLO);
    server.mutex.Lock();
    packet.Push(server.allow_colors);
    server.mutex.Unlock();
    if(extdebug) std::cerr << "ClientSocket::ConnectionChat: id: 0x" << std::hex << GetThreadID() << " send hello...";
    if(!packet.Send(*this))
    {
	Close();
        SetModes(0);
	if(extdebug) std::cerr << "error" << std::endl;
	if(conf.Debug()) std::cerr << "ClientSocket::ConnectionChat: id: 0x" << std::hex << GetThreadID() << " unknown client" << std::endl;
	return -1;
    }
    if(extdebug) std::cerr << "ok" << std::endl;

    // recv ready, color
    if(extdebug) std::cerr << "ClientSocket::ConnectionChat: id: 0x" << std::hex << GetThreadID() << " recv ready...";
    if(!packet.Recv(*this))
    {
	Close();
        SetModes(0);
	if(extdebug) std::cerr << "error" << std::endl;
        if(conf.Debug()) std::cerr << "ClientSocket::ConnectionChat: id: 0x" << std::hex << GetThreadID() << " close socket" << std::endl;
        return -1;
    }
    if(MSG_READY != packet.GetID())
    {
        Close();
        SetModes(0);
	if(extdebug) std::cerr << "error" << std::endl;
	if(conf.Debug()) std::cerr << "ClientSocket::ConnectionChat: id: 0x" << std::hex << GetThreadID() << " unknown client" << std::endl;
        return -1;
    }
    if(extdebug) std::cerr << "ok" << std::endl;

    u8 color;
    packet.Pop(color);

    // check color
    if(! (color & server.allow_colors))
    {
        Close();
        SetModes(0);
	if(conf.Debug()) std::cerr << "ClientSocket::ConnectionChat: id: 0x" << std::hex << GetThreadID() << " unknown color" << std::endl;
        return -1;
    }

    server.mutex.Lock();
    server.allow_colors &= ~color;
    server.mutex.Unlock();

    while(1)
    {
        if(Modes(ST_SHUTDOWN)) break;

	if(!packet.Recv(*this, extdebug))
        {
            Close();
            SetModes(0);
            if(conf.Debug()) std::cerr << "ClientSocket::ConnectionChat: close socket" << std::endl;
    	    return -1;
        }

	server.mutex.Lock();
	server.queue.push_back(std::make_pair(packet, thread.GetID()));
	server.mutex.Unlock();

        if(packet.GetID() == MSG_LOGOUT) break;

        DELAY(10);
    }

    Close();
    SetModes(0);

    return 0;
}

#endif
