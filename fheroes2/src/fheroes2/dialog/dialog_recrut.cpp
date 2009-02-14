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

#include "agg.h"
#include "settings.h"
#include "cursor.h"
#include "payment.h"
#include "world.h"
#include "button.h"
#include "kingdom.h"
#include "dialog.h"

u16 Dialog::RecruitMonster(const Monster & monster, u16 available)
{
    Display & display = Display::Get();

    // cursor
    Cursor & cursor = Cursor::Get();
    const Cursor::themes_t oldcursor = cursor.Themes();
    cursor.Hide();
    cursor.SetThemes(Cursor::POINTER);
    
    // calculate max count
    u16 max = 0;
    const PaymentConditions::BuyMonster paymentMonster(monster());
    const Resource::funds_t & kingdomResource = world.GetMyKingdom().GetFundsResource();
    while(Resource::funds_t(paymentMonster * max) <= kingdomResource && max <= available) ++max;

    u16 result = --max;

    PaymentConditions::payment_t paymentCosts(paymentMonster * result);

    const Sprite & box = AGG::GetICN(ICN::RECRBKG, 0);
    const Rect pos((display.w() - box.w()) / 2, (display.h() - box.h()) / 2, box.w(), box.h());

    Background back(pos);
    back.Save();

    display.Blit(box, pos.x, pos.y);
    
    LocalEvent & le = LocalEvent::GetLocalEvent();

    Point dst_pt;

    std::string str;
    Text text;

    // text recruit monster
    str = _("Recruit %{name}");
    String::Replace(str, "%{name}", monster.GetName());
    text.Set(str, Font::BIG);
    dst_pt.x = pos.x + (pos.w - text.w()) / 2;
    dst_pt.y = pos.y + 25;
    text.Blit(dst_pt);

    // sprite monster
    const Sprite & smon = AGG::GetICN(monster.ICNMonh(), 0);
    dst_pt.x = pos.x + 70 - smon.w() / 2;
    dst_pt.y = pos.y + 120 - smon.h();
    display.Blit(smon, dst_pt);

    // info resource
    // gold
    const Sprite & sgold = AGG::GetICN(ICN::RESOURCE, 6);
    dst_pt.x = pos.x + (paymentMonster.ore ||
                        paymentMonster.wood ||
                        paymentMonster.mercury ||
                        paymentMonster.crystal ||
                        paymentMonster.sulfur ||
                        paymentMonster.gems ? 150 : 175);
    dst_pt.y = pos.y + 75;
    display.Blit(sgold, dst_pt);

    dst_pt.x = pos.x + (paymentMonster.ore ||
                        paymentMonster.wood ||
                        paymentMonster.mercury ||
                        paymentMonster.crystal ||
                        paymentMonster.sulfur ||
                        paymentMonster.gems ? 105 : 130);
    dst_pt.y = pos.y + 200;
    display.Blit(sgold, dst_pt);

    str.clear();
    String::AddInt(str, paymentMonster.gold);
    text.Set(str, Font::SMALL);
    dst_pt.x = pos.x + (paymentMonster.ore ||
                        paymentMonster.wood ||
                        paymentMonster.mercury ||
                        paymentMonster.crystal ||
                        paymentMonster.sulfur ||
                        paymentMonster.gems ? 183 : 205) - text.w() / 2;
    dst_pt.y = pos.y + 103;
    text.Blit(dst_pt);

    // crystal
    if(paymentMonster.crystal)
    {
        const Sprite & sres = AGG::GetICN(ICN::RESOURCE, 4);
        dst_pt.x = pos.x + 225;
        dst_pt.y = pos.y + 75;
        display.Blit(sres, dst_pt);

        dst_pt.x = pos.x + 180;
        dst_pt.y = pos.y + 200;
        display.Blit(sres, dst_pt);

        str.clear();
        String::AddInt(str, paymentMonster.crystal);
	text.Set(str);
        dst_pt.x = pos.x + 240 - text.w() / 2;
        dst_pt.y = pos.y + 103;
        text.Blit(dst_pt);
    }
    else
    // mercury
    if(paymentMonster.mercury)
    {
        const Sprite & sres = AGG::GetICN(ICN::RESOURCE, 1);
        dst_pt.x = pos.x + 225;
        dst_pt.y = pos.y + 72;
        display.Blit(sres, dst_pt);

        dst_pt.x = pos.x + 180;
        dst_pt.y = pos.y + 197;
        display.Blit(sres, dst_pt);

        str.clear();
        String::AddInt(str, paymentMonster.mercury);
	text.Set(str);
        dst_pt.x = pos.x + 240 - text.w() / 2;
        dst_pt.y = pos.y + 103;
        text.Blit(dst_pt);
    }
    else
    // wood
    if(paymentMonster.wood)
    {
        const Sprite & sres = AGG::GetICN(ICN::RESOURCE, 0);
        dst_pt.x = pos.x + 225;
        dst_pt.y = pos.y + 72;
        display.Blit(sres, dst_pt);

        dst_pt.x = pos.x + 180;
        dst_pt.y = pos.y + 197;
        display.Blit(sres, dst_pt);

        str.clear();
        String::AddInt(str, paymentMonster.wood);
	text.Set(str);
        dst_pt.x = pos.x + 240 - text.w() / 2;
        dst_pt.y = pos.y + 103;
        text.Blit(dst_pt);
    }
    else
    // ore
    if(paymentMonster.ore)
    {
        const Sprite & sres = AGG::GetICN(ICN::RESOURCE, 2);
        dst_pt.x = pos.x + 225;
        dst_pt.y = pos.y + 72;
        display.Blit(sres, dst_pt);

        dst_pt.x = pos.x + 180;
        dst_pt.y = pos.y + 197;
        display.Blit(sres, dst_pt);

        str.clear();
        String::AddInt(str, paymentMonster.ore);
	text.Set(str);
        dst_pt.x = pos.x + 240 - text.w() / 2;
        dst_pt.y = pos.y + 103;
        text.Blit(dst_pt);
    }
    else
    // sulfur
    if(paymentMonster.sulfur)
    {
        const Sprite & sres = AGG::GetICN(ICN::RESOURCE, 3);
        dst_pt.x = pos.x + 225;
        dst_pt.y = pos.y + 75;
        display.Blit(sres, dst_pt);

        dst_pt.x = pos.x + 180;
        dst_pt.y = pos.y + 200;
        display.Blit(sres, dst_pt);

        str.clear();
        String::AddInt(str, paymentMonster.sulfur);
	text.Set(str);
        dst_pt.x = pos.x + 240 - text.w() / 2;
        dst_pt.y = pos.y + 103;
        text.Blit(dst_pt);
    }
    else
    // gems
    if(paymentMonster.gems)
    {
        const Sprite & sres = AGG::GetICN(ICN::RESOURCE, 5);
        dst_pt.x = pos.x + 225;
        dst_pt.y = pos.y + 75;
        display.Blit(sres, dst_pt);

        dst_pt.x = pos.x + 180;
        dst_pt.y = pos.y + 200;
        display.Blit(sres, dst_pt);

        str.clear();
        String::AddInt(str, paymentMonster.gems);
	text.Set(str);
        dst_pt.x = pos.x + 240 - text.w() / 2;
        dst_pt.y = pos.y + 103;
        text.Blit(dst_pt);
    }

    // text number buy
    text.Set(_("Number to buy:"));
    dst_pt.x = pos.x + 30;
    dst_pt.y = pos.y + 163;
    text.Blit(dst_pt);

    Background static_info(Rect(pos.x + 16, pos.y + 125, pos.w - 32, 122));
    static_info.Save();

#define RedrawCurrentInfo \
    str = _("Available: %{count}"); \
    String::Replace(str, "%{count}", available); \
    text.Set(str, Font::SMALL); \
    text.Blit(pos.x + 70 - text.w() / 2, pos.y + 130); \
    str.clear(); \
    String::AddInt(str, result); \
    text.Set(str, Font::BIG); \
    text.Blit(pos.x + 167 - text.w() / 2, pos.y + 160); \
    if(paymentMonster.ore || \
       paymentMonster.wood || \
       paymentMonster.mercury || \
       paymentMonster.crystal || \
       paymentMonster.sulfur || \
       paymentMonster.gems) \
    { \
	str.clear(); \
	String::AddInt(str, paymentCosts.gold); \
	text.Set(str, Font::SMALL); \
	text.Blit(pos.x + 133 - text.w() / 2, pos.y + 228); \
	str.clear(); \
	if(paymentMonster.ore) String::AddInt(str, paymentCosts.ore); \
        else \
	if(paymentMonster.wood) String::AddInt(str, paymentCosts.wood); \
        else \
        if(paymentMonster.mercury) String::AddInt(str, paymentCosts.mercury); \
        else \
        if(paymentMonster.crystal) String::AddInt(str, paymentCosts.crystal); \
        else \
        if(paymentMonster.sulfur) String::AddInt(str, paymentCosts.sulfur); \
        else \
        if(paymentMonster.gems) String::AddInt(str, paymentCosts.gems); \
	text.Set(str, Font::SMALL); \
	text.Blit(pos.x + 195 - text.w() / 2, pos.y + 228); \
    } \
    else \
    { \
	str.clear(); \
	String::AddInt(str, paymentCosts.gold); \
	text.Set(str, Font::SMALL); \
	text.Blit(pos.x + 160 - text.w() / 2, pos.y + 228); \
    }

    RedrawCurrentInfo;

    // buttons
    dst_pt.x = pos.x + 34;
    dst_pt.y = pos.y + 249;
    Button buttonOk(dst_pt, ICN::RECRUIT, 8, 9);

    dst_pt.x = pos.x + 187;
    dst_pt.y = pos.y + 249;
    Button buttonCancel(dst_pt, ICN::RECRUIT, 6, 7);

    dst_pt.x = pos.x + 230;
    dst_pt.y = pos.y + 155;
    Button buttonMax(dst_pt, ICN::RECRUIT, 4, 5);
    dst_pt.x = pos.x + 208;
    dst_pt.y = pos.y + 156;
    Button buttonUp(dst_pt, ICN::RECRUIT, 0, 1);

    dst_pt.x = pos.x + 208;
    dst_pt.y = pos.y + 171;
    Button buttonDn(dst_pt, ICN::RECRUIT, 2, 3);

    buttonOk.Draw();
    buttonCancel.Draw();
    buttonMax.Draw();
    buttonUp.Draw();
    buttonDn.Draw();

    cursor.Show();
    display.Flip();

    bool first = true;

    // str loop
    while(le.HandleEvents())
    {
	le.MousePressLeft(buttonOk) ? buttonOk.PressDraw() : buttonOk.ReleaseDraw();
	le.MousePressLeft(buttonCancel) ? buttonCancel.PressDraw() : buttonCancel.ReleaseDraw();
	le.MousePressLeft(buttonMax) ? buttonMax.PressDraw() : buttonMax.ReleaseDraw();
	le.MousePressLeft(buttonUp) ? buttonUp.PressDraw() : buttonUp.ReleaseDraw();
	le.MousePressLeft(buttonDn) ? buttonDn.PressDraw() : buttonDn.ReleaseDraw();

	if(le.KeyPress(KEY_BACKSPACE))
	{
	    if(0 < result)
	    {
		result /= 10;
		if(0 == result) first = true;
	    }

	    paymentCosts = paymentMonster * result;
	    cursor.Hide();
	    static_info.Restore();
	    RedrawCurrentInfo;
	    cursor.Show();
	    display.Flip();
	}

	if(le.KeyPress(KEY_0) ||
	   le.KeyPress(KEY_1) ||
	   le.KeyPress(KEY_2) ||
	   le.KeyPress(KEY_3) ||
	   le.KeyPress(KEY_4) ||
	   le.KeyPress(KEY_5) ||
	   le.KeyPress(KEY_6) ||
	   le.KeyPress(KEY_7) ||
	   le.KeyPress(KEY_8) ||
	   le.KeyPress(KEY_9))
	{
	    if(first)
	    {
		result = 0;
		first = false;
	    }

	    if(max > result)
	    {
		result *= 10;
		switch(le.KeyValue())
		{
		    case KEY_1:	result += 1; break;
		    case KEY_2:	result += 2; break;
		    case KEY_3:	result += 3; break;
		    case KEY_4:	result += 4; break;
		    case KEY_5:	result += 5; break;
		    case KEY_6:	result += 6; break;
		    case KEY_7:	result += 7; break;
		    case KEY_8:	result += 8; break;
		    case KEY_9:	result += 9; break;
		    default: break;
		}
		if(result > max) result = max;
	    }

	    paymentCosts = paymentMonster * result;
	    cursor.Hide();
	    static_info.Restore();
	    RedrawCurrentInfo;
	    cursor.Show();
	    display.Flip();
	}

	if(le.MouseClickLeft(buttonUp) && result < max)
	{
	    ++result;
	    paymentCosts += paymentMonster;
	    cursor.Hide();
	    static_info.Restore();
	    RedrawCurrentInfo;
	    cursor.Show();
	    display.Flip();
	}

	if(le.MouseClickLeft(buttonDn) && result)
	{
	    --result;
	    paymentCosts -= paymentMonster;
	    cursor.Hide();
	    static_info.Restore();
	    RedrawCurrentInfo;
	    cursor.Show();
	    display.Flip();
	}

	if(le.MouseClickLeft(buttonMax) && result != max)
	{
	    result = max;
	    paymentCosts = paymentMonster * max;
	    cursor.Hide();
	    static_info.Restore();
	    RedrawCurrentInfo;
	    cursor.Show();
	    display.Flip();
	}
	
	if(le.MouseClickLeft(buttonOk) || le.KeyPress(KEY_RETURN)) break;
	
	if(le.MouseClickLeft(buttonCancel) || le.KeyPress(KEY_ESCAPE)){ result = 0; break; }
    }

    cursor.Hide();

    back.Restore();
    cursor.SetThemes(oldcursor);

    cursor.Show();
    display.Flip();

    return result;
}

void Dialog::DwellingInfo(const Monster & monster, u16 available)
{
    Display & display = Display::Get();

    // cursor
    Cursor & cursor = Cursor::Get();
    const Cursor::themes_t oldcursor = cursor.Themes();
    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);
    
    const PaymentConditions::BuyMonster paymentMonster(monster());

    const Sprite & box = AGG::GetICN(ICN::RECR2BKG, 0);
    const Rect pos((display.w() - box.w()) / 2, (display.h() - box.h()) / 2, box.w(), box.h());

    Background back(pos);
    back.Save();

    display.Blit(box, pos.x, pos.y);
    
    LocalEvent & le = LocalEvent::GetLocalEvent();

    Point dst_pt;
    Text text;
    std::string str;

    // text recruit monster
    str = _("Recruit %{name}");
    String::Replace(str, "%{name}", monster.GetName());
    text.Set(str, Font::BIG);
    text.Blit(pos.x + (pos.w - text.w()) / 2, pos.y + 25);

    // sprite monster
    const Sprite & smon = AGG::GetICN(monster.ICNMonh(), 0);
    dst_pt.x = pos.x + 70 - smon.w() / 2;
    dst_pt.y = pos.y + 120 - smon.h();
    display.Blit(smon, dst_pt);

    // info resource
    // gold
    const Sprite & sgold = AGG::GetICN(ICN::RESOURCE, 6);
    dst_pt.x = pos.x + (paymentMonster.ore ||
                        paymentMonster.wood ||
                        paymentMonster.mercury ||
                        paymentMonster.crystal ||
                        paymentMonster.sulfur ||
                        paymentMonster.gems ? 150 : 175);
    dst_pt.y = pos.y + 75;
    display.Blit(sgold, dst_pt);

    str.clear();
    String::AddInt(str, paymentMonster.gold);
    text.Set(str, Font::SMALL);
    dst_pt.x = pos.x + (paymentMonster.ore ||
                        paymentMonster.wood ||
                        paymentMonster.mercury ||
                        paymentMonster.crystal ||
                        paymentMonster.sulfur ||
                        paymentMonster.gems ? 183 : 205) - text.w() / 2;
    dst_pt.y = pos.y + 103;
    text.Blit(dst_pt);
    // crystal
    if(paymentMonster.crystal)
    {
	const Sprite & sres = AGG::GetICN(ICN::RESOURCE, 4);
	dst_pt.x = pos.x + 225;
	dst_pt.y = pos.y + 75;
	display.Blit(sres, dst_pt);

	str.clear();
	String::AddInt(str, paymentMonster.crystal);
	text.Set(str);
	dst_pt.x = pos.x + 240 - text.w() / 2;
	dst_pt.y = pos.y + 103;
	text.Blit(dst_pt);
    }
    else
    // mercury
    if(paymentMonster.mercury)
    {
	const Sprite & sres = AGG::GetICN(ICN::RESOURCE, 1);
	dst_pt.x = pos.x + 225;
	dst_pt.y = pos.y + 72;
	display.Blit(sres, dst_pt);

	str.clear();
	String::AddInt(str, paymentMonster.mercury);
	text.Set(str);
	dst_pt.x = pos.x + 240 - text.w() / 2;
	dst_pt.y = pos.y + 103;
	text.Blit(dst_pt);
    }
    else
    // wood
    if(paymentMonster.wood)
    {
	const Sprite & sres = AGG::GetICN(ICN::RESOURCE, 0);
	dst_pt.x = pos.x + 225;
	dst_pt.y = pos.y + 72;
	display.Blit(sres, dst_pt);

	str.clear();
	String::AddInt(str, paymentMonster.wood);
	text.Set(str);
	dst_pt.x = pos.x + 240 - text.w() / 2;
	dst_pt.y = pos.y + 103;
	text.Blit(dst_pt);
    }
    else
    // ore
    if(paymentMonster.ore)
    {
	const Sprite & sres = AGG::GetICN(ICN::RESOURCE, 2);
	dst_pt.x = pos.x + 225;
	dst_pt.y = pos.y + 72;
	display.Blit(sres, dst_pt);

	str.clear();
	String::AddInt(str, paymentMonster.ore);
	text.Set(str);
	dst_pt.x = pos.x + 240 - text.w() / 2;
	dst_pt.y = pos.y + 103;
	text.Blit(dst_pt);
    }
    else
    // sulfur
    if(paymentMonster.sulfur)
    {
	const Sprite & sres = AGG::GetICN(ICN::RESOURCE, 3);
	dst_pt.x = pos.x + 225;
	dst_pt.y = pos.y + 75;
	display.Blit(sres, dst_pt);

	str.clear();
	String::AddInt(str, paymentMonster.sulfur);
	text.Set(str);
	dst_pt.x = pos.x + 240 - text.w() / 2;
	dst_pt.y = pos.y + 103;
	text.Blit(dst_pt);
    }
    else
    // gems
    if(paymentMonster.gems)
    {
	const Sprite & sres = AGG::GetICN(ICN::RESOURCE, 5);
	dst_pt.x = pos.x + 225;
	dst_pt.y = pos.y + 75;
	display.Blit(sres, dst_pt);

	str.clear();
	String::AddInt(str, paymentMonster.gems);
	text.Set(str);
	dst_pt.x = pos.x + 240 - text.w() / 2;
	dst_pt.y = pos.y + 103;
	text.Blit(dst_pt);
    }

    // text available
    str = _("Available: %{count}");
    String::Replace(str, "%{count}", available);
    text.Set(str);
    text.Blit(pos.x + 70 - text.w() / 2, pos.y + 130);

    cursor.Show();
    display.Flip();

    //
    while(le.HandleEvents() && le.MouseRight());

    cursor.Hide();

    back.Restore();
    cursor.SetThemes(oldcursor);

    cursor.Show();
    display.Flip();
}
