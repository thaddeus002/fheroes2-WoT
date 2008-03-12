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

#include "agg.h"
#include "rect.h"
#include "error.h"
#include "config.h"
#include "sprite.h"
#include "cursor.h"

/* constructor */
Cursor::Cursor() : theme(NONE), offset_x(0), offset_y(0)
{
}

Cursor & Cursor::Get(void)
{
    static Cursor cursor;

    return cursor;
}

/* get theme cursor */
Cursor::themes_t Cursor::Themes(void)
{
    return theme;
}

/* set cursor theme */
bool Cursor::SetThemes(const Cursor::themes_t name)
{
    if(theme != name)
    {
	theme = name;

	switch(0xF000 & name)
	{
	    case 0x3000:
		SetSprite(AGG::GetICN(ICN::SPELCO, 0xFF & name));
		if(H2Config::Debug()) Error::Verbose("Cursor::Set: SPELCO.ICN, ", 0xFF & name);
		break;
	    
	    case 0x2000:
		SetSprite(AGG::GetICN(ICN::CMSECO, 0xFF & name));
		if(H2Config::Debug()) Error::Verbose("Cursor::Set: CMSECO.ICN, ", 0xFF & name);
		break;
	    
	    case 0x1000:
		SetSprite(AGG::GetICN(ICN::ADVMCO, 0xFF & name));
		if(H2Config::Debug()) Error::Verbose("Cursor::Set: ADVMCO.ICN, ", 0xFF & name);
		break;

	    default:
		// default Cursor::POINTER
		SetSprite(AGG::GetICN(ICN::ADVMCO, 0));
		break;
	}

	SetOffset(name);
	
	return true;
    }

    return false;
}

/* redraw cursor wrapper for local event */
void Cursor::Redraw(u16 x, u16 y)
{
    Cursor & cur = Cursor::Get();
    
    if(cur.isVisible())
    {
	cur.Move(x, y);

	Display::Get().Flip();
    }
}

/* move cursor */
void Cursor::Move(u16 x, u16 y)
{
    if(isVisible()) SpriteCursor::Move(x + offset_x, y + offset_y);
}

/* set offset big cursor */
void Cursor::SetOffset(const themes_t name)
{
    switch(name)
    {
	    case Cursor::MOVE:
	    case Cursor::MOVE2:
	    case Cursor::MOVE3:
	    case Cursor::MOVE4:
		offset_x = -12;
		offset_y = -8;
		break;

	    case Cursor::ACTION:
	    case Cursor::ACTION2:
	    case Cursor::ACTION3:
	    case Cursor::ACTION4:
		offset_x = -14;
		offset_y = -10;
		break;

	    case Cursor::CASTLE:
		offset_x = -6;
		offset_y = -4;
		break;

	    default:
		offset_x = 0;
		offset_y = 0;
		break;
    }
}
