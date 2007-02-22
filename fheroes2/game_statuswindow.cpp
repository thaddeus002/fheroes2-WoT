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
#include "world.h"
#include "config.h"
#include "game_statuswindow.h"

Game::StatusWindow::StatusWindow(const Point &pt)
    : ston(AGG::GetICN(H2Config::EvilInterface() ? "STONBAKE.ICN" : "STONBACK.ICN", 0)), pos(pt, ston.w(), ston.h())
{
}

void Game::StatusWindow::Redraw(Game::focus_t focus)
{
    std::string count;

    // sprite all resource
    display.Blit(AGG::GetICN("RESSMALL.ICN", 0), pos.x + 6, pos.y + 3);

    // count castle
    Text(pos.x + 26, pos.y + 28, World::GetWorld().GetKingdom(H2Config::GetHumanColor()).GetCountCastle(), Font::SMALL, true);
    // count town
    Text(pos.x + 78, pos.y + 28, World::GetWorld().GetKingdom(H2Config::GetHumanColor()).GetCountTown(), Font::SMALL, true);
    // count gold
    Text(pos.x + 122, pos.y + 28, World::GetWorld().GetKingdom(H2Config::GetHumanColor()).resource.gold, Font::SMALL, true);
    // count wood
    Text(pos.x + 15, pos.y + 58, World::GetWorld().GetKingdom(H2Config::GetHumanColor()).resource.wood, Font::SMALL, true);
    // count mercury
    Text(pos.x + 37, pos.y + 58, World::GetWorld().GetKingdom(H2Config::GetHumanColor()).resource.mercury, Font::SMALL, true);
    // count ore
    Text(pos.x + 60, pos.y + 58, World::GetWorld().GetKingdom(H2Config::GetHumanColor()).resource.ore, Font::SMALL, true);
    // count sulfur
    Text(pos.x + 84, pos.y + 58, World::GetWorld().GetKingdom(H2Config::GetHumanColor()).resource.sulfur, Font::SMALL, true);
    // count crystal
    Text(pos.x + 108, pos.y + 58, World::GetWorld().GetKingdom(H2Config::GetHumanColor()).resource.crystal, Font::SMALL, true);
    // count gems
    Text(pos.x + 130, pos.y + 58, World::GetWorld().GetKingdom(H2Config::GetHumanColor()).resource.gems, Font::SMALL, true);
}
