/***************************************************************************
 *   Copyright (C) 2008 by Andrey Afletdinov                               *
 *   Copyright (C) 2008 by Josh Matthews  <josh@joshmatthews.net>          *
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
#include "SDL_mixer.h"
#include "engine.h"
#include "audio.h"
#include "audio_cdrom.h"
#include "audio_music.h"
#include "audio_mixer.h"

#define CHANNEL_RESERVED        22
#define CHANNEL_FREE            6
#define CHANNEL_MAX             CHANNEL_RESERVED + CHANNEL_FREE

namespace Mixer
{
    void        HaltChannel(const int ch, bool force = false);
    void        Init(void);
    void        Quit(void);

    static bool valid = false;
    std::vector<Mix_Chunk *> chunks;
};

u8 Mixer::CountChannelReserved(void)
{
    return CHANNEL_RESERVED;
}

void Mixer::Init(void)
{
    if(SDL::SubSystem(SDL_INIT_AUDIO))
    {
	Audio::Spec & hardware = Audio::GetHardwareSpec();
        hardware.freq = 22050;
        hardware.format = AUDIO_S16;
        hardware.channels = 2;
        hardware.samples = 4096;

        if(0 != Mix_OpenAudio(hardware.freq, hardware.format, hardware.channels, hardware.samples))
        {
            Error::Warning("Mixer: " + Error::SDLError());
            valid = false;
        }
        else
        {
            int channels = 0;
    	    Mix_QuerySpec(&hardware.freq, &hardware.format, &channels);
            hardware.channels = channels;

            Mix_AllocateChannels(CHANNEL_MAX);
    	    Mix_ReserveChannels(CHANNEL_RESERVED);
            Mix_GroupChannels(CHANNEL_RESERVED, CHANNEL_MAX - 1, 0);
            chunks.resize(CHANNEL_MAX, NULL);
            
            valid = true;
        }
    }
    else
    {
        Error::Warning("Mixer: audio subsystem not initialize");

        valid = false;
    }
}

void Mixer::Quit(void)
{
    if(! SDL::SubSystem(SDL_INIT_AUDIO)) return;

    Music::Reset();
    Mixer::Reset();

    HaltChannel(-1, true);
    
    valid = false;

    Mix_CloseAudio();
}

void Mixer::Pause(const int ch)
{
    if(! valid) return;

    if(0 > ch)
    	for(u8 ii = CHANNEL_RESERVED; ii < CHANNEL_MAX; ++ii) Mix_Pause(ii);
    else
	Mix_Pause(ch);
}

void Mixer::PauseLoops(void)
{
    if(! valid) return;

    for(u8 ii = 0; ii < CHANNEL_RESERVED; ++ii)
        Mix_Pause(ii);
}

void Mixer::Resume(const int ch)
{
    if(! valid) return;

    if(0 > ch)
    	for(u8 ii = CHANNEL_RESERVED; ii < CHANNEL_MAX; ++ii) Mix_Resume(ii);
    else
	Mix_Resume(ch);
}

void Mixer::ResumeLoops(void)
{
    if(! valid) return;

    for(u8 ii = 0; ii < CHANNEL_RESERVED; ++ii)
        Mix_Resume(ii);
}

/* vol range: 0 - 10 */
u8 Mixer::Volume(const int ch, int vol)
{
    if(valid)
    {
	if(vol > 0)
	    vol = (vol > 10 ? 10 : vol) * MIX_MAX_VOLUME / 10;
	return Mix_Volume(ch, vol);
    }
    return 0;
}

void Mixer::Reset(const int ch)
{
    if(! valid) return;

    if(0 > ch)
    {
        Music::Reset();

        if(Cdrom::isValid()) Cdrom::Pause();

        for(u8 ii = 0; ii < CHANNEL_MAX; ++ii)
            if(ii < CHANNEL_RESERVED) Mix_Pause(ii);
            else HaltChannel(ii);
    }
    else HaltChannel(ch);
}

void Mixer::HaltChannel(const int ch, bool force)
{
    if(ch < 0)
    {
        for(u8 ii = 0; ii < CHANNEL_RESERVED + CHANNEL_FREE; ++ii)
            HaltChannel(ii, force);
    }
    else
    if(ch < CHANNEL_MAX)	// potential out_of_range for vector
    {
        Mix_HaltChannel(ch);
        if(chunks[ch] && (ch >= CHANNEL_RESERVED || force))
        {
            Mix_Chunk *chunk = Mix_GetChunk(ch);
            if(chunk)
                Mix_FreeChunk(chunk);
            chunks[ch] = NULL;
        }
    }
}

u8 Mixer::isPlaying(const int ch)
{
    if(! valid) return 0;

    return Mix_Playing(ch);
}

u8 Mixer::isPaused(const int ch)
{
    if(! valid) return 0;

    return Mix_Paused(ch);
}

bool Mixer::isValid(void)
{
    return valid;
}

void Mixer::PlayRAW(const std::vector<u8> & body, int ch)
{
    if(! valid) return;

    if(ch == -1)
        ch = Mix_GroupAvailable(0);
    HaltChannel(ch, true);

    if(body.size())
    {
        Mix_Chunk* chunk = Mix_QuickLoad_RAW(const_cast<u8 *>(&body[0]), body.size());

        if(chunks[ch] != NULL)
            Error::Warning("Mixer::PlayRAW: Previous mix chunk was not freed");
        chunks[ch] = chunk;

        if(chunk)
            Mix_PlayChannel(ch, chunk, 0);
        else
            Error::Warning(Mix_GetError());
    }
}

void Mixer::LoadRAW(const std::vector<u8> & body, bool loop, const u8 ch)
{
    if(! valid) return;

    if(CHANNEL_RESERVED <= ch)
    {
        Error::Verbose("Mixer::LoadRAW: need reserved channel: ", ch);
        return;
    }

    HaltChannel(ch, true);

    if(body.size())
    {
        Mix_Chunk *chunk = Mix_QuickLoad_RAW(const_cast<u8 *>(&body[0]), body.size());

        if(chunks[ch] != NULL)
            Error::Warning("Mixer::LoadRAW: Previous mix chunk was not freed");
        chunks[ch] = chunk;

        if(chunk)
        {
            Mix_PlayChannel(ch, chunk, loop ? -1 : 0);
            Mix_Pause(ch);
        }
        else Error::Warning(Mix_GetError());
    }
}

void Mixer::Reduce(void)
{
    if(! Mixer::valid) return;
}

void Mixer::Enhance(void)
{
    if(! Mixer::valid) return;
}
