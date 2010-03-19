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

#include <algorithm>
#include "agg.h"
#include "settings.h"
#include "cursor.h"
#include "button.h"
#include "dialog.h"

class State
{
public:
    State(const Point & p, u32 f);

    bool operator() (void) const { return state; };

    void Redraw(void);
    bool QueueEventProcessing(void);

private:
    Rect area;
    Text msg;
    const u32 state;
};

State::State(const Point & p, u32 f) : area(p, 0, 0), state(f)
{
    const Settings & conf = Settings::Get();

    msg.Set(conf.ExtName(f), Font::SMALL);

    const Sprite & cell = AGG::GetICN(ICN::CELLWIN, 1);

    area.w = cell.w() + 5 + msg.w();
    area.h = cell.h();
}

void State::Redraw(void)
{
    Display & display = Display::Get();
    const Settings & conf = Settings::Get();

    const Sprite & cell = AGG::GetICN(ICN::CELLWIN, 1);
    const Sprite & mark = AGG::GetICN(ICN::CELLWIN, 2);

    display.Blit(cell, area);
    if(conf.ExtModes(state)) display.Blit(mark, area.x + 3, area.y + 2);

    msg.Blit(area.x + cell.w() + 5, area.y + 4);
}

bool State::QueueEventProcessing(void)
{
    LocalEvent & le = LocalEvent::Get();
    Cursor & cursor = Cursor::Get();
    Settings & conf = Settings::Get();
    
    if(le.MouseClickLeft(area))
    {
        conf.ExtModes(state) ? conf.ExtResetModes(state) : conf.ExtSetModes(state);
        cursor.Hide();
        Redraw();
	return true;
    }
    return false;
}

void Dialog::ExtSettings(void)
{
    Display & display = Display::Get();

    // cursor
    Cursor & cursor = Cursor::Get();
    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);

    const u16 window_w = 320;
    const u16 window_h = 224;

    Dialog::FrameBorder frameborder;
    frameborder.SetPosition((display.w() - window_w) / 2 - BORDERWIDTH, (display.h() - window_h) / 2 - BORDERWIDTH, window_w, window_h);
    frameborder.Redraw();

    Point pt;
    const Rect & area = frameborder.GetArea();
    const Sprite & background = AGG::GetICN(ICN::STONEBAK, 0);
    display.Blit(background, Rect(0, 0, window_w, window_h), area);

    Text text("FHeroes2 Settings", Font::YELLOW_BIG);
    text.Blit(area.x + (area.w - text.w()) / 2, area.y + 6);

    std::vector<State *> states;

    pt.x = area.x + 10;
    pt.y = area.y + 30;
    states.push_back(new State(pt, Settings::ALLOW_BUY_FROM_WELL));

    pt.x = area.x + 10;
    pt.y = area.y + 60;
    states.push_back(new State(pt, Settings::SHOW_VISITED_CONTENT));

    pt.x = area.x + 10;
    pt.y = area.y + 90;
    states.push_back(new State(pt, Settings::ABANDONED_MINE_RANDOM));

    pt.x = area.x + 10;
    pt.y = area.y + 120;
    states.push_back(new State(pt, Settings::SAVE_MONSTER_BATTLE));

    pt.x = area.x + 10;
    pt.y = area.y + 150;
    states.push_back(new State(pt, Settings::ALLOW_SET_GUARDIAN));

    pt.x = area.x + 10;
    pt.y = area.y + 180;
    states.push_back(new State(pt, Settings::LEARN_SPELLS_WITH_DAY));


    pt.x = area.x + area.w / 2 + 10;
    pt.y = area.y + 30;
    states.push_back(new State(pt, Settings::BATTLE_SHOW_DAMAGE));

    pt.x = area.x + area.w / 2 + 10;
    pt.y = area.y + 60;
    states.push_back(new State(pt, Settings::BATTLE_TROOP_DIRECTION));

    pt.x = area.x + area.w / 2 + 10;
    pt.y = area.y + 90;
    states.push_back(new State(pt, Settings::REMEMBER_LAST_FOCUS));

    pt.x = area.x + area.w / 2 + 10;
    pt.y = area.y + 120;
    states.push_back(new State(pt, Settings::FAST_LOAD_GAME_DIALOG));

    LocalEvent & le = LocalEvent::Get();

    ButtonGroups btnGroups(area, Dialog::OK|Dialog::CANCEL);
    btnGroups.Draw();

    std::for_each(states.begin(), states.end(), std::mem_fun(&State::Redraw));

    cursor.Show();
    display.Flip();

    // message loop
    u16 result = Dialog::ZERO;

    while(result == Dialog::ZERO && le.HandleEvents())
    {
	result = btnGroups.QueueEventProcessing();

	for(std::vector<State *>::iterator it = states.begin(); it != states.end(); ++it) if((**it).QueueEventProcessing()) break;

	if(!cursor.isVisible())
	{
	    cursor.Show();
	    display.Flip();
	}
    }

    for(std::vector<State *>::iterator it = states.begin(); it != states.end(); ++it) delete *it;
}
