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

Game::StatusWindow::StatusWindow() : state(Game::StatusWindow::DAY)
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
    switch(state)
    {
	case DAY:	DrawDayInfo();		break;
	case FUNDS:	DrawKingdomInfo();	break;
	case ARMY:	DrawArmyInfo();		break;
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

void Game::StatusWindow::DrawKingdomInfo(void)
{
    std::string count;

    Kingdom & myKingdom = world.GetMyKingdom();

    Display & display = Display::Get();
    const Sprite & ston = AGG::GetICN(H2Config::EvilInterface() ? ICN::STONBAKE : ICN::STONBACK, 0);

    // restore background
    display.Blit(ston, pos.x, pos.y);

    // sprite all resource
    display.Blit(AGG::GetICN(ICN::RESSMALL, 0), pos.x + 6, pos.y + 3);

    // count castle
    String::AddInt(count, myKingdom.GetCountCastle());
    Text(count, Font::SMALL, pos.x + 26 - Text::width(count, Font::SMALL) / 2, pos.y + 28);
    // count town
    count.clear();
    String::AddInt(count, myKingdom.GetCountTown());
    Text(count, Font::SMALL, pos.x + 78 - Text::width(count, Font::SMALL) / 2, pos.y + 28);
    // count gold
    count.clear();
    String::AddInt(count, myKingdom.GetFundsGold());
    Text(count, Font::SMALL, pos.x + 122 - Text::width(count, Font::SMALL) / 2, pos.y + 28);
    // count wood
    count.clear();
    String::AddInt(count, myKingdom.GetFundsWood());
    Text(count, Font::SMALL, pos.x + 15 - Text::width(count, Font::SMALL) / 2, pos.y + 58);
    // count mercury
    count.clear();
    String::AddInt(count, myKingdom.GetFundsMercury());
    Text(count, Font::SMALL, pos.x + 37 - Text::width(count, Font::SMALL) / 2, pos.y + 58);
    // count ore
    count.clear();
    String::AddInt(count, myKingdom.GetFundsOre());
    Text(count, Font::SMALL, pos.x + 60 - Text::width(count, Font::SMALL) / 2, pos.y + 58);
    // count sulfur
    count.clear();
    String::AddInt(count, myKingdom.GetFundsSulfur());
    Text(count, Font::SMALL, pos.x + 84 - Text::width(count, Font::SMALL) / 2, pos.y + 58);
    // count crystal
    count.clear();
    String::AddInt(count, myKingdom.GetFundsCrystal());
    Text(count, Font::SMALL, pos.x + 108 - Text::width(count, Font::SMALL) / 2, pos.y + 58);
    // count gems
    count.clear();
    String::AddInt(count, myKingdom.GetFundsGems());
    Text(count, Font::SMALL, pos.x + 130 - Text::width(count, Font::SMALL) / 2, pos.y + 58);
}

void Game::StatusWindow::DrawDayInfo(void)
{
    std::string message;

    Display & display = Display::Get();
    const Sprite & ston = AGG::GetICN(H2Config::EvilInterface() ? ICN::STONBAKE : ICN::STONBACK, 0);

    // restore background
    display.Blit(ston, pos.x, pos.y);

    display.Blit(AGG::GetICN(H2Config::EvilInterface() ? ICN::SUNMOONE : ICN::SUNMOON, (world.GetWeek() - 1) % 5), pos.x, pos.y + 1);

    message = "Month: ";
    String::AddInt(message, world.GetMonth());

    message += " Week: ";
    String::AddInt(message, world.GetWeek());
    Text(message, Font::SMALL, pos.x + (pos.w - Text::width(message, Font::SMALL)) / 2, pos.y + 30);

    message = "Day: ";
    String::AddInt(message, world.GetDay());
    Text(message, Font::BIG, pos.x + (pos.w - Text::width(message, Font::BIG)) / 2, pos.y + 46);
}

void Game::StatusWindow::DrawArmyInfo(void)
{
    const Game::Focus & focus = Game::Focus::Get();

    if(Game::Focus::UNSEL == focus.Type()) return;

    const std::vector<Army::Troops> & armies = (Game::Focus::HEROES == focus.Type() ? focus.GetHeroes().GetArmy() : focus.GetCastle().GetArmy());

    Display & display = Display::Get();
    const Sprite & ston = AGG::GetICN(H2Config::EvilInterface() ? ICN::STONBAKE : ICN::STONBACK, 0);

    // restore background
    display.Blit(ston, pos.x, pos.y);
    
    // valid count army
    u16 count = std::count_if(armies.begin(), armies.end(), Army::Troops::PredicateIsValid);

    Point dst_pt;
    u8 current = 0;
    std::string str;

    const u8 one_line = 3;

    // one lines
    if(4 > count)
    {
	for(u8 ii = 0; ii < HEROESMAXARMY; ++ii)
	    if(armies[ii].isValid())
	    {
		const Sprite & sprite = AGG::GetICN(ICN::MONS32, armies[ii].Monster());

		dst_pt.x = (pos.w / one_line - sprite.w()) / 2 + pos.x + current * pos.w / count + ((pos.w / one_line) * (one_line - count) / (2 * count));
		dst_pt.y = pos.y + 55 - sprite.h();

		display.Blit(sprite, dst_pt);

		str.clear();
		String::AddInt(str, armies[ii].Count());
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

	for(; ii < HEROESMAXARMY; ++ii)
	    if(armies[ii].isValid())
	    {
		const Sprite & sprite = AGG::GetICN(ICN::MONS32, armies[ii].Monster());

		dst_pt.x = (pos.w / one_line - sprite.w()) / 2 + pos.x + current * pos.w / count + ((pos.w / one_line) * (one_line - count) / (2 * count));
		dst_pt.y = pos.y + 42 - sprite.h();

		display.Blit(sprite, dst_pt);

		str.clear();
		String::AddInt(str, armies[ii].Count());
		Text(str, Font::SMALL, dst_pt.x + (sprite.w() - Text::width(str, Font::SMALL)) / 2, dst_pt.y + sprite.h() - 6);

		++current;

		if(current == count) break;
	    }

	// bottom
	count = 2;
	current = 0;
	++ii;

	for(; ii < HEROESMAXARMY; ++ii)
	    if(armies[ii].isValid())
	    {
		const Sprite & sprite = AGG::GetICN(ICN::MONS32, armies[ii].Monster());

		dst_pt.x = (pos.w / one_line - sprite.w()) / 2 + pos.x + current * pos.w / count + ((pos.w / one_line) * (one_line - count) / (2 * count));
		dst_pt.y = pos.y + 65 - sprite.h();

		display.Blit(sprite, dst_pt);

		str.clear();
		String::AddInt(str, armies[ii].Count());
		Text(str, Font::SMALL, dst_pt.x + (sprite.w() - Text::width(str, Font::SMALL)) / 2, dst_pt.y + sprite.h() - 6);

		++current;
	    }
    }
}

void Game::StatusWindow::RedrawAITurns(Color::color_t color, u8 progress) const
{
    Display & display = Display::Get();
    const Sprite & ston = AGG::GetICN(H2Config::EvilInterface() ? ICN::STONBAKE : ICN::STONBACK, 0);

    // restore background
    display.Blit(ston, pos.x, pos.y);

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
