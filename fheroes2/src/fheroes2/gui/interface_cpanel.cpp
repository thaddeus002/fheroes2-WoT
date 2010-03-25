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
#include "game_interface.h"
#include "interface_cpanel.h"

namespace Game
{
    // game_startgame.cpp
    void SwitchShowRadar(void);
    void SwitchShowStatus(void);
    void SwitchShowButtons(void);
    void SwitchShowIcons(void);
    void KeyPress_e(menu_t &);
    void KeyPress_ESC(menu_t &);
}

Interface::ControlPanel::ControlPanel()
{
    if(Display::Get().h() > 240)
    {
	w = 180;
	h = 36;
	rt_radr.w = 36;
	rt_radr.h = 36;
	rt_icon.w = 36;
	rt_icon.h = 36;
	rt_bttn.w = 36;
	rt_bttn.h = 36;
	rt_stat.w = 36;
	rt_stat.h = 36;
	rt_quit.w = 36;
	rt_quit.h = 36;
    }
    else
    {
	const Sprite & sf = AGG::GetICN(ICN::CELLWIN, 11);
	w = sf.w();
	h = sf.h();
	rt_radr.w = 17;
	rt_radr.h = 17;
	rt_icon.w = 17;
	rt_icon.h = 17;
	rt_bttn.w = 17;
	rt_bttn.h = 17;
	rt_stat.w = 17;
	rt_stat.h = 17;
	rt_quit.w = 17;
	rt_quit.h = 17;
    }
}

Interface::ControlPanel & Interface::ControlPanel::Get(void)
{
    static ControlPanel cp;
    return cp;
}

const Rect & Interface::ControlPanel::GetArea(void)
{
    return *this;
}

void Interface::ControlPanel::SetPos(s16 ox, s16 oy)
{
    x = ox;
    y = oy;
    if(Display::Get().h() > 240)
    {
	rt_radr.x = x;
	rt_radr.y = y;
	rt_icon.x = x + 36;
	rt_icon.y = y;
	rt_bttn.x = x + 72;
	rt_bttn.y = y;
	rt_stat.x = x + 108;
	rt_stat.y = y;
	rt_quit.x = x + 144;
	rt_quit.y = y;
    }
    else
    {
	rt_radr.x = x + 2;
	rt_radr.y = y + 3;
	rt_icon.x = x + 20;
	rt_icon.y = y + 3;
	rt_bttn.x = x + 38;
	rt_bttn.y = y + 3;
	rt_stat.x = x + 56;
	rt_stat.y = y + 3;
	rt_quit.x = x + 74;
	rt_quit.y = y + 3;
    }
}

void Interface::ControlPanel::Redraw(void)
{
    Display & display = Display::Get();

    if(Display::Get().h() > 240)
    {
	ICN::icn_t icn = Settings::Get().EvilInterface() ? ICN::ADVEBTNS : ICN::ADVBTNS;
	display.Blit(AGG::GetICN(icn, 4),  x, y);
	display.Blit(AGG::GetICN(icn, 0),  x + 36, y);
	display.Blit(AGG::GetICN(icn, 12), x + 72, y);
	display.Blit(AGG::GetICN(icn, 10), x + 108, y);
	display.Blit(AGG::GetICN(icn, 8),  x + 144, y);
    }
    else
    {
	const Sprite & sf = AGG::GetICN(ICN::CELLWIN, 11);
	display.Blit(sf, x, y);
	display.Blit(AGG::GetICN(ICN::REQUESTS, 31), x + 2, y + 3);
	display.Blit(AGG::GetICN(ICN::REQUESTS, 32), x + 20, y + 3);
	display.Blit(AGG::GetICN(ICN::REQUESTS, 34), x + 38, y + 3);
	display.Blit(AGG::GetICN(ICN::REQUESTS, 35), x + 56, y + 3);
	display.Blit(AGG::GetICN(ICN::REQUESTS, 39), x + 74, y + 3);
    }
}

void Interface::ControlPanel::QueueEventProcessing(Game::menu_t & ret)
{
    LocalEvent & le = LocalEvent::Get();

    if(le.MouseClickLeft(rt_radr))	Game::SwitchShowRadar();
    else
    if(le.MouseClickLeft(rt_icon))	Game::SwitchShowIcons();
    else
    if(le.MouseClickLeft(rt_bttn))	Game::SwitchShowButtons();
    else
    if(le.MouseClickLeft(rt_stat))	Game::SwitchShowStatus();
    else
    if(le.MouseClickLeft(rt_quit))	Game::KeyPress_e(ret);
    //if(le.MouseClickLeft(rt_quit))	Game::KeyPress_ESC(ret);
}
