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

#include "rand.h"
#include "resource.h"

Resource::resource_t Resource::Rand(void)
{
    switch(Rand::Get(1, 7)){
        case 1: return Resource::WOOD;
        case 2: return Resource::MERCURY;
        case 3: return Resource::ORE;
        case 4: return Resource::SULFUR;
        case 5: return Resource::CRYSTAL;
        case 6: return Resource::GEMS;
        default: break;
    }

    return Resource::GOLD;
}

// operator funds_t + funds_t
Resource::funds_t Resource::operator+ (const Resource::funds_t &pm1, const Resource::funds_t &pm2)
{
    funds_t pm;
    
    pm.wood = (MAXU16 - pm2.wood < pm1.wood ? MAXU16 : pm1.wood + pm2.wood);
    pm.mercury = (MAXU16 - pm2.mercury < pm1.mercury ? MAXU16 : pm1.mercury + pm2.mercury);
    pm.ore = (MAXU16 - pm2.ore < pm1.ore ? MAXU16 : pm1.ore + pm2.ore);
    pm.sulfur = (MAXU16 - pm2.sulfur < pm1.sulfur ? MAXU16 : pm1.sulfur + pm2.sulfur);
    pm.crystal = (MAXU16 - pm2.crystal < pm1.crystal ? MAXU16 : pm1.crystal + pm2.crystal);
    pm.gems = (MAXU16 - pm2.gems < pm1.gems ? MAXU16 : pm1.gems + pm2.gems);
    pm.gold = pm1.gold + pm2.gold;
				
    return pm;
}

// operator funds_t - funds_t
Resource::funds_t Resource::operator- (const Resource::funds_t &fd1, const Resource::funds_t &fd2)
{
    funds_t fd;

    fd.wood = (fd2.wood > fd1.wood ? 0 : fd1.wood - fd2.wood);
    fd.mercury = (fd2.mercury > fd1.mercury ? 0 : fd1.mercury - fd2.mercury);
    fd.ore = (fd2.ore > fd1.ore ? 0 : fd1.ore - fd2.ore);
    fd.sulfur = (fd2.sulfur > fd1.sulfur ? 0 : fd1.sulfur - fd2.sulfur);
    fd.crystal = (fd2.crystal > fd1.crystal ? 0 : fd1.crystal - fd2.crystal);
    fd.gems = (fd2.gems > fd1.gems ? 0 : fd1.gems - fd2.gems);
    fd.gold = (fd2.gold > fd1.gold ? 0 : fd1.gold - fd2.gold);
	    
    return fd;
}
