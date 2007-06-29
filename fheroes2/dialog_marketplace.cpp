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
#include "localevent.h"
#include "button.h"
#include "world.h"
#include "cursor.h"
#include "config.h"
#include "resource.h"
#include "kingdom.h"
#include "background.h"
#include "spritecursor.h"
#include "tools.h"
#include "rect.h"
#include "text.h"
#include "marketplace.h"
#include "dialog.h"

void RedrawFromResource(const Point & pt, const Resource::funds_t & rs);
void RedrawToResource(const Point & pt, bool showcost, u8 from_resource = 0);
void GetStringTradeCosts(std::string & str, u8 rs_from, u8 rs_to);
u16 GetTradeCosts(u8 rs_from, u8 rs_to);

void Dialog::Marketplace(void)
{
    const std::string & tradpost = (H2Config::EvilInterface() ? "TRADPOSE.ICN" : "TRADPOST.ICN");
    const std::string & header = "Marketplace";
    const std::string & message_info = "Please inspect our fine wares. If you feel like offering a trade, click on the items you wish to trade with and for.";

    Cursor::Hide();

    Dialog::Box box(250, true);

    const Rect & pos_rt = box.GetArea();
    Point dst_pt(pos_rt);
    Rect dst_rt(pos_rt);

    // header
    dst_pt.x = pos_rt.x + (pos_rt.w - Text::width(header, Font::BIG)) / 2;
    dst_pt.y = pos_rt.y;
    Text(dst_pt.x, dst_pt.y, header, Font::BIG, true);

    // save background
    dst_rt.x = pos_rt.x - 5;
    dst_rt.y = pos_rt.y + 15;
    dst_rt.w = pos_rt.w + 10;
    dst_rt.h = 160;
    Background back(dst_rt);
    back.Save();


    // message info
    dst_rt.x = pos_rt.x;
    dst_rt.y = pos_rt.y + 30;
    dst_rt.w = pos_rt.w;
    dst_rt.h = 100;
    TextBox(dst_rt, message_info, Font::BIG, true);

    const Kingdom & kingdom = world.GetMyKingdom();
    const Sprite & cursor = AGG::GetICN(tradpost, 14);

    const std::string & header_from = "Your Resources";

    Resource::funds_t fundsFrom = kingdom.GetFundsResource();
    u8 resourceFrom = 0;
    const Point pt1(pos_rt.x, pos_rt.y + 190);
    const Rect rectFromWood(pt1.x, pt1.y, 34, 34);
    const Rect rectFromMercury(pt1.x + 37, pt1.y, 34, 34);
    const Rect rectFromOre(pt1.x + 74, pt1.y, 34, 34);
    const Rect rectFromSulfur(pt1.x, pt1.y + 37, 34, 34);
    const Rect rectFromCrystal(pt1.x + 37, pt1.y + 37, 34, 34);
    const Rect rectFromGems(pt1.x + 74, pt1.y + 37, 34, 34);
    const Rect rectFromGold(pt1.x + 37, pt1.y + 74, 34, 34);
    SpriteCursor cursorFrom(cursor);
    dst_pt.x = pt1.x + (108 - Text::width(header_from, Font::SMALL)) / 2;
    dst_pt.y = pt1.y - 15;
    Text(dst_pt.x, dst_pt.y, header_from, Font::SMALL, true);
    RedrawFromResource(pt1, fundsFrom);

    const std::string & header_to = "Available Trades";

    Resource::funds_t fundsTo;
    u8 resourceTo = 0;
    const Point pt2(130 + pos_rt.x, pos_rt.y + 190);
    const Rect rectToWood(pt2.x, pt2.y, 34, 34);
    const Rect rectToMercury(pt2.x + 37, pt2.y, 34, 34);
    const Rect rectToOre(pt2.x + 74, pt2.y, 34, 34);
    const Rect rectToSulfur(pt2.x, pt2.y + 37, 34, 34);
    const Rect rectToCrystal(pt2.x + 37, pt2.y + 37, 34, 34);
    const Rect rectToGems(pt2.x + 74, pt2.y + 37, 34, 34);
    const Rect rectToGold(pt2.x + 37, pt2.y + 74, 34, 34);
    SpriteCursor cursorTo(cursor);
    dst_pt.x = pt2.x + (108 - Text::width(header_to, Font::SMALL)) / 2;
    dst_pt.y = pt2.y - 15;
    Text(dst_pt.x, dst_pt.y, header_to, Font::SMALL, true);
    RedrawToResource(pt2, false);



    Button *buttonTrade = NULL;
    Button *buttonLeft = NULL;
    Button *buttonRight = NULL;

#define CleanTradeArea \
    Cursor::Hide(); \
    back.Restore(); \
    dst_rt.x = pos_rt.x; \
    dst_rt.y = pos_rt.y + 30; \
    dst_rt.w = pos_rt.w; \
    dst_rt.h = 100; \
    TextBox(dst_rt, message_info, Font::BIG, true); \
    if(buttonTrade) delete buttonTrade; \
    if(buttonLeft) delete buttonLeft; \
    if(buttonRight) delete buttonRight; \
    buttonTrade = NULL; \
    buttonLeft = NULL; \
    buttonRight = NULL; \
    display.Flip(); \
    Cursor::Show();


#define ShowTradeArea \
    if(resourceFrom == resourceTo) \
    { \
	CleanTradeArea; \
    } \
    else \
    { \
	Cursor::Hide(); \
	back.Restore(); \
	if(buttonTrade) delete buttonTrade; \
	if(buttonLeft) delete buttonLeft; \
	if(buttonRight) delete buttonRight; \
	dst_pt.x = pos_rt.x + (pos_rt.w - sprite_exit.w()) / 2; \
	dst_pt.y = pos_rt.y + 150; \
	buttonTrade = new Button(dst_pt, tradpost, 15, 16); \
	const Sprite & bar = AGG::GetICN(tradpost, 1); \
	dst_pt.x = pos_rt.x + (pos_rt.w - bar.w()) / 2 - 2; \
	dst_pt.y = pos_rt.y + 128; \
	display.Blit(bar, dst_pt); \
	dst_pt.x = pos_rt.x + 6; \
	dst_pt.y = pos_rt.y + 129; \
	buttonLeft = new Button(dst_pt, tradpost, 3, 4); \
	dst_pt.x = pos_rt.x + 215; \
	dst_pt.y = pos_rt.y + 129; \
	buttonRight = new Button(dst_pt, tradpost, 5, 6); \
	Resource::resource_t rs_from = static_cast<Resource::resource_t>(resourceFrom); \
	Resource::resource_t rs_to   = static_cast<Resource::resource_t>(resourceTo); \
	u16 exchange_rate = GetTradeCosts(resourceFrom, resourceTo); \
	std::string message; \
	if(Resource::GOLD == resourceTo) \
	{ \
	    message = "I can offer you "; \
	    String::AddInt(message, exchange_rate); \
	    message += " units of " + Resource::String(rs_to) + " for 1 unit of " + Resource::String(rs_from); \
	} \
	else \
	{ \
	    message = "I can offer you 1 unit of " + Resource::String(rs_to) + " for "; \
	    String::AddInt(message, exchange_rate); \
	    message += " units of " + Resource::String(rs_from) + "."; \
	} \
	dst_rt.x = pos_rt.x; \
	dst_rt.y = pos_rt.y + 30; \
	dst_rt.w = pos_rt.w; \
	dst_rt.h = 100; \
	TextBox(dst_rt, message, Font::BIG, true); \
	display.Flip(); \
	Cursor::Show(); \
    }
    
    // button exit
    const Sprite & sprite_exit = AGG::GetICN(tradpost, 17);
    dst_pt.x = pos_rt.x + (pos_rt.w - sprite_exit.w()) / 2;
    dst_pt.y = pos_rt.y + pos_rt.h + BUTTON_HEIGHT - sprite_exit.h();
    Button buttonExit(dst_pt, tradpost, 17, 18);

    display.Flip();
    Cursor::Show();

    LocalEvent & le = LocalEvent::GetLocalEvent();
   
    le.ResetKey();

    // message loop
    bool exit = false;

    while(!exit)
    {
        le.HandleEvents();

        if(buttonTrade) le.MousePressLeft(*buttonTrade) ? (*buttonTrade).Press() : (*buttonTrade).Release();
        if(buttonLeft) le.MousePressLeft(*buttonLeft) ? (*buttonLeft).Press() : (*buttonLeft).Release();
        if(buttonRight) le.MousePressLeft(*buttonRight) ? (*buttonRight).Press() : (*buttonRight).Release();

        le.MousePressLeft(buttonExit) ? buttonExit.Press() : buttonExit.Release();

        if(le.MouseClickLeft(buttonExit) || le.KeyPress(SDLK_RETURN) || le.KeyPress(SDLK_ESCAPE)){ exit = true; }
	
	// click from
	if(le.MouseClickLeft(rectFromWood))
	{
	    Cursor::Hide();
	    resourceFrom = Resource::WOOD;
	    cursorFrom.Move(rectFromWood.x - 2, rectFromWood.y - 2);
	    if(resourceTo)
	    {
		cursorTo.Hide();
		RedrawToResource(pt2, true, resourceFrom);
		cursorTo.Show();
		ShowTradeArea;
	    }
	    Cursor::Show();
	}
	else
	if(le.MouseClickLeft(rectFromMercury))
	{
	    Cursor::Hide();
	    resourceFrom = Resource::MERCURY;
	    cursorFrom.Move(rectFromMercury.x - 2, rectFromMercury.y - 2);
	    if(resourceTo)
	    {
		cursorTo.Hide();
		RedrawToResource(pt2, true, resourceFrom);
		cursorTo.Show();
		ShowTradeArea;
	    }
	    Cursor::Show();
	}
	else
	if(le.MouseClickLeft(rectFromOre))
	{
	    Cursor::Hide();
	    resourceFrom = Resource::ORE;
	    cursorFrom.Move(rectFromOre.x - 2, rectFromOre.y - 2);
	    if(resourceTo)
	    {
		cursorTo.Hide();
		RedrawToResource(pt2, true, resourceFrom);
		cursorTo.Show();
		ShowTradeArea;
	    }
	    Cursor::Show();
	}
	else
	if(le.MouseClickLeft(rectFromSulfur))
	{
	    Cursor::Hide();
	    resourceFrom = Resource::SULFUR;
	    cursorFrom.Move(rectFromSulfur.x - 2, rectFromSulfur.y - 2);
	    if(resourceTo)
	    {
		cursorTo.Hide();
		RedrawToResource(pt2, true, resourceFrom);
		cursorTo.Show();
		ShowTradeArea;
	    }
	    Cursor::Show();
	}
	else
	if(le.MouseClickLeft(rectFromCrystal))
	{
	    Cursor::Hide();
	    resourceFrom = Resource::CRYSTAL;
	    cursorFrom.Move(rectFromCrystal.x - 2, rectFromCrystal.y - 2);
	    if(resourceTo)
	    {
		cursorTo.Hide();
		RedrawToResource(pt2, true, resourceFrom);
		cursorTo.Show();
		ShowTradeArea;
	    }
	    Cursor::Show();
	}
	else
	if(le.MouseClickLeft(rectFromGems))
	{
	    Cursor::Hide();
	    resourceFrom = Resource::GEMS;
	    cursorFrom.Move(rectFromGems.x - 2, rectFromGems.y - 2);
	    if(resourceTo)
	    {
		cursorTo.Hide();
		RedrawToResource(pt2, true, resourceFrom);
		cursorTo.Show();
		ShowTradeArea;
	    }
	    Cursor::Show();
	}
	else
	if(le.MouseClickLeft(rectFromGold))
	{
	    Cursor::Hide();
	    resourceFrom = Resource::GOLD;
	    cursorFrom.Move(rectFromGold.x - 2, rectFromGold.y - 2);
	    if(resourceTo)
	    {
		cursorTo.Hide();
		RedrawToResource(pt2, true, resourceFrom);
		cursorTo.Show();
		ShowTradeArea;
	    }
	    Cursor::Show();
	}

	// click to
	if(le.MouseClickLeft(rectToWood))
	{
	    resourceTo = Resource::WOOD;
	    cursorTo.Move(rectToWood.x - 2, rectToWood.y - 2);
	    if(resourceFrom) ShowTradeArea;
	}
	else
	if(le.MouseClickLeft(rectToMercury))
	{
	    resourceTo = Resource::MERCURY;
	    cursorTo.Move(rectToMercury.x - 2, rectToMercury.y - 2);
	    if(resourceFrom) ShowTradeArea;
	}
	else
	if(le.MouseClickLeft(rectToOre))
	{
	    resourceTo = Resource::ORE;
	    cursorTo.Move(rectToOre.x - 2, rectToOre.y - 2);
	    if(resourceFrom) ShowTradeArea;
	}
	else
	if(le.MouseClickLeft(rectToSulfur))
	{
	    resourceTo = Resource::SULFUR;
	    cursorTo.Move(rectToSulfur.x - 2, rectToSulfur.y - 2);
	    if(resourceFrom) ShowTradeArea;
	}
	else
	if(le.MouseClickLeft(rectToCrystal))
	{
	    resourceTo = Resource::CRYSTAL;
	    cursorTo.Move(rectToCrystal.x - 2, rectToCrystal.y - 2);
	    if(resourceFrom) ShowTradeArea;
	}
	else
	if(le.MouseClickLeft(rectToGems))
	{
	    resourceTo = Resource::GEMS;
	    cursorTo.Move(rectToGems.x - 2, rectToGems.y - 2);
	    if(resourceFrom) ShowTradeArea;
	}
	else
	if(le.MouseClickLeft(rectToGold))
	{
	    resourceTo = Resource::GOLD;
	    cursorTo.Move(rectToGold.x - 2, rectToGold.y - 2);
	    if(resourceFrom) ShowTradeArea;
	}
	
	//if(buttonTrade)
	//if(buttonLeft)
	//if(buttonRight)
    }

    le.ResetKey();

    if(buttonTrade) delete buttonTrade;
    if(buttonLeft) delete buttonLeft;
    if(buttonRight) delete buttonRight;

    Cursor::Show();
}

void RedrawFromResource(const Point & pt, const Resource::funds_t & rs)
{
    const std::string & tradpost = (H2Config::EvilInterface() ? "TRADPOSE.ICN" : "TRADPOST.ICN");
    std::string str;
    Point dst_pt;

    // wood
    dst_pt.x = pt.x;
    dst_pt.y = pt.y;
    display.Blit(AGG::GetICN(tradpost, 7), dst_pt);
    str.clear();
    String::AddInt(str, rs.wood);
    dst_pt.x += (34 - Text::width(str, Font::SMALL)) / 2;
    dst_pt.y += 21;
    Text(dst_pt.x, dst_pt.y, str, Font::SMALL, true);
    
    // mercury
    dst_pt.x = pt.x + 37;
    dst_pt.y = pt.y;
    display.Blit(AGG::GetICN(tradpost, 8), dst_pt);
    str.clear();
    String::AddInt(str, rs.mercury);
    dst_pt.x += (34 - Text::width(str, Font::SMALL)) / 2;
    dst_pt.y += 21;
    Text(dst_pt.x, dst_pt.y, str, Font::SMALL, true);

    // ore
    dst_pt.x = pt.x + 74;
    dst_pt.y = pt.y;
    display.Blit(AGG::GetICN(tradpost, 9), dst_pt);
    str.clear();
    String::AddInt(str, rs.ore);
    dst_pt.x += (34 - Text::width(str, Font::SMALL)) / 2;
    dst_pt.y += 21;
    Text(dst_pt.x, dst_pt.y, str, Font::SMALL, true);

    // sulfur
    dst_pt.x = pt.x;
    dst_pt.y = pt.y + 37;
    display.Blit(AGG::GetICN(tradpost, 10), dst_pt);
    str.clear();
    String::AddInt(str, rs.sulfur);
    dst_pt.x += (34 - Text::width(str, Font::SMALL)) / 2;
    dst_pt.y += 21;
    Text(dst_pt.x, dst_pt.y, str, Font::SMALL, true);

    // crystal
    dst_pt.x = pt.x + 37;
    dst_pt.y = pt.y + 37;
    display.Blit(AGG::GetICN(tradpost, 11), dst_pt);
    str.clear();
    String::AddInt(str, rs.crystal);
    dst_pt.x += (34 - Text::width(str, Font::SMALL)) / 2;
    dst_pt.y += 21;
    Text(dst_pt.x, dst_pt.y, str, Font::SMALL, true);

    // gems
    dst_pt.x = pt.x + 74;
    dst_pt.y = pt.y + 37;
    display.Blit(AGG::GetICN(tradpost, 12), dst_pt);
    str.clear();
    String::AddInt(str, rs.gems);
    dst_pt.x += (34 - Text::width(str, Font::SMALL)) / 2;
    dst_pt.y += 21;
    Text(dst_pt.x, dst_pt.y, str, Font::SMALL, true);

    // gold
    dst_pt.x = pt.x + 37;
    dst_pt.y = pt.y + 74;
    display.Blit(AGG::GetICN(tradpost, 13), dst_pt);
    str.clear();
    String::AddInt(str, rs.gold);
    dst_pt.x += (34 - Text::width(str, Font::SMALL)) / 2;
    dst_pt.y += 21;
    Text(dst_pt.x, dst_pt.y, str, Font::SMALL, true);
}

void RedrawToResource(const Point & pt, bool showcost, u8 from_resource)
{
    const std::string & tradpost = (H2Config::EvilInterface() ? "TRADPOSE.ICN" : "TRADPOST.ICN");
    std::string str;
    Point dst_pt;

    // wood
    dst_pt.x = pt.x;
    dst_pt.y = pt.y;
    display.Blit(AGG::GetICN(tradpost, 7), dst_pt);
    if(showcost)
    {
	GetStringTradeCosts(str, from_resource, Resource::WOOD);
	dst_pt.x += (34 - Text::width(str, Font::SMALL)) / 2;
	dst_pt.y += 21;
	Text(dst_pt.x, dst_pt.y, str, Font::SMALL, true);
    }
    
    // mercury
    dst_pt.x = pt.x + 37;
    dst_pt.y = pt.y;
    display.Blit(AGG::GetICN(tradpost, 8), dst_pt);
    if(showcost)
    {
	GetStringTradeCosts(str, from_resource, Resource::MERCURY);
	dst_pt.x += (34 - Text::width(str, Font::SMALL)) / 2;
	dst_pt.y += 21;
        Text(dst_pt.x, dst_pt.y, str, Font::SMALL, true);
    }

    // ore
    dst_pt.x = pt.x + 74;
    dst_pt.y = pt.y;
    display.Blit(AGG::GetICN(tradpost, 9), dst_pt);
    if(showcost)
    {
	GetStringTradeCosts(str, from_resource, Resource::ORE);
	dst_pt.x += (34 - Text::width(str, Font::SMALL)) / 2;
	dst_pt.y += 21;
	Text(dst_pt.x, dst_pt.y, str, Font::SMALL, true);
    }

    // sulfur
    dst_pt.x = pt.x;
    dst_pt.y = pt.y + 37;
    display.Blit(AGG::GetICN(tradpost, 10), dst_pt);
    if(showcost)
    {
	GetStringTradeCosts(str, from_resource, Resource::SULFUR);
	dst_pt.x += (34 - Text::width(str, Font::SMALL)) / 2;
	dst_pt.y += 21;
	Text(dst_pt.x, dst_pt.y, str, Font::SMALL, true);
    }

    // crystal
    dst_pt.x = pt.x + 37;
    dst_pt.y = pt.y + 37;
    display.Blit(AGG::GetICN(tradpost, 11), dst_pt);
    if(showcost)
    {
	GetStringTradeCosts(str, from_resource, Resource::CRYSTAL);
	dst_pt.x += (34 - Text::width(str, Font::SMALL)) / 2;
	dst_pt.y += 21;
	Text(dst_pt.x, dst_pt.y, str, Font::SMALL, true);
    }

    // gems
    dst_pt.x = pt.x + 74;
    dst_pt.y = pt.y + 37;
    display.Blit(AGG::GetICN(tradpost, 12), dst_pt);
    if(showcost)
    {
	GetStringTradeCosts(str, from_resource, Resource::GEMS);
	dst_pt.x += (34 - Text::width(str, Font::SMALL)) / 2;
	dst_pt.y += 21;
	Text(dst_pt.x, dst_pt.y, str, Font::SMALL, true);
    }

    // gold
    dst_pt.x = pt.x + 37;
    dst_pt.y = pt.y + 74;
    display.Blit(AGG::GetICN(tradpost, 13), dst_pt);
    if(showcost)
    {
	GetStringTradeCosts(str, from_resource, Resource::GOLD);
	dst_pt.x += (34 - Text::width(str, Font::SMALL)) / 2;
	dst_pt.y += 21;
	Text(dst_pt.x, dst_pt.y, str, Font::SMALL, true);
    }
}

void GetStringTradeCosts(std::string & str, u8 rs_from, u8 rs_to)
{
    if(str.size()) str.clear();

    if(rs_from == rs_to)
    {
	str = "n/a";
	
	return;
    }

    if(Resource::GOLD != rs_from && Resource::GOLD != rs_to) str = "1/";

    String::AddInt(str, GetTradeCosts(rs_from, rs_to));
}

u16 GetTradeCosts(u8 rs_from, u8 rs_to)
{
    u8 markets = world.GetMyKingdom().GetCountMarketplace();

    if(rs_from == rs_to) return 0;

    switch(rs_from)
    {
	// uncostly
	case Resource::WOOD:
	case Resource::ORE:

    	    switch(rs_to)
    	    {
    		// sale uncostly
    		case Resource::GOLD:
    		    if(1 == markets) return SALE_UNCOSTLY1;
        	    else
        	    if(2 == markets) return SALE_UNCOSTLY2;
        	    else
        	    if(3 == markets) return SALE_UNCOSTLY3;
        	    else
        	    if(4 == markets) return SALE_UNCOSTLY4;
        	    else
        	    if(5 == markets) return SALE_UNCOSTLY5;
        	    else
        	    if(6 == markets) return SALE_UNCOSTLY6;
        	    else
        	    if(7 == markets) return SALE_UNCOSTLY7;
        	    else
        	    if(8 == markets) return SALE_UNCOSTLY8;
        	    else
        	    if(8 <  markets) return SALE_UNCOSTLY9;
    		    break;

		// change uncostly to costly
		case Resource::MERCURY:
		case Resource::SULFUR:
		case Resource::CRYSTAL:
		case Resource::GEMS:
    		    if(1 == markets) return UNCOSTLY_COSTLY1;
        	    else
        	    if(2 == markets) return UNCOSTLY_COSTLY2;
        	    else
        	    if(3 == markets) return UNCOSTLY_COSTLY3;
        	    else
        	    if(4 == markets) return UNCOSTLY_COSTLY4;
        	    else
        	    if(5 == markets) return UNCOSTLY_COSTLY5;
        	    else
        	    if(6 == markets) return UNCOSTLY_COSTLY6;
        	    else
        	    if(7 == markets) return UNCOSTLY_COSTLY7;
        	    else
        	    if(8 == markets) return UNCOSTLY_COSTLY8;
        	    else
        	    if(8 <  markets) return UNCOSTLY_COSTLY9;
    		    break;

		// change uncostly to uncostly
		case Resource::WOOD:
		case Resource::ORE:
    		    if(1 == markets) return COSTLY_COSTLY1;
        	    else
        	    if(2 == markets) return COSTLY_COSTLY2;
        	    else
        	    if(3 == markets) return COSTLY_COSTLY3;
        	    else
        	    if(4 == markets) return COSTLY_COSTLY4;
        	    else
        	    if(5 == markets) return COSTLY_COSTLY5;
        	    else
        	    if(6 == markets) return COSTLY_COSTLY6;
        	    else
        	    if(7 == markets) return COSTLY_COSTLY7;
        	    else
        	    if(8 == markets) return COSTLY_COSTLY8;
        	    else
        	    if(8 <  markets) return COSTLY_COSTLY9;
        	    break;
    	    }
	    break;

	// costly
	case Resource::MERCURY:
	case Resource::SULFUR:
	case Resource::CRYSTAL:
	case Resource::GEMS:
    	    
    	    switch(rs_to)
    	    {
    		// sale costly
    		case Resource::GOLD:
    		    if(1 == markets) return SALE_COSTLY1;
        	    else
        	    if(2 == markets) return SALE_COSTLY2;
        	    else
        	    if(3 == markets) return SALE_COSTLY3;
        	    else
        	    if(4 == markets) return SALE_COSTLY4;
        	    else
        	    if(5 == markets) return SALE_COSTLY5;
        	    else
        	    if(6 == markets) return SALE_COSTLY6;
        	    else
        	    if(7 == markets) return SALE_COSTLY7;
        	    else
        	    if(8 == markets) return SALE_COSTLY8;
        	    else
        	    if(8 <  markets) return SALE_COSTLY9;
        	    break;
        	
		// change costly to costly
		case Resource::MERCURY:
		case Resource::SULFUR:
		case Resource::CRYSTAL:
		case Resource::GEMS:
    		    if(1 == markets) return COSTLY_COSTLY1;
        	    else
        	    if(2 == markets) return COSTLY_COSTLY2;
        	    else
        	    if(3 == markets) return COSTLY_COSTLY3;
        	    else
        	    if(4 == markets) return COSTLY_COSTLY4;
        	    else
        	    if(5 == markets) return COSTLY_COSTLY5;
        	    else
        	    if(6 == markets) return COSTLY_COSTLY6;
        	    else
        	    if(7 == markets) return COSTLY_COSTLY7;
        	    else
        	    if(8 == markets) return COSTLY_COSTLY8;
        	    else
        	    if(8 <  markets) return COSTLY_COSTLY9;
        	    break;

		// change costly to uncostly
		case Resource::WOOD:
		case Resource::ORE:
    		    if(1 == markets) return COSTLY_UNCOSTLY1;
        	    else
        	    if(2 == markets) return COSTLY_UNCOSTLY2;
        	    else
        	    if(3 == markets) return COSTLY_UNCOSTLY3;
        	    else
        	    if(4 == markets) return COSTLY_UNCOSTLY4;
        	    else
        	    if(5 == markets) return COSTLY_UNCOSTLY5;
        	    else
        	    if(6 == markets) return COSTLY_UNCOSTLY6;
        	    else
        	    if(7 == markets) return COSTLY_UNCOSTLY7;
        	    else
        	    if(8 == markets) return COSTLY_UNCOSTLY8;
        	    else
        	    if(8 <  markets) return COSTLY_UNCOSTLY9;
        	    break;
    	    }
	    break;

	// gold
	case Resource::GOLD:
    	    
    	    switch(rs_to)
    	    {
    		default: break;
        	
		// buy costly
		case Resource::MERCURY:
		case Resource::SULFUR:
		case Resource::CRYSTAL:
		case Resource::GEMS:
    		    if(1 == markets) return BUY_COSTLY1;
        	    else
        	    if(2 == markets) return BUY_COSTLY2;
        	    else
        	    if(3 == markets) return BUY_COSTLY3;
        	    else
        	    if(4 == markets) return BUY_COSTLY4;
        	    else
        	    if(5 == markets) return BUY_COSTLY5;
        	    else
        	    if(6 == markets) return BUY_COSTLY6;
        	    else
        	    if(7 == markets) return BUY_COSTLY7;
        	    else
        	    if(8 == markets) return BUY_COSTLY8;
        	    else
        	    if(8 <  markets) return BUY_COSTLY9;
        	    break;

		// buy uncostly
		case Resource::WOOD:
		case Resource::ORE:
    		    if(1 == markets) return BUY_UNCOSTLY1;
        	    else
        	    if(2 == markets) return BUY_UNCOSTLY2;
        	    else
        	    if(3 == markets) return BUY_UNCOSTLY3;
        	    else
        	    if(4 == markets) return BUY_UNCOSTLY4;
        	    else
        	    if(5 == markets) return BUY_UNCOSTLY5;
        	    else
        	    if(6 == markets) return BUY_UNCOSTLY6;
        	    else
        	    if(7 == markets) return BUY_UNCOSTLY7;
        	    else
        	    if(8 == markets) return BUY_UNCOSTLY8;
        	    else
        	    if(8 <  markets) return BUY_UNCOSTLY9;
        	    break;
    	    }
	    break;

	// not select
	default:  break;
    }

    return 0;
}

