/***************************************************************************
 *   Copyright (C) 2008 by Andrey Afletdinov                               *
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

#include "gamedefs.h"
#include "agg.h"
#include "cursor.h"
#include "game.h"
#include "statusbar.h"
#include "dialog.h"
#include "monster.h"
#include "button.h"

void TestMonsterSprite(void)
{
    // cursor
    Cursor & cursor = Cursor::Get();
    cursor.Hide();
    cursor.SetThemes(Cursor::POINTER);

    Monster monster(Monster::PEASANT);
    Background back;
    Rect pos;

    Display & display = Display::Get();
    LocalEvent & le = LocalEvent::Get();

    std::string str;

    StatusBar speed_bar;
    StatusBar count_bar;
    StatusBar start_bar;
    StatusBar frame_bar;

    start_bar.SetCenter(100, display.h() - 15);
    count_bar.SetCenter(200, display.h() - 15);
    speed_bar.SetCenter(300, display.h() - 15);
    frame_bar.SetCenter(400, display.h() - 15);

    u32 ticket = 0;

    u8 start = 0;
    u8 count = AGG::GetICNCount(monster.ICNFile());
    u8 frame = 0;
    u8 speed = 10;

    str.clear();
    String::AddInt(str, frame);
    frame_bar.ShowMessage("frame: " + str);

    str.clear();
    String::AddInt(str, speed);
    speed_bar.ShowMessage("speed: " + str);

    str.clear();
    String::AddInt(str, start);
    start_bar.ShowMessage("start: " + str);

    str.clear();
    String::AddInt(str, count);
    count_bar.ShowMessage("count: " + str);

    cursor.Show();
    display.Flip();

    // mainmenu loop
    while(le.HandleEvents())
    {
	if(le.KeyPress(KEY_ESCAPE)) break;

	if(le.MouseClickLeft(pos))
	{
            u16 mons = monster();
            if(Dialog::SelectCount("Monster", Monster::PEASANT, Monster::WATER_ELEMENT, mons))
	    {
        	cursor.Hide();
		monster.Set(Monster::FromInt(mons));
		start = 0;
		count = AGG::GetICNCount(monster.ICNFile());
		frame = 0;
        	cursor.Show();
        	display.Flip();
	    }
	}

	if(le.MouseClickLeft(start_bar.GetRect()))
	{
	    u16 start2 = start;
	    if(Dialog::SelectCount("Start", 0, AGG::GetICNCount(monster.ICNFile()) - 1, start2))
	    {
        	cursor.Hide();
		start = start2;
		if(start + count > AGG::GetICNCount(monster.ICNFile())) count = AGG::GetICNCount(monster.ICNFile()) - start;
		str.clear();
		String::AddInt(str, start);
		start_bar.ShowMessage("start: " + str);
        	cursor.Show();
        	display.Flip();
    	    }
	}

	if(le.MouseClickLeft(count_bar.GetRect()))
	{
	    u16 count2 = count;
	    if(Dialog::SelectCount("Count", 1, AGG::GetICNCount(monster.ICNFile()), count2))
	    {
        	cursor.Hide();
		count = count2;
		frame = start;
		str.clear();
		String::AddInt(str, count);
		count_bar.ShowMessage("count: " + str);
        	cursor.Show();
        	display.Flip();
	    }
	}

	if(le.MouseClickLeft(speed_bar.GetRect()))
	{
	    u16 speed2 = speed;
	    if(Dialog::SelectCount("Speed", 1, 50, speed2))
	    {
        	cursor.Hide();
		speed = speed2;
		frame = start;
		str.clear();
		String::AddInt(str, speed);
		speed_bar.ShowMessage("speed: " + str);
        	cursor.Show();
        	display.Flip();
	    }
	}

        if(Game::ShouldAnimateInfrequent(ticket, speed))
        {
            cursor.Hide();
            const Sprite & sprite = AGG::GetICN(monster.ICNFile(), frame);
	    pos.x = 320 + sprite.x();
	    pos.y = 240 + sprite.y();
	    pos.w = sprite.w();
	    pos.h = sprite.h();
	    back.Restore();
	    back.Save(pos);
            display.Blit(sprite, pos);

	    str.clear();
	    String::AddInt(str, frame);
	    frame_bar.ShowMessage("frame: " + str);

            cursor.Show();
            display.Flip();

	    ++frame;
	    if(frame >= start + count) frame = start;
        }

        ++ticket;
    }
}
