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

#include <sstream>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <sys/utsname.h>

#include "network.h"
#include "network_protocol.h"

void Network::InitStateHandler(IOEvent&, const NetworkMessage&)
{
    std::cout << "Network::InitStateHandler" << std::endl;
}

void Network::ConnectingStateHandler(IOEvent&, const NetworkMessage&)
{
    std::cout << "Network::ConnectingStateHandler" << std::endl;
}

void Network::ConnectedStateHandler(IOEvent&, const NetworkMessage &Msg)
{
    std::cout << "Network::ConnectedStateHandler" << std::endl;

    switch(Msg.GetType()) {
        case HMM2_SERVER_INFO:
            std::cout << "Connected to server " << Msg.GetStr(HMM2_SERVER_NAME) << std::endl;
            std::cout << "Server running " << Msg.GetStr(HMM2_SERVER_SOFTNAME) << " "
                     << Msg.GetStr(HMM2_SERVER_SOFTVERSION) << " on "
                     << Msg.GetStr(HMM2_SERVER_OSNAME) << " "
                     << Msg.GetStr(HMM2_SERVER_OSVERSION) << std::endl;

            SetState(ST_IDENTIFIED, Msg);
            break;
        case HMM2_CLIENT_UNKNOWN:
            SetState(ST_ERROR, "Server won't let us in because client version is invalid");
            break;
        case HMM2_REDIRECT:
        default:
            SetState(ST_ERROR, "Unable to login to server");
            break;
    }
}

void Network::IdentifiedStateHandler(IOEvent&, const NetworkMessage &Msg)
{
    std::cout << "Network::IdentifiedStateHandler" << std::endl;

    if(Msg.GetType() == HMM2_CREATE_GAME_RESPONSE) {
        SetState(ST_INGAME, Msg);
    }
    else {
        QueueInputMessage(Msg);
    }
}

void Network::AuthorizedStateHandler(IOEvent&, const NetworkMessage&)
{
    std::cout << "Network::AuthorizedStateHandler" << std::endl;
}

void Network::InChatStateHandler(IOEvent&, const NetworkMessage&)
{
    std::cout << "Network::InChatStateHandler" << std::endl;
}

void Network::InGameStateHandler(IOEvent&, const NetworkMessage &Msg)
{
    std::cout << "Network::InGameStateHandler" << std::endl;
    QueueInputMessage(Msg);
}

void Network::PlayingStateHandler(IOEvent&, const NetworkMessage&)
{
    std::cout << "Network::PlayingStateHandler" << std::endl;
}

void Network::DisconnectedStateHandler(IOEvent&, const NetworkMessage&)
{
    std::cout << "Network::DisconnectedStateHandler" << std::endl;
}

void Network::ErrorStateHandler(IOEvent&, const NetworkMessage&)
{
    std::cout << "Network::ErrorStateHandler" << std::endl;
}

void (Network::*Network::StateHandlers[])(IOEvent&, const NetworkMessage&) = {
    &Network::InitStateHandler,
    &Network::ConnectingStateHandler,
    &Network::ConnectedStateHandler,
    &Network::IdentifiedStateHandler,
    &Network::AuthorizedStateHandler,
    &Network::InChatStateHandler,
    &Network::InGameStateHandler,
    &Network::PlayingStateHandler,
    &Network::DisconnectedStateHandler,
    &Network::ErrorStateHandler
};

void Network::ConnectedHandler(IOEvent &e)
{
    int error;
    socklen_t optlen;
    long opt;

    if(e.error) {
        if(::getsockopt(e.fd, SOL_SOCKET, SO_ERROR, &error, &optlen) >= 0) {
            std::cout << "Network error occured: " << strerror(error) << std::endl;
        }
        else {
            std::cout << "Network error occured" << std::endl;
        }

        SyncMutex.Lock();
        InputQueue.push_back(NetworkEvent());

        NetworkEvent &ne = InputQueue.back();

        ne.OldState = State;
        ne.NewState = ST_DISCONNECTED;
        SyncMutex.Unlock();

        SetState(ST_DISCONNECTED);

        e.error = 0;
        return;
    }

    std::cout << "Connected" << std::endl;

    ParseState = ST_LEN_HI;
    MessageLen = 0;

    opt = 1;
    if(::setsockopt(e.fd, SOL_TCP, TCP_NODELAY, (const void*)&opt, sizeof(opt)) < 0) {
        std::cout << "Unable to turn off Nagle algorithm for socket: " << strerror(errno) << std::endl;
        return;
    }

    SyncMutex.Lock();
    InputQueue.push_back(NetworkEvent());

    NetworkEvent &ne = InputQueue.back();

    ne.OldState = State;
    ne.NewState = ST_CONNECTED;
    SyncMutex.Unlock();

    SetState(ST_CONNECTED);

    OutputQueue.push_back(NetworkEvent());

    NetworkEvent &one = OutputQueue.back();

    struct utsname name;

    ::uname(&name);

    one.Message.reset(new NetworkMessage(HMM2_CLIENT_INFO));

    one.Message->add_str_chunk(HMM2_CLIENT_TAG, "HMM2");
    one.Message->add_str_chunk(HMM2_CLIENT_VERSION, "0.1");
    one.Message->add_str_chunk(HMM2_CLIENT_OSNAME, name.sysname);
    one.Message->add_str_chunk(HMM2_CLIENT_OSVERSION, name.version);
    one.Message->add_str_chunk(HMM2_CLIENT_SOFTNAME, SoftName);
    one.Message->add_str_chunk(HMM2_CLIENT_SOFTVERSION, SoftVersion);

    std::cout << "Queued output message " << &one << std::endl;

    e.read.armed = 1;
    e.read.handler = &Network::MessageReadHandler;
    e.write.handler = &Network::MessageWriteHandler;

    RearmEvent(e);

    Network::MessageWriteHandler(e);
}

void Network::MessageReadHandler(IOEvent &e)
{
    int rc;
    u_char buf[1024];
    u_char *p, *q;

    do {
        rc = ::read(e.fd, buf, sizeof(buf));

        std::cout << "read #" << e.fd << " " << rc << std::endl;

        if(rc == -1) {
            if(errno == EAGAIN || errno == EWOULDBLOCK) {
                e.read.ready = 0;
                RearmEvent(e);
                break;
            }
            break;
        }
        else if(rc == 0) {
            std::cout << "#" << e.fd << " input finished" << std::endl;
            SetState(ST_DISCONNECTED);
            e.read.armed = 0;
            RearmEvent(e);
            break;
        }

        p = buf;
        q = p + rc;

        while(p != q) {
            switch(ParseState) {
                case ST_LEN_HI:
                    MessageLen = *p << 8;
                    ParseState = ST_LEN_LO;
                    break;
                case ST_LEN_LO:
                    MessageLen |= *p;

                    Message = new u_char[MessageLen];

                    if(Message == NULL) {
                        return;
                    }

                    MessagePtr = Message;

                    *MessagePtr++ = MessageLen & 0xff;
                    *MessagePtr++ = MessageLen >> 8;

                    MessageLen -= 2;

                    ParseState = ST_BODY;
                    break;
                case ST_BODY:
                    *MessagePtr++ = *p;
                    MessageLen--;

                    if(MessageLen == 0) {
                        ParseState = ST_LEN_HI;

                        NetworkMessage Msg;

                        std::istringstream i(std::string((char*)Message, MessagePtr - Message));
                        i >> Msg;

                        std::cout << ">>> " << Msg.GetType() << std::endl;

                        delete Message;
                        Message = MessagePtr = 0;

                        if(i.good()) {
                            (this->*StateHandlers[State])(e, Msg);
                        }
                    }
                    break;
            }
            p++;
        }
    }while(1);
}

void Network::MessageWriteHandler(IOEvent &e)
{
    const char *pos, *last;
    ssize_t rc;

    if(OutputBuffer.string.get() == 0) {
        std::ostringstream message;

        NetworkEvent &ne = OutputQueue.front();

        message << *ne.Message;

        OutputBuffer.string.reset(new std::string(message.str()));
        OutputBuffer.pos = 0;
    }

    do {
        pos = OutputBuffer.string->c_str() + OutputBuffer.pos;
        last = OutputBuffer.string->c_str() + OutputBuffer.string->size();

        if(pos == last) {
            OutputBuffer.string.reset(0);

            std::cout << "<<< " << OutputQueue.front().Message->GetType() << std::endl;
            std::cout << "Removed output message " << &OutputQueue.front() << std::endl;

            OutputQueue.pop_front();

            if(OutputQueue.empty()) {
                e.write.armed = 0;
                RearmEvent(e);
            }

            break;
        }

        rc = ::write(e.fd, pos, last - pos);

        std::cout << "write #" << e.fd << " " << rc << std::endl;

        if(rc == -1 || rc == 0) {
            if(errno == EAGAIN || errno == EWOULDBLOCK) {
                e.write.ready = 0;
                RearmEvent(e);
                break;
            }
            break;
        }
        else {
            OutputBuffer.pos += rc;
        }
    }while(1);
}
