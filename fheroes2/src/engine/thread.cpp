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

#include <iostream>
#include "thread.h"

Thread::Thread() : thread(NULL)
{
}

Thread::~Thread()
{
    Kill();
}

void Thread::Create(int (*fn)(void *), void *param)
{
    thread = SDL_CreateThread(fn, param);
}

int Thread::Wait(void)
{
    int status = 0;
    if(thread) SDL_WaitThread(thread, &status);
    thread = NULL;
    return status;
}

void Thread::Kill(void)
{
    if(thread) SDL_KillThread(thread);
    thread = NULL;
}

bool Thread::IsRun(void) const
{
    return thread;
}

u32 Thread::GetID(void) const
{
    return thread ? SDL_GetThreadID(thread) : 0;
}

Mutex::Mutex() : mutex(SDL_CreateMutex())
{
}

Mutex::~Mutex()
{
    if(mutex) SDL_DestroyMutex(mutex);
}

bool Mutex::Lock(void) const
{
    return mutex ? 0 == SDL_mutexP(mutex) : false;
}

bool Mutex::Unlock(void) const
{
    return mutex ? 0 == SDL_mutexV(mutex) : false;
}

Timer::Timer() : id(0)
{
}

void Timer::Run(Timer & timer, u32 interval, u32 (*fn)(u32, void *), void *param)
{
    if(timer.id) Remove(timer);

    timer.id = SDL_AddTimer(interval, fn, param);
}

void Timer::Remove(Timer & timer)
{
    if(timer.id)
    {
	SDL_RemoveTimer(timer.id);
	timer.id = 0;
    }
}

bool Timer::IsValid(void) const
{
    return id;
}

Time::Time()
{
}

void Time::Start(void)
{
    gettimeofday(&clock1, NULL);
}

void Time::Stop(void)
{
    gettimeofday(&clock2, NULL);
}

double Time::Get(void) const
{
    return ((clock2.tv_sec - clock1.tv_sec) * 1000 + (clock2.tv_usec - clock1.tv_usec) / 1000.0) + 0.5;
}

void Time::Print(void) const
{
    std::cout << "Performance: " << Get() << " ms" << std::endl;
}
