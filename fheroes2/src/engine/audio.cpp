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

#include "engine.h"
#include "error.h"
#include "audio.h"

#define		NUM_SOUNDS	16

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

Audio::CVT::CVT()
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

bool Audio::CVT::Build(const Audio::Spec & src, const Audio::Spec & dst)
{
    if(1 == SDL_BuildAudioCVT(this, src.format, src.channels, src.freq, dst.format, dst.channels, dst.freq)) return true;

    Error::Warning("Audio::CVT::Build: " + Error::SDLError());

    return false;
}

bool Audio::CVT::Convert(void)
{
    if(0 == SDL_ConvertAudio(this)) return true;
    
    Error::Warning("Audio::CVT::Convert: " + Error::SDLError());

    return false;
}

Audio::Mixer::Mixer() : valid(true)
{
    if(SDL::SubSystem(INIT_AUDIO))
    {
	SDL_AudioSpec fmt;

	fmt.freq = 22050;
	fmt.format = AUDIO_S16;
        fmt.channels = 1;
	fmt.samples = 4096;
        fmt.callback = CallBack;
	fmt.userdata = NULL;

	if(0 > SDL_OpenAudio(&fmt, &hardware))
	{
	    Error::Warning("Audio::Mixer: " + std::string(SDL_GetError()));
	    
	    valid = false;
	}
	else
	{
	    SDL_PauseAudio(0);

	    sounds.resize(NUM_SOUNDS);
	}
    }
    else
    {
	Error::Warning("Audio::Mixer: audio subsystem not initialize");

	valid = false;
    }
}

Audio::Mixer::~Mixer()
{
    SDL_CloseAudio();

    sounds.clear();
}

Audio::Mixer & Audio::Mixer::Get(void)
{
    static Audio::Mixer mix;

    return mix;
}

bool Audio::Mixer::isValid(void) const
{
    return valid;
}

const Audio::Spec & Audio::Mixer::HardwareSpec(void) const
{
    return hardware;
}

bool Audio::Mixer::PredicateIsFreeSound(const mixer_t & header)
{
    return !header.active || (!header.loop && header.length == header.position);
}

void Audio::Mixer::PredicateStopSound(mixer_t & header)
{
    header.active = false;
}

void Audio::Mixer::CallBack(void *unused, u8 *stream, int size)
{
    std::vector<mixer_t> & sounds = Audio::Mixer::Get().sounds;

    int amount = 0;

    for(u8 ii = 0; ii < sounds.size(); ++ii)
    {
	mixer_t & header = sounds[ii];
	
        if(header.active)
        {
    	    amount = header.length - header.position;

	    if(amount > size) amount = size;

    	    SDL_MixAudio(stream, &header.data[header.position], amount, header.volume);

    	    header.position += amount;
    	    
    	    if(header.loop && header.length - amount <= header.position) header.position = 0;
	}
    }
}

void Audio::Mixer::Clear(void)
{
    if(! valid) return;

    SDL_PauseAudio(1);

    std::for_each(sounds.begin(), sounds.end(), PredicateStopSound);

    SDL_PauseAudio(0);
}

/* play sound, volume MIX_MAXVOLUME */
void Audio::Mixer::Play(const std::vector<u8> & body, const u8 volume, bool loop)
{
    if(! valid || body.empty()) return;

    // find same sound
    std::vector<mixer_t>::iterator it1 = sounds.begin();
    std::vector<mixer_t>::const_iterator it2 = sounds.end();
    
    const u8 * data = &body[0];

    for(; it1 != it2; ++it1)
    {
	const mixer_t & header = *it1;

	if(header.data == data) break;
    }
    
    // find free
    if(it2 == it1)
    {
	it1 = std::find_if(sounds.begin(), sounds.end(), PredicateIsFreeSound);

	if(it2 == it1)
	{
	    Error::Warning("Audio::Play: mixer is full, increase NUM_SOUNDS");

	    return;
	}
    }

    mixer_t & mixer = *it1;

    SDL_LockAudio();

    if(mixer.data != data || !loop)
    {
	mixer.data = data;
	mixer.length = body.size();
	mixer.position = 0;
    }

    mixer.loop = loop;
    mixer.volume = MIX_MAXVOLUME < volume ? MIX_MAXVOLUME : volume;
    mixer.active = true;

    SDL_UnlockAudio();
}
