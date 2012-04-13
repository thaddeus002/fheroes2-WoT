/***************************************************************************
 *   Copyright (C) 2009 by Andrey Afletdinov <fheroes2@gmail.com>          *
 *                                                                         *
 *   Part of the Free Heroes2 Engine:                                      *
 *   http://sourceforge.net/projects/fheroes2                              *
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
#include "button.h"
#include "artifact.h"
#include "dialog.h"

u16 Dialog::ArtifactInfo(const std::string & hdr, const std::string & msg, const Artifact & art, const u16 buttons)
{
    const Sprite & border = AGG::GetICN(ICN::RESOURCE, 7);
    const Sprite & artifact = AGG::GetICN(ICN::ARTIFACT, art.IndexSprite64());
    Surface image(border.w(), border.h());
    border.Blit(image);
    artifact.Blit(5, 5, image);

    std::string ext = msg;
    ext.append("\n");
    ext.append(" ");
    ext.append("\n");
    ext.append(art.GetDescription());

    return Dialog::SpriteInfo(hdr, ext, image, buttons);
}

u16 Dialog::SpriteInfo(const std::string &header, const std::string &message, const Surface & sprite, u16 buttons)
{
    Display & display = Display::Get();
    const ICN::icn_t system = Settings::Get().ExtGameEvilInterface() ? ICN::SYSTEME : ICN::SYSTEM;

    // preload
    AGG::Cache::PreloadObject(system);

    // cursor
    Cursor & cursor = Cursor::Get();

    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);

    TextBox box1(header, Font::YELLOW_BIG, BOXAREA_WIDTH);
    TextBox box2(message, Font::BIG, BOXAREA_WIDTH);
    const u8 spacer = Settings::Get().QVGA() ? 5 : 10;

    Box box(box1.h() + spacer + box2.h() + spacer + sprite.h(), buttons);
    Rect pos = box.GetArea();

    if(header.size()) box1.Blit(pos);
    pos.y += box1.h() + spacer;

    if(message.size()) box2.Blit(pos);
    pos.y += box2.h() + spacer;

    // blit sprite
    pos.x = box.GetArea().x + (pos.w - sprite.w()) / 2;
    sprite.Blit(pos.x, pos.y, display);

    LocalEvent & le = LocalEvent::Get();

    ButtonGroups btnGroups(box.GetArea(), buttons);
    btnGroups.Draw();

    cursor.Show();
    display.Flip();

    // message loop
    u16 result = Dialog::ZERO;

    while(result == Dialog::ZERO && le.HandleEvents())
    {
        if(!buttons && !le.MousePressRight()) break;

        result = btnGroups.QueueEventProcessing();
    }

    cursor.Hide();

    return result;
}
