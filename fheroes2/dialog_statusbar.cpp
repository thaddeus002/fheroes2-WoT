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

#include <string>
#include "text.h"
#include "cursor.h"
#include "dialog.h"

void Dialog::StatusBar::ShowMessage(const std::string & message)
{
    if(message == status) return;

    if(! status.empty()) Clear();

    Cursor::Hide();

    Text(pos_pt.x + (sprite.w() - Text::width(message, font)) / 2,
         pos_pt.y + (sprite.h() - Text::height(sprite.w(), message, font)) / 2, message, font, true);

    status = message;

    Cursor::Show();
}

void Dialog::StatusBar::Clear(void)
{
    bool localcursor = Cursor::Visible();

    if(localcursor) Cursor::Hide();

    display.Blit(sprite, pos_pt);

    status.clear();

    if(localcursor) Cursor::Show();
}

void Dialog::StatusBar::Clear(const std::string & message)
{
    if(message == status) return;

    bool localcursor = Cursor::Visible();

    if(localcursor) Cursor::Hide();

    display.Blit(sprite, pos_pt);

    status = message;

    Text(pos_pt.x + (sprite.w() - Text::width(message, font)) / 2,
         pos_pt.y + (sprite.h() - Text::height(sprite.w(), message, font)) / 2, message, font, true);

    if(localcursor) Cursor::Show();
}

bool Dialog::StatusBar::isEmpty(void)
{
    return status.empty();
}
