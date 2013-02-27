/***************************************************************************
 *   Copyright (C) 2013 by Star Marine  <starmarine10@yahoo.com>           *
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
#define H2NETOWRK_H

#include <string>
#include <list>
#include <memory>

#include <SDL_net.h>

#include "rect.h"
#include "network_message.h"
#include "thread.h"
#include "types.h"

enum StateEnum
{
    ST_INIT,
    ST_CONNECTING,
    ST_CONNECTED,
    ST_IDENTIFIED,
    ST_AUTHORIZED,
    ST_INCHAT,
    ST_INGAME,
    ST_PLAYING,
    ST_DISCONNECTED,
    ST_ERROR
};

enum ParseStateEnum
{
    ST_LEN_LO,
    ST_LEN_HI,
    ST_BODY
};

class NetworkEvent
{
public:
    NetworkEvent()
        : OldState(ST_INIT)
        , NewState(ST_INIT)
        , Message()
        , ErrorMessage()
    {
    }

    NetworkEvent(const NetworkEvent &that)
        : OldState(that.OldState)
        , NewState(that.NewState)
        , Message(that.Message.get())
        , ErrorMessage()
    {
    }

    StateEnum		          OldState, NewState;
    std::auto_ptr<NetworkMessage> Message;
    std::string                   ErrorMessage;
};

class IOEvent;
class Network;

struct channel_t
{
    void                (Network::*handler)(IOEvent&);
    unsigned            armed:1;
    unsigned            ready:1;
    unsigned            io_in_progress:1;
};

class IOEvent
{
public:
    IOEvent()
        : socket()
    {
        read.handler = NULL;
        read.armed = 0;
        read.ready = 0;
        read.io_in_progress = 0;

        write.handler = NULL;
        write.armed = 0;
        write.ready = 0;
        write.io_in_progress = 0;
    }

    TCPsocket           socket;
    channel_t           read;
    channel_t           write;

    unsigned            error:1;
};

class StrBuffer
{
public:
    std::auto_ptr<std::string>        string;
    size_t                            pos;
};

class Network
{
public:
    Network()
        : State(ST_INIT)
        , SocketEvent()
        , NEvents(100)
        , SocketSet()
        , NetworkThread()
        , SyncMutex(true)
        , QuitFlag(false)
        , InputQueue()
        , OutputQueue()
        , OutputBuffer()
        , MessageLen(0)
        , Message(0)
    {
    }

    void Init();
    void Run();
    void Destroy();

    bool IsActive() const { return NetworkThread.IsRun(); };
    bool IsInputPending() const;

    void DequeueInputEvent(NetworkEvent&);
    void QueueOutputMessage(const NetworkMessage&);
    
    void StartNetworkThread(const std::string&, const std::string&);
    void JoinNetworkThread();

    static Network &Get();

    StateEnum                           State;
    ParseStateEnum                      ParseState;
    std::auto_ptr<IOEvent>              SocketEvent;
    size_t                              NEvents;
    SDLNet_SocketSet                    SocketSet;
    SDL::Thread                         NetworkThread;
    SDL::Mutex                          SyncMutex;
    volatile bool                       QuitFlag;

    std::list<NetworkEvent>             InputQueue;
    std::list<NetworkEvent>             OutputQueue;

    StrBuffer                           OutputBuffer;

    std::string                         SoftName;
    std::string                         SoftVersion;

    size_t                              MessageLen;
    Uint8                               *Message, *MessagePtr;

private:
    static int NetworkThreadWrapper(void*);

    void SetState(StateEnum);
    void SetState(StateEnum, const NetworkMessage&);
    void SetState(StateEnum, const std::string&);
    void QueueInputMessage(const NetworkMessage&);

    static void (Network::*StateHandlers[])(IOEvent&, const NetworkMessage&);

    void InitStateHandler(IOEvent&, const NetworkMessage&);
    void ConnectingStateHandler(IOEvent&, const NetworkMessage&);
    void ConnectedStateHandler(IOEvent&, const NetworkMessage&);
    void IdentifiedStateHandler(IOEvent&, const NetworkMessage&);
    void AuthorizedStateHandler(IOEvent&, const NetworkMessage&);
    void InChatStateHandler(IOEvent&, const NetworkMessage&);
    void InGameStateHandler(IOEvent&, const NetworkMessage&);
    void PlayingStateHandler(IOEvent&, const NetworkMessage&);
    void DisconnectedStateHandler(IOEvent&, const NetworkMessage&);
    void ErrorStateHandler(IOEvent&, const NetworkMessage&);

    void ConnectedHandler(IOEvent&);
    void MessageReadHandler(IOEvent&);
    void MessageWriteHandler(IOEvent&);

    void ArmEvent(IOEvent&);
    void DisarmEvent(IOEvent&);
};

#endif
