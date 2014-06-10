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

#ifdef WITH_TTF

#include <iostream>
#include "font.h"
#include "engine.h"
#include "surface.h"
#include "SDL_ttf.h"

SDL::Font::Font() : ptr(NULL)
{
}

SDL::Font::~Font()
{
    if(ptr) TTF_CloseFont(ptr);
}

void SDL::Font::Init(void)
{
    if(0 != TTF_Init()) Error::Message(__FUNCTION__, SDL_GetError());
}

void SDL::Font::Quit(void)
{
    TTF_Quit();
}

bool SDL::Font::isValid(void) const
{
    return ptr;
}

bool SDL::Font::Open(const std::string & filename, int size)
{
    if(ptr) TTF_CloseFont(ptr);
    ptr = TTF_OpenFont(filename.c_str(), size);
    if(!ptr) Error::Message(__FUNCTION__, SDL_GetError());
    return ptr;
}

void SDL::Font::SetStyle(int style)
{
    TTF_SetFontStyle(ptr, style);
}

int SDL::Font::Height(void) const
{
    return TTF_FontHeight(ptr);
}

int SDL::Font::Ascent(void) const
{
    return TTF_FontAscent(ptr);
}

int SDL::Font::Descent(void) const
{
    return TTF_FontDescent(ptr);
}

int SDL::Font::LineSkip(void) const
{
    return TTF_FontLineSkip(ptr);
}

#endif
