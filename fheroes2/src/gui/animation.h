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
#ifndef H2ANIMATION_H
#define H2ANIMATION_H

#include <vector>
#include "sprite.h"
#include "rect.h"
#include "icn.h"
#include "background.h"
#include "gamedefs.h"

class Animation
{
public:
    typedef enum { INFINITY = 0x01, RING = 0x02, LOW = 0x04, MEDIUM = 0x08, HIGH = 0x10 } animation_t;

    Animation(const Point &dp, const ICN::icn_t icn, u16 index, u8 count, bool first = false, u8 amode = INFINITY | RING | MEDIUM);

    const Rect & GetMaxRect(void) const{ return max_rect; };
    const Sprite & GetFirstSprite(void) const{ return *sprites[0]; };

    bool Reset(void);

    bool DrawSprite(u32 ticket, bool forceredraw = false);

protected:
    const Point dst_pt;
    bool use_first;
    const u8 mode;

    u32 frame;

    std::vector<const Sprite *> sprites;
    Rect max_rect;

    bool disable;
    bool reset;
    Background background;
};

#endif
