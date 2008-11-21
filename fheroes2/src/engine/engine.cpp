/***************************************************************************
 *   Copyright (C) 2008 by Andrey Afletdinov                               *
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

#include "error.h"
#include "engine.h"
#include "SDL.h"

namespace Mixer
{
    void Init(void);
    void Quit(void);
}

namespace Cdrom
{
    void Open(void);
    void Close(void);
}

bool SDL::Init(const u32 system)
{
    u32 flags = INIT_NONE;
    if(system & INIT_VIDEO)
        flags |= SDL_INIT_VIDEO;
    if(system & INIT_AUDIO)
        flags |= SDL_INIT_AUDIO;
    if(system  & INIT_TIMER)
        flags |= SDL_INIT_TIMER;
    if(system  & INIT_CDROM)
        flags |= SDL_INIT_CDROM;
    
    if(0 > SDL_Init(flags))
    {
	Error::Warning("SDL::Init: error: " + std::string(SDL_GetError()));

	return false;
    }

    if(INIT_AUDIO & system) Mixer::Init();
    if(INIT_CDROM & system) Cdrom::Open();

    return true;
}

void SDL::Quit(void)
{
    if(SubSystem(INIT_CDROM)) Cdrom::Close();
    if(SubSystem(INIT_AUDIO)) Mixer::Quit();

    SDL_Quit();
}

bool SDL::SubSystem(const u32 system)
{
    return system & SDL_WasInit(system);
}
