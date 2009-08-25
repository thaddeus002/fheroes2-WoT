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
#include "maps.h"
#include "dialog.h"
#include "game_interface.h"

Interface::Basic::Basic() : radar(Radar::Get()), iconsPanel(IconsPanel::Get()), buttonsArea(ButtonsArea::Get()),
    statusWindow(StatusWindow::Get()), borderWindow(BorderWindow::Get()), redraw(0)
{
    radar.SetPos(Display::Get().w() - BORDERWIDTH - RADARWIDTH, BORDERWIDTH);
    iconsPanel.SetPos(Display::Get().w() - RADARWIDTH - BORDERWIDTH, RADARWIDTH + 2 * BORDERWIDTH);
    buttonsArea.SetPos(iconsPanel.GetArea().x, iconsPanel.GetArea().y + iconsPanel.GetArea().h + BORDERWIDTH);
    statusWindow.SetPos(buttonsArea.GetArea().x, buttonsArea.GetArea().y + buttonsArea.GetArea().h);
}

Interface::Basic & Interface::Basic::Get(void)
{
    static Basic basic;
    return basic;
}

bool Interface::Basic::NeedRedraw(void) const
{
    return redraw;
}

void Interface::Basic::SetRedraw(redraw_t f)
{
    redraw |= f;
}

void Interface::Basic::Redraw(u8 force)
{
    if((redraw | force) & REDRAW_RADAR) radar.Redraw();

    if((redraw | force) & REDRAW_ICONS) iconsPanel.Redraw();
    else
    if((redraw | force) & REDRAW_HEROES) iconsPanel.GetHeroesBar().Redraw();
    else
    if((redraw | force) & REDRAW_CASTLES) iconsPanel.GetCastleBar().Redraw();

    if((redraw | force) & REDRAW_BUTTONS) buttonsArea.Redraw();

    if((redraw | force) & REDRAW_STATUS) statusWindow.Redraw();

    if((redraw | force) & REDRAW_BORDER) borderWindow.Redraw();

    redraw = 0;
}
