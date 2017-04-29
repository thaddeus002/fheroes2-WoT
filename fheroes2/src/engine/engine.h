/***************************************************************************
 *   Copyright (C) 2009 by Andrey Afletdinov <fheroes2@gmail.com>          *
 *                                                                         *
 *   Part of the Free Heroes2 Engine:                                      *
 *   http://sourceforge.net/projects/fheroes2                              *
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
#ifndef H2ENGINE_H
#define H2ENGINE_H

#include "types.h"
#include "sprites.h"
#include "display.h"
#include "localevent.h"
#include "error.h"
#include "rect.h"
#include "surface.h"
#include "rand.h"
#include "font.h"
#include "tools.h"
#include "serialize.h"
#include "translations.h"
#include "system.h"

#include "audio.h"
#include "audio_mixer.h"
#include "audio_music.h"
#include "audio_cdrom.h"

#define INIT_VIDEO        SDL_INIT_VIDEO
#define INIT_AUDIO        SDL_INIT_AUDIO
#define INIT_TIMER        SDL_INIT_TIMER
#define INIT_CDROM        SDL_INIT_CDROM

namespace SDL
{
    bool Init(const u32 system = INIT_VIDEO);
    void Quit(void);

    bool SubSystem(const u32 system);
}

#endif
