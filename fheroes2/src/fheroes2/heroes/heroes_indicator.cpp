/*************************************************************************** 
 *   Copyright (C) 2008 by Andrey Afletdinov                               * 
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

#include <cmath>
#include "agg.h"
#include "luck.h"
#include "morale.h"
#include "heroes.h"
#include "heroes_indicator.h"

const char* MoraleString(s8 morale)
{
    switch(morale)
    {
        case Morale::TREASON:
        case Morale::AWFUL:
        case Morale::POOR:
            return "Bad Morale";

        case Morale::NORMAL:
            return "Neutral Morale";

        case Morale::GOOD:
        case Morale::GREAT:
        case Morale::BLOOD:
    	    return "Good Morale";

        default: break;
    }
    return NULL;
}

const char* LuckString(s8 luck)
{
    switch(luck)
    {
        case Luck::CURSED:
        case Luck::AWFUL:
        case Luck::BAD:
            return "Bad Luck";

        case Luck::NORMAL:
            return "Neutral Luck";

        case Luck::GOOD:
        case Luck::GREAT:
        case Luck::IRISH:
            return "Good Luck";

        default: break;
    }
    return NULL;
}

HeroesIndicator::HeroesIndicator(const Heroes & h) : hero(h)
{
}

const Rect & HeroesIndicator::GetArea(void) const
{
    return area;
}

const std::list<std::string> & HeroesIndicator::GetLists(void) const
{
    return lists;
}

void HeroesIndicator::SetPos(const Point & pt)
{
    area = Rect(pt.x, pt.y, 35, 26);
    back.Save(area);
}

LuckIndicator::LuckIndicator(const Heroes & h) : HeroesIndicator(h)
{
}

void LuckIndicator::Redraw(void)
{
    lists.clear();
    luck = hero.GetLuckWithModificators(&lists);
    const Sprite & sprite = AGG::GetICN(ICN::HSICONS, (0 > luck ? 3 : (0 < luck ? 2 : 6)));
    const u8 inter = 6;
    u8 count = (0 == luck ? 1 : std::abs((double)luck));
    s16 cx = area.x + (area.w - (sprite.w() + inter * (count - 1))) / 2;
    s16 cy = area.y + (area.h - sprite.h()) / 2;

    if(lists.size()) lists.push_front("Current Luck Modifiers:");
    lists.push_front(" ");
    lists.push_front(Luck::Description(luck));

    back.Restore();
    while(count--)
    {
        Display::Get().Blit(sprite, cx, cy);
        cx += inter;
    }
}

void LuckIndicator::QueueEventProcessing(LuckIndicator & indicator)
{
    LocalEvent & le = LocalEvent::GetLocalEvent();

    if(le.MouseClickLeft(indicator.area)) Dialog::Message(LuckString(indicator.luck), indicator.lists, Font::BIG, Dialog::OK);
    else
    if(le.MousePressRight(indicator.area)) Dialog::Message(LuckString(indicator.luck), indicator.lists, Font::BIG);
}

MoraleIndicator::MoraleIndicator(const Heroes & h) : HeroesIndicator(h), morale(Morale::NORMAL)
{
}

void MoraleIndicator::Redraw(void)
{
    lists.clear();
    morale = hero.GetMoraleWithModificators(&lists);
    const Sprite & sprite = AGG::GetICN(ICN::HSICONS, (0 > morale ? 5 : (0 < morale ? 4 : 7)));
    const u8 inter = 6;
    u8 count = (0 == morale ? 1 : std::abs((double)morale));
    s16 cx = area.x + (area.w - (sprite.w() + inter * (count - 1))) / 2;
    s16 cy = area.y + (area.h - sprite.h()) / 2;

    if(lists.size()) lists.push_front("Current Morale Modifiers:");
    lists.push_front(" ");
    lists.push_front(Morale::Description(morale));

    back.Restore();
    while(count--)
    {
        Display::Get().Blit(sprite, cx, cy);
        cx += inter;
    }
}

void MoraleIndicator::QueueEventProcessing(MoraleIndicator & indicator)
{
    LocalEvent & le = LocalEvent::GetLocalEvent();

    if(le.MouseClickLeft(indicator.area)) Dialog::Message(MoraleString(indicator.morale), indicator.lists, Font::BIG, Dialog::OK);
    else
    if(le.MousePressRight(indicator.area)) Dialog::Message(MoraleString(indicator.morale), indicator.lists, Font::BIG);
}
