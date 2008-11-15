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
#include <vector>
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
#include "splitter.h"
#include "tools.h"
#include "rect.h"
#include "text.h"
#include "marketplace.h"
#include "dialog.h"
#include "error.h"

void RedrawFromResource(const Point & pt, const Resource::funds_t & rs);
void RedrawToResource(const Point & pt, bool showcost, bool tradingPost, u8 from_resource = 0);
void GetStringTradeCosts(std::string & str, u8 rs_from, u8 rs_to, bool tradingPost);
u16 GetTradeCosts(u8 rs_from, u8 rs_to, bool tradingPost);

void Dialog::Marketplace(bool fromTradingPost)
{
    Display & display = Display::Get();
    const ICN::icn_t tradpost = H2Config::EvilInterface() ? ICN::TRADPOSE : ICN::TRADPOST;
    const std::string & header = "Marketplace";
    const std::string & message_info = "Please inspect our fine wares. If you feel like offering a trade, click on the items you wish to trade with and for.";

    Cursor & cursor = Cursor::Get();
    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);

    Dialog::Box box(250, true);

    const Rect & pos_rt = box.GetArea();
    Point dst_pt(pos_rt.x, pos_rt.y);
    Rect dst_rt(pos_rt);

    // header
    dst_pt.x = pos_rt.x + (pos_rt.w - Text::width(header, Font::BIG)) / 2;
    dst_pt.y = pos_rt.y;
    Text(header, Font::BIG, dst_pt);

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
    TextBox(message_info, Font::BIG, dst_rt);

    Kingdom & kingdom = world.GetMyKingdom();
    const Sprite & spritecursor = AGG::GetICN(tradpost, 14);

    const std::string & header_from = "Your Resources";

    Resource::funds_t fundsFrom = kingdom.GetFundsResource();
    u8 resourceFrom = 0;
    const Point pt1(pos_rt.x, pos_rt.y + 190);
    std::vector<Rect> rectsFrom(7);
    rectsFrom[0] = Rect(pt1.x, pt1.y, 34, 34);		// wood
    rectsFrom[1] = Rect(pt1.x + 37, pt1.y, 34, 34);	// mercury
    rectsFrom[2] = Rect(pt1.x + 74, pt1.y, 34, 34);	// ore
    rectsFrom[3] = Rect(pt1.x, pt1.y + 37, 34, 34);	// sulfur
    rectsFrom[4] = Rect(pt1.x + 37, pt1.y + 37, 34, 34);// crystal
    rectsFrom[5] = Rect(pt1.x + 74, pt1.y + 37, 34, 34);// gems
    rectsFrom[6] = Rect(pt1.x + 37, pt1.y + 74, 34, 34);// gold
    SpriteCursor cursorFrom(spritecursor);
    dst_pt.x = pt1.x + (108 - Text::width(header_from, Font::SMALL)) / 2;
    dst_pt.y = pt1.y - 15;
    Text(header_from, Font::SMALL, dst_pt);
    RedrawFromResource(pt1, fundsFrom);

    const std::string & header_to = "Available Trades";

    Resource::funds_t fundsTo;
    u8 resourceTo = 0;
    const Point pt2(130 + pos_rt.x, pos_rt.y + 190);
    std::vector<Rect> rectsTo(7);
    rectsTo[0] = Rect(pt2.x, pt2.y, 34, 34);		// wood
    rectsTo[1] = Rect(pt2.x + 37, pt2.y, 34, 34);	// mercury
    rectsTo[2] = Rect(pt2.x + 74, pt2.y, 34, 34);	// ore
    rectsTo[3] = Rect(pt2.x, pt2.y + 37, 34, 34);	// sulfur
    rectsTo[4] = Rect(pt2.x + 37, pt2.y + 37, 34, 34);	// crystal
    rectsTo[5] = Rect(pt2.x + 74, pt2.y + 37, 34, 34);	// gems
    rectsTo[6] = Rect(pt2.x + 37, pt2.y + 74, 34, 34);	// gold
    SpriteCursor cursorTo(spritecursor);
    dst_pt.x = pt2.x + (108 - Text::width(header_to, Font::SMALL)) / 2;
    dst_pt.y = pt2.y - 15;
    Text(header_to, Font::SMALL, dst_pt);
    RedrawToResource(pt2, false, fromTradingPost);

    u32 count_sell = 0;
    u32 count_buy = 0;
    
    u32 max_sell = 0;
    u32 max_buy = 0;

    Button *buttonTrade = NULL;
    Button *buttonLeft = NULL;
    Button *buttonRight = NULL;

    Splitter *splitter = NULL;
    
    TextSprite *textSell = NULL;
    TextSprite *textBuy = NULL;

    std::string message;

#define ShowTradeArea \
    if(resourceFrom == resourceTo || (Resource::GOLD != resourceTo && 0 == max_buy)) \
    { \
	cursor.Hide(); \
	back.Restore(); \
	dst_rt.x = pos_rt.x; \
	dst_rt.y = pos_rt.y + 30; \
	dst_rt.w = pos_rt.w; \
	dst_rt.h = 100; \
	TextBox(message_info, Font::BIG, dst_rt); \
	if(buttonTrade) delete buttonTrade; \
	if(buttonLeft) delete buttonLeft; \
        if(buttonRight) delete buttonRight; \
        if(splitter) delete splitter; \
        if(textSell) delete textSell; \
        if(textBuy) delete textBuy; \
        buttonTrade = NULL; \
        buttonLeft = NULL; \
        buttonRight = NULL; \
        textSell = NULL; \
        textBuy = NULL; \
        splitter = NULL; \
        cursor.Show(); \
        display.Flip(); \
    } \
    else \
    { \
	cursor.Hide(); \
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
	dst_rt.x = dst_pt.x + 24; \
	dst_rt.y = dst_pt.y + 4; \
	dst_rt.w = 188; \
	dst_rt.h = 10; \
	splitter = new Splitter(AGG::GetICN(tradpost, 2), dst_rt, Splitter::HORIZONTAL); \
	splitter->SetRange(0, (Resource::GOLD == resourceTo ? max_sell : max_buy)); \
	dst_pt.x = pos_rt.x + 6; \
	dst_pt.y = pos_rt.y + 129; \
	buttonLeft = new Button(dst_pt, tradpost, 3, 4); \
	dst_pt.x = pos_rt.x + 215; \
	dst_pt.y = pos_rt.y + 129; \
	buttonRight = new Button(dst_pt, tradpost, 5, 6); \
	Resource::resource_t rs_from = static_cast<Resource::resource_t>(resourceFrom); \
	Resource::resource_t rs_to   = static_cast<Resource::resource_t>(resourceTo); \
	u16 exchange_rate = GetTradeCosts(resourceFrom, resourceTo, fromTradingPost); \
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
	TextBox(message, Font::BIG, dst_rt); \
	const Sprite & sprite_from = AGG::GetICN(ICN::RESOURCE, Resource::GetIndexSprite2(rs_from)); \
	dst_pt.x = pos_rt.x + pos_rt.w / 2 - 70 - sprite_from.w() / 2; \
	dst_pt.y = pos_rt.y + 115 - sprite_from.h(); \
	display.Blit(sprite_from, dst_pt); \
	message.clear(); \
	String::AddInt(message, count_sell); \
	dst_pt.x = pos_rt.x + pos_rt.w / 2 - 70 - Text::width(message, Font::SMALL) / 2; \
	dst_pt.y = pos_rt.y + 116; \
	textSell = new TextSprite(message, Font::SMALL, dst_pt); \
	const Sprite & sprite_to = AGG::GetICN(ICN::RESOURCE, Resource::GetIndexSprite2(rs_to)); \
	dst_pt.x = pos_rt.x + pos_rt.w / 2 + 70 - sprite_to.w() / 2; \
	dst_pt.y = pos_rt.y + 115 - sprite_to.h(); \
	display.Blit(sprite_to, dst_pt); \
	message.clear(); \
	String::AddInt(message, count_buy); \
	dst_pt.x = pos_rt.x + pos_rt.w / 2 + 70 - Text::width(message, Font::SMALL) / 2; \
	dst_pt.y = pos_rt.y + 116; \
	textBuy = new TextSprite(message, Font::SMALL, dst_pt); \
	const Sprite & sprite_fromto = AGG::GetICN(tradpost, 0); \
	dst_pt.x = pos_rt.x + pos_rt.w / 2 - sprite_fromto.w() / 2; \
	dst_pt.y = pos_rt.y + 90; \
	display.Blit(sprite_fromto, dst_pt); \
	const std::string & str_qty = "Qty to trade"; \
	dst_pt.x = pos_rt.x + (pos_rt.w - Text::width(str_qty, Font::SMALL)) / 2; \
	dst_pt.y = pos_rt.y + 110; \
	Text(str_qty, Font::SMALL, dst_pt); \
	(*buttonTrade).Draw(); \
	(*buttonLeft).Draw(); \
	(*buttonRight).Draw(); \
	cursor.Show(); \
	display.Flip(); \
	RedrawInfoBuySell; \
    } \

#define RedrawInfoBuySell \
	cursor.Hide(); \
	if(textSell) \
	{ \
		message.clear(); \
		String::AddInt(message, count_sell); \
		dst_pt.x = pos_rt.x + pos_rt.w / 2 - 70 - Text::width(message, Font::SMALL) / 2; \
		dst_pt.y = pos_rt.y + 116; \
		textSell->Hide(); \
		textSell->SetText(message); \
		textSell->SetPos(dst_pt); \
		textSell->Show(); \
	} \
	if(textBuy) \
	{ \
		message.clear(); \
		String::AddInt(message, count_buy); \
		dst_pt.x = pos_rt.x + pos_rt.w / 2 + 70 - Text::width(message, Font::SMALL) / 2; \
		dst_pt.y = pos_rt.y + 116; \
		textBuy->Hide(); \
		textBuy->SetText(message); \
		textBuy->SetPos(dst_pt); \
		textBuy->Show(); \
	} \
	cursor.Show(); \
	display.Flip();


    // button exit
    const Sprite & sprite_exit = AGG::GetICN(tradpost, 17);
    dst_pt.x = pos_rt.x + (pos_rt.w - sprite_exit.w()) / 2;
    dst_pt.y = pos_rt.y + pos_rt.h + BUTTON_HEIGHT - sprite_exit.h();
    Button buttonExit(dst_pt, tradpost, 17, 18);

    buttonExit.Draw();

    cursor.Show();
    display.Flip();

    LocalEvent & le = LocalEvent::GetLocalEvent();
   
    // message loop
    while(le.HandleEvents())
    {
        if(buttonTrade) le.MousePressLeft(*buttonTrade) ? (*buttonTrade).PressDraw() : (*buttonTrade).ReleaseDraw();
        if(buttonLeft) le.MousePressLeft(*buttonLeft) ? (*buttonLeft).PressDraw() : (*buttonLeft).ReleaseDraw();
        if(buttonRight) le.MousePressLeft(*buttonRight) ? (*buttonRight).PressDraw() : (*buttonRight).ReleaseDraw();

        le.MousePressLeft(buttonExit) ? buttonExit.PressDraw() : buttonExit.ReleaseDraw();

        if(le.MouseClickLeft(buttonExit) || le.KeyPress(KEY_RETURN) || le.KeyPress(KEY_ESCAPE)) break;
	
	// click from
	for(u8 ii = 0; ii < rectsFrom.size(); ++ii)
	{
	    const Rect & rect_from = rectsFrom[ii];

	    if(le.MouseClickLeft(rect_from))
	    {
		switch(ii)
		{
		    case 0:
			resourceFrom = Resource::WOOD;
			max_sell = fundsFrom.wood;
			break;
		    case 1:
			resourceFrom = Resource::MERCURY;
			max_sell = fundsFrom.mercury;
			break;
		    case 2:
			resourceFrom = Resource::ORE;
			max_sell = fundsFrom.ore;
			break;
		    case 3:
			resourceFrom = Resource::SULFUR;
			max_sell = fundsFrom.sulfur;
			break;
		    case 4:
			resourceFrom = Resource::CRYSTAL;
			max_sell = fundsFrom.crystal;
			break;
		    case 5:
			resourceFrom = Resource::GEMS;
			max_sell = fundsFrom.gems;
			break;
		    case 6:
			resourceFrom = Resource::GOLD;
			max_sell = fundsFrom.gold;
			break;
		    default: break;
		}

		if(GetTradeCosts(resourceFrom, resourceTo, fromTradingPost))
		{
		    max_buy = Resource::GOLD == resourceTo ? 
			max_sell * GetTradeCosts(resourceFrom, resourceTo, fromTradingPost) :
                        max_sell / GetTradeCosts(resourceFrom, resourceTo, fromTradingPost);
		}

		count_sell = 0;
		count_buy = 0;

		cursor.Hide();
		cursorFrom.Move(rect_from.x - 2, rect_from.y - 2);

		if(resourceTo) cursorTo.Hide();
		RedrawToResource(pt2, true, fromTradingPost, resourceFrom);
		if(resourceTo) cursorTo.Show();
		if(resourceTo) ShowTradeArea;

		cursor.Show();
		display.Flip();
	    }
	}

	// click to
	for(u8 ii = 0; ii < rectsTo.size(); ++ii)
	{
	    const Rect & rect_to = rectsTo[ii];

	    if(le.MouseClickLeft(rect_to))
	    {
		switch(ii)
		{
		    case 0: resourceTo = Resource::WOOD; break;
		    case 1: resourceTo = Resource::MERCURY; break;
		    case 2: resourceTo = Resource::ORE; break;
		    case 3: resourceTo = Resource::SULFUR; break;
		    case 4: resourceTo = Resource::CRYSTAL; break;
		    case 5: resourceTo = Resource::GEMS; break;
		    case 6: resourceTo = Resource::GOLD; break;
		    default: break;
		}

		if(GetTradeCosts(resourceFrom, resourceTo, fromTradingPost))
		{
		    max_buy = Resource::GOLD == resourceTo ? 
			max_sell * GetTradeCosts(resourceFrom, resourceTo, fromTradingPost) :
                        max_sell / GetTradeCosts(resourceFrom, resourceTo, fromTradingPost);
		}

		count_sell = 0;
		count_buy = 0;

		cursor.Hide();
		cursorTo.Move(rect_to.x - 2, rect_to.y - 2);

		if(resourceFrom)
		{
		    cursorTo.Hide();
		    RedrawToResource(pt2, true, fromTradingPost, resourceFrom);
		    cursorTo.Show();
		    ShowTradeArea;
		}
		cursor.Show();
		display.Flip();
	    }
	}

	// move splitter
        if(splitter && max_buy && le.MousePressLeft(splitter->GetRect()))
        {
            u32 seek = (le.MouseCursor().x - splitter->GetRect().x) * 100 / splitter->GetStep();

            if(seek < splitter->Min()) seek = splitter->Min();
            else
            if(seek > splitter->Max()) seek = splitter->Max();

	    count_buy = seek * (Resource::GOLD == resourceTo ? GetTradeCosts(resourceFrom, resourceTo, fromTradingPost) : 1);
	    count_sell = seek * (Resource::GOLD == resourceTo ? 1: GetTradeCosts(resourceFrom, resourceTo, fromTradingPost));

            cursor.Hide();
            splitter->Move(seek);
	    RedrawInfoBuySell;
            cursor.Show();
            display.Flip();
        }

	// trade
	if(buttonTrade && le.MouseClickLeft(*buttonTrade) && count_sell && count_buy)
	{
	    kingdom.OddFundsResource(Resource::funds_t(static_cast<Resource::resource_t>(resourceFrom), count_sell));
    	    kingdom.AddFundsResource(Resource::funds_t(static_cast<Resource::resource_t>(resourceTo), count_buy));

    	    // exit
    	    break;
	}

	// decrease trade resource
	if(count_buy &&
	    ((buttonLeft && le.MouseClickLeft(*buttonLeft)) ||
	    le.MouseWheelDn(splitter->GetRect())))
	{
	    count_buy -= Resource::GOLD == resourceTo ? GetTradeCosts(resourceFrom, resourceTo, fromTradingPost) : 1;

	    count_sell -= Resource::GOLD == resourceTo ? 1: GetTradeCosts(resourceFrom, resourceTo, fromTradingPost);

            cursor.Hide();
	    splitter->Backward();
            cursor.Show();
            display.Flip();

	    RedrawInfoBuySell;
	}

	// increase trade resource
	if( count_buy < max_buy &&
	    ((buttonRight && le.MouseClickLeft(*buttonRight)) ||
	    le.MouseWheelUp(splitter->GetRect())))
	{
	    count_buy += Resource::GOLD == resourceTo ? GetTradeCosts(resourceFrom, resourceTo, fromTradingPost) : 1;

	    count_sell += Resource::GOLD == resourceTo ? 1: GetTradeCosts(resourceFrom, resourceTo, fromTradingPost);

            cursor.Hide();
	    splitter->Forward();
            cursor.Show();
            display.Flip();

	    RedrawInfoBuySell;
	}
    }

    if(buttonTrade) delete buttonTrade;
    if(buttonLeft) delete buttonLeft;
    if(buttonRight) delete buttonRight;
    if(splitter) delete splitter;
    if(textSell) delete textSell;
    if(textBuy) delete textBuy;

    cursor.Hide();
}

void RedrawFromResource(const Point & pt, const Resource::funds_t & rs)
{
    Display & display = Display::Get();
    const ICN::icn_t tradpost = H2Config::EvilInterface() ? ICN::TRADPOSE : ICN::TRADPOST;
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
    Text(str, Font::SMALL, dst_pt);
    
    // mercury
    dst_pt.x = pt.x + 37;
    dst_pt.y = pt.y;
    display.Blit(AGG::GetICN(tradpost, 8), dst_pt);
    str.clear();
    String::AddInt(str, rs.mercury);
    dst_pt.x += (34 - Text::width(str, Font::SMALL)) / 2;
    dst_pt.y += 21;
    Text(str, Font::SMALL, dst_pt);

    // ore
    dst_pt.x = pt.x + 74;
    dst_pt.y = pt.y;
    display.Blit(AGG::GetICN(tradpost, 9), dst_pt);
    str.clear();
    String::AddInt(str, rs.ore);
    dst_pt.x += (34 - Text::width(str, Font::SMALL)) / 2;
    dst_pt.y += 21;
    Text(str, Font::SMALL, dst_pt);

    // sulfur
    dst_pt.x = pt.x;
    dst_pt.y = pt.y + 37;
    display.Blit(AGG::GetICN(tradpost, 10), dst_pt);
    str.clear();
    String::AddInt(str, rs.sulfur);
    dst_pt.x += (34 - Text::width(str, Font::SMALL)) / 2;
    dst_pt.y += 21;
    Text(str, Font::SMALL, dst_pt);

    // crystal
    dst_pt.x = pt.x + 37;
    dst_pt.y = pt.y + 37;
    display.Blit(AGG::GetICN(tradpost, 11), dst_pt);
    str.clear();
    String::AddInt(str, rs.crystal);
    dst_pt.x += (34 - Text::width(str, Font::SMALL)) / 2;
    dst_pt.y += 21;
    Text(str, Font::SMALL, dst_pt);

    // gems
    dst_pt.x = pt.x + 74;
    dst_pt.y = pt.y + 37;
    display.Blit(AGG::GetICN(tradpost, 12), dst_pt);
    str.clear();
    String::AddInt(str, rs.gems);
    dst_pt.x += (34 - Text::width(str, Font::SMALL)) / 2;
    dst_pt.y += 21;
    Text(str, Font::SMALL, dst_pt);

    // gold
    dst_pt.x = pt.x + 37;
    dst_pt.y = pt.y + 74;
    display.Blit(AGG::GetICN(tradpost, 13), dst_pt);
    str.clear();
    String::AddInt(str, rs.gold);
    dst_pt.x += (34 - Text::width(str, Font::SMALL)) / 2;
    dst_pt.y += 21;
    Text(str, Font::SMALL, dst_pt);
}

void RedrawToResource(const Point & pt, bool showcost, bool tradingPost, u8 from_resource)
{
    Display & display = Display::Get();
    const ICN::icn_t tradpost = H2Config::EvilInterface() ? ICN::TRADPOSE : ICN::TRADPOST;
    std::string str;
    Point dst_pt;

    // wood
    dst_pt.x = pt.x;
    dst_pt.y = pt.y;
    display.Blit(AGG::GetICN(tradpost, 7), dst_pt);
    if(showcost)
    {
	GetStringTradeCosts(str, from_resource, Resource::WOOD, tradingPost);
	dst_pt.x += (34 - Text::width(str, Font::SMALL)) / 2;
	dst_pt.y += 21;
	Text(str, Font::SMALL, dst_pt);
    }
    
    // mercury
    dst_pt.x = pt.x + 37;
    dst_pt.y = pt.y;
    display.Blit(AGG::GetICN(tradpost, 8), dst_pt);
    if(showcost)
    {
	GetStringTradeCosts(str, from_resource, Resource::MERCURY, tradingPost);
	dst_pt.x += (34 - Text::width(str, Font::SMALL)) / 2;
	dst_pt.y += 21;
        Text(str, Font::SMALL, dst_pt);
    }

    // ore
    dst_pt.x = pt.x + 74;
    dst_pt.y = pt.y;
    display.Blit(AGG::GetICN(tradpost, 9), dst_pt);
    if(showcost)
    {
	GetStringTradeCosts(str, from_resource, Resource::ORE, tradingPost);
	dst_pt.x += (34 - Text::width(str, Font::SMALL)) / 2;
	dst_pt.y += 21;
	Text(str, Font::SMALL, dst_pt);
    }

    // sulfur
    dst_pt.x = pt.x;
    dst_pt.y = pt.y + 37;
    display.Blit(AGG::GetICN(tradpost, 10), dst_pt);
    if(showcost)
    {
	GetStringTradeCosts(str, from_resource, Resource::SULFUR, tradingPost);
	dst_pt.x += (34 - Text::width(str, Font::SMALL)) / 2;
	dst_pt.y += 21;
	Text(str, Font::SMALL, dst_pt);
    }

    // crystal
    dst_pt.x = pt.x + 37;
    dst_pt.y = pt.y + 37;
    display.Blit(AGG::GetICN(tradpost, 11), dst_pt);
    if(showcost)
    {
	GetStringTradeCosts(str, from_resource, Resource::CRYSTAL, tradingPost);
	dst_pt.x += (34 - Text::width(str, Font::SMALL)) / 2;
	dst_pt.y += 21;
	Text(str, Font::SMALL, dst_pt);
    }

    // gems
    dst_pt.x = pt.x + 74;
    dst_pt.y = pt.y + 37;
    display.Blit(AGG::GetICN(tradpost, 12), dst_pt);
    if(showcost)
    {
	GetStringTradeCosts(str, from_resource, Resource::GEMS, tradingPost);
	dst_pt.x += (34 - Text::width(str, Font::SMALL)) / 2;
	dst_pt.y += 21;
	Text(str, Font::SMALL, dst_pt);
    }

    // gold
    dst_pt.x = pt.x + 37;
    dst_pt.y = pt.y + 74;
    display.Blit(AGG::GetICN(tradpost, 13), dst_pt);
    if(showcost)
    {
	GetStringTradeCosts(str, from_resource, Resource::GOLD, tradingPost);
	dst_pt.x += (34 - Text::width(str, Font::SMALL)) / 2;
	dst_pt.y += 21;
	Text(str, Font::SMALL, dst_pt);
    }
}

void GetStringTradeCosts(std::string & str, u8 rs_from, u8 rs_to, bool tradingPost)
{
    if(str.size()) str.clear();

    if(rs_from == rs_to)
    {
	str = "n/a";
	
	return;
    }

    if(Resource::GOLD != rs_from && Resource::GOLD != rs_to) str = "1/";

    String::AddInt(str, GetTradeCosts(rs_from, rs_to, tradingPost));
}

u16 GetTradeCosts(u8 rs_from, u8 rs_to, bool tradingPost)
{
    u8 markets = world.GetMyKingdom().GetCountMarketplace();
    
    if(tradingPost)
        markets++;

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
