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
#include "config.h"
#include "game_statuswindow.h"

Game::StatusWindow::StatusWindow(const Point &pt, const Kingdom &my)
    : ston(AGG::GetICN(H2Config::EvilInterface() ? "STONBAKE.ICN" : "STONBACK.ICN", 0)), pos(pt, ston.w(), ston.h()), myKingdom(my)
{
}

void Game::StatusWindow::Redraw(Game::focus_t focus)
{
    std::string count;

    // sprite all resource
    display.Blit(AGG::GetICN("RESSMALL.ICN", 0), pos.x + 6, pos.y + 3);

    //const Kingdom &myKingdom = world.GetMyKingdom();
    // count castle
    String::AddInt(count, myKingdom.GetCountCastle());
    Text(pos.x + 26 - Text::width(count, count[0], count.size(), Font::SMALL) / 2, pos.y + 28, count, Font::SMALL, true);
    // count town
    count.clear();
    String::AddInt(count, myKingdom.GetCountTown());
    Text(pos.x + 78 - Text::width(count, count[0], count.size(), Font::SMALL) / 2, pos.y + 28, count, Font::SMALL, true);
    // count gold
    count.clear();
    String::AddInt(count, myKingdom.resource.gold);
    Text(pos.x + 122 - Text::width(count, count[0], count.size(), Font::SMALL) / 2, pos.y + 28, count, Font::SMALL, true);
    // count wood
    count.clear();
    String::AddInt(count, myKingdom.resource.wood);
    Text(pos.x + 15 - Text::width(count, count[0], count.size(), Font::SMALL) / 2, pos.y + 58, count, Font::SMALL, true);
    // count mercury
    count.clear();
    String::AddInt(count, myKingdom.resource.mercury);
    Text(pos.x + 37 - Text::width(count, count[0], count.size(), Font::SMALL) / 2, pos.y + 58, count, Font::SMALL, true);
    // count ore
    count.clear();
    String::AddInt(count, myKingdom.resource.ore);
    Text(pos.x + 60 - Text::width(count, count[0], count.size(), Font::SMALL) / 2, pos.y + 58, count, Font::SMALL, true);
    // count sulfur
    count.clear();
    String::AddInt(count, myKingdom.resource.sulfur);
    Text(pos.x + 84 - Text::width(count, count[0], count.size(), Font::SMALL) / 2, pos.y + 58, count, Font::SMALL, true);
    // count crystal
    count.clear();
    String::AddInt(count, myKingdom.resource.crystal);
    Text(pos.x + 108 - Text::width(count, count[0], count.size(), Font::SMALL) / 2, pos.y + 58, count, Font::SMALL, true);
    // count gems
    count.clear();
    String::AddInt(count, myKingdom.resource.gems);
    Text(pos.x + 130 - Text::width(count, count[0], count.size(), Font::SMALL) / 2, pos.y + 58, count, Font::SMALL, true);
}
