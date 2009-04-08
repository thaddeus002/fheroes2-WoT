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

#ifndef SDLTHREAD_H
#define SDLTHREAD_H

#include "SDL_thread.h"
#include "types.h"

class Thread
{
public:
    Thread();
    ~Thread();

    void	Create(int (*)(void *), void *param = NULL);
    int		Wait(void);
    void	Kill(void);

    bool	IsRun(void) const;

    u32		GetID(void) const;

private:
    SDL_Thread *thread;
};

class Mutex
{
public:
    Mutex();
    ~Mutex();

    bool Lock(void) const;
    bool Unlock(void) const;

private:
    SDL_mutex *mutex;
};

class Timer
{
public:
    Timer();

    static void Run(Timer &, u32, u32 (*)(u32, void *), void *param = NULL);
    static void Remove(Timer &);

private:
    SDL_TimerID id;
};

#endif
