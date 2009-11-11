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
#include "settings.h"
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
	if(isVisible()) Hide();
	theme = name;

	switch(0xF000 & name)
	{
	    case 0x3000:
		SetSprite(AGG::GetICN(ICN::SPELCO, 0xFF & name));
		DEBUG(DBG_ENGINE , DBG_TRACE, "Cursor::Set: SPELCO.ICN, " << static_cast<int>(0xFF & name));
		break;
	    
	    case 0x2000:
		SetSprite(AGG::GetICN(ICN::CMSECO, 0xFF & name));
		DEBUG(DBG_ENGINE , DBG_TRACE, "Cursor::Set: CMSECO.ICN, " << static_cast<int>(0xFF & name));
		break;
	    
	    case 0x1000:
		SetSprite(AGG::GetICN(ICN::ADVMCO, 0xFF & name));
		DEBUG(DBG_ENGINE , DBG_TRACE, "Cursor::Set: ADVMCO.ICN, " << static_cast<int>(0xFF & name));
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

	    case Cursor::BOAT:
	    case Cursor::BOAT2:
	    case Cursor::BOAT3:
	    case Cursor::BOAT4:
	    case Cursor::REDBOAT:
	    case Cursor::REDBOAT2:
	    case Cursor::REDBOAT3:
	    case Cursor::REDBOAT4:
		offset_x = -12;
		offset_y = -12;
		break;

	    case Cursor::CASTLE:
		offset_x = -6;
		offset_y = -4;
		break;

	    case Cursor::SCROLL_TOPRIGHT:
            case Cursor::SCROLL_RIGHT:
		offset_x = -15;
		offset_y = 0;
		break;

	    case Cursor::SCROLL_BOTTOM:
            case Cursor::SCROLL_BOTTOMLEFT:
		offset_x = 0;
		offset_y = -15;
		break;
            
            case Cursor::SCROLL_BOTTOMRIGHT:
                offset_x = -15;
                offset_y = -15;
                break;
            
            case Cursor::WAR_NONE:
            case Cursor::WAR_MOVE:
            case Cursor::WAR_HELMET:
            case Cursor::WAR_FLIGHT:
            case Cursor::WAR_ARROW:
            case Cursor::WAR_INFO:
            case Cursor::SWORD_TOPLEFT:
            case Cursor::SWORD_LEFT:
            case Cursor::SWORD_BOTTOMLEFT:
            case Cursor::WAR_BROKENARROW:
            case Cursor::SWORD_TOPRIGHT:
                offset_x = -7;
                offset_y = -7;
                break;

	    default:
		offset_x = 0;
		offset_y = 0;
		break;
    }
}

/* draw simple cursor */
void Cursor::DrawCursor(Surface &surface, const u8 indexcolor, bool solid)
{
    if(! surface.valid()) return;

    surface.SetColorKey();

    u16 width  = surface.w();
    u16 height = surface.h();

    // draw cursor
    u32 color = surface.GetColor(indexcolor);
    surface.Lock();
    if(solid)
    {
	for(u8 i = 0; i < width; ++i)
        {
    	    surface.SetPixel(i, 0, color);
            surface.SetPixel(i, height - 1, color);
        }

        for(u8 i = 0; i < height; ++i)
        {
            surface.SetPixel(0, i, color);
    	    surface.SetPixel(width - 1, i, color);
        }
    }
    else
    {
	for(u8 i = 0; i < width; ++i)
	{
    	    surface.SetPixel(i, 0, color);
    	    if(i + 1 < width) surface.SetPixel(i + 1, 0, color);
    	    i += 3;
	}
	for(u8 i = 0; i < width; ++i)
	{
    	    surface.SetPixel(i, height - 1, color);
    	    if(i + 1 < width) surface.SetPixel(i + 1, height - 1, color);
    	    i += 3;
	}
	for(u8 i = 0; i < height; ++i)
	{
    	    surface.SetPixel(0, i, color);
    	    if(i + 1 < height) surface.SetPixel(0, i + 1, color);
    	    i += 3;
	}
	for(u8 i = 0; i < height; ++i)
	{
    	    surface.SetPixel(width - 1, i, color);
    	    if(i + 1 < height) surface.SetPixel(width - 1, i + 1, color);
    	    i += 3;
	}
    }
    surface.Unlock();
}
