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

#include "army.h"
#include "display.h"
#include "agg.h"
#include "sprite.h"

const std::string & Army::String(Army::size_t size)
{
    static const std::string str_size[] = { "Few", "Several", "Pack", "Lots", "Horde", "Throng", "Swarm", "Zounds", "Legion" };

    switch(size)
    {
	case FEW:	return str_size[0];
        case SEVERAL:	return str_size[1];
        case PACK:	return str_size[2];
        case LOTS:	return str_size[3];
        case HORDE:	return str_size[4];
        case THRONG:	return str_size[5];
        case SWARM:	return str_size[6];
        case ZOUNDS:	return str_size[7];
        case LEGION:	return str_size[8];
    }

    return str_size[0];
}

Army::size_t Army::GetSize(u16 count)
{
    if(LEGION <= count)		return LEGION;
    else
    if(ZOUNDS <= count)		return ZOUNDS;
    else
    if(SWARM <= count)		return SWARM;
    else
    if(THRONG <= count)		return THRONG;
    else
    if(HORDE <= count)		return HORDE;
    else
    if(LOTS <= count)		return LOTS;
    else
    if(PACK <= count)		return PACK;
    else
    if(SEVERAL <= count)	return SEVERAL;

    return FEW;
}

Army::Troops::Troops(const Army::Troops & troops) : monster(troops.Monster()), count(troops.Count()), master_skill(NULL), pos(0,0), saved(false)
{
}

Army::Troops & Army::Troops::operator= (const Army::Troops & troops)
{
    monster = troops.Monster();
    count = troops.Count();
    pos = troops.Position();

    return *this;
}

bool Army::Troops::PredicateIsValid(const Army::Troops & t)
{
    return Monster::UNKNOWN > t.Monster() && t.Count();
}

bool Army::Troops::isValid(void) const
{
    return Monster::UNKNOWN > monster && count;
}

bool Army::isValid(const Troops & army)
{
    return army.isValid();
}

void Army::Troops::BlitR(const Point& dst_pt, bool reflect, int frame)
{
    Display & display = Display::Get();
    if(saved) bg.Restore();
    const Sprite & sp = AGG::GetICN(Monster::GetStats(monster).file_icn, frame<0 ? aframe : frame, reflect);
    saved = true;
    bg.Save(dst_pt.x + sp.x(), dst_pt.y + sp.y(), sp.w(), sp.h());
    display.Blit(sp, dst_pt.x + sp.x(), dst_pt.y + sp.y());
}

void Army::Troops::Blit(const Point& dst_pt, bool reflect, int frame)
{
    Display & display = Display::Get();
    const Sprite & sp = AGG::GetICN(Monster::GetStats(monster).file_icn, frame<0 ? aframe : frame, reflect);
    saved = true;
    bg.Save(dst_pt.x + sp.x(), dst_pt.y + sp.y(), sp.w(), sp.h());
    display.Blit(sp, dst_pt.x + sp.x(), dst_pt.y + sp.y());
}

void Army::Troops::Animate(Monster::animstate_t as)
{
    bool ranged = Monster::GetStats(monster).miss_icn != ICN::UNKNOWN;
    u8 start, count;
    if(as != Monster::AS_NONE) {
	astate = as;
	Monster::GetAnimFrames(monster, as & Monster::AS_ATTPREP ? Monster::AS_ATTPREP : as, start, count, ranged);
	aframe = start;
    } else {
	Monster::GetAnimFrames(monster, astate & Monster::AS_ATTPREP ? Monster::AS_ATTPREP : astate, start, count, ranged);
	aframe++;
	if(aframe >= start+count) {
	    if(astate & Monster::AS_ATTPREP) 
		astate = (Monster::animstate_t)(astate & ~Monster::AS_ATTPREP);
	    else
		astate = Monster::AS_NONE;
	    Monster::GetAnimFrames(monster, astate, start, count, ranged);
	    aframe = start;
	}
    }
}
