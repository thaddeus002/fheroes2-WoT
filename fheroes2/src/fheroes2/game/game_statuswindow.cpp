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
#include "tools.h"
#include "world.h"
#include "config.h"
#include "sprite.h"
#include "kingdom.h"
#include "castle.h"
#include "heroes.h"
#include "army.h"
#include "game_focus.h"
#include "game_statuswindow.h"

Game::StatusWindow::StatusWindow() : state(UNKNOWN)
{
}

Game::StatusWindow & Game::StatusWindow::Get(void)
{
    static Game::StatusWindow wstatus;

    return wstatus;
}

void Game::StatusWindow::SetPos(const Point &pt)
{
    const Sprite & ston = AGG::GetICN(H2Config::EvilInterface() ? ICN::STONBAKE : ICN::STONBACK, 0);
    pos = Rect(pt, ston.w(), ston.h());
    count = (Display::Get().h() - BORDERWIDTH - pos.y) / TILEWIDTH;
}

const Rect & Game::StatusWindow::GetRect(void) const
{
    return pos;
}

void Game::StatusWindow::SetState(const info_t info)
{
    state = info;
}

void Game::StatusWindow::Redraw(void)
{
    // restore background
    DrawBackground();
    const Sprite & ston = AGG::GetICN(H2Config::EvilInterface() ? ICN::STONBAKE : ICN::STONBACK, 0);

    // draw info: Day and Funds and Army
    if(UNKNOWN != state && count >= ((ston.h() * 3 + 15)/ TILEWIDTH))
    {
	DrawDayInfo();
	DrawKingdomInfo(ston.h() + 5);
	DrawArmyInfo(2 * ston.h() + 10);
    }
    else
    switch(state)
    {
	case DAY:	DrawDayInfo();		break;
	case FUNDS:	DrawKingdomInfo();	break;
	case ARMY:	DrawArmyInfo();		break;
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
    Text(count, Font::SMALL, pos.x + 26 - Text::width(count, Font::SMALL) / 2, pos.y + 28 + oh);
    // count town
    count.clear();
    String::AddInt(count, myKingdom.GetCountTown());
    Text(count, Font::SMALL, pos.x + 78 - Text::width(count, Font::SMALL) / 2, pos.y + 28 + oh);
    // count gold
    count.clear();
    String::AddInt(count, myKingdom.GetFundsGold());
    Text(count, Font::SMALL, pos.x + 122 - Text::width(count, Font::SMALL) / 2, pos.y + 28 + oh);
    // count wood
    count.clear();
    String::AddInt(count, myKingdom.GetFundsWood());
    Text(count, Font::SMALL, pos.x + 15 - Text::width(count, Font::SMALL) / 2, pos.y + 58 + oh);
    // count mercury
    count.clear();
    String::AddInt(count, myKingdom.GetFundsMercury());
    Text(count, Font::SMALL, pos.x + 37 - Text::width(count, Font::SMALL) / 2, pos.y + 58 + oh);
    // count ore
    count.clear();
    String::AddInt(count, myKingdom.GetFundsOre());
    Text(count, Font::SMALL, pos.x + 60 - Text::width(count, Font::SMALL) / 2, pos.y + 58 + oh);
    // count sulfur
    count.clear();
    String::AddInt(count, myKingdom.GetFundsSulfur());
    Text(count, Font::SMALL, pos.x + 84 - Text::width(count, Font::SMALL) / 2, pos.y + 58 + oh);
    // count crystal
    count.clear();
    String::AddInt(count, myKingdom.GetFundsCrystal());
    Text(count, Font::SMALL, pos.x + 108 - Text::width(count, Font::SMALL) / 2, pos.y + 58 + oh);
    // count gems
    count.clear();
    String::AddInt(count, myKingdom.GetFundsGems());
    Text(count, Font::SMALL, pos.x + 130 - Text::width(count, Font::SMALL) / 2, pos.y + 58 + oh);
}

void Game::StatusWindow::DrawDayInfo(const u8 oh) const
{
    std::string message;
    Display & display = Display::Get();

    display.Blit(AGG::GetICN(H2Config::EvilInterface() ? ICN::SUNMOONE : ICN::SUNMOON, (world.GetWeek() - 1) % 5), pos.x, pos.y + 1 + oh);

    message = "Month: ";
    String::AddInt(message, world.GetMonth());

    message += " Week: ";
    String::AddInt(message, world.GetWeek());
    Text(message, Font::SMALL, pos.x + (pos.w - Text::width(message, Font::SMALL)) / 2, pos.y + 30 + oh);

    message = "Day: ";
    String::AddInt(message, world.GetDay());
    Text(message, Font::BIG, pos.x + (pos.w - Text::width(message, Font::BIG)) / 2, pos.y + 46 + oh);
}

void Game::StatusWindow::DrawArmyInfo(const u8 oh) const
{
    const Game::Focus & focus = Game::Focus::Get();

    if(Game::Focus::UNSEL == focus.Type()) return;

    const Army::army_t & armies = (Game::Focus::HEROES == focus.Type() ? focus.GetHeroes().GetArmy() : focus.GetCastle().GetArmy());
    Display & display = Display::Get();

    // valid count army
    u16 count = std::count_if(armies.begin(), armies.end(), Army::isValid);

    Point dst_pt;
    u8 current = 0;
    std::string str;

    const u8 one_line = 3;

    // one lines
    if(4 > count)
    {
	Army::army_t::const_iterator it1 = armies.begin();
	Army::army_t::const_iterator it2 = armies.end();
	for(; it1 != it2; ++it1)
	    if((*it1).isValid())
	    {
		const Sprite & sprite = AGG::GetICN(ICN::MONS32, (*it1).Monster());

		dst_pt.x = (pos.w / one_line - sprite.w()) / 2 + pos.x + current * pos.w / count + ((pos.w / one_line) * (one_line - count) / (2 * count));
		dst_pt.y = pos.y + 55 - sprite.h() + oh;

		display.Blit(sprite, dst_pt);

		str.clear();
		String::AddInt(str, (*it1).Count());
		Text(str, Font::SMALL, dst_pt.x + (sprite.w() - Text::width(str, Font::SMALL)) / 2, dst_pt.y + sprite.h() + 2);

		++current;
	    }
    }
    else
    // two lines
    {
	// top
	u8 ii = 0;
	count = ( 4 < count ? 3 : 2);

	Army::army_t::const_iterator it1 = armies.begin();
	Army::army_t::const_iterator it2 = armies.end();
	for(; it1 != it2; ++it1)
	    if((*it1).isValid())
	    {
		const Sprite & sprite = AGG::GetICN(ICN::MONS32, (*it1).Monster());

		dst_pt.x = (pos.w / one_line - sprite.w()) / 2 + pos.x + current * pos.w / count + ((pos.w / one_line) * (one_line - count) / (2 * count));
		dst_pt.y = pos.y + 42 - sprite.h() + oh;

		display.Blit(sprite, dst_pt);

		str.clear();
		String::AddInt(str, (*it1).Count());
		Text(str, Font::SMALL, dst_pt.x + (sprite.w() - Text::width(str, Font::SMALL)) / 2, dst_pt.y + sprite.h() - 6);

		++current;

		if(current == count) break;
	    }

	// bottom
	count = 2;
	current = 0;
	++ii;

	it1 = armies.begin();
	for(; it1 != it2; ++it1)
	    if((*it1).isValid())
	    {
		const Sprite & sprite = AGG::GetICN(ICN::MONS32, (*it1).Monster());

		dst_pt.x = (pos.w / one_line - sprite.w()) / 2 + pos.x + current * pos.w / count + ((pos.w / one_line) * (one_line - count) / (2 * count));
		dst_pt.y = pos.y + 65 - sprite.h() + oh;

		display.Blit(sprite, dst_pt);

		str.clear();
		String::AddInt(str, (*it1).Count());
		Text(str, Font::SMALL, dst_pt.x + (sprite.w() - Text::width(str, Font::SMALL)) / 2, dst_pt.y + sprite.h() - 6);

		++current;
	    }
    }
}

void Game::StatusWindow::RedrawAITurns(Color::color_t color, u8 progress) const
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

    display.Flip();
}

void Game::StatusWindow::DrawBackground(void) const
{
    Display & display = Display::Get();
    const Sprite & icnston = AGG::GetICN(H2Config::EvilInterface() ? ICN::STONBAKE : ICN::STONBACK, 0);
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
