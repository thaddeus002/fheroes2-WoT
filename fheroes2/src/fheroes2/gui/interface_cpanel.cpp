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

#include "agg.h"
#include "settings.h"
#include "game_interface.h"
#include "interface_cpanel.h"

namespace Game
{
    extern void KeyPress_r(void);
    extern void KeyPress_b(void);
    extern void KeyPress_w(void);
    extern void KeyPress_c(void);
    extern void KeyPress_e(menu_t &);
    extern void KeyPress_ESC(menu_t &);
};

Interface::ControlPanel::ControlPanel()
{
    sf_area = Surface(AGG::GetICN(ICN::CELLWIN, 11));
    sf_area.Blit(AGG::GetICN(ICN::REQUESTS, 31),  2, 3);
    sf_area.Blit(AGG::GetICN(ICN::REQUESTS, 32), 20, 3);
    sf_area.Blit(AGG::GetICN(ICN::REQUESTS, 34), 38, 3);
    sf_area.Blit(AGG::GetICN(ICN::REQUESTS, 35), 56, 3);
    sf_area.Blit(AGG::GetICN(ICN::REQUESTS, 39), 74, 3);
    w = sf_area.w();
    h = sf_area.h();
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
    AGG::FreeObject(ICN::REQUESTS);
    AGG::FreeObject(ICN::CELLWIN);
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

void Interface::ControlPanel::Redraw(void)
{
    Display::Get().Blit(sf_area, x, y);
}

void Interface::ControlPanel::QueueEventProcessing(Game::menu_t & ret)
{
    LocalEvent & le = LocalEvent::Get();

    if(le.MouseClickLeft(rt_radr))	Game::KeyPress_r();
    else
    if(le.MouseClickLeft(rt_icon))	Game::KeyPress_c();
    else
    if(le.MouseClickLeft(rt_bttn))	Game::KeyPress_b();
    else
    if(le.MouseClickLeft(rt_stat))	Game::KeyPress_w();
    else
    if(le.MouseClickLeft(rt_quit))	Game::KeyPress_e(ret);
    //if(le.MouseClickLeft(rt_quit))	Game::KeyPress_ESC(ret);
}
