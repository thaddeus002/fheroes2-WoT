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
#include "error.h"
#include "cursor.h"

namespace Cursor {
    static themes_t      name = NONE;
    static Surface       sprite;
    static Background    background;
    static Rect          position;
    static bool          show = true;
    static bool		 save = false;
    static s16		 offset_x = 0;
    static s16		 offset_y = 0;
};

/* init cursor */
void Cursor::Init(Cursor::themes_t cursor)
{
    // preload
    AGG::PreloadObject("ADVMCO.ICN");

    SDL_ShowCursor(SDL_DISABLE);
    Cursor::show = true;

    Cursor::Set(cursor);

    int x, y;
    SDL_GetMouseState(&x, &y);
    Cursor::position.x = x;
    Cursor::position.y = y;
    Cursor::position.w = Cursor::sprite.w() + 1;
    Cursor::position.h = Cursor::sprite.h() + 1;

    Cursor::Redraw();

    // quit
    atexit(Cursor::Quit);
}

/* quit cursor */
void Cursor::Quit(){ SDL_ShowCursor(SDL_ENABLE); }

/* show cursor */
void Cursor::Show(void)
{
    if(show) return;

    show = true;
    
    Cursor::Redraw(true);
}

/* hide cursor */
void Cursor::Hide(void)
{ 
    if(!show) return;

    show = false;
    
    Cursor::Redraw(true);
}

/* get theme cursor */
Cursor::themes_t Cursor::Get(void){ return name; }

/* set cursor theme */
void Cursor::Set(Cursor::themes_t cursor)
{
    if(Cursor::name != cursor){
    
	Cursor::name = cursor;

	switch(0xF000 & cursor){

	    case 0x3000:
		Cursor::sprite = AGG::GetICN("SPELCO.ICN", 0xFF & cursor);
		Error::Verbose("Cursor::Set: SPELCO.ICN, ", 0xFF & cursor);
		break;
	    
	    case 0x2000:
		Cursor::sprite = AGG::GetICN("CMSECO.ICN", 0xFF & cursor);
		Error::Verbose("Cursor::Set: CMSECO.ICN, ", 0xFF & cursor);
		break;
	    
	    case 0x1000:
		Cursor::sprite = AGG::GetICN("ADVMCO.ICN", 0xFF & cursor);
		Error::Verbose("Cursor::Set: ADVMCO.ICN, ", 0xFF & cursor);
		break;

	    default:
		// default Cursor::POINTER
		Cursor::sprite = AGG::GetICN("ADVMCO.ICN", 0);
		break;
	}
	
	Cursor::SetOffset(cursor);
    }
}

/* redraw cursor */
void Cursor::Redraw(bool flag)
{
    int x, y;
    SDL_GetMouseState(&x, &y);

    Cursor::Redraw(x, y, flag);
}

/* redraw cursor x y */
void Cursor::Redraw(u16 x, u16 y, bool flag)
{
    if(!flag && Cursor::position.x == x && Cursor::position.y == y) return;

    if(save){ Cursor::background.Restore(); save = false; }

    Cursor::position.x = x + offset_x;
    Cursor::position.y = y + offset_y;
    Cursor::position.w = (Cursor::position.x + Cursor::position.w > display.w() ? display.w() - Cursor::position.x : Cursor::sprite.w());
    Cursor::position.h = (Cursor::position.y + Cursor::position.h > display.h() ? display.h() - Cursor::position.y : Cursor::sprite.h());

    // draw cursor
    if(Cursor::show){

	Cursor::background.Save(Cursor::position);

	display.Blit(Cursor::sprite, Cursor::position);
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
	switch(cursor){

	    case Cursor::MOVE:
	    case Cursor::MOVE2:
	    case Cursor::MOVE3:
	    case Cursor::MOVE4:
		Cursor::offset_x = -12;
		Cursor::offset_y = -8;
		break;

	    case Cursor::ACTION:
	    case Cursor::ACTION2:
	    case Cursor::ACTION3:
	    case Cursor::ACTION4:
		Cursor::offset_x = -14;
		Cursor::offset_y = -10;
		break;

	    case Cursor::CASTLE:
		Cursor::offset_x = -6;
		Cursor::offset_y = -4;
		break;

	    default:
		Cursor::offset_x = 0;
		Cursor::offset_y = 0;
		break;
	}
}

/* get rect */
const Rect & Cursor::GetRect(void)
{ return Cursor::position; }
