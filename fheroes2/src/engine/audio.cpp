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

#include <string>
#include <vector>

#include "engine.h"
#include "error.h"
#include "audio.h"

#define		NUM_SOUNDS	16

namespace Audio
{
    struct mixer_t
    {
	mixer_t() : data(NULL), size(0), pos(0) {};

	u8 *	data;
	u32	size;
	u32	pos;
    };

    void Mixer(void *unused, u8 *stream, int size);
    bool PredicateIsFreeSound(const mixer_t & header);

    static std::vector<mixer_t> sounds;
    static Spec hard;
};

Audio::Spec::Spec()
{
    freq = 0;
    format = 0;
    channels = 0;
    silence = 0;
    samples = 0;
    size = 0;
    callback = NULL;
    userdata = NULL;
}

Audio::CVT::CVT() : valid(false)
{
    needed = 0;
    src_format = 0;
    dst_format = 0;
    rate_incr = 0;
    buf = NULL;
    len = 0;
    len_cvt = 0;
    len_mult = 0;
    len_ratio = 0;
    filters[0] = NULL;
    filters[1] = NULL;
    filters[2] = NULL;
    filters[3] = NULL;
    filters[4] = NULL;
    filters[5] = NULL;
    filters[6] = NULL;
    filters[7] = NULL;
    filters[8] = NULL;
    filters[9] = NULL;
    filter_index = 0;
}

const Audio::Spec & Audio::HardwareSpec(void)
{
    return hard;
}

bool Audio::OpenDevice(void)
{
    if(! SDL::SubSystem(INIT_AUDIO))
    {
	Error::Warning("Audio::OpenDevice: audio subsystem not initialize");

	return false;
    }

    Spec fmt;

    fmt.freq = 22050;
    fmt.format = AUDIO_S16;
    fmt.channels = 1;
    fmt.samples = 8192;
    fmt.callback = Audio::Mixer;
    fmt.userdata = NULL;

    if(0 > SDL_OpenAudio(&fmt, &hard))
    {
	Error::Warning("Audio::OpenDevice: " + std::string(SDL_GetError()));

	return false;
    }

    SDL_PauseAudio(0);

    sounds.resize(NUM_SOUNDS);

    return true;
}

void Audio::CloseDevice(void)
{
    SDL_CloseAudio();

    sounds.clear();
}

void Audio::Mixer(void *unused, u8 *stream, int size)
{
    int amount = 0;

    for(u8 ii = 0; ii < sounds.size(); ++ii)
    {
	mixer_t & header = sounds[ii];
	
        amount = header.size - header.pos;

	if(amount > size) amount = size;

        SDL_MixAudio(stream, &header.data[header.pos], amount, SDL_MIX_MAXVOLUME);

        header.pos += amount;
    }
}

void Audio::Lock(void)
{
    SDL_LockAudio();
}

void Audio::Unlock(void)
{
    SDL_UnlockAudio();
}

void Audio::Play(const CVT & cvt)
{
    if(! SDL::SubSystem(INIT_AUDIO))
    {
	Error::Warning("Audio::Play: audio subsystem not initialize");

	return;
    }

    std::vector<mixer_t>::iterator it = std::find_if(sounds.begin(), sounds.end(), Audio::PredicateIsFreeSound);
    
    if(sounds.end() == it)
    {
	Error::Warning("Audio::Play: mixer is full, increase NUM_SOUNDS");

	return;
    }

    mixer_t & mixer = *it;

    SDL_LockAudio();

    mixer.data = cvt.buf;
    mixer.size = cvt.len_cvt;
    mixer.pos = 0;

    SDL_UnlockAudio();
}

bool Audio::PredicateIsFreeSound(const mixer_t & header)
{
    return header.size == header.pos;
}
