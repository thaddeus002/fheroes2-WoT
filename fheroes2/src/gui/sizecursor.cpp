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

#include "rect.h"
#include "spritecursor.h"
#include "error.h"
#include "maps.h"
#include "radar.h"
#include "gamearea.h"
#include "sizecursor.h"

SizeCursor::SizeCursor() : sf(NULL), sc(NULL), hide(false)
{
}

SizeCursor::~SizeCursor()
{
    if(sc) delete sc;
    if(sf) delete sf;
}

void SizeCursor::Hide(void)
{
    if(hide || !sc) return;

    sc->Hide();
}

void SizeCursor::Show(void)
{
    if(!hide || !sc) return;
    
    sc->Show();
}

void SizeCursor::Move(const Point & pt)
{
    Move(pt.x, pt.y);
}

void SizeCursor::Move(const u16 px, const u16 py)
{
    if(hide || !sc) return;

    sc->Move(px, py);
}

u8 SizeCursor::w(void)
{
    return sf ? sf->w() / TILEWIDTH : 0;
}

u8 SizeCursor::h(void)
{
    return sf ? sf->h() / TILEWIDTH : 0;
}

void SizeCursor::ModifySize(const Size & sz)
{
    ModifySize(sz.w, sz.h);
}

void SizeCursor::ModifySize(const u8 w, const u8 h)
{
    if(w > GameArea::GetRect().w || h > GameArea::GetRect().h)
    {
    	Error::Warning("SizeCursor::SizeCursor: size out of range.");
	ModifyCursor(1, 1);
    }
    else
	ModifyCursor(w, h);
}

void SizeCursor::ModifyCursor(const u8 w, const u8 h)
{
    if(sf && sc && sf->w() == w && sf->h() == h) return;

    if(sc) delete sc;
    if(sf) delete sf;
    
    sf = new Surface(w * TILEWIDTH, h * TILEWIDTH);
    
    sf->SetColorKey();
    Radar::DrawCursor(*sf);
    
    sc = new SpriteCursor(*sf);
}
