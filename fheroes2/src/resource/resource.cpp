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
#include "mp2.h"
#include "world.h"
#include "agg.h"
#include "sprite.h"
#include "tools.h"
#include "display.h"
#include "resource.h"

Resource::funds_t::funds_t(const resource_t rs, u32 count) : wood(0), mercury(0), ore(0), sulfur(0), crystal(0), gems(0), gold(0)
{
    switch(rs)
    {
	case ORE:	ore = count;	break;
	case WOOD:	wood = count;	break;
	case MERCURY:	mercury = count;break;
	case SULFUR:	sulfur = count;	break;
	case GEMS:	gems = count;	break;
	case CRYSTAL:	crystal = count;break;
	case GOLD:	gold = count;	break;

	default:	Error::Warning("Resource::funds_t::funds_t: unknown resource, ", rs); break;
    }
}

Resource::funds_t::funds_t(const u8 obj) : wood(0), mercury(0), ore(0), sulfur(0), crystal(0), gems(0), gold(0)
{
    u8 count = 0;

    switch(obj)
    {
	case MP2::OBJ_CAMPFIRE:
	    count = Rand::Get(4, 6);
	    switch(Rand::Get(1, 6))
	    {
    		case 1: wood = count; break;
    		case 2: mercury = count; break;
    		case 3: ore = count; break;
    		case 4: sulfur = count; break;
    		case 5: crystal = count; break;
    		case 6: gems = count; break;
    		default: break;
	    }
	    gold = count * 100;
	    break;

	case MP2::OBJ_LEANTO:
	    count = Rand::Get(1, 4);
	    switch(Rand::Get(1, 6))
	    {
    		case 1: wood = count; break;
    		case 2: mercury = count; break;
    		case 3: ore = count; break;
    		case 4: sulfur = count; break;
    		case 5: crystal = count; break;
    		case 6: gems = count; break;
    		default: break;
	    }
	    break;

	default: break;
    }
}

Resource::resource_t Resource::Rand(void)
{
    switch(Rand::Get(1, 7))
    {
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

/* return index sprite resource.icn */
u8 Resource::GetIndexSprite2(Resource::resource_t resource)
{
    switch(resource){
        case Resource::WOOD:	return  0;
        case Resource::MERCURY:	return  1;
        case Resource::ORE:	return  2;
        case Resource::SULFUR:	return  3;
        case Resource::CRYSTAL: return  4;
        case Resource::GEMS:	return  5;
	case Resource::GOLD:	return  6;
        default: 
	    Error::Warning("Resource::GetIndexSprite2: unknown");
    }

    return 0;
}

u8 Resource::funds_t::GetValidItems(void) const
{
	u8 result = 0;

	if(wood) ++result;
	if(ore) ++result;
	if(mercury) ++result;
	if(sulfur) ++result;
	if(crystal) ++result;
	if(gems) ++result;
	if(gold) ++result;

	return result;
}

/* return rnd count resource */
u16 Resource::RandCount(Resource::resource_t res)
{ return Resource::GOLD == res ? 100 * Rand::Get(RNDRESOURCEMIN, RNDRESOURCEMAX) : Rand::Get(RNDRESOURCEMIN, RNDRESOURCEMAX); }

void Resource::ChangeTileWithRNDResource(Maps::Tiles & tile)
{
    Maps::TilesAddon *addon = tile.FindRNDResource();

    if(addon)
    {
	const u16 center = tile.GetIndex();
       	const u32 uniq = addon->uniq;
        const u8 index = Resource::GetIndexSprite(Resource::Rand());

        addon->index = index;
        tile.SetObject(MP2::OBJ_RESOURCE);

        // replace shadow artifact
        if(Maps::isValidDirection(center, Direction::LEFT))
        {
            Maps::Tiles & left_tile = world.GetTiles(Maps::GetDirectionIndex(center, Direction::LEFT));
            Maps::TilesAddon *shadow = left_tile.FindAddonLevel1(uniq);

            if(shadow) shadow->index = index - 1;
        }
    }
}

void Resource::AlignDraw(const funds_t & rs, const Rect & dst_rt)
{
    const u8 valid_resource = rs.GetValidItems();

    u16 index = 2 < valid_resource ? dst_rt.w / 3 : dst_rt.w / valid_resource;

    u8 count = 0;
    u8 offset = 50;

    std::string str;
    Point dst_pt;

    Display & display = Display::Get();

    if(rs.wood)
    {
	const Sprite & sprite = AGG::GetICN(ICN::RESOURCE, 0);
	dst_pt.x = dst_rt.x + index / 2 + count * index - sprite.w() / 2;
	dst_pt.y = dst_rt.y - sprite.h() + offset;
	display.Blit(sprite, dst_pt);

	str.clear();
	String::AddInt(str, rs.wood);
	dst_pt.x = dst_rt.x + index / 2 + count * index - Text::width(str, Font::SMALL) / 2;
	dst_pt.y = dst_rt.y + 2 + offset;
	Text(str, Font::SMALL, dst_pt);

	++count;
    }

    if(rs.ore)
    {
	const Sprite & sprite = AGG::GetICN(ICN::RESOURCE, 2);
	dst_pt.x = dst_rt.x + index / 2 + count * index - sprite.w() / 2;
	dst_pt.y = dst_rt.y - sprite.h() + offset;
	display.Blit(sprite, dst_pt);

	str.clear();
	String::AddInt(str, rs.ore);
	dst_pt.x = dst_rt.x + index / 2 + count * index - Text::width(str, Font::SMALL) / 2;
	dst_pt.y = dst_rt.y + 2 + offset;
	Text(str, Font::SMALL, dst_pt);

	++count;
    }

    if(rs.mercury)
    {
	const Sprite & sprite = AGG::GetICN(ICN::RESOURCE, 1);
	dst_pt.x = dst_rt.x + index / 2 + count * index - sprite.w() / 2;
	dst_pt.y = dst_rt.y - sprite.h() + offset;
	display.Blit(sprite, dst_pt);

	str.clear();
	String::AddInt(str, rs.mercury);
	dst_pt.x = dst_rt.x + index / 2 + count * index - Text::width(str, Font::SMALL) / 2;
	dst_pt.y = dst_rt.y + 2 + offset;
	Text(str, Font::SMALL, dst_pt);

	++count;
    }

    if(2 < count){ count = 0; offset += 50; }

    if(rs.sulfur)
    {
	const Sprite & sprite = AGG::GetICN(ICN::RESOURCE, 3);
	dst_pt.x = dst_rt.x + index / 2 + count * index - sprite.w() / 2;
	dst_pt.y = dst_rt.y - sprite.h() + offset;
	display.Blit(sprite, dst_pt);

	str.clear();
	String::AddInt(str, rs.sulfur);
	dst_pt.x = dst_rt.x + index / 2 + count * index - Text::width(str, Font::SMALL) / 2;
	dst_pt.y = dst_rt.y + 2 + offset;
	Text(str, Font::SMALL, dst_pt);

	++count;
    }

    if(2 < count){ count = 0; offset += 50; }
    if(rs.crystal)
    {
	const Sprite & sprite = AGG::GetICN(ICN::RESOURCE, 4);
	dst_pt.x = dst_rt.x + index / 2 + count * index - sprite.w() / 2;
	dst_pt.y = dst_rt.y - sprite.h()  + offset;
	display.Blit(sprite, dst_pt);

	str.clear();
	String::AddInt(str, rs.crystal);
	dst_pt.x = dst_rt.x + index / 2 + count * index - Text::width(str, Font::SMALL) / 2;
	dst_pt.y = dst_rt.y + 2 + offset;
	Text(str, Font::SMALL, dst_pt);

	++count;
    }

    if(2 < count){ count = 0; offset += 50; }
    if(rs.gems)
    {
	const Sprite & sprite = AGG::GetICN(ICN::RESOURCE, 5);
	dst_pt.x = dst_rt.x + index / 2 + count * index - sprite.w() / 2;
	dst_pt.y = dst_rt.y - sprite.h() + offset;
	display.Blit(sprite, dst_pt);

	str.clear();
	String::AddInt(str, rs.gems);
	dst_pt.x = dst_rt.x + index / 2 + count * index - Text::width(str, Font::SMALL) / 2;
	dst_pt.y = dst_rt.y + 2 + offset;
	Text(str, Font::SMALL, dst_pt);

	++count;
    }

    if(2 < count){ count = 0; offset += 50; }
    if(rs.gold)
    {
	const Sprite & sprite = AGG::GetICN(ICN::RESOURCE, 6);
	if(! count) index = dst_rt.w;
	dst_pt.x = dst_rt.x + index / 2 + count * index - sprite.w() / 2;
	dst_pt.y = dst_rt.y - sprite.h() + offset;
	display.Blit(sprite, dst_pt);

	str.clear();
	String::AddInt(str, rs.gold);
	dst_pt.x = dst_rt.x + index / 2 + count * index - Text::width(str, Font::SMALL) / 2;
	dst_pt.y = dst_rt.y + 2 + offset;
	Text(str, Font::SMALL, dst_pt);
    }
}
