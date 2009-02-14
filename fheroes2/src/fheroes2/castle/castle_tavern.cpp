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
#include "button.h"
#include "world.h"
#include "cursor.h"
#include "settings.h"
#include "resource.h"
#include "castle.h"
#include "heroes.h"
#include "kingdom.h"
#include "text.h"

void Castle::OpenTavern(void)
{
    const std::string & header = _("A generous tip for the barkeep yields the following rumor:");
    const ICN::icn_t system = (Settings::Get().EvilInterface() ? ICN::SYSTEME : ICN::SYSTEM);
    const ICN::icn_t tavwin = ICN::TAVWIN;
    const std::string & tavern = GetStringBuilding(BUILD_TAVERN);
    const std::string & message = world.GetRumors();

    Display & display = Display::Get();
    Cursor & cursor = Cursor::Get();

    cursor.Hide();

    TextBox box1(header, Font::BIG, BOXAREA_WIDTH);
    TextBox box2(message, Font::BIG, BOXAREA_WIDTH);

    Dialog::Box box(box1.h() + 130 + box2.h(), true);

    const Rect & pos = box.GetArea();
    Point dst_pt(pos.x, pos.y);

    Text text(tavern, Font::BIG);
    text.Blit(pos.x + (pos.w - text.w()) / 2, dst_pt.y);

    const Sprite & s1 = AGG::GetICN(tavwin, 0);
    dst_pt.x = pos.x + (pos.w - s1.w()) / 2;
    dst_pt.y = pos.y + 30;
    display.Blit(s1, dst_pt);

    dst_pt.x += 3;
    dst_pt.y += 3;

    const Sprite & s20 = AGG::GetICN(tavwin, 1);
    display.Blit(s20, dst_pt);

    if(const u16 index = ICN::AnimationFrame(tavwin, 0))
    {
	const Sprite & s21 = AGG::GetICN(tavwin, index);
	display.Blit(s21, dst_pt.x + s21.x(), dst_pt.y + s21.y());
    }

    Rect rt(pos.x, dst_pt.y + s1.h() + 10, pos.w, pos.h);
    box1.Blit(rt);

    rt.y += 60;
    box2.Blit(rt);

    // button yes
    const Sprite & s4 = AGG::GetICN(system, 5);
    Button buttonYes(pos.x + (pos.w - s4.w()) / 2, pos.y + pos.h + BUTTON_HEIGHT - s4.h(), system, 5, 6);

    buttonYes.Draw();

    cursor.Show();
    display.Flip();

    LocalEvent & le = LocalEvent::GetLocalEvent();

    u32 speed = 0;
    u32 ticket = 0;

    // message loop
    while(le.HandleEvents())
    {
        le.MousePressLeft(buttonYes) ? buttonYes.PressDraw() : buttonYes.ReleaseDraw();

        if(le.MouseClickLeft(buttonYes) || le.KeyPress(KEY_RETURN) || le.KeyPress(KEY_ESCAPE)) break;


        // animation
	if(Game::ShouldAnimateInfrequent(speed, 8))
	{
	    cursor.Hide();

	    display.Blit(s20, dst_pt);

	    if(const u16 index = ICN::AnimationFrame(tavwin, 0, ticket++))
	    {
		const Sprite & s22 = AGG::GetICN(tavwin, index);
		display.Blit(s22, dst_pt.x + s22.x(), dst_pt.y + s22.y());
	    }

	    cursor.Show();
	    display.Flip();
	}

	++speed;
    }
}
