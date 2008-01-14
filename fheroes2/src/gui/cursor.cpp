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
#include "background.h"
#include "localevent.h"
#include "error.h"
#include "display.h"
#include "config.h"
#include "cursor.h"

namespace Cursor
{
    static themes_t      name = NONE;
    static const Sprite* sprite = NULL;
    static Background    background;
    static Rect          position;
    static bool          show = true;
    static bool		 save = false;
    static s16		 offset_x = 0;
    static s16		 offset_y = 0;

    void SetOffset(themes_t cursor);
};

/* init cursor */
void Cursor::Init(Cursor::themes_t cursor)
{
    // preload
    AGG::PreloadObject("ADVMCO.ICN");

    show = true;

    Set(cursor);

    const Point & mouse = LocalEvent::MouseCursor();

    position.x = mouse.x;
    position.y = mouse.y;

    if(sprite)
    {
	position.w = sprite->w() + 1;
	position.h = sprite->h() + 1;
    }

    Redraw(mouse.x, mouse.y);
}

/* show cursor */
void Cursor::Show(void)
{
    if(show) return;

    show = true;
    
    const Point & mouse = LocalEvent::MouseCursor();

    Redraw(mouse.x, mouse.y, true);
}

/* hide cursor */
void Cursor::Hide(void)
{ 
    if(!show) return;

    show = false;
    
    const Point & mouse = LocalEvent::MouseCursor();

    Redraw(mouse.x, mouse.y, true);
}

/* get theme cursor */
Cursor::themes_t Cursor::Get(void)
{
    return name;
}

/* set cursor theme */
bool Cursor::Set(Cursor::themes_t cursor)
{
    if(name != cursor)
    {
	name = cursor;

	switch(0xF000 & cursor)
	{
	    case 0x3000:
		sprite = &AGG::GetICN("SPELCO.ICN", 0xFF & cursor);
		if(H2Config::Debug()) Error::Verbose("Cursor::Set: SPELCO.ICN, ", 0xFF & cursor);
		break;
	    
	    case 0x2000:
		sprite = &AGG::GetICN("CMSECO.ICN", 0xFF & cursor);
		if(H2Config::Debug()) Error::Verbose("Cursor::Set: CMSECO.ICN, ", 0xFF & cursor);
		break;
	    
	    case 0x1000:
		sprite = &AGG::GetICN("ADVMCO.ICN", 0xFF & cursor);
		if(H2Config::Debug()) Error::Verbose("Cursor::Set: ADVMCO.ICN, ", 0xFF & cursor);
		break;

	    default:
		// default Cursor::POINTER
		sprite = &AGG::GetICN("ADVMCO.ICN", 0);
		break;
	}

	SetOffset(cursor);
	
	return true;
    }

    return false;
}

/* redraw cursor wrapper for local event */
void Cursor::Redraw(u16 x, u16 y)
{
    if(show) Redraw(x, y, false);
}

/* redraw cursor x y */
void Cursor::Redraw(u16 x, u16 y, bool flag)
{
    if(!flag && position.x == x && position.y == y) return;

    if(save)
    {
	background.Restore();

	save = false;
    }

    position.x = x + offset_x;
    position.y = y + offset_y;

    if(sprite)
    {
	position.w = (position.x + position.w > display.w() ? display.w() - position.x : sprite->w());
	position.h = (position.y + position.h > display.h() ? display.h() - position.y : sprite->h());
    }

    // draw cursor
    if(show && sprite)
    {
	background.Save(position);

	display.Blit(*sprite, position.x, position.y);

	save = true;
    }
}

bool Cursor::Visible(void)
{
    return show;
}

/* set offset big cursor */
void Cursor::SetOffset(Cursor::themes_t cursor)
{
	switch(cursor)
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

/* get rect */
const Rect & Cursor::GetRect(void)
{
    return position;
}
