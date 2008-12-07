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

#ifdef WITH_TTF

#include "font.h"
#include "error.h"
#include "engine.h"
#include "surface.h"
#include "SDL_ttf.h"

bool SDL::Font::init = false;

SDL::Font::Font() : fnt(NULL)
{
}

SDL::Font::~Font()
{
    if(fnt) TTF_CloseFont(fnt);
}

void SDL::Font::Init(void)
{
    if(0 != TTF_Init()) Error::Verbose("Font::Init: error");
    else init = true;
}

void SDL::Font::Quit(void)
{
    TTF_Quit();
    init = false;
}

bool SDL::Font::isValid(void) const
{
    return fnt;
}

bool SDL::Font::Open(const std::string & filename, u8 size)
{
    if(init)
    {
	if(fnt) TTF_CloseFont(fnt);

	fnt = TTF_OpenFont(filename.c_str(), size);

	if(!fnt) Error::Warning("Font::Open: error open: " + filename);
    }
    return fnt;
}

void SDL::Font::SetStyle(u8 style)
{
    if(fnt) TTF_SetFontStyle(fnt, style);
}

void SDL::Font::RenderText(Surface & dst, const std::string & msg, const Colors & clr)
{
    if(dst.surface) SDL_FreeSurface(dst.surface);
    if(fnt)
    dst.surface = TTF_STYLE_BOLD & TTF_GetFontStyle(fnt) ? TTF_RenderUTF8_Blended(fnt, msg.c_str(), clr) : TTF_RenderUTF8_Solid(fnt, msg.c_str(), clr);
}

void SDL::Font::RenderChar(Surface & dst, char ch, const Colors & clr)
{
    char buf[2] = { '\0', '\0' };
         buf[0] = ch;

    if(dst.surface) SDL_FreeSurface(dst.surface);
    if(fnt)
    dst.surface = TTF_STYLE_BOLD & TTF_GetFontStyle(fnt) ? TTF_RenderUTF8_Blended(fnt, buf, clr) : TTF_RenderUTF8_Solid(fnt, buf, clr);
}

#endif
