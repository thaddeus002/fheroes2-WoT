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

namespace AI
{
    enum modes_t
    {
	HEROES_SCOUTER	= 0x10000000,
	HEROES_HUNTER	= 0x20000000,
	HEROES_WAITING	= 0x40000000,
	HEROES_STUPID	= 0x80000000
    };

    void AddCastle(const Castle &);
    void RemoveCastle(const Castle &);
    void AddHeroes(const Heroes &);
    void RemoveHeroes(const Heroes &);

    void Init(void);

    void KingdomTurn(Kingdom &);
    void BattleTurn(Battle::Arena &, const Battle::Unit &, Battle::Actions &);
    bool BattleMagicTurn(Battle::Arena &, const Battle::Unit &, Battle::Actions &, const Battle::Unit*);
    void HeroesPreBattle(HeroBase &);
    void HeroesAfterBattle(HeroBase &);
    void HeroesPostLoad(Heroes &);
    bool HeroesValidObject(const Heroes &, s32);
    void HeroesAction(Heroes &, s32);
    void HeroesActionNewPosition(Heroes &);
    void HeroesLevelUp(Heroes &);
    void HeroesClearTask(const Heroes &);
    std::string HeroesString(const Heroes &);

    void CastlePreBattle(Castle &);
    void CastleAfterBattle(Castle &, bool attacker_wins);

    const char* Type(void);
    const char* License(void);
}

#endif
