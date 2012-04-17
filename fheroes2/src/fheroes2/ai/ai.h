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
class HeroBase;
class Heroes;
class Kingdom;
namespace Battle { class Arena; class Unit; class Actions; }

struct AI
{
    static void AddCastle(const Castle &);
    static void RemoveCastle(const Castle &);
    static void AddHeroes(const Heroes &);
    static void RemoveHeroes(const Heroes &);

    static void Init(void);

    static void KingdomTurn(Kingdom &);
    static void BattleTurn(Battle::Arena &, const Battle::Unit &, Battle::Actions &);
    static bool BattleMagicTurn(Battle::Arena &, const Battle::Unit &, Battle::Actions &, const Battle::Unit*);
    static void HeroesPreBattle(HeroBase &);
    static void HeroesAfterBattle(HeroBase &);
    static void HeroesAction(Heroes &, s32);
    static void HeroesActionNewPosition(Heroes &);
    static void HeroesLevelUp(Heroes &);
    static void HeroesClearTask(void);
    static std::string HeroesString(const Heroes &);

    static void CastlePreBattle(Castle &);
    static void CastleAfterBattle(Castle &, bool attacker_wins);

    static const char* Type(void);
    static const char* License(void);
};

#endif
