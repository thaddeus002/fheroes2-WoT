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
    SelectFocusObject(const s16 px, const s16 py);
    virtual ~SelectFocusObject(){};

    bool isSelected(void) const{ return selected; };
    const std::vector<Rect> & GetCoords(void) const{ return coords; };
    u8 GetCursorIndex(void) const { return cursor_index; };
    u8 GetTopIndex(void) const{ return top_index; };
    u8 GetSelectIndex(void) const{ return cursor_index - top_index; };
    const Rect & GetMaxRect(void) const{ return max_area; };

    void Reset(void);
    void Select(u8 index);
    bool Prev(void);
    bool Next(void);
    bool SetTop(u8 index);

    virtual u8 GetSizeObject(void) const{ return 0; };
    virtual void Redraw(void) = 0;

protected:
    const Point pos_pt;
    const Surface & empty_back;
    const u8 step;
    Surface sprite_cursor;
    SpriteCursor cursor;
    bool selected;
    u8 top_index;
    u8 cursor_index;
    std::vector<Rect> coords;
    Rect max_area;
};

class SelectFocusCastles : public SelectFocusObject
{
public:
    static SelectFocusCastles & Get(void);

    void SetCastles(const std::vector<Castle *> & vec);

    u8 GetSizeObject(void) const{ return castles ? castles->size() : 0; };

    void Redraw(void);

    void Select(void);
    void Select(u8 index);

    const Point & GetCenter(u8 index) const;

protected:
    SelectFocusCastles();

private:
    const std::vector<Castle *> *castles;
};

class SelectFocusHeroes : public SelectFocusObject
{
public:
    static SelectFocusHeroes & Get(void);

    void SetHeroes(const std::vector<Heroes *> & vec);

    u8 GetSizeObject(void) const{ return heroes ? heroes->size() : 0; };

    void Redraw(void);

    void Select(void);
    void Select(u8 index);

    const Point & GetCenter(u8 index) const;

protected:
    SelectFocusHeroes();

private:
    const std::vector<Heroes *> *heroes;
    Surface sprite_blue;
};

};

#endif
