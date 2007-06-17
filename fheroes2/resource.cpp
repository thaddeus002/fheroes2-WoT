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
#include "error.h"
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
	case 7: return Resource::GOLD;
        default: break;
    }

    return Resource::WOOD;
}
// operator funds_t +
const Resource::funds_t Resource::funds_t::operator+ (const Resource::funds_t & pm) const
{
    Resource::funds_t res;

    res.wood = wood + pm.wood;
    res.mercury = mercury + pm.mercury;
    res.ore = ore + pm.ore;
    res.sulfur = sulfur + pm.sulfur;
    res.crystal = crystal + pm.crystal;
    res.gems = gems + pm.gems;
    res.gold = gold + pm.gold;

    return res;
}

const Resource::funds_t & Resource::funds_t::operator+= (const Resource::funds_t & pm)
{
    wood += pm.wood;
    mercury += pm.mercury;
    ore += pm.ore;
    sulfur += pm.sulfur;
    crystal += pm.crystal;
    gems += pm.gems;
    gold += pm.gold;

    return *this;
}

// operator funds_t -
const Resource::funds_t Resource::funds_t::operator- (const Resource::funds_t & pm) const
{
    Resource::funds_t res;

    res.wood = wood - pm.wood;
    res.mercury = mercury - pm.mercury;
    res.ore = ore - pm.ore;
    res.sulfur = sulfur - pm.sulfur;
    res.crystal = crystal - pm.crystal;
    res.gems = gems - pm.gems;
    res.gold = gold - pm.gold;

    return res;
}

const Resource::funds_t & Resource::funds_t::operator-= (const Resource::funds_t & pm)
{
    wood -= pm.wood;
    mercury -= pm.mercury;
    ore -= pm.ore;
    sulfur -= pm.sulfur;
    crystal -= pm.crystal;
    gems -= pm.gems;
    gold -= pm.gold;

    return *this;
}

// operator funds_t *
const Resource::funds_t Resource::funds_t::operator* (u32 mul) const
{
    Resource::funds_t res;

    res.wood = wood * mul;
    res.mercury = mercury * mul;
    res.ore = ore * mul;
    res.sulfur = sulfur * mul;
    res.crystal = crystal * mul;
    res.gems = gems * mul;
    res.gold = gold * mul;

    return res;
}

const Resource::funds_t & Resource::funds_t::operator*= (u32 mul)
{
    wood *= mul;
    mercury *= mul;
    ore *= mul;
    sulfur *= mul;
    crystal *= mul;
    gems *= mul;
    gold *= mul;

    return *this;
}

// operator funds_t <
bool Resource::funds_t::operator< (const funds_t & pm) const
{
    return      wood < pm.wood &&
                mercury < pm.mercury &&
                ore < pm.ore &&
                sulfur < pm.sulfur &&
                crystal < pm.crystal &&
                gems < pm.gems &&
                gold < pm.gold;
}

// operator funds_t <=
bool Resource::funds_t::operator<= (const funds_t & pm) const
{
    return      wood <= pm.wood &&
                mercury <= pm.mercury &&
                ore <= pm.ore &&
                sulfur <= pm.sulfur &&
                crystal <= pm.crystal &&
                gems <= pm.gems &&
                gold <= pm.gold;
}

// operator funds_t >
bool Resource::funds_t::operator> (const funds_t & pm) const
{
    return      wood > pm.wood ||
                mercury > pm.mercury ||
                ore > pm.ore ||
                sulfur > pm.sulfur ||
                crystal > pm.crystal ||
                gems > pm.gems ||
                gold > pm.gold;
}

// operator funds_t >=
bool Resource::funds_t::operator>= (const funds_t & pm) const
{
    return      wood >= pm.wood ||
                mercury >= pm.mercury ||
                ore >= pm.ore ||
                sulfur >= pm.sulfur ||
                crystal >= pm.crystal ||
                gems >= pm.gems ||
                gold >= pm.gold;
}

/* name resource */
const std::string & Resource::String(resource_t resource)
{
    static const std::string stringWood("Wood");
    static const std::string stringMercury("Mercury");
    static const std::string stringOre("Ore");
    static const std::string stringSulfur("Sulfur");
    static const std::string stringCrystal("Crystal");
    static const std::string stringGems("Gems");
    static const std::string stringGold("Gold");

    switch(resource){
        case Resource::WOOD:	return stringWood;
        case Resource::MERCURY:	return stringMercury;
        case Resource::ORE:	return stringOre;
        case Resource::SULFUR:	return stringSulfur;
        case Resource::CRYSTAL: return stringCrystal;
        case Resource::GEMS:	return stringGems;
	case Resource::GOLD:	return stringGold;
	default: break;
	    Error::Warning("Resource::String: unknown");
    }
    
    return stringWood;
}

Resource::resource_t Resource::FromMP2(u8 index)
{
    switch(index){ 
	case 1:  return Resource::WOOD;
        case 3:  return Resource::MERCURY;
        case 5:  return Resource::ORE;
        case 7:  return Resource::SULFUR;
        case 9:  return Resource::CRYSTAL;
        case 11: return Resource::GEMS;
        case 13: return Resource::GOLD;
        case 17: return Resource::Rand();
        default: 
	    Error::Warning("Resource::FromMP2: unknown: ", index);
    }
    
    return Resource::WOOD;
}

/* return index sprite objnrsrc.icn */
u8 Resource::GetIndexSprite(Resource::resource_t resource)
{
    switch(resource){
        case Resource::WOOD:	return  1;
        case Resource::MERCURY:	return  3;
        case Resource::ORE:	return  5;
        case Resource::SULFUR:	return  7;
        case Resource::CRYSTAL: return  9;
        case Resource::GEMS:	return  11;
	case Resource::GOLD:	return  13;
        default: 
	    Error::Warning("Resource::GetIndexSprite: unknown");
    }

    return 0;
}

/* return rnd count resource */
u16 Resource::RandCount(Resource::resource_t res)
{ return Resource::GOLD == res ? 100 * Rand::Get(RNDRESOURCEMIN, RNDRESOURCEMAX) : Rand::Get(RNDRESOURCEMIN, RNDRESOURCEMAX); }

void Resource::ChangeTileWithRNDResource(std::vector<Maps::Tiles *> & vector, u16 center)
{
    Maps::Tiles & tile = *vector[center];
    const Maps::TilesAddon *addon = NULL;

    if( (addon = tile.FindAddon(0xB8, 0x11)) ||
	(addon = tile.FindAddon(0xB9, 0x11)) ||
        (addon = tile.FindAddon(0xBA, 0x11)) ||
        (addon = tile.FindAddon(0xBB, 0x11)))
    {
	u32 uniq = (*addon).GetUniq();
        u8 index = Resource::GetIndexSprite(Resource::Rand());
        (*const_cast<Maps::TilesAddon *>(addon)).SetIndex(index);
        tile.SetObject(MP2::OBJ_RESOURCE);

        // replace shadow resource
	if(center)
	    if(const Maps::TilesAddon *shadow = (*vector[center - 1]).FindAddonLevel1(uniq))
		(*const_cast<Maps::TilesAddon *>(shadow)).SetIndex(index - 1);
    }
}
