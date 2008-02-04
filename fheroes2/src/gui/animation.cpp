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
#include "cursor.h"
#include "config.h"
#include "localevent.h"
#include "sprite.h"
#include "display.h"
#include "animation.h"

#define ANIMATION_HIGH		8
#define ANIMATION_MEDIUM	25
#define ANIMATION_LOW		50

Animation::Animation(const Point &dp, const std::string &icn, u16 index, u8 count, bool first, u8 amode)
    : dst_pt(dp), use_first(first), mode(amode), frame(0), max_rect(Display::Get().w(), Display::Get().h(), 0, 0), disable(false), reset(false)
{
    std::vector<Rect> vec_rect;
    
    for(int ii = index; ii < index + count; ++ii)
    {
        const Sprite &sprite = AGG::GetICN(icn, ii);

        sprites.push_back(&sprite);

	vec_rect.push_back(Rect(sprite.x(), sprite.y(), sprite.w(), sprite.h()));
    }
    
    max_rect = Rect(vec_rect);
    max_rect.x += dst_pt.x;
    max_rect.y += dst_pt.y;
}

bool Animation::DrawSprite(u32 ticket, bool forceredraw)
{
    if(disable) return false;

    if(false == forceredraw)
    {
	// HIGH sleep
	if(mode & HIGH && (0 != (ticket % ANIMATION_HIGH)) && !reset) return false;
	// MEDIUM sleep
	if(mode & MEDIUM && (0 != (ticket % ANIMATION_MEDIUM)) && !reset) return false;
	// LOW sleep
	if(mode & LOW && (0 != (ticket % ANIMATION_LOW)) && !reset) return false;
    }

    // hide cursor
    bool localcursor = false;
    Cursor & cursor = Cursor::Get();
    if(max_rect & cursor.GetRect() && cursor.isVisible()){ cursor.Hide(); localcursor = true; }

    Display & display = Display::Get();

    // restore or save background
    background.valid() ? background.Restore() : background.Save(max_rect);

    // blit frame sprites
    if(use_first)
    {
	const Sprite & first_sprite = *sprites[0];
	const Sprite & sprite = *sprites[1 + frame % (sprites.size() - 1)];

	display.Blit(first_sprite, dst_pt.x + first_sprite.x(), dst_pt.y + first_sprite.y());
	display.Blit(sprite, dst_pt.x + sprite.x(), dst_pt.y + sprite.y());
    }
    else
    {
	const Sprite & sprite = *sprites[frame % sprites.size()];

	display.Blit(sprite, dst_pt.x + sprite.x(), dst_pt.y + sprite.y());
    }

    if(localcursor) cursor.Show();

    if(!reset) frame++;

    if(reset) reset = false;

    if(!(mode & INFINITY) && frame == sprites.size()) disable = true;

    //if(!(mode & RING))
    
    return true;
}

bool Animation::Reset(void)
{
    if(false == disable && 0 == frame) return false;

    frame = 0;
    disable = false;
    reset = true;
    
    return true;
}
