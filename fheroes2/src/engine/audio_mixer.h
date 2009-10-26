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

#ifndef H2AUDIO_MIXER_H
#define H2AUDIO_MIXER_H

#include <vector>
#include "types.h"

namespace Mixer
{
    bool	isValid(void);
    u8		Volume(const int ch, int vol = -1);
    void	Pause(const int ch = -1);
    void	PauseLoops(void);
    void	Resume(const int ch = -1);
    void	ResumeLoops(void);
    void	Reset(const int ch = -1);
    u8		isPlaying(const int ch);
    u8		isPaused(const int ch);
    void	PlayRAW(const std::vector<u8> & body, int ch = -1);
    void	LoadRAW(const std::vector<u8> & body, bool loop, const u8 ch);
    u8		CountChannelReserved(void);

#ifdef WITH_MIXER
    void	PlayRAW(const char* file, int ch = -1);
    void	LoadRAW(const char* file, bool loop, const u8 ch);
#endif

    void	Reduce(void);
    void	Enhance(void);
}

#endif
