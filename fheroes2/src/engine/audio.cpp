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
#include <algorithm>

#include "engine.h"
#include "error.h"
#include "audio.h"

namespace Music
{
    static Mix_Music * music	= NULL;
    static const void * id	= NULL;
}

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

Audio::Cdrom & Audio::Cdrom::Get(void)
{
    static Audio::Cdrom cdrom;

    return cdrom;
}

Audio::Cdrom::Cdrom() : cd(NULL)
{
    for(int i = 0; i < SDL_CDNumDrives(); i++)
    {
        SDL_CD *drive = SDL_CDOpen(i);

        if(drive)
        {
            if(!CD_INDRIVE(SDL_CDStatus(drive)))
            {
                SDL_CDClose(drive);
                continue;
            }
            else
	    if(drive->numtracks > 1 && drive->track[0].type == SDL_DATA_TRACK)
            {
                cd = drive;
                break;
            }
        }
    }

    Error::Verbose("Audio::CD: " + std::string(cd ? "found CD audio device." : "no CDROM devices available."));
}

Audio::Cdrom::~Cdrom()
{
    if(cd) SDL_CDClose(cd);
}

void Audio::Cdrom::Play(const u8 track)
{
    if(SDL_CDPlayTracks(cd, track - 1, 0, 1, 0) < 0)
        Error::Verbose("Couldn't play track ", track);

    if(SDL_CDStatus(cd) != CD_PLAYING)
    {
        Error::Warning("CD is not playing");
        Error::Warning(SDL_GetError());
    }
}

void Audio::Cdrom::Pause(void)
{
    if(cd) SDL_CDPause(cd);
}

void Music::Play(const std::vector<u8> & body)
{
    bool skip = false;

    if(id != &body[0]) Reset();
    else skip = true;

    if(!music && body.size())
    {
	id = &body[0];
	music = Mix_LoadMUS_RW(SDL_RWFromConstMem(&body[0], body.size()));
	Mix_HookMusicFinished(Reset);
    }

    if(music && !skip) Mix_PlayMusic(music, 0);
}

void Music::Volume(const u8 vol)
{
    Mix_VolumeMusic(MIX_MAX_VOLUME > vol ? vol : MIX_MAX_VOLUME);
}

void Music::Pause(void)
{
    if(music) Mix_PauseMusic();
}

void Music::Resume(void)
{
    if(music) Mix_ResumeMusic();
}

bool Music::isPlaying(void)
{
    return Mix_PlayingMusic();
}

bool Music::isPaused(void)
{
    return Mix_PausedMusic();
}

void Music::Reset(void)
{
    if(music)
    {
	Mix_HaltMusic();
        Mix_FreeMusic(music);
	music = NULL;
        id = NULL;
    }
}

Audio::Mixer::Mixer() : valid(true)
{
    if(SDL::SubSystem(INIT_AUDIO))
    {
	hardware.freq = 22050;
	hardware.format = AUDIO_S16;
        hardware.channels = 2;
	hardware.samples = 4096;

	if(0 != Mix_OpenAudio(hardware.freq, hardware.format, hardware.channels, hardware.samples))
	{
	    Error::Warning("Audio::Mixer: " + std::string(SDL_GetError()));
	    
	    valid = false;
	}
	else
	{
	    int channels = 0;
            Mix_QuerySpec(&hardware.freq, &hardware.format, &channels);
	    hardware.channels = channels;

	    Mix_AllocateChannels(CHANNEL_RESERVED + CHANNEL_FREE);
            Mix_ReserveChannels(CHANNEL_RESERVED);
	    Mix_ChannelFinished(FreeChunk);
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
    Music::Reset();
    Mixer::Reset();

    // free reserved
    for(u8 ch = 0; ch < CHANNEL_RESERVED; ++ch)
    {
	Mix_Chunk *chunk = Mix_GetChunk(ch);

	if(chunk) Mix_FreeChunk(chunk);
    }

    Mix_CloseAudio();
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

void Audio::Mixer::Pause(const int ch)
{
    if(0 > ch)
    	for(u8 ii = CHANNEL_RESERVED; ii < CHANNEL_RESERVED + CHANNEL_FREE; ++ii) Mix_Pause(ii);
    else
	Mix_Pause(ch);
}

void Audio::Mixer::PauseLoops(void)
{
    for(u8 ii = 0; ii < CHANNEL_RESERVED; ++ii)
        Mix_Pause(ii);
}

void Audio::Mixer::Resume(const int ch)
{
    if(0 > ch)
    	for(u8 ii = CHANNEL_RESERVED; ii < CHANNEL_RESERVED + CHANNEL_FREE; ++ii) Mix_Resume(ii);
    else
	Mix_Resume(ch);
}

void Audio::Mixer::ResumeLoops(void)
{
    for(u8 ii = 0; ii < CHANNEL_RESERVED; ++ii)
        Mix_Resume(ii);
}

u8 Audio::Mixer::Volume(const int ch, const int vol)
{
    return Mix_Volume(ch, MIX_MAX_VOLUME > vol ? vol : MIX_MAX_VOLUME);
}

void Audio::Mixer::Reset(const int ch)
{
    if(0 > ch)
    {
	Music::Reset();

	if(Audio::Cdrom::Get().isValid()) Audio::Cdrom::Get().Pause();

	for(u8 ii = 0; ii < CHANNEL_RESERVED + CHANNEL_FREE; ++ii)
	    if(ii < CHANNEL_RESERVED) Mix_Pause(ii);
	    else Mix_HaltChannel(ii);
    }
    else Mix_HaltChannel(ch);

    Error::Verbose("Audio::Mixer::Reset");
}

u8 Audio::Mixer::isPlaying(const int ch)
{
    return Mix_Playing(ch);
}

u8 Audio::Mixer::isPaused(const int ch)
{
    return Mix_Paused(ch);
}

void Audio::Mixer::PlayRAW(const std::vector<u8> & body, const int ch)
{
    if(0 <= ch && Mix_GetChunk(ch)) Mix_HaltChannel(ch);

    if(body.size())
    {
	Mix_Chunk* chunk = Mix_QuickLoad_RAW(const_cast<u8 *>(&body[0]), body.size());

	if(chunk)
	    Mix_PlayChannel(ch, chunk, 0);
	else
	    Error::Warning(Mix_GetError());
    }
}

void Audio::Mixer::LoadRAW(const std::vector<u8> & body, bool loop, const u8 ch)
{
    if(CHANNEL_RESERVED <= ch)
    {
	Error::Verbose("Audio::Mixer::LoadRAW: need reserved channel: ", ch);
	return;
    }

    Mix_HaltChannel(ch);
    Mix_Chunk* chunk = Mix_GetChunk(ch);
    if(ch) Mix_FreeChunk(chunk);

    if(body.size())
    {
	chunk = Mix_QuickLoad_RAW(const_cast<u8 *>(&body[0]), body.size());

	if(chunk)
	{
	    Mix_PlayChannel(ch, chunk, loop ? -1 : 0);
	    Mix_Pause(ch);
	}
	else
	    Error::Warning(Mix_GetError());
    }
}

void Audio::Mixer::FreeChunk(const int ch)
{
    if(CHANNEL_RESERVED <= ch)
    {
	Mix_Chunk *chunk = Mix_GetChunk(ch);

	if(chunk) Mix_FreeChunk(chunk);
    }
}

void Audio::Mixer::Reduce(void)
{
    // deprecater, remove later
}

void Audio::Mixer::Enhance(void)
{
    // deprecater, remove later
}
