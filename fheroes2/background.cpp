/***************************************************************************
 *   Copyright (C) 2006 by Andrey Afletdinov                               *
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

#include "background.h"

void Background::Save(void)
{
    // resize background
    if((surface && rect.w != surface->w()) || (surface && rect.h != surface->h())){ delete surface; surface = NULL; }

    if(!surface) surface = new SDLmm::Surface(SDLmm::Surface::CreateSurface(SDL_SWSURFACE, rect.w, rect.h, DEFAULT_DEPTH));

    surface->Blit(SDLmm::Display::GetDisplay(), rect, Point(0,0));
}

void Background::Restore(void)
{
    static SDLmm::Display &display = SDLmm::Display::GetDisplay();

    display.Blit(*surface, rect);
}
