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

#include "error.h"
#include "castle.h"
#include "heroes.h"
#include "game_focus.h"

Game::Focus::Focus() : castle(NULL), heroes(NULL)
{
}

Game::Focus & Game::Focus::Get(void)
{
    static Game::Focus gfocus;
    
    return gfocus;
}

void Game::Focus::Set(const Heroes & hr)
{
    heroes = &hr;
    castle = NULL;

    center = hr.GetCenter();
}

void Game::Focus::Set(const Castle & cs)
{
    castle = & cs;
    heroes = NULL;

    center = cs.GetCenter();
}

Game::Focus::focus_t Game::Focus::Type(void) const
{
    if(heroes) return HEROES;
    else
    if(castle) return CASTLE;

    return UNSEL;
}

const Castle & Game::Focus::GetCastle(void) const
{
    if(NULL == castle) Error::Warning("Game::Focus::GetCastle: is NULL");

    return *castle;
}

Castle & Game::Focus::GetCastle(void)
{
    if(NULL == castle) Error::Warning("Game::Focus::GetCastle: is NULL");

    return *const_cast<Castle *>(castle);
}

const Heroes & Game::Focus::GetHeroes(void) const
{
    if(NULL == heroes) Error::Warning("Game::Focus::GetHeroes: is NULL");

    return *heroes;
}

Heroes & Game::Focus::GetHeroes(void)
{
    if(NULL == heroes) Error::Warning("Game::Focus::GetHeroes: is NULL");

    return *const_cast<Heroes *>(heroes);
}

const Point & Game::Focus::Center(void) const
{
    return center;
}

void Game::Focus::Update(void)
{
    if(heroes) center = heroes->GetCenter();
    else
    if(castle) center = castle->GetCenter();
}
