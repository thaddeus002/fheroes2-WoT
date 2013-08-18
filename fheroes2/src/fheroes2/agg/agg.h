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

#ifndef H2AGG_H
#define H2AGG_H

#include "gamedefs.h"
#include "icn.h"
#include "til.h"
#include "m82.h"
#include "sprite.h"
#include "mus.h"

namespace AGG
{	
    bool	Init(void);
    void	Quit(void);

    Sprite	GetICN(ICN::icn_t, u32 index, bool reflect = false);
    int		GetICNCount(ICN::icn_t);
    Surface	GetTIL(TIL::til_t, u32 index, u8 shape);
    Surface	GetLetter(char ch, u8 ft);
#ifdef WITH_TTF
    Surface	GetUnicodeLetter(u16 ch, u8 ft);
    int		GetFontHeight(bool small);
#endif
    void	LoadLOOPXXSounds(const u16*);
    void	PlaySound(M82::m82_t);
    void	PlayMusic(MUS::mus_t, bool loop = true);
    void	ResetMixer(void);
}

#endif
