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

#include "agg.h"
#include "text.h"
#include "tools.h"
#include "kingdom.h"
#include "config.h"
#include "game_statuswindow.h"

Game::StatusWindow::StatusWindow(const Point &pt, const World &wd)
    : ston(AGG::GetICN(H2Config::EvilInterface() ? "STONBAKE.ICN" : "STONBACK.ICN", 0)), 
      pos(pt, ston.w(), ston.h()), world(wd), state(Game::StatusWindow::DAY)
{}

void Game::StatusWindow::Redraw(Game::focus_t focus)
{
    switch(state){
    
	case DAY:
	    DrawDayInfo();
	    break;
	
	case FUNDS:
	    DrawKingdomInfo();
	    break;
	
	case ARMY:
	    DrawArmyInfo();
	    break;
    }
}

void Game::StatusWindow::NextState(void)
{
    if(DAY == state) state = FUNDS;
    else
    if(FUNDS == state) state = ARMY;
    else
    if(ARMY == state) state = DAY;
}

void Game::StatusWindow::NewDay(void)
{
    state = DAY;
}

void Game::StatusWindow::DrawKingdomInfo(void)
{
    std::string count;

    // restore background
    display.Blit(ston, pos);

    // sprite all resource
    display.Blit(AGG::GetICN("RESSMALL.ICN", 0), pos.x + 6, pos.y + 3);

    const Kingdom &myKingdom = world.GetMyKingdom();

    // count castle
    String::AddInt(count, myKingdom.GetCountCastle());
    Text(pos.x + 26 - Text::width(count, Font::SMALL) / 2, pos.y + 28, count, Font::SMALL, true);
    // count town
    count.clear();
    String::AddInt(count, myKingdom.GetCountTown());
    Text(pos.x + 78 - Text::width(count, Font::SMALL) / 2, pos.y + 28, count, Font::SMALL, true);
    // count gold
    count.clear();
    String::AddInt(count, myKingdom.resource.gold);
    Text(pos.x + 122 - Text::width(count, Font::SMALL) / 2, pos.y + 28, count, Font::SMALL, true);
    // count wood
    count.clear();
    String::AddInt(count, myKingdom.resource.wood);
    Text(pos.x + 15 - Text::width(count, Font::SMALL) / 2, pos.y + 58, count, Font::SMALL, true);
    // count mercury
    count.clear();
    String::AddInt(count, myKingdom.resource.mercury);
    Text(pos.x + 37 - Text::width(count, Font::SMALL) / 2, pos.y + 58, count, Font::SMALL, true);
    // count ore
    count.clear();
    String::AddInt(count, myKingdom.resource.ore);
    Text(pos.x + 60 - Text::width(count, Font::SMALL) / 2, pos.y + 58, count, Font::SMALL, true);
    // count sulfur
    count.clear();
    String::AddInt(count, myKingdom.resource.sulfur);
    Text(pos.x + 84 - Text::width(count, Font::SMALL) / 2, pos.y + 58, count, Font::SMALL, true);
    // count crystal
    count.clear();
    String::AddInt(count, myKingdom.resource.crystal);
    Text(pos.x + 108 - Text::width(count, Font::SMALL) / 2, pos.y + 58, count, Font::SMALL, true);
    // count gems
    count.clear();
    String::AddInt(count, myKingdom.resource.gems);
    Text(pos.x + 130 - Text::width(count, Font::SMALL) / 2, pos.y + 58, count, Font::SMALL, true);
}

void Game::StatusWindow::DrawDayInfo(void)
{
    std::string message;

    // restore background
    display.Blit(ston, pos);

    display.Blit(AGG::GetICN(H2Config::EvilInterface() ? "SUNMOONE.ICN" : "SUNMOON.ICN", (world.GetWeek() - 1) % 5), pos.x, pos.y + 1);

    message = "Month: ";
    String::AddInt(message, world.GetMonth());

    message += " Week: ";
    String::AddInt(message, world.GetWeek());
    Text(pos.x + (pos.w - Text::width(message, Font::SMALL)) / 2, pos.y + 30, message, Font::SMALL, true);

    message = "Day: ";
    String::AddInt(message, world.GetDay());
    Text(pos.x + (pos.w - Text::width(message, Font::BIG)) / 2, pos.y + 46, message, Font::BIG, true);
}

void Game::StatusWindow::DrawArmyInfo(void)
{
    // restore background
    display.Blit(ston, pos);
}

