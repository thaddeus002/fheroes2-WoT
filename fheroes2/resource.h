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
#ifndef H2RESOURCE_H
#define H2RESOURCE_H

#include <string>
#include <vector>
#include "maps_tiles.h"
#include "gamedefs.h"

#define RNDRESOURCEMAX  15 
#define RNDRESOURCEMIN  5 

namespace Resource
{
    typedef enum {
        WOOD	= 0x01,
        MERCURY	= 0x02,
        ORE	= 0x04,
        SULFUR	= 0x08,
        CRYSTAL	= 0x10,
        GEMS	= 0x20,
        GOLD	= 0x40,
    } resource_t;

    typedef enum {
	TREASURECHEST,
	ANCIENTLAMP,
	CAMPFIRE,
	SHIPWRECKSURVIROR,
	FLOTSAM,
	BOTTLE,
    } other_t;

    class funds_t
    {
    public:
	funds_t() : wood(0), mercury(0), ore(0), sulfur(0), crystal(0), gems(0), gold(0) {};
	funds_t(const resource_t & rs, u32 count);

	const funds_t operator+ (const funds_t &pm) const;
	const funds_t operator* (u32 mul) const;
	const funds_t operator- (const funds_t &pm) const;
	const funds_t & operator+= (const funds_t &pm);
	const funds_t & operator*= (u32 mul);
	const funds_t & operator-= (const funds_t &pm);

	bool operator< (const funds_t &pm) const;
	bool operator<= (const funds_t &pm) const;
	bool operator> (const funds_t &pm) const;
	bool operator>= (const funds_t &pm) const;

	u8 GetValidItems(void);

        u32 wood;
        u32 mercury;
        u32 ore;
        u32 sulfur;
        u32 crystal;
        u32 gems;
        u32 gold;
    };

    const std::string & String(resource_t resource);

    resource_t Rand(void);
    resource_t FromMP2(u8 index);
    u16	RandCount(Resource::resource_t res);
    
    /* return index sprite objnrsrc.icn */
    u8 GetIndexSprite(resource_t resource);

    /* return index sprite resource.icn */
    u8 GetIndexSprite2(resource_t resource);

    void ChangeTileWithRNDResource(std::vector<Maps::Tiles *> & vector, u16 center);

};

#endif
