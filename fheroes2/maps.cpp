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

#include "maps.h"

// operator ++difficulty_t
Maps::difficulty_t& Maps::operator++ (Maps::difficulty_t& difficulty)
{ return difficulty = ( Maps::IMPOSSIBLE == difficulty ? Maps::EASY : Maps::difficulty_t(difficulty + 1)); };

// operator --difficulty_t
Maps::difficulty_t& Maps::operator-- (Maps::difficulty_t& difficulty)
{ return difficulty = ( Maps::EASY == difficulty ? Maps::IMPOSSIBLE : Maps::difficulty_t(difficulty - 1)); };

// operator payment_t + payment_t
Maps::payment_t Maps::operator+ (const Maps::payment_t &pm1, const Maps::payment_t &pm2)
{
    payment_t pm;

    pm.wood = (MAXU16 - pm2.wood < pm1.wood ? MAXU16 : pm1.wood + pm2.wood);
    pm.mercury = (MAXU16 - pm2.mercury < pm1.mercury ? MAXU16 : pm1.mercury + pm2.mercury);
    pm.ore = (MAXU16 - pm2.ore < pm1.ore ? MAXU16 : pm1.ore + pm2.ore);
    pm.sulfur = (MAXU16 - pm2.sulfur < pm1.sulfur ? MAXU16 : pm1.sulfur + pm2.sulfur);
    pm.crystal = (MAXU16 - pm2.crystal < pm1.crystal ? MAXU16 : pm1.crystal + pm2.crystal);
    pm.gems = (MAXU16 - pm2.gems < pm1.gems ? MAXU16 : pm1.gems + pm2.gems);
    pm.gold = pm1.gold + pm2.gold;

    return pm;
}

// operator payment_t - payment_t
Maps::payment_t Maps::operator- (const Maps::payment_t &pm1, const Maps::payment_t &pm2)
{
    payment_t pm;

    pm.wood = (pm2.wood > pm1.wood ? 0 : pm1.wood - pm2.wood);
    pm.mercury = (pm2.mercury > pm1.mercury ? 0 : pm1.mercury - pm2.mercury);
    pm.ore = (pm2.ore > pm1.ore ? 0 : pm1.ore - pm2.ore);
    pm.sulfur = (pm2.sulfur > pm1.sulfur ? 0 : pm1.sulfur - pm2.sulfur);
    pm.crystal = (pm2.crystal > pm1.crystal ? 0 : pm1.crystal - pm2.crystal);
    pm.gems = (pm2.gems > pm1.gems ? 0 : pm1.gems - pm2.gems);
    pm.gold = (pm2.gold > pm1.gold ? 0 : pm1.gold - pm2.gold);

    return pm;
}
