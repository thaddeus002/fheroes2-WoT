/***************************************************************************
 *   Copyright (C) 2010 by Andrey Afletdinov <fheroes2@gmail.com>          *
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

#ifndef H2AI_H
#define H2AI_H

#include "gamedefs.h"

class Castle;
class Heroes;
class Kingdom;
namespace Battle2 { class Arena; class Stats; class Actions; }

struct AI
{
    static void AddCastle(const Castle &);
    static void RemoveCastle(const Castle &);
    static void AddHeroes(const Heroes &);
    static void RemoveHeroes(const Heroes &);

    static void KingdomTurn(Kingdom &);
    static void BattleTurn(Battle2::Arena &, const Battle2::Stats &, Battle2::Actions &);
    static void BattleMagicTurn(Battle2::Arena &, const Battle2::Stats &, Battle2::Actions &, const Battle2::Stats*);
    static void JoinRNDArmy(Castle &);
    static void HeroesAction(Heroes &, s32);
};

#endif
