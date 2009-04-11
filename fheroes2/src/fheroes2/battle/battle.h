/***************************************************************************
 *   Copyright (C) 2008 by Vasily Makarov <drmoriarty@rambler.ru>          *
 *   Copyright (C) 2008 by Josh Matthews  <josh@joshmatthews.net>          *
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

#ifndef BATTLE_H
#define BATTLE_H

#include <vector>
#include "types.h"
#include "icn.h"
#include "rect.h"
#include "bitmodes.h"
#include "race.h"
#include "battle_troop.h"
#include "statusbar.h"
#include "heroes.h"

namespace Dialog { class FrameBorder; }

class StatusBar;

class Heroes;
class Castle;
namespace Maps { class Tiles; };

namespace Army
{    
    typedef enum { WIN, LOSE, RETREAT, SURRENDER, NONE } battle_t;
    
    class army_t;
    class BattleTroop;

    Army::battle_t Battle(Heroes& hero1, Heroes& hero2, const Maps::Tiles & tile, u32 &);
    Army::battle_t Battle(Heroes& hero, Army::army_t & army, const Maps::Tiles & tile, u32 &);
    Army::battle_t Battle(Heroes& hero, Castle& castle, const Maps::Tiles &tile, u32 &);

    typedef std::vector<std::pair<const Army::BattleArmy_t *, const Army::BattleArmy_t *> > ArmyPairs;
    
    void ArmyToBattleArmy(const army_t &, BattleArmy_t &);
    void BattleArmyToArmy(const BattleArmy_t &, army_t &);
}
#endif
