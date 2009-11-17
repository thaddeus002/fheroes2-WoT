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

#include <sstream>
#include "agg.h"
#include "settings.h"
#include "maps.h"
#include "dialog.h"
#include "game_interface.h"

Interface::Basic::Basic() : gameArea(GameArea::Get()), radar(Radar::Get()),
    iconsPanel(IconsPanel::Get()), buttonsArea(ButtonsArea::Get()),
    statusWindow(StatusWindow::Get()), borderWindow(BorderWindow::Get()),
    controlPanel(ControlPanel::Get()), redraw(0)
{
    const Display & display = Display::Get();
    const u16 & px = display.w() - BORDERWIDTH - RADARWIDTH;
    const u8 scroll_width = Settings::Get().PocketPC() ? 12 : BORDERWIDTH;

    if(Settings::Get().PocketPC())
    {
        iconsPanel.SetCount(2);
	radar.SetPos(BORDERWIDTH, BORDERWIDTH);
	iconsPanel.SetPos(BORDERWIDTH, BORDERWIDTH);
	buttonsArea.SetPos(BORDERWIDTH, BORDERWIDTH);
	statusWindow.SetPos(BORDERWIDTH, BORDERWIDTH);
	controlPanel.SetPos(display.w() - controlPanel.GetArea().w - scroll_width, 0);
    }
    else
    if(Settings::Get().HideInterface())
    {
        iconsPanel.SetCount(2);
	radar.SetPos(BORDERWIDTH, BORDERWIDTH);
	iconsPanel.SetPos(px - BORDERWIDTH, radar.GetArea().y + radar.GetArea().h);
	buttonsArea.SetPos(px - BORDERWIDTH, iconsPanel.GetArea().y + iconsPanel.GetArea().h);
	statusWindow.SetPos(px - BORDERWIDTH, buttonsArea.GetArea().y + buttonsArea.GetArea().h);
	controlPanel.SetPos(display.w() - controlPanel.GetArea().w - scroll_width, 0);
    }
    else
    {
	radar.SetPos(px, BORDERWIDTH);
	iconsPanel.SetPos(px, radar.GetArea().y + radar.GetArea().h + BORDERWIDTH);

        const u8 count_h = (display.h() - 480) / TILEWIDTH;
        iconsPanel.SetCount(count_h > 3 ? 8 : ( count_h < 3 ? 4 : 7));

	buttonsArea.SetPos(px, iconsPanel.GetArea().y + iconsPanel.GetArea().h + BORDERWIDTH);
	statusWindow.SetPos(px, buttonsArea.GetArea().y + buttonsArea.GetArea().h);
    }

    scrollLeft = Rect(0, 0, scroll_width, display.h());
    scrollRight = Rect(display.w() - scroll_width, 0, scroll_width, display.h());
    scrollTop = Settings::Get().PocketPC() ? Rect(0, 0, controlPanel.GetArea().x, scroll_width) : Rect(0, 0, display.w() - radar.GetArea().w, scroll_width);
    scrollBottom = Rect(0, display.h() - scroll_width, display.w(), scroll_width);
    
    system_info.Set(Font::YELLOW_SMALL);
}

Interface::Basic & Interface::Basic::Get(void)
{
    static Basic basic;
    return basic;
}

const Rect & Interface::Basic::GetAreaScrollLeft(void) const
{
    return scrollLeft;
}

const Rect & Interface::Basic::GetAreaScrollRight(void) const
{
    return scrollRight;
}

const Rect & Interface::Basic::GetAreaScrollTop(void) const
{
    return scrollTop;
}

const Rect & Interface::Basic::GetAreaScrollBottom(void) const
{
    return scrollBottom;
}


bool Interface::Basic::NeedRedraw(void) const
{
    return redraw;
}

void Interface::Basic::SetRedraw(u8 f)
{
    redraw |= f;
}

void Interface::Basic::Redraw(u8 force)
{
    const Settings & conf = Settings::Get();

    if((redraw | force) & REDRAW_GAMEAREA) gameArea.Redraw(Display::Get());

    if((conf.HideInterface() && conf.ShowRadar()) || ((redraw | force) & REDRAW_RADAR)) radar.Redraw();

    if((conf.HideInterface() && conf.ShowIcons()) || ((redraw | force) & REDRAW_ICONS)) iconsPanel.Redraw();
    else
    if((redraw | force) & REDRAW_HEROES) iconsPanel.GetHeroesBar().Redraw();
    else
    if((redraw | force) & REDRAW_CASTLES) iconsPanel.GetCastleBar().Redraw();

    if((conf.HideInterface() && conf.ShowButtons()) || ((redraw | force) & REDRAW_BUTTONS)) buttonsArea.Redraw();

    if((conf.HideInterface() && conf.ShowStatus()) || ((redraw | force) & REDRAW_STATUS)) statusWindow.Redraw();

    if(conf.HideInterface() && conf.ShowControlPanel() && (redraw & REDRAW_GAMEAREA)) controlPanel.Redraw();

    // show system info
    if((0x000F & conf.Debug()) >= DBG_INFO)
	RedrawSystemInfo((conf.HideInterface() ? 10 : 26), Display::Get().h() - (conf.HideInterface() ? 14 : 30));

    if((redraw | force) & REDRAW_BORDER) borderWindow.Redraw();

    redraw = 0;
}

void Interface::Basic::RedrawSystemInfo(s16 cx, s16 cy)
{
    std::ostringstream os;
    os << "fps: " << 0 << ", usage memory: " << GetMemoryUsage() << "Kb";
    system_info.Set(os.str());
    system_info.Blit(cx, cy);
}
