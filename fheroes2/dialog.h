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
#ifndef H2DIALOG_H
#define H2DIALOG_H

#include <string>
#include "gamedefs.h"
#include "background.h"
#include "monster.h"
#include "rect.h"
#include "game.h"

#define	SHADOWWIDTH	16

#define BOX_WIDTH       306
#define BOXAREA_WIDTH   234
#define BUTTON_HEIGHT   50

class Castle;
class Heroes;
class Surface;

namespace Dialog
{
    typedef enum {
	ZERO	= 0x0000,
	YES     = 0x0001,
        OK      = 0x0002,
        NO      = 0x0004,
        CANCEL  = 0x0008,
        DISMISS = 0x0010,
        UPGRADE = 0x0020,
        MAX     = 0x0040,
	PREV	= 0x0080,
	NEXT	= 0x0100
    } answer_t;

    Dialog::answer_t AdventureOptions(void);
    Game::menu_t FileOptions(void);
    Dialog::answer_t SystemOptions(void);

    // show info cell maps
    void QuickInfo(const std::string & object);
    void QuickInfo(const Castle & castle);
    void QuickInfo(const Heroes & heroes);
    
    // buttons: OK : CANCEL : OK|CANCEL : YES|NO
    u16 Message(const std::string &header, const std::string &message, Font::type_t ft, u16 buttons = 0);

    // redistribute count
    u16 SelectCount(u16 max);

    // recrut monster
    u16 RecrutMonster(const Monster::monster_t & monster, u16 available);
    void DwellingInfo(const Monster::monster_t & monster, u16 available);

class Box
{
public:
    Box(u16 height, bool buttons = false);
    ~Box();

    const Rect & GetArea(void){ return area; };

private:
    Background back;
    Rect area;
};

class FrameBorder : private Background
{
public:
    FrameBorder();
    ~FrameBorder();

    const Rect & GetArea(void) const{ return area; };

private:
    Rect area;
};

class StatusBar
{
public:
    StatusBar(const Point & pt, const Surface & sf, const Font::type_t ft) : pos_pt(pt), sprite(sf), font(ft) {};

    void ShowMessage(const std::string & message);
    void Clear(void);
    void Clear(const std::string & message);
    bool isEmpty(void);

private:
    const Point pos_pt;
    const Surface & sprite;
    const Font::type_t font;
    std::string status;
};

};

#endif
