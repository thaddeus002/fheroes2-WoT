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
#include "font.h"
#include "sdlnet.h"

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
    if(0 > SDL_Init(system))
    {
	Error::Warning("SDL::Init: error: " + std::string(SDL_GetError()));

	return false;
    }

    if(SDL_INIT_AUDIO & system) Mixer::Init();
    if(SDL_INIT_CDROM & system) Cdrom::Open();
#ifdef WITH_TTF
    SDL::Font::Init();
#endif
#ifdef WITH_NET
    Network::Init();
#endif

    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

    return true;
}

void SDL::Quit(void)
{
#ifdef WITH_NET
    Network::Quit();
#endif
#ifdef WITH_TTF
    SDL::Font::Quit();
#endif
    if(SubSystem(SDL_INIT_CDROM)) Cdrom::Close();
    if(SubSystem(SDL_INIT_AUDIO)) Mixer::Quit();

    SDL_Quit();
}

bool SDL::SubSystem(const u32 system)
{
    return system & SDL_WasInit(system);
}
