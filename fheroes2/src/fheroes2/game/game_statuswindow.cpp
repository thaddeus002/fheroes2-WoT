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

#include <algorithm>
#include "agg.h"
#include "text.h"
#include "world.h"
#include "settings.h"
#include "kingdom.h"
#include "castle.h"
#include "heroes.h"
#include "army.h"
#include "game_focus.h"
#include "game_statuswindow.h"

#define RESOURCE_WINDOW_EXPIRE 4000

Game::StatusWindow::StatusWindow() : state(UNKNOWN)
{
}

Game::StatusWindow & Game::StatusWindow::Get(void)
{
    static Game::StatusWindow wstatus;

    return wstatus;
}

u32 Game::StatusWindow::ResetResourceStatus(u32 tick, void *ptr)
{
    if(ptr && RESOURCE == reinterpret_cast<Game::StatusWindow*>(ptr)->state)
    {
	reinterpret_cast<Game::StatusWindow*>(ptr)->state = reinterpret_cast<Game::StatusWindow*>(ptr)->old_state;
	reinterpret_cast<Game::StatusWindow*>(ptr)->Redraw();
    }
    else
	Timer::Remove(reinterpret_cast<Game::StatusWindow*>(ptr)->timerShowLastResource);

    return tick;
}

void Game::StatusWindow::SetPos(const Point &pt)
{
    const Sprite & ston = AGG::GetICN(Settings::Get().EvilInterface() ? ICN::STONBAKE : ICN::STONBACK, 0);
    pos = Rect(pt, ston.w(), ston.h());
    count = (Display::Get().h() - BORDERWIDTH - pos.y) / TILEWIDTH;
}

const Rect & Game::StatusWindow::GetRect(void) const
{
    return pos;
}

void Game::StatusWindow::SetState(const info_t info)
{
    if(RESOURCE != state)
    state = info;
}

void Game::StatusWindow::Redraw(void)
{
    // restore background
    DrawBackground();
    const Sprite & ston = AGG::GetICN(Settings::Get().EvilInterface() ? ICN::STONBAKE : ICN::STONBACK, 0);

    // draw info: Day and Funds and Army
    if(UNKNOWN != state && count >= ((ston.h() * 3 + 15)/ TILEWIDTH))
    {
        DrawDayInfo();
        DrawKingdomInfo(ston.h() + 5);
        if(state != RESOURCE)
            DrawArmyInfo(2 * ston.h() + 10);
        else DrawResourceInfo(2 * ston.h() + 10);
    }
    else
    switch(state)
    {
        case DAY:	DrawDayInfo();		break;
        case FUNDS:	DrawKingdomInfo();	break;
        case ARMY:	DrawArmyInfo();		break;
        case RESOURCE: DrawResourceInfo(); break;
	default: break;
    }
}

void Game::StatusWindow::NextState(void)
{
    if(DAY == state) state = FUNDS;
    else
    if(FUNDS == state) state = (Game::Focus::UNSEL == Game::Focus::Get().Type() ? DAY : ARMY);
    else
    if(ARMY == state) state = DAY;
    else
    if(RESOURCE == state) state = ARMY;
}

void Game::StatusWindow::DrawKingdomInfo(const u8 oh) const
{
    std::string count;

    Kingdom & myKingdom = world.GetMyKingdom();
    Display & display = Display::Get();

    // sprite all resource
    display.Blit(AGG::GetICN(ICN::RESSMALL, 0), pos.x + 6, pos.y + 3 + oh);

    // count castle
    String::AddInt(count, myKingdom.GetCountCastle());
    Text text(count, Font::SMALL);
    text.Blit(pos.x + 26 - text.w() / 2, pos.y + 28 + oh);
    // count town
    count.clear();
    String::AddInt(count, myKingdom.GetCountTown());
    text.Set(count);
    text.Blit(pos.x + 78 - text.w() / 2, pos.y + 28 + oh);
    // count gold
    count.clear();
    String::AddInt(count, myKingdom.GetFundsGold());
    text.Set(count);
    text.Blit(pos.x + 122 - text.w() / 2, pos.y + 28 + oh);
    // count wood
    count.clear();
    String::AddInt(count, myKingdom.GetFundsWood());
    text.Set(count);
    text.Blit(pos.x + 15 - text.w() / 2, pos.y + 58 + oh);
    // count mercury
    count.clear();
    String::AddInt(count, myKingdom.GetFundsMercury());
    text.Set(count);
    text.Blit(pos.x + 37 - text.w() / 2, pos.y + 58 + oh);
    // count ore
    count.clear();
    String::AddInt(count, myKingdom.GetFundsOre());
    text.Set(count);
    text.Blit(pos.x + 60 - text.w() / 2, pos.y + 58 + oh);
    // count sulfur
    count.clear();
    String::AddInt(count, myKingdom.GetFundsSulfur());
    text.Set(count);
    text.Blit(pos.x + 84 - text.w() / 2, pos.y + 58 + oh);
    // count crystal
    count.clear();
    String::AddInt(count, myKingdom.GetFundsCrystal());
    text.Set(count);
    text.Blit(pos.x + 108 - text.w() / 2, pos.y + 58 + oh);
    // count gems
    count.clear();
    String::AddInt(count, myKingdom.GetFundsGems());
    text.Set(count);
    text.Blit(pos.x + 130 - text.w() / 2, pos.y + 58 + oh);
}

void Game::StatusWindow::DrawDayInfo(const u8 oh) const
{
    std::string message;
    Display & display = Display::Get();

    display.Blit(AGG::GetICN(Settings::Get().EvilInterface() ? ICN::SUNMOONE : ICN::SUNMOON, (world.GetWeek() - 1) % 5), pos.x, pos.y + 1 + oh);

    message = _("Month: %{month} Week: %{week}");
    String::Replace(message, "%{month}", world.GetMonth());
    String::Replace(message, "%{week}", world.GetWeek());
    Text text(message, Font::SMALL);
    text.Blit(pos.x + (pos.w - text.w()) / 2, pos.y + 30 + oh);

    message = _("Day: %{day}");
    String::Replace(message, "%{day}", world.GetDay());
    text.Set(message, Font::BIG);
    text.Blit(pos.x + (pos.w - text.w()) / 2, pos.y + 46 + oh);
}

void Game::StatusWindow::SetResource(const Resource::resource_t res, u16 count)
{
    lastResource = res;
    countLastResource = count;

    if(timerShowLastResource.IsValid())
	Timer::Remove(timerShowLastResource);
    else
	old_state = state;

    state = RESOURCE;
    Timer::Run(timerShowLastResource, RESOURCE_WINDOW_EXPIRE, ResetResourceStatus, this);
}

void Game::StatusWindow::DrawResourceInfo(const u8 oh) const
{
    std::string message = _("You find a small\nquantity of %{resource}.");
    String::Replace(message, "%{resource}", Resource::String(lastResource));
    TextBox text(message, Font::SMALL, pos.w);
    text.Blit(pos.x, pos.y + 4 + oh);
    
    Display & display = Display::Get();
    const Sprite &spr = AGG::GetICN(ICN::RESOURCE, Resource::GetIndexSprite2(lastResource));
    display.Blit(spr, pos.x + (pos.w - spr.w()) / 2, pos.y + 6 + oh + text.h());

    message.clear();
    String::AddInt(message, countLastResource);
    text.Set(message, Font::SMALL, pos.w);
    text.Blit(pos.x + (pos.w - text.w()) / 2, pos.y + oh + text.h() + spr.h() - 8);
}

void Game::StatusWindow::DrawArmyInfo(const u8 oh) const
{
    const Game::Focus & focus = Game::Focus::Get();

    if(Game::Focus::UNSEL == focus.Type()) return;

    const Army::army_t & armies = (Game::Focus::HEROES == focus.Type() ? focus.GetHeroes().GetArmy() : focus.GetCastle().GetArmy());
    const u8 count = armies.GetCount();

    if(4 > count)
    {
	armies.DrawMons32Line(pos.x, pos.y + 20 + oh, 144);
    }
    else
    if(5 > count)
    {
	armies.DrawMons32Line(pos.x, pos.y + 15 + oh, 110, 0, 2);
	armies.DrawMons32Line(pos.x + 20, pos.y + 30 + oh, 120, 2, 2);
    }
    else
    {
	armies.DrawMons32Line(pos.x, pos.y + 15 + oh, 140, 0, 3);
	armies.DrawMons32Line(pos.x + 10, pos.y + 30 + oh, 120, 3, 2);
    }
}

void Game::StatusWindow::RedrawAITurns(u8 color, u8 progress) const
{
    // restore background
    DrawBackground();

    Display & display = Display::Get();
    const Sprite & glass = AGG::GetICN(ICN::HOURGLAS, 0);

    u16 dst_x = pos.x + (pos.w - glass.w()) / 2;
    u16 dst_y = pos.y + (pos.h - glass.h()) / 2;

    display.Blit(glass, dst_x, dst_y);

    u8 color_index = 0;

    switch(color)
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

    display.Blit(crest, dst_x, dst_y);

    const Sprite & sand = AGG::GetICN(ICN::HOURGLAS, 1 + (progress % 10));

    dst_x += (glass.w() - sand.w() - sand.x() - 3);
    dst_y += sand.y();

    display.Blit(sand, dst_x, dst_y);
    
    // animation sand
    //
    // sprites ICN::HOURGLAS, 11, 30
    //
}

void Game::StatusWindow::DrawBackground(void) const
{
    Display & display = Display::Get();
    const Sprite & icnston = AGG::GetICN(Settings::Get().EvilInterface() ? ICN::STONBAKE : ICN::STONBACK, 0);
    Point dstpt(pos);

    if(display.h() - BORDERWIDTH - icnston.h() > dstpt.y)
    {
        Rect srcrt;

        srcrt.x = 0;
        srcrt.y = 0;
        srcrt.w = icnston.w();
        srcrt.h = 12;
        display.Blit(icnston, srcrt, dstpt);

        srcrt.y = 12;
        srcrt.h = 32;
        dstpt.y = dstpt.y + 12;

        for(u8 ii = 0; ii < count; ++ii)
        {
            display.Blit(icnston, srcrt, dstpt);
    	    dstpt.y = dstpt.y + TILEWIDTH;
        }

        dstpt.y = display.h() - BORDERWIDTH - 12;
        srcrt.y = icnston.h() - 12;
        display.Blit(icnston, srcrt, dstpt);
    }
    else
	display.Blit(icnston, dstpt);
}
