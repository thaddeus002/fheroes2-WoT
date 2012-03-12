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
#include "text.h"
#include "cursor.h"
#include "world.h"
#include "settings.h"
#include "kingdom.h"
#include "castle.h"
#include "heroes.h"
#include "army.h"
#include "resource.h"
#include "game_focus.h"
#include "game_interface.h"
#include "interface_status.h"

#define AITURN_REDRAW_EXPIRE 20
#define RESOURCE_WINDOW_EXPIRE 2500

Interface::StatusWindow::StatusWindow() : state(STATUS_UNKNOWN), oldState(STATUS_UNKNOWN)
{
    const Sprite & ston = AGG::GetICN(Settings::Get().ExtGameEvilInterface() ? ICN::STONBAKE : ICN::STONBACK, 0);
    Rect::w = ston.w();
    Rect::h = ston.h();
}

Interface::StatusWindow & Interface::StatusWindow::Get(void)
{
    static Interface::StatusWindow wstatus;
    return wstatus;
}

void Interface::StatusWindow::Reset(void)
{
    state = STATUS_DAY;
    oldState = STATUS_UNKNOWN;
    lastResource = Resource::UNKNOWN;
    countLastResource = 0;
    ResetTimer();
}

info_t Interface::StatusWindow::GetState(void) const
{
    return state;
}

u32 Interface::StatusWindow::ResetResourceStatus(u32 tick, void *ptr)
{
    if(ptr)
    {
	Interface::StatusWindow* status = reinterpret_cast<Interface::StatusWindow*>(ptr);
	if(STATUS_RESOURCE == status->state)
	{
	    status->state = status->oldState;
	    Interface::Basic::Get().Redraw(REDRAW_STATUS);
	}
	else
	    SDL::Timer::Remove(status->timerShowLastResource);
    }

    return tick;
}

void Interface::StatusWindow::SetPos(s16 ox, s16 oy)
{
    if(Settings::Get().ExtGameHideInterface())
    {
	FixOutOfDisplay(*this, ox, oy); 

	Rect::x = ox + BORDERWIDTH;
	Rect::y = oy + BORDERWIDTH;

	border.SetPosition(ox, oy, Rect::w, Rect::h);
	Settings::Get().SetPosStatus(Point(ox, oy));
    }
    else
    {
	Rect::x = ox;
	Rect::y = oy;
	Rect::h = Display::Get().h() - oy - BORDERWIDTH;
    }
}

const Rect & Interface::StatusWindow::GetArea(void) const
{
    return Settings::Get().ExtGameHideInterface() && border.isValid() ? border.GetRect() : *this;
}

void Interface::StatusWindow::SetState(info_t info)
{
    if(STATUS_RESOURCE != state) state = info;
}

void Interface::StatusWindow::Redraw(void)
{
    const Settings & conf = Settings::Get();
    if(conf.ExtGameHideInterface() && !conf.ShowStatus()) return;

    // restore background
    DrawBackground();

    // draw info: Day and Funds and Army
    const Sprite & ston = AGG::GetICN(Settings::Get().ExtGameEvilInterface() ? ICN::STONBAKE : ICN::STONBACK, 0);

    if(STATUS_AITURN == state)
	DrawAITurns();
    else
    if(STATUS_UNKNOWN != state && h >= (ston.h() * 3 + 15))
    {
        DrawDayInfo();
	if(conf.CurrentColor() & Players::HumanColors())
	{
    	    DrawKingdomInfo(ston.h() + 5);

    	    if(state != STATUS_RESOURCE)
        	DrawArmyInfo(2 * ston.h() + 10);
    	    else
    		DrawResourceInfo(2 * ston.h() + 10);
	}
    }
    else
    switch(state)
    {
        case STATUS_DAY:	DrawDayInfo();		break;
        case STATUS_FUNDS:	DrawKingdomInfo();	break;
        case STATUS_ARMY:	DrawArmyInfo();		break;
        case STATUS_RESOURCE:   DrawResourceInfo();     break;
	default: break;
    }
    
    // redraw border
    if(conf.ExtGameHideInterface()) border.Redraw();
}

void Interface::StatusWindow::NextState(void)
{
    if(STATUS_DAY == state) state = STATUS_FUNDS;
    else
    if(STATUS_FUNDS == state) state = (GameFocus::UNSEL == GameFocus::Type() ? STATUS_DAY : STATUS_ARMY);
    else
    if(STATUS_ARMY == state) state = STATUS_DAY;
    else
    if(STATUS_RESOURCE == state) state = STATUS_ARMY;
    
    if(state == STATUS_ARMY)
    {
	const Castle* castle = GameFocus::GetCastle();

	// skip empty army for castle
        if(castle && ! castle->GetArmy().isValid()) NextState();
    }
}

void Interface::StatusWindow::DrawKingdomInfo(const u8 oh) const
{
    std::string str;

    Kingdom & myKingdom = world.GetKingdom(Settings::Get().CurrentColor());

    // sprite all resource
    AGG::GetICN(ICN::RESSMALL, 0).Blit(x + 6, y + 3 + oh);

    // count castle
    Text text(GetString(myKingdom.GetCountCastle()), Font::SMALL);
    text.Blit(x + 26 - text.w() / 2, y + 28 + oh);
    // count town
    text.Set(GetString(myKingdom.GetCountTown()));
    text.Blit(x + 78 - text.w() / 2, y + 28 + oh);
    // count gold
    text.Set(GetString(myKingdom.GetFundsGold()));
    text.Blit(x + 122 - text.w() / 2, y + 28 + oh);
    // count wood
    text.Set(GetString(myKingdom.GetFundsWood()));
    text.Blit(x + 15 - text.w() / 2, y + 58 + oh);
    // count mercury
    text.Set(GetString(myKingdom.GetFundsMercury()));
    text.Blit(x + 37 - text.w() / 2, y + 58 + oh);
    // count ore
    text.Set(GetString(myKingdom.GetFundsOre()));
    text.Blit(x + 60 - text.w() / 2, y + 58 + oh);
    // count sulfur
    text.Set(GetString(myKingdom.GetFundsSulfur()));
    text.Blit(x + 84 - text.w() / 2, y + 58 + oh);
    // count crystal
    text.Set(GetString(myKingdom.GetFundsCrystal()));
    text.Blit(x + 108 - text.w() / 2, y + 58 + oh);
    // count gems
    text.Set(GetString(myKingdom.GetFundsGems()));
    text.Blit(x + 130 - text.w() / 2, y + 58 + oh);
}

void Interface::StatusWindow::DrawDayInfo(const u8 oh) const
{
    std::string message;

    AGG::GetICN(Settings::Get().ExtGameEvilInterface() ? ICN::SUNMOONE : ICN::SUNMOON, (world.GetWeek() - 1) % 5).Blit(x, y + 1 + oh);

    message = _("Month: %{month} Week: %{week}");
    String::Replace(message, "%{month}", world.GetMonth());
    String::Replace(message, "%{week}", world.GetWeek());
    Text text(message, Font::SMALL);
    text.Blit(x + (w - text.w()) / 2, y + 30 + oh);

    message = _("Day: %{day}");
    String::Replace(message, "%{day}", world.GetDay());
    text.Set(message, Font::BIG);
    text.Blit(x + (w - text.w()) / 2, y + 46 + oh);
}

void Interface::StatusWindow::SetResource(u8 res, u16 count)
{
    lastResource = res;
    countLastResource = count;

    if(timerShowLastResource.IsValid())
	SDL::Timer::Remove(timerShowLastResource);
    else
	oldState = state;

    state = STATUS_RESOURCE;
    SDL::Timer::Run(timerShowLastResource, RESOURCE_WINDOW_EXPIRE, ResetResourceStatus, this);
}

void Interface::StatusWindow::ResetTimer(void)
{
    StatusWindow & window = Get();
    if(window.timerShowLastResource.IsValid())
    {
	SDL::Timer::Remove(window.timerShowLastResource);
	ResetResourceStatus(0, &window);
    }
}

void Interface::StatusWindow::DrawResourceInfo(const u8 oh) const
{
    std::string message = _("You find a small\nquantity of %{resource}.");
    String::Replace(message, "%{resource}", Resource::String(lastResource));
    TextBox text(message, Font::SMALL, w);
    text.Blit(x, y + 4 + oh);
    
    const Sprite &spr = AGG::GetICN(ICN::RESOURCE, Resource::GetIndexSprite2(lastResource));
    spr.Blit(x + (w - spr.w()) / 2, y + 6 + oh + text.h());

    text.Set(GetString(countLastResource), Font::SMALL, w);
    text.Blit(x + (w - text.w()) / 2, y + oh + text.h() + spr.h() - 8);
}

void Interface::StatusWindow::DrawArmyInfo(const u8 oh) const
{
    const Army::army_t* armies = NULL;

    if(GameFocus::GetHeroes())
	armies = &GameFocus::GetHeroes()->GetArmy();
    else
    if(GameFocus::GetCastle())
	armies = &GameFocus::GetCastle()->GetArmy();

    if(armies)
    {
	u8 count = armies->GetCount();

	if(4 > count)
	{
	    Army::DrawMons32Line(*armies, x, y + 20 + oh, 144);
	}
	else
	if(5 > count)
	{
	    Army::DrawMons32Line(*armies, x, y + 15 + oh, 110, 0, 2);
	    Army::DrawMons32Line(*armies, x + 20, y + 30 + oh, 120, 2, 2);
	}
	else
	{
	    Army::DrawMons32Line(*armies, x, y + 15 + oh, 140, 0, 3);
	    Army::DrawMons32Line(*armies, x + 10, y + 30 + oh, 120, 3, 2);
	}
    }
}

void Interface::StatusWindow::DrawAITurns(void) const
{
    const Settings & conf = Settings::Get();
    if(conf.ExtGameHideInterface() && !conf.ShowStatus()) return;

    // restore background
    DrawBackground();

    const Sprite & glass = AGG::GetICN(ICN::HOURGLAS, 0);

    u16 dst_x = x + (w - glass.w()) / 2;
    u16 dst_y = y + (h - glass.h()) / 2;

    glass.Blit(dst_x, dst_y);

    u8 color_index = 0;

    switch(conf.CurrentColor())
    {
	case Color::BLUE:	color_index = 0; break;
	case Color::GREEN:	color_index = 1; break;
	case Color::RED:	color_index = 2; break;
	case Color::YELLOW:	color_index = 3; break;
	case Color::ORANGE:	color_index = 4; break;
	case Color::PURPLE:	color_index = 5; break;
	default: return;
    }

    const Sprite & crest = AGG::GetICN(ICN::BRCREST, color_index);

    dst_x += 2;
    dst_y += 2;

    crest.Blit(dst_x, dst_y);

    const Sprite & sand = AGG::GetICN(ICN::HOURGLAS, 1 + (turn_progress % 10));

    dst_x += (glass.w() - sand.w() - sand.x() - 3);
    dst_y += sand.y();

    sand.Blit(dst_x, dst_y);
    
    // animation sand
    //
    // sprites ICN::HOURGLAS, 11, 30
    //
}

void Interface::StatusWindow::DrawBackground(void) const
{
    Display & display = Display::Get();
    const Sprite & icnston = AGG::GetICN(Settings::Get().ExtGameEvilInterface() ? ICN::STONBAKE : ICN::STONBACK, 0);

    if(!Settings::Get().ExtGameHideInterface() && display.h() - BORDERWIDTH - icnston.h() > y)
    {
        Rect srcrt;
	Point dstpt(x, y);

        srcrt.x = 0;
        srcrt.y = 0;
        srcrt.w = icnston.w();
        srcrt.h = 16;
        icnston.Blit(srcrt, dstpt);

        srcrt.y = 16;
        srcrt.h = 32;
        dstpt.y = dstpt.y + 16;

	u16 body = h - 32;

        for(u8 ii = 0; ii < body / 32; ++ii)
        {
            icnston.Blit(srcrt, dstpt);
    	    dstpt.y = dstpt.y + srcrt.h;
        }

	if(body % 32)
	{
    	    dstpt.y = display.h() - BORDERWIDTH - 48;
    	    srcrt.y = icnston.h() - 48;
    	    srcrt.h = 48;
    	    icnston.Blit(srcrt, dstpt);
	}
	else
	{
	    dstpt.y = display.h() - BORDERWIDTH - 16;
    	    srcrt.y = icnston.h() - 16;
    	    srcrt.h = 16;
    	    icnston.Blit(srcrt, dstpt);
	}
    }
    else
	icnston.Blit(x, y);
}

void Interface::StatusWindow::QueueEventProcessing(void)
{
    Display & display = Display::Get();
    Cursor & cursor = Cursor::Get();
    Settings & conf = Settings::Get();
    LocalEvent & le = LocalEvent::Get();

    if(conf.ExtGameHideInterface() && conf.ShowStatus() && le.MousePressLeft(border.GetTop()))
    {
        Surface sf(border.GetRect().w, border.GetRect().h);
        Cursor::DrawCursor(sf, 0x70);
        const Point & mp = le.GetMouseCursor();
        const s16 ox = mp.x - border.GetRect().x;
        const s16 oy = mp.y - border.GetRect().y;
        SpriteCursor sp(sf, border.GetRect().x, border.GetRect().y);
        cursor.Hide();
        sp.Redraw();
        cursor.Show();
        display.Flip();
        while(le.HandleEvents() && le.MousePressLeft())
        {
    	    if(le.MouseMotion())
            {
		cursor.Hide();
        	sp.Move(mp.x - ox, mp.y - oy);
                cursor.Show();
                display.Flip();
            }
        }
        cursor.Hide();
        SetPos(mp.x - ox, mp.y - oy);
        Interface::Basic::Get().SetRedraw(REDRAW_GAMEAREA);
    }
    else
    if(le.MouseClickLeft(*this))
    {
        cursor.Hide();
        NextState();
        Redraw();
        cursor.Show();
        display.Flip();
    }
    if(le.MousePressRight(*this)) Dialog::Message(_("Status Window"), _("This window provides information on the status of your hero or kingdom, and shows the date. Left click here to cycle throungh these windows."), Font::BIG);
}

void Interface::StatusWindow::RedrawTurnProgress(u8 v)
{
    turn_progress = v;
    Interface::Basic & I = Interface::Basic::Get();
    I.SetRedraw(REDRAW_STATUS);

    Cursor::Get().Hide();
    I.Redraw();
    Cursor::Get().Show();
    Display::Get().Flip();
}
