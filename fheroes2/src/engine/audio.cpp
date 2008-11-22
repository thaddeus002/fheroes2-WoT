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

#include "SDL.h"
#include "SDL_mixer.h"
#include "engine.h"
#include "error.h"
#include "audio.h"
#include "audio_interface.h"

const u32 MAXVOLUME = MIX_MAX_VOLUME;

namespace Music
{
    static Mix_Music * music	= NULL;
    static const void * id	= NULL;
}

namespace Mixer
{
    void FreeChunk(const int ch);
    void Init(void);
    void Quit(void);

    static Audio::Spec hardware;
    static bool valid		= false;
}

namespace Cdrom
{
    void Open(void);
    void Close(void);
    
    static SDL_CD *cd		  = NULL;
    static int currentTrack       = -1;
    static unsigned int startTime = 0;
    static unsigned int tickLength;
    static SDL_Thread *loopThread;
    static SDL_mutex *cdLock;
    
    int LoopCheck(void *data);
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

void Cdrom::Open(void)
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
    
    if(cd)
    {
        loopThread = SDL_CreateThread(&LoopCheck, NULL);
        cdLock = SDL_CreateMutex();
    }

    Error::Verbose("Cdrom: " + std::string(cd ? "found CD audio device." : "no CDROM devices available."));
}

void Cdrom::Close(void)
{
    if(cd)
    {
        SDL_CDStop(cd);
        SDL_KillThread(loopThread);
        SDL_DestroyMutex(cdLock);
	SDL_CDClose(cd);
	cd = NULL;
    }
}

bool Cdrom::isValid(void)
{
    return cd;
}

int Cdrom::LoopCheck(void *data)
{
    while(1)
    {
        SDL_Delay(5000);
        SDL_LockMutex(cdLock);
        if(startTime && SDL_GetTicks() - startTime > tickLength)
            Play(currentTrack, true, true);
        SDL_UnlockMutex(cdLock);
    }
    return 0;
}

void Cdrom::Play(const u8 track, bool loop, bool force)
{
    if(Mixer::valid && cd)
    {
        SDL_LockMutex(cdLock);
        
        if(currentTrack != track || force)
        {
            if(SDL_CDPlayTracks(cd, track, 0, 1, 0) < 0)
                Error::Verbose("Couldn't play track ", track);
            
            currentTrack = track;
            if(loop)
            {
              tickLength = (unsigned int)((cd->track[track].length / CD_FPS) * 0.01f);
              startTime = SDL_GetTicks();
            }
            else startTime = 0;

            if(SDL_CDStatus(cd) != CD_PLAYING)
            {
                Error::Warning("CD is not playing");
                Error::Warning(SDL_GetError());
            }
        }
        
        SDL_UnlockMutex(cdLock);
    }
}

void Cdrom::Pause(void)
{
    if(cd) SDL_CDPause(cd);
}

void Music::Play(const std::vector<u8> & body, bool loop)
{
    if(! Mixer::valid) return;

    bool skip = false;

    if(id != &body[0]) Reset();
    else skip = true;

    if(!music && body.size())
    {
	id = &body[0];
	music = Mix_LoadMUS_RW(SDL_RWFromConstMem(&body[0], body.size()));
	Mix_HookMusicFinished(Reset);
    }

    if(music && !skip) Mix_PlayMusic(music, loop ? -1 : 0);
}

void Music::Volume(const u8 vol)
{
    if(Mixer::valid) Mix_VolumeMusic(MIX_MAX_VOLUME > vol ? vol : MIX_MAX_VOLUME);
}

void Music::Pause(void)
{
    if(Mixer::valid && music) Mix_PauseMusic();
}

void Music::Resume(void)
{
    if(Mixer::valid && music) Mix_ResumeMusic();
}

bool Music::isPlaying(void)
{
    return Mixer::valid && Mix_PlayingMusic();
}

bool Music::isPaused(void)
{
    return Mixer::valid && Mix_PausedMusic();
}

void Music::Reset(void)
{
    if(Mixer::valid && music)
    {
	Mix_HaltMusic();
        Mix_FreeMusic(music);
	music = NULL;
        id = NULL;
    }
}

void Mixer::Init(void)
{
    if(SDL::SubSystem(SDL_INIT_AUDIO))
    {
	hardware.freq = 22050;
	hardware.format = AUDIO_S16;
	hardware.channels = 2;
	hardware.samples = 4096;

	if(0 != Mix_OpenAudio(hardware.freq, hardware.format, hardware.channels, hardware.samples))
	{
	    Error::Warning("Mixer: " + std::string(SDL_GetError()));
	    
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
    if(! SDL::SubSystem(INIT_AUDIO)) return;

    Music::Reset();
    Mixer::Reset();

    // free reserved
    for(u8 ch = 0; ch < CHANNEL_RESERVED; ++ch)
    {
	Mix_Chunk *chunk = Mix_GetChunk(ch);

	if(chunk) Mix_FreeChunk(chunk);
    }

    valid = false;

    Mix_CloseAudio();
}

const Audio::Spec & Mixer::HardwareSpec(void)
{
    return hardware;
}

void Mixer::Pause(const int ch)
{
    if(! valid) return;

    if(0 > ch)
    	for(u8 ii = CHANNEL_RESERVED; ii < CHANNEL_RESERVED + CHANNEL_FREE; ++ii) Mix_Pause(ii);
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
    	for(u8 ii = CHANNEL_RESERVED; ii < CHANNEL_RESERVED + CHANNEL_FREE; ++ii) Mix_Resume(ii);
    else
	Mix_Resume(ch);
}

void Mixer::ResumeLoops(void)
{
    if(! valid) return;

    for(u8 ii = 0; ii < CHANNEL_RESERVED; ++ii)
        Mix_Resume(ii);
}

u8 Mixer::Volume(const int ch, const int vol)
{
    if(! valid) return 0;

    return Mix_Volume(ch, MIX_MAX_VOLUME > vol ? vol : MIX_MAX_VOLUME);
}

void Mixer::Reset(const int ch)
{
    if(! valid) return;

    if(0 > ch)
    {
	Music::Reset();

	if(Cdrom::isValid()) Cdrom::Pause();

	for(u8 ii = 0; ii < CHANNEL_RESERVED + CHANNEL_FREE; ++ii)
	    if(ii < CHANNEL_RESERVED) Mix_Pause(ii);
	    else Mix_HaltChannel(ii);
    }
    else Mix_HaltChannel(ch);
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

void Mixer::PlayRAW(const std::vector<u8> & body, const int ch)
{
    if(! valid) return;

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

void Mixer::LoadRAW(const std::vector<u8> & body, bool loop, const u8 ch)
{
    if(! valid) return;

    if(CHANNEL_RESERVED <= ch)
    {
	Error::Verbose("Mixer::LoadRAW: need reserved channel: ", ch);
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

void Mixer::FreeChunk(const int ch)
{
    if(CHANNEL_RESERVED <= ch)
    {
	Mix_Chunk *chunk = Mix_GetChunk(ch);

	if(chunk) Mix_FreeChunk(chunk);
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
