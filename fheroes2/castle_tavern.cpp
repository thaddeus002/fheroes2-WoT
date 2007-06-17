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
#include "agg.h"
#include "animation.h"
#include "localevent.h"
#include "button.h"
#include "world.h"
#include "cursor.h"
#include "config.h"
#include "resource.h"
#include "castle.h"
#include "heroes.h"
#include "kingdom.h"
#include "background.h"
#include "tools.h"
#include "text.h"


void Castle::OpenTavern(void)
{
    const std::string & header = "A generous tip for the barkeep yields the following rumor:";
    const std::string & system = (H2Config::EvilInterface() ? "SYSTEME.ICN" : "SYSTEM.ICN");
    const std::string & tavern = GetStringBuilding(BUILD_TAVERN);
    const std::string & message = world.GetRumors();

    Cursor::Hide();

    Dialog::Box box(Text::height(BOXAREA_WIDTH, header, Font::BIG) + 130 + Text::height(BOXAREA_WIDTH, message, Font::BIG), true);

    const Rect & pos = box.GetArea();
    Point dst_pt(pos);

    Text(pos.x + (pos.w - Text::width(tavern, Font::BIG)) / 2, dst_pt.y, tavern, Font::BIG, true);

    const Sprite & s1 = AGG::GetICN("TAVWIN.ICN", 0);
    dst_pt.x = pos.x + (pos.w - s1.w()) / 2;
    dst_pt.y = pos.y + 30;
    display.Blit(s1, dst_pt);

    const Sprite & s2 = AGG::GetICN("TAVWIN.ICN", 1);
    dst_pt.x += 3;
    dst_pt.y += 3;
    display.Blit(s2, dst_pt);

    Rect rt(pos.x, dst_pt.y + s1.h() + 10, pos.w, pos.h);
    TextBox(rt, header, Font::BIG, true);

    rt.y += 60;
    TextBox(rt, message, Font::BIG, true);

    Animation animeBeer(dst_pt, "TAVWIN.ICN", 2, 20, false, Animation::INFINITY | Animation::RING | Animation::LOW);

    const Sprite & s3 = animeBeer.GetFirstSprite();
    dst_pt.x += s3.x();
    dst_pt.y += s3.y();
    display.Blit(s3, dst_pt);

    // button yes
    const Sprite & s4 = AGG::GetICN(system, 5);
    dst_pt.x = pos.x + (pos.w - s4.w()) / 2;
    dst_pt.y = pos.y + pos.h + BUTTON_HEIGHT - s4.h();
    Button buttonYes(dst_pt, system, 5, 6);

    display.Flip();
    Cursor::Show();

    LocalEvent & le = LocalEvent::GetLocalEvent();
   
    le.ResetKey();

    // message loop
    bool exit = false;

    while(!exit)
    {
        le.HandleEvents();

        le.MousePressLeft(buttonYes) ? buttonYes.Press() : buttonYes.Release();

        if(le.MouseClickLeft(buttonYes) || le.KeyPress(SDLK_RETURN) || le.KeyPress(SDLK_ESCAPE)){ exit = true; }

	animeBeer.DrawSprite();
    }

    le.ResetKey();

    Cursor::Show();
}
