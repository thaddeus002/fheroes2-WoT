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

#include "error.h"
#include "engine.h"
#include "surface.h"
#include "font.h"

#ifdef WITH_TTF
#include "SDL_ttf.h"

namespace Font
{
    TTF_Font *Get(void);
    
    static TTF_Font * fnt = NULL;
};

void Font::Init(void)
{
    if(0 != TTF_Init()) Error::Verbose("Font::Init: error");
}

void Font::Quit(void)
{
    TTF_Quit();
}

bool Font::isValid(void)
{
    return fnt;
}

TTF_Font* Font::Get(void)
{
    return fnt;
}

bool Font::Open(const std::string & filename, u8 size)
{
    if(fnt) TTF_CloseFont(fnt);

    fnt = TTF_OpenFont(filename.c_str(), size);

    if(!fnt) Error::Warning("Font::Open: error open: " + filename);

    return fnt;
}

void Font::Close(void)
{
    if(fnt) TTF_CloseFont(fnt);
    fnt = NULL;
}

#else
void Font::Init(void){};
void Font::Quit(void){};
bool Font::Open(const std::string &, u8){ return false; };
void Font::Close(void){};
#endif
