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

#ifndef H2SELECTFOCUS_H
#define H2SELECTFOCUS_H

#include <vector>
#include "gamedefs.h"
#include "surface.h"
#include "spritecursor.h"
#include "game.h"
#include "rect.h"

class Castle;
class Heroes;

namespace Game
{

class SelectFocusObject
{
public:
    SelectFocusObject();
    virtual ~SelectFocusObject(){};

    bool isActive(void) const{ return active; };

    void HideCursor(void);
    void ShowCursor(void);
    void MoveCursor(u8 index_icon);

    const Rect & GetMaxRect(void) const{ return max; };
    u8 GetFirstIndex(void) const {return first_icons_index; };
    u8 GetCursorIndex(void) const {return cursor_index; };

    virtual bool Prev(void) = 0;
    virtual bool Next(void) = 0;
    virtual void Redraw(void) = 0;
    const std::vector<Rect> & GetCoords(void) const{ return coords; };

protected:
    bool active;
    std::vector<Rect> coords;
    u8 cursor_index;
    u8 first_icons_index;
    Surface spriteCursor;
    SpriteCursor cursor;
    Rect max;
};

class SelectFocusCastles : public SelectFocusObject
{
public:
    SelectFocusCastles(const std::vector<Castle *> & vec);
    
    bool Valid(u8 index_icon){ return first_icons_index + index_icon < castles.size(); };
    bool Prev(void);
    bool Next(void);
    void Redraw(void);
    
    const Point & GetCenter(u8 index_icon);
    void SetCenter(const Point & pt);

private:
    const std::vector<Castle *> & castles;
};

class SelectFocusHeroes : public SelectFocusObject
{
public:
    SelectFocusHeroes(const std::vector<Heroes *> & vec);

    bool Valid(u8 index_icon){ return first_icons_index + index_icon < heroes.size(); };
    bool Prev(void);
    bool Next(void);
    void Redraw(void);

    const Point & GetCenter(u8 index_icon);
    void SetCenter(const Point & pt);

private:
    const std::vector<Heroes *> & heroes;
};

};

#endif
