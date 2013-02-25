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

#include <sys/eventfd.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <stdexcept>

#include "network.h"

void Network::Init()
{
    struct sockaddr_in sin;
    struct sockaddr_in server_addr;

    State = ST_INIT;

    EventFdEvent.reset(new IOEvent());
    SocketEvent.reset(new IOEvent());

    std::cout << "Network thread is initializing" << std::endl;

    EpollEvents = new struct epoll_event[NEvents];

    if(EpollEvents == 0)
        throw std::runtime_error("Out of memory");

    EpollFd = ::epoll_create(NEvents);

    if(EpollFd == -1)
        throw std::runtime_error("Unable to initialize epoll interface");

    EventFdEvent->fd = ::eventfd(0, EFD_NONBLOCK);

    if(EventFdEvent->fd == -1)
        throw std::runtime_error("Unable to create event_fd");

    ::sigemptyset(&Sigmask);

    SocketEvent->fd = ::socket(AF_INET, SOCK_STREAM, 0);

    if(SocketEvent->fd == -1) {
        std::cout << "Could not create socket: " << strerror(errno) << std::endl;
        throw std::runtime_error("Could not create socket");
    }

    if(::fcntl(SocketEvent->fd, F_SETFL, O_NONBLOCK) < 0) {
        std::cout << "Unable to set non-blocking mode: " << strerror(errno) << std::endl;
        throw std::runtime_error("Unable to set non-blocking mode");
    }

    sin.sin_family = AF_INET;
    sin.sin_port = 0;
    sin.sin_addr.s_addr = INADDR_ANY;

    if(::bind(SocketEvent->fd, (const sockaddr*)&sin, sizeof(struct sockaddr_in)) != 0) {
        std::cout << "Unable to bind socket: " << strerror(errno) << std::endl;
        throw std::runtime_error("Unable to bind socket");
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(13536);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if(::connect(SocketEvent->fd, (const sockaddr*)&server_addr, sizeof(struct sockaddr_in)) < 0) {
        if(errno == EINPROGRESS) {
            State = ST_CONNECTING;
            std::cout << "Connecting" << std::endl;

            EventFdEvent->error = 0;
            EventFdEvent->read.armed = 1;
            EventFdEvent->read.handler = &Network::EventFdReadHandler;

            SocketEvent->error = 0;
            SocketEvent->write.armed = 1;
            SocketEvent->write.handler = &Network::ConnectedHandler;

            ArmEvent(*EventFdEvent);
            ArmEvent(*SocketEvent);
            return;
        }
    }

    std::cout << "Unable to connect: " << strerror(errno) << std::endl;
    throw std::runtime_error("Unable to connect");
}

void Network::Destroy()
{
    std::cout << "Network thread shutting down" << std::endl;

    delete Message;

    ::close(SocketEvent->fd);
    ::close(EventFdEvent->fd);
    ::close(EpollFd);
    delete EpollEvents;

    SocketEvent.reset(0);
    EventFdEvent.reset(0);
}

void Network::Run()
{
    bool quit = false;
    int NDesc;
    time_t Timeout;
    struct epoll_event *p, *q;

    ::signal(SIGPIPE, SIG_IGN);

    do {
        Timeout = -1;

        NDesc = ::epoll_wait(EpollFd, EpollEvents, NEvents, Timeout);

        if(NDesc == -1) {
            if(errno == EINTR) {
                continue;
            }

            std::cout << "epoll_wait() failed " << strerror(errno) << std::endl;
            throw std::runtime_error("epoll_wait() failed");
        }

        p = EpollEvents;
        q = p + NDesc;

        while(p != q) {
            IOEvent *ev = reinterpret_cast<IOEvent*>(p->data.ptr);

            if(p->events & EPOLLIN) {
                ev->read.ready = 1;
            }

            if(p->events & EPOLLOUT) {
                ev->write.ready = 1;
            }

            if(p->events & EPOLLERR) {
                ev->error = 1;
            }

            std::cout << "event fd=" << ev->fd << " read=" << ev->read.ready << " write=" << ev->write.ready << " error=" << ev->error << std::endl;

            if(p->events & EPOLLIN) {
                if(ev->read.handler != NULL) {
                    (this->*ev->read.handler)(*ev);
                }
            }

            if(p->events & EPOLLOUT) {
                if(ev->write.handler != NULL) {
                    (this->*ev->write.handler)(*ev);
                }
            }
            p++;
        }

        SyncMutex.Lock();
        quit = QuitFlag;
        SyncMutex.Unlock();
    } while(!quit);

    SyncMutex.Lock();
    QuitFlag = false;
    SyncMutex.Unlock();
}

void Network::EventFdReadHandler(IOEvent &e)
{
    uint64_t _hint;
    ssize_t s;

    do{
        s = ::read(e.fd, &_hint, sizeof(uint64_t));

        if(s < 0) {
            if(errno == EAGAIN) {
                e.read.ready = 0;
                return;
            }

            if(errno == EINTR) {
                continue;
            }
        }

        if (s == sizeof(uint64_t) && _hint > 0) {
            std::cout << "Network thread woken up" << std::endl;

            if(!OutputQueue.empty()) {
                Network::MessageWriteHandler(*SocketEvent);
            }
        }
    }while(s > 0);
}

void Network::ArmEvent(IOEvent &e, bool once) {
    struct epoll_event epoll_event;

    memset(&epoll_event, 0, sizeof(struct epoll_event));
    epoll_event.events = (e.read.armed ? EPOLLIN : 0) |
        (e.write.armed ? EPOLLOUT : 0) |
        (once ? EPOLLONESHOT : 0) |
        EPOLLET;
    epoll_event.data.ptr = &e;

    if(epoll_event.events) {
        if(::epoll_ctl(EpollFd, EPOLL_CTL_ADD, e.fd, &epoll_event) == -1) {
            std::cout << "epoll_ctl() failed: " << strerror(errno) << std::endl;
            throw std::runtime_error("epoll_ctl() failed");
        }
    }
}

void Network::RearmEvent(IOEvent &e, bool once) {
    struct epoll_event epoll_event;

    memset(&epoll_event, 0, sizeof(struct epoll_event));
    epoll_event.events = (e.read.armed ? EPOLLIN : 0) |
        (e.write.armed ? EPOLLOUT : 0) |
        (once ? EPOLLONESHOT : 0) |
        EPOLLET;
    epoll_event.data.ptr = &e;

    if(epoll_event.events) {
        if(::epoll_ctl(EpollFd, EPOLL_CTL_MOD, e.fd, &epoll_event) == -1) {
            std::cout << "epoll_ctl() failed: " << strerror(errno) << std::endl;
            throw std::runtime_error("epoll_ctl() failed");
        }
    }
}

void Network::DisarmEvent(IOEvent &e, bool once) {
    struct epoll_event epoll_event;

    memset(&epoll_event, 0, sizeof(struct epoll_event));
    epoll_event.events = (e.read.armed ? EPOLLIN : 0) |
        (e.write.armed ? EPOLLOUT : 0) |
        (once ? EPOLLONESHOT : 0) |
        EPOLLET;
    epoll_event.data.ptr = &e;

    if(epoll_event.events) {
        if(::epoll_ctl(EpollFd, EPOLL_CTL_DEL, e.fd, &epoll_event) == -1) {
            std::cout << "epoll_ctl() failed: " << strerror(errno) << std::endl;
            throw std::runtime_error("epoll_ctl failed");
        }
    }
}

void Network::WakeNetworkThread()
{
    static uint64_t value = 1;
    ssize_t rc;

    rc = ::write(EventFdEvent->fd, &value, sizeof(value));

    if(rc != sizeof(value)) {
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

void Network::StartNetworkThread(const std::string &_SoftName, const std::string &_SoftVersion)
{
    Network::Get().SoftName = _SoftName;
    Network::Get().SoftVersion = _SoftVersion;

    NetworkThread.Create(Network::NetworkThreadWrapper, (void*)this);
}

void Network::JoinNetworkThread()
{
    if(IsActive()) {
        WakeNetworkThread();    

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

    WakeNetworkThread();
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
