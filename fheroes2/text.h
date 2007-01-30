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

#ifndef H2TEXT_H
#define H2TEXT_H

#include <vector>
#include "gamedefs.h"
#include "rect.h"
#include "sprite.h"

namespace Font
{
    typedef enum { SMALL, BIG } type_t;
};

class Text : public Rect
{
public:

    typedef enum { LEFT, CENTER, RIGHT } align_t;

    Text(const Point &pt, const std::string &message, Font::type_t ft = Font::SMALL);
    Text(const Rect &rt, const std::string &message, Font::type_t ft = Font::SMALL);
    
    void Redraw(void);

private:

    typedef struct {
	char letter;
	const Sprite *sprite;
    } letter_t;

    Font::type_t font;

    std::vector<letter_t> sprites;
};

#endif
