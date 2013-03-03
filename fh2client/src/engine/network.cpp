/***************************************************************************
 *   Copyright (C) 2013 by Star Marine <starmarine10@yahoo.com>            *
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

#include <stdexcept>

#include "system.h"
#include "network.h"

void Network::Init()
{
    IPaddress ip;

    State = ST_INIT;

    SocketEvent.reset(new IOEvent());

    std::cout << "Network thread is initializing" << std::endl;

    if(SDL_Init(0) == -1) {
        std::cout << "Cannot init network: " << SDLNet_GetError() << std::endl;
        throw std::runtime_error("Cannot init network");
    }

    SocketSet = SDLNet_AllocSocketSet(NEvents);

    if(!SocketSet) {
        std::cout << "Unable to allocate socket set: " << SDLNet_GetError() << std::endl;
        throw std::runtime_error("Unable to allocate socket set");
    }

    const char* fh2serv = System::GetEnvironment("FH2SERVER");

    if(SDLNet_ResolveHost(&ip, (fh2serv ? fh2serv : "localhost"), 13536) == -1) {
        std::cout << "Unable to resolve server address: " << SDLNet_GetError() << std::endl;
        throw std::runtime_error("Unable to resolve server address");
    }

    State = ST_CONNECTING;
    std::cout << "Connecting" << std::endl;

    SocketEvent->socket = SDLNet_TCP_Open(&ip);

    if(!SocketEvent->socket) {
        SetState(ST_ERROR, "Unable to connect to server");
        std::cout << "Unable to connect to server: " << SDLNet_GetError() << std::endl;
        throw std::runtime_error("Unable to connect to server");
    }

    SocketEvent->error = 0;
    SocketEvent->write.armed = 1;
    SocketEvent->write.handler = &Network::ConnectedHandler;

    ConnectedHandler(*SocketEvent);
}

void Network::Destroy()
{
    std::cout << "Network thread shutting down" << std::endl;

    delete Message;

    SDLNet_TCP_Close(SocketEvent->socket);

    SDLNet_FreeSocketSet(SocketSet);

    SocketEvent.reset(0);

    SDLNet_Quit();
}

void Network::Run()
{
    bool quit = false;
    int NDesc;
    Uint32 Timeout = 100;

    do {
        NDesc = SDLNet_CheckSockets(SocketSet, Timeout);

        if(NDesc == -1) {
            std::cout << "SDLNet_CheckSockets() failed: " << SDLNet_GetError() << std::endl;
            throw std::runtime_error("SDLNet_CheckSockets() failed");
        }

        if(SDLNet_SocketReady(SocketEvent->socket)) {
            if(SocketEvent->read.handler != NULL) {
                (this->*SocketEvent->read.handler)(*SocketEvent);
            }
        }

        if(!OutputQueue.empty()) {
            std::cout << "Network thread woken up" << std::endl;
            Network::MessageWriteHandler(*SocketEvent);
        }

        SyncMutex.Lock();
        quit = QuitFlag;
        SyncMutex.Unlock();
    } while(!quit);

    SyncMutex.Lock();
    QuitFlag = false;
    SyncMutex.Unlock();
}

void Network::ArmEvent(IOEvent &e) {
    if(SDLNet_TCP_AddSocket(SocketSet, e.socket) == -1) {
        std::cout << "SDLNet_TCP_AddSocket() failed: " << SDLNet_GetError() << std::endl;
        throw std::runtime_error("SDLNet_TCP_AddSocket() failed");
    }
}

void Network::DisarmEvent(IOEvent &e) {
    if(SDLNet_TCP_DelSocket(SocketSet, e.socket) == -1) {
        std::cout << "SDLNet_TCP_DelSocket() failed: " << SDLNet_GetError() << std::endl;
        throw std::runtime_error("SDLNet_TCP_DelSocket() failed");
    }
}

int Network::NetworkThreadWrapper(void *param)
{
    Network *network = static_cast<Network*>(param);

    std::cout << "Network thread started" << std::endl;

    try {
        network->Init();
        network->Run();
    }
    catch(const std::runtime_error &e) {
        std::cout << e.what() << std::endl;
    }

    network->Destroy();

    std::cout << "Network thread stopped" << std::endl;

    return 0;
}

void Network::StartNetworkThread(const std::string &_SoftName, int Major, int Minor, int Revision)
{
    Network::Get().SoftName = _SoftName;
    Network::Get().SoftVersionMajor = Major;
    Network::Get().SoftVersionMinor = Minor;
    Network::Get().SoftVersionRevision = Revision;

    NetworkThread.Create(Network::NetworkThreadWrapper, (void*)this);
}

void Network::JoinNetworkThread()
{
    if(IsActive()) {
        SyncMutex.Lock();
        QuitFlag = true;
        SyncMutex.Unlock();

        NetworkThread.Wait();
    }
}

Network &Network::Get() {
    static Network theNetwork;

    return theNetwork;
}

bool Network::IsInputPending() const {
    bool result;

    SyncMutex.Lock();
    result = !InputQueue.empty();
    SyncMutex.Unlock();

    return result;
}

void Network::DequeueInputEvent(NetworkEvent &ne) {
    SyncMutex.Lock();
    ne = InputQueue.front();
    InputQueue.front().Message.release();
    InputQueue.pop_front();
    SyncMutex.Unlock();
}

void Network::QueueOutputMessage(const NetworkMessage &Msg) {
    SyncMutex.Lock();
    OutputQueue.push_back(NetworkEvent());
    NetworkEvent &one = OutputQueue.back();
    one.Message.reset(new NetworkMessage(Msg));
    SyncMutex.Unlock();
}

void Network::SetState(StateEnum NewState)
{
    SyncMutex.Lock();
    InputQueue.push_back(NetworkEvent());

    NetworkEvent &ne = InputQueue.back();

    ne.OldState = State;
    ne.NewState = NewState;
    State = NewState;
    SyncMutex.Unlock();
}

void Network::SetState(StateEnum NewState, const NetworkMessage &Msg)
{
    SyncMutex.Lock();
    InputQueue.push_back(NetworkEvent());

    NetworkEvent &ne = InputQueue.back();

    ne.OldState = State;
    ne.NewState = NewState;
    ne.Message.reset(new NetworkMessage(Msg));

    State = NewState;
    SyncMutex.Unlock();
}

void Network::SetState(StateEnum NewState, const std::string &ErrorMessage)
{
    SyncMutex.Lock();
    InputQueue.push_back(NetworkEvent());

    NetworkEvent &ne = InputQueue.back();

    ne.OldState = State;
    ne.NewState = NewState;
    ne.ErrorMessage.assign(ErrorMessage);

    State = NewState;
    SyncMutex.Unlock();
}

void Network::QueueInputMessage(const NetworkMessage &Msg)
{
    SyncMutex.Lock();
    InputQueue.push_back(NetworkEvent());

    NetworkEvent &ne = InputQueue.back();

    ne.OldState = ne.NewState = State;
    ne.Message.reset(new NetworkMessage(Msg));

    SyncMutex.Unlock();
}
