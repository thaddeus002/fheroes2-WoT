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

#include <string>
#include "agg.h"
#include "animation.h"
#include "localevent.h"
#include "button.h"
#include "cursor.h"
#include "speed.h"
#include "config.h"
#include "castle.h"
#include "background.h"
#include "tools.h"
#include "text.h"

void Castle::OpenWell(void)
{
    Display & display = Display::Get();
    // cursor
    Cursor & cursor = Cursor::Get();

    cursor.Hide();

    Dialog::FrameBorder background;

    const Point cur_pt(background.GetArea().x, background.GetArea().y);

    const std::string & str_msg = "Town Population Information and Statistics";

    Point dst_pt(cur_pt);

    // text
    dst_pt.x = cur_pt.x + 280 - Text::width(str_msg, Font::BIG) / 2;
    dst_pt.y = cur_pt.y + 462;
    Text(str_msg, Font::BIG, dst_pt);

    // button exit
    dst_pt.x = cur_pt.x + 578;
    dst_pt.y = cur_pt.y + 461;
    Button buttonExit(dst_pt, ICN::WELLXTRA, 0, 1);

    const Rect rectMonster1(cur_pt.x + 20, cur_pt.y + 18, 288, 124);
    const Rect rectMonster2(cur_pt.x + 20, cur_pt.y + 168, 288, 124);
    const Rect rectMonster3(cur_pt.x + 20, cur_pt.y + 318, 288, 124);
    const Rect rectMonster4(cur_pt.x + 334, cur_pt.y + 18, 288, 124);
    const Rect rectMonster5(cur_pt.x + 334, cur_pt.y + 168, 288, 124);
    const Rect rectMonster6(cur_pt.x + 334, cur_pt.y + 318, 288, 124);
    
    buttonExit.Draw();

    WellRedrawInfoArea(cur_pt);

    cursor.Show();
    display.Flip();

    LocalEvent & le = LocalEvent::GetLocalEvent();

    // loop
    while(le.HandleEvents())
    {
        le.MousePressLeft(buttonExit) ? buttonExit.PressDraw() : buttonExit.ReleaseDraw();

        if(le.MouseClickLeft(buttonExit) || le.KeyPress(KEY_RETURN) || le.KeyPress(KEY_ESCAPE)) break;

        // extended version (click - buy dialog monster)
        if(! Settings::Get().Original())
        {
    	    if(building & DWELLING_MONSTER1 && le.MouseClickLeft(rectMonster1) &&
    		Castle::RecruitMonster(DWELLING_MONSTER1, Dialog::RecruitMonster(
            	    Monster::Monster(race, DWELLING_MONSTER1), dwelling[0])))
    	    {
        	cursor.Hide();
		WellRedrawInfoArea(cur_pt);
		cursor.Show();
        	display.Flip();
    	    }
    	    else
    	    if(building & DWELLING_MONSTER2 && le.MouseClickLeft(rectMonster2) &&
    		Castle::RecruitMonster(DWELLING_MONSTER2, Dialog::RecruitMonster(
            	    Monster::Monster(race, building & DWELLING_UPGRADE2 ? DWELLING_UPGRADE2 : DWELLING_MONSTER2), dwelling[1])))
    	    {
        	cursor.Hide();
		WellRedrawInfoArea(cur_pt);
		cursor.Show();
        	display.Flip();
    	    }
    	    else
    	    if(building & DWELLING_MONSTER3 && le.MouseClickLeft(rectMonster3) &&
    		Castle::RecruitMonster(DWELLING_MONSTER3, Dialog::RecruitMonster(
            	    Monster::Monster(race, building & DWELLING_UPGRADE3 ? DWELLING_UPGRADE3 : DWELLING_MONSTER3), dwelling[2])))
    	    {
        	cursor.Hide();
		WellRedrawInfoArea(cur_pt);
		cursor.Show();
        	display.Flip();
    	    }
    	    else
    	    if(building & DWELLING_MONSTER4 && le.MouseClickLeft(rectMonster4) &&
    		Castle::RecruitMonster(DWELLING_MONSTER4, Dialog::RecruitMonster(
            	    Monster::Monster(race, building & DWELLING_UPGRADE4 ? DWELLING_UPGRADE4 : DWELLING_MONSTER4), dwelling[3])))
    	    {
        	cursor.Hide();
		WellRedrawInfoArea(cur_pt);
		cursor.Show();
        	display.Flip();
    	    }
    	    else
    	    if(building & DWELLING_MONSTER5 && le.MouseClickLeft(rectMonster5) &&
    		Castle::RecruitMonster(DWELLING_MONSTER5, Dialog::RecruitMonster(
            	    Monster::Monster(race, building & DWELLING_UPGRADE5 ? DWELLING_UPGRADE5 : DWELLING_MONSTER5), dwelling[4])))
    	    {
        	cursor.Hide();
		WellRedrawInfoArea(cur_pt);
		cursor.Show();
        	display.Flip();
    	    }
    	    else
    	    if(building & DWELLING_MONSTER6 && le.MouseClickLeft(rectMonster6) &&
                Castle::RecruitMonster(DWELLING_MONSTER6, Dialog::RecruitMonster(
                    Monster::Monster(race, building & DWELLING_UPGRADE7 ? DWELLING_UPGRADE7 : (building & DWELLING_UPGRADE6 ? DWELLING_UPGRADE6 : DWELLING_MONSTER6)), dwelling[5])))
    	    {
        	cursor.Hide();
		WellRedrawInfoArea(cur_pt);
		cursor.Show();
        	display.Flip();
    	    }
	}        
    }
}

void Castle::WellRedrawInfoArea(const Point & cur_pt)
{
    Display & display = Display::Get();
    display.Blit(AGG::GetICN(ICN::WELLBKG, 0), cur_pt);

    u32 dw = DWELLING_MONSTER1;

    while(dw <= DWELLING_MONSTER6)
    {
	Point pt(cur_pt);
	bool present = false;
	building_t dw_orig = DWELLING_MONSTER1;
	u8 icnindex = 0;
	u16 available = 0;

	switch(dw)
	{
	    case DWELLING_MONSTER1:
		pt.x = cur_pt.x;
		pt.y = cur_pt.y;
		present = DWELLING_MONSTER1 & building;
		icnindex = 19;
		available = dwelling[0];
		break;
	    case DWELLING_MONSTER2:
		pt.x = cur_pt.x;
		pt.y = cur_pt.y + 150;
		present = DWELLING_MONSTER2 & building;
		dw_orig = DWELLING_UPGRADE2 & building ? DWELLING_UPGRADE2 : DWELLING_MONSTER2;
		icnindex = DWELLING_UPGRADE2 & building ? 25 : 20;
		available = dwelling[1];
		break;
	    case DWELLING_MONSTER3:
		pt.x = cur_pt.x;
		pt.y = cur_pt.y + 300;
		present = DWELLING_MONSTER3 & building;
		dw_orig = DWELLING_UPGRADE3 & building ? DWELLING_UPGRADE3 : DWELLING_MONSTER3;
		icnindex = DWELLING_UPGRADE3 & building ? 26 : 21;
		available = dwelling[2];
		break;
	    case DWELLING_MONSTER4:
		pt.x = cur_pt.x + 314;
		pt.y = cur_pt.y + 1;
		present = DWELLING_MONSTER4 & building;
		dw_orig = DWELLING_UPGRADE4 & building ? DWELLING_UPGRADE4 : DWELLING_MONSTER4;
		icnindex = DWELLING_UPGRADE4 & building ? 27 : 22;
		available = dwelling[3];
		break;
	    case DWELLING_MONSTER5:
		pt.x = cur_pt.x + 314;
		pt.y = cur_pt.y + 151;
		present = DWELLING_MONSTER5 & building;
		dw_orig = DWELLING_UPGRADE5 & building ? DWELLING_UPGRADE5 : DWELLING_MONSTER5;
		icnindex = DWELLING_UPGRADE5 & building ? 28 : 23;
		available = dwelling[4];
		break;
	    case DWELLING_MONSTER6:
		pt.x = cur_pt.x + 314;
		pt.y = cur_pt.y + 301;
		present = DWELLING_MONSTER6 & building;
		dw_orig = DWELLING_UPGRADE7 & building ? DWELLING_UPGRADE7 : (DWELLING_UPGRADE6 & building ? DWELLING_UPGRADE6 : DWELLING_MONSTER6);
		icnindex = DWELLING_UPGRADE7 & building ? 30 : (DWELLING_UPGRADE6 & building ? 29 : 24);
		available = dwelling[5];
		break;
	    default:
		break;
	}

	const Monster::stats_t & monster = Monster::GetStats(Monster::Monster(race, dw_orig));

	Point dst_pt;
	std::string str;
	ICN::icn_t icnname = ICN::UNKNOWN;

	switch(race)
	{
    	    case Race::BARB: icnname = ICN::CSTLBARB; break;
    	    case Race::KNGT: icnname = ICN::CSTLKNGT; break;
    	    case Race::SORC: icnname = ICN::CSTLSORC; break;
    	    case Race::WRLK: icnname = ICN::CSTLWRLK; break;
    	    case Race::WZRD: icnname = ICN::CSTLWZRD; break;
    	    case Race::NECR: icnname = ICN::CSTLNECR; break;
    	    default: break;
	}

	// sprite
	dst_pt.x = pt.x + 21;
	dst_pt.y = pt.y + 35;
	display.Blit(AGG::GetICN(icnname, icnindex), dst_pt);
	// text
	str = GetStringBuilding(dw_orig, race);
	dst_pt.x = pt.x + 86 - Text::width(str, Font::SMALL) / 2;
	dst_pt.y = pt.y + 103;
	Text(str, Font::SMALL, dst_pt);
	// monster
	const Sprite & smonster = AGG::GetICN(monster.monh_icn, 0);
	dst_pt.x = pt.x + 193 - smonster.w() / 2;
	dst_pt.y = pt.y + 124 - smonster.h();
	display.Blit(smonster, dst_pt);
	// name
	str = monster.name;
	dst_pt.x = pt.x + 122 - Text::width(monster.name, Font::SMALL) / 2;
	dst_pt.y = pt.y + 16;
	Text(str, Font::SMALL, dst_pt);
	// attack
	str = "Attack: ";
	String::AddInt(str, monster.attack);
	dst_pt.x = pt.x + 268 - Text::width(str, Font::SMALL) / 2;
	dst_pt.y = pt.y + 22;
	Text(str, Font::SMALL, dst_pt);
	// defense
	str = "Defense: ";
	String::AddInt(str, monster.defence);
	dst_pt.x = pt.x + 268 - Text::width(str, Font::SMALL) / 2;
	dst_pt.y = pt.y + 34;
	Text(str, Font::SMALL, dst_pt);
	// damage
	str = "Damg: ";
	String::AddInt(str, monster.damageMin);
	str += "-";
	String::AddInt(str, monster.damageMax);
	dst_pt.x = pt.x + 268 - Text::width(str, Font::SMALL) / 2;
	dst_pt.y = pt.y + 46;
	Text(str, Font::SMALL, dst_pt);
	// hp
	str = "HP: ";
	String::AddInt(str, monster.hp);
	dst_pt.x = pt.x + 268 - Text::width(str, Font::SMALL) / 2;
        dst_pt.y = pt.y + 58;
	Text(str, Font::SMALL, dst_pt);
        // speed
        str = "Speed:";
	dst_pt.x = pt.x + 268 - Text::width(str, Font::SMALL) / 2;
        dst_pt.y = pt.y + 78;
        Text(str, Font::SMALL, dst_pt);
	str = Speed::String(monster.speed);
        dst_pt.x = pt.x + 268 - Text::width(str, Font::SMALL) / 2;
        dst_pt.y = pt.y + 90;
	Text(str, Font::SMALL, dst_pt);
	
	if(present)
	{
	    u8 grown = monster.grown;
	    grown += building & BUILD_WELL ? GROWN_WELL : 0;
	    if(DWELLING_MONSTER1 & building) grown += building & BUILD_WEL2 ? GROWN_WEL2 : 0;

    	    str = "Growth";
	    dst_pt.x = pt.x + 268 - Text::width(str, Font::SMALL) / 2;
    	    dst_pt.y = pt.y + 110;
    	    Text(str, Font::SMALL, dst_pt);
	    str = "+ ";
	    String::AddInt(str, grown);
    	    str += " / week";
	    dst_pt.x = pt.x + 268 - Text::width(str, Font::SMALL) / 2;
    	    dst_pt.y = pt.y + 122;
    	    Text(str, Font::SMALL, dst_pt);

    	    str = "Available:";
	    dst_pt.x = pt.x + 44;
    	    dst_pt.y = pt.y + 122;
    	    Text(str, Font::SMALL, dst_pt);
	    str.clear();
	    String::AddInt(str, available);
    	    dst_pt.x = pt.x + 129 - Text::width(str, Font::BIG) / 2;
    	    dst_pt.y = pt.y + 119;
	    Text(str, Font::BIG, dst_pt);
	}
	
	dw <<= 1;
    }
}
