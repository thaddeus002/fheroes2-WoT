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
#include "localevent.h"
#include "sprite.h"
#include "display.h"
#include "animation.h"

#define ANIMATION_HIGH		3
#define ANIMATION_MEDIUM	9
#define ANIMATION_LOW		27

Animation::Animation(const std::string &icn, u16 index, u8 count, u8 amode) 
    : area(display.w(), display.h(), 0, 0), disable(false), reset(false), frame(0), ticket(0), mode(amode), sprites()
{
    for(int ii = index; ii < index + count; ++ii){
        const Sprite &sprite = AGG::GetICN(icn, ii);
        if(sprite.x() < area.x) area.x = sprite.x();
        if(sprite.y() < area.y) area.y = sprite.y();
        if(sprite.w() > area.w) area.w = sprite.w();
        if(sprite.h() > area.h) area.h = sprite.h();
        sprites.push_back(&sprite);
    }
}

void Animation::DrawSprite(void)
{
    if(disable) return;

    ticket++;

    // HIGH sleep
    if(mode & HIGH && (0 != (ticket % ANIMATION_HIGH))) return;
    // MEDIUM sleep
    if(mode & MEDIUM && (0 != (ticket % ANIMATION_MEDIUM))) return;
    // LOW sleep
    if(mode & LOW && (0 != (ticket % ANIMATION_LOW))) return;

    // hide cursor
    bool localcursor = false;
    if(area & LocalEvent::MouseCursor() && Cursor::Visible()){ Cursor::Hide(); localcursor = true; }

    const Sprite & sprite = *sprites[frame % sprites.size()];

    display.Blit(sprite, sprite.x(), sprite.y());
    display.Flip();

    if(localcursor) Cursor::Show();

    frame++;

    reset = false;

    if(!(mode & INFINITY) && frame == sprites.size()) disable = true;
    //if(!(mode & RING))
}

void Animation::Reset(void)
{
    if(!disable || reset) return;

    bool localcursor = false;
    if(area & LocalEvent::MouseCursor() && Cursor::Visible()){ Cursor::Hide(); localcursor = true; }

    const Sprite & sprite = *sprites[0];
    display.Blit(sprite, sprite.x(), sprite.y());

    ticket = 0;
    frame = 0;
    reset = true;
    disable = false;

    localcursor ? Cursor::Show() : display.Flip();
}
