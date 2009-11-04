/***************************************************************************
 *   Copyright (C) 2008 by Andrey Afletdinov                               *
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
#include "audio_mixer.h"
#include "audio_music.h"

#ifdef WITH_MIXER
#include "SDL_mixer.h"

namespace Music
{
    static Mix_Music * music	= NULL;
    static const void * id	= NULL;
}

void Music::Play(const std::vector<u8> & body, bool loop)
{
    if(! Mixer::isValid()) return;

    if(id != &body[0])
    {
        Reset();

        if(body.size())
        {
            id = &body[0];
            SDL_RWops *rwops = SDL_RWFromConstMem(&body[0], body.size());
            music = Mix_LoadMUS_RW(rwops);
            SDL_FreeRW(rwops);
            Mix_PlayMusic(music, loop ? -1 : 0);
        }
    }
}

void Music::Play(const std::string &, bool)
{
}

/* range 0 - 10 */
u8 Music::Volume(int vol)
{
    if(Mixer::isValid())
    {
	if(vol > 0)
	    vol = (vol > 10 ? 10 : vol) * MIX_MAX_VOLUME / 10;
	return Mix_VolumeMusic(vol);
    }
    return 0;
}

void Music::Pause(void)
{
    if(Mixer::isValid() && music) Mix_PauseMusic();
}

void Music::Resume(void)
{
    if(Mixer::isValid() && music) Mix_ResumeMusic();
}

bool Music::isPlaying(void)
{
    return Mixer::isValid() && Mix_PlayingMusic();
}

bool Music::isPaused(void)
{
    return Mixer::isValid() && Mix_PausedMusic();
}

void Music::Reset(void)
{
    if(Mixer::isValid() && music)
    {
        Mix_HaltMusic();
        Mix_FreeMusic(music);
        music = NULL;
        id = NULL;
    }
}

#else
#include "thread.h"

struct info_t
{
    info_t() : loop(false){};
    std::string run;
    bool loop;
};

namespace Music
{
    Thread music;
    info_t info;
};

int callbackPlayMusic(void *ptr)
{
    if(ptr && system(NULL))
    {
	info_t & info = *reinterpret_cast<info_t *>(ptr);
	if(info.loop)
	{
	    while(1){ system(info.run.c_str()); DELAY(10); }
	}
	else
	return system(info.run.c_str());
    }
    return -1;
}

void Music::Play(const std::vector<u8> &, bool)
{
}

void Music::Play(const std::string & run, bool loop)
{
    if(music.IsRun())
    {
	if(info.run == run) return;
	music.Kill();
    }
    info.run = run;
    info.loop = loop;
    music.Create(callbackPlayMusic, &info);
}

u8 Music::Volume(int vol)
{
    return 0;
}

void Music::Pause(void)
{
}

void Music::Resume(void)
{
}

bool Music::isPlaying(void)
{
    return music.IsRun();
}

bool Music::isPaused(void)
{
    return false;
}

void Music::Reset(void)
{
    if(music.IsRun()) music.Kill();
}

#endif
