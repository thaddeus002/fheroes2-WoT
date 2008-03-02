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
#include "world.h"
#include "localevent.h"
#include "button.h"
#include "cursor.h"
#include "castle.h"
#include "kingdom.h"
#include "config.h"
#include "dialog.h"
#include "background.h"
#include "tools.h"
#include "text.h"
#include "payment.h"

Dialog::answer_t Dialog::BuyBoat(bool enable)
{
    Display & display = Display::Get();

    const ICN::icn_t system = H2Config::EvilInterface() ? ICN::SYSTEME : ICN::SYSTEM;

    Cursor & cursor = Cursor::Get();
    cursor.Hide();

    const std::string & string_header = "Build a new ship:";
    const std::string & string_cost = "Resource cost:";


    Dialog::Box box(100, true);

    const Rect & box_rt = box.GetArea();

    Point dst_pt;

    dst_pt.x = box_rt.x + (box_rt.w - Text::width(string_header, Font::BIG)) / 2;
    dst_pt.y = box_rt.y;
    Text(string_header, Font::BIG, dst_pt);

    const Sprite & sprite = AGG::GetICN(ICN::BOATWIND, 0);
    dst_pt.x = box_rt.x + (box_rt.w - sprite.w()) / 2;
    dst_pt.y = box_rt.y + 25;
    display.Blit(sprite, dst_pt);

    dst_pt.x = box_rt.x + (box_rt.w - Text::width(string_cost, Font::BIG)) / 2;
    dst_pt.y = box_rt.y + 35 + sprite.h();
    Text(string_cost, Font::BIG, dst_pt);

    const Rect src_rt(box_rt.x, box_rt.y + 40 + sprite.h(), box_rt.w, box_rt.h);

    const PaymentConditions::BuyBuilding payment(world.GetMyKingdom().GetRace(), Castle::BUILD_BOAT);
    
    PaymentConditions::AlignDraw(payment, src_rt);

    // buttons
    dst_pt.x = box_rt.x;
    dst_pt.y = box_rt.y + box_rt.h + BUTTON_HEIGHT - AGG::GetICN(system, 1).h();
    Button button1(dst_pt, system, 1, 2);

    dst_pt.x = box_rt.x + box_rt.w - AGG::GetICN(system, 3).w();
    dst_pt.y = box_rt.y + box_rt.h + BUTTON_HEIGHT - AGG::GetICN(system, 3).h();
    Button button2(dst_pt, system, 3, 4);

    if(!enable)
    {
	button1.Press();
	button1.SetDisable(true);
    }

    button1.Draw();
    button2.Draw();
	
    cursor.Show();
    display.Flip();

    LocalEvent & le = LocalEvent::GetLocalEvent();

    // message loop
    while(le.HandleEvents())
    {
        if(button1.isEnable()) le.MousePressLeft(button1) ? button1.PressDraw() : button1.ReleaseDraw();
        le.MousePressLeft(button2) ? button2.PressDraw() : button2.ReleaseDraw();

        if(le.KeyPress(KEY_RETURN) ||
    	    (button1.isEnable() && le.MouseClickLeft(button1))) return Dialog::OK;

        if(le.KeyPress(KEY_ESCAPE) ||
    	    le.MouseClickLeft(button2)) return Dialog::CANCEL;
    }

    return Dialog::ZERO;
}
