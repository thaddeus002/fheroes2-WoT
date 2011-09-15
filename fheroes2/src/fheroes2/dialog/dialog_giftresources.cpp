/***************************************************************************
 *   Copyright (C) 2011 by Andrey Afletdinov <fheroes2@gmail.com>          *
 *                                                                         *
 *   Part of the Free Heroes2 Engine:                                      *
 *   http://sourceforge.net/projects/fheroes2                              *
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
#include "button.h"
#include "world.h"
#include "gameevent.h"
#include "dialog.h"

s32 GetIndexClickRects(const std::vector<Rect> & rects)
{
    LocalEvent & le = LocalEvent::Get();

    for(std::vector<Rect>::const_iterator
    	it = rects.begin(); it != rects.end(); ++it)
    {
	size_t index = std::distance(rects.begin(), it);
	    if(rects[index] & le.GetMouseCursor() &&
		le.MouseClickLeft(rects[index]))
		return index;
    }
    return -1;
}

struct SelectRecipientsColors
{
    const Colors	colors;
    u8			recipients;
    std::vector<Rect>	positions;

    SelectRecipientsColors(const Point & pos) :
	colors(Settings::Get().GetPlayers().GetColors() & ~Settings::Get().GetPlayers().current_color), recipients(0)
    {
	positions.reserve(colors.size());

	for(Colors::const_iterator
    	    it = colors.begin(); it != colors.end(); ++it)
	{
    	    const u8 current = std::distance(colors.begin(), it);
	    const Sprite & sprite = AGG::GetICN(ICN::CELLWIN, 43);

    	    positions.push_back(Rect(pos.x + Game::GetStep4Player(current, sprite.w() + 15, colors.size()),
									    pos.y, sprite.w(), sprite.h()));
	}
    }

    s8 GetIndexClick(void) const
    {
	return GetIndexClickRects(positions);
    }

    void Redraw(void) const
    {
	for(Colors::const_iterator
    	    it = colors.begin(); it != colors.end(); ++it)
	{
	    Display & display = Display::Get();
	    const Rect & pos = positions[std::distance(colors.begin(), it)];

	    display.Blit(AGG::GetICN(ICN::CELLWIN, 43 + Color::GetIndex(*it)), pos);
	    if(recipients & *it)
		display.Blit(AGG::GetICN(ICN::CELLWIN, 2), pos.x + 2, pos.y + 2);
	}
    }

    bool QueueEventProcessing(void)
    {
	const s8 index = GetIndexClick();

    	if(index >= 0)
	{
	    const u8 & cols = colors[index];

	    if(recipients & cols)
		recipients &= ~cols;
	    else
		recipients |= cols;

	    return true;
	}

	return false;
    }
};

struct ResourceBar
{
    Funds &		resource;
    Point		pos;
    std::vector<Rect>	positions;

    ResourceBar(Funds & funds, s16 posx, s16 posy) : resource(funds), pos(posx, posy)
    {
	positions.reserve(7);
	const Sprite & sprite = AGG::GetICN(ICN::TRADPOST, 7);

	positions.push_back(Rect(posx, posy, sprite.w(), sprite.h()));
	positions.push_back(Rect(posx + 40, posy, sprite.w(), sprite.h()));
	positions.push_back(Rect(posx + 80, posy, sprite.w(), sprite.h()));
	positions.push_back(Rect(posx + 120, posy, sprite.w(), sprite.h()));
	positions.push_back(Rect(posx + 160, posy, sprite.w(), sprite.h()));
	positions.push_back(Rect(posx + 200, posy, sprite.w(), sprite.h()));
	positions.push_back(Rect(posx + 240, posy, sprite.w(), sprite.h()));
    }

    static void RedrawResource(u8 type, s32 count, s16 posx, s16 posy)
    {
	Display & display = Display::Get();
	std::ostringstream os;

	os << count;
	Text text(os.str(), Font::SMALL);
	const Sprite & sprite = AGG::GetICN(ICN::TRADPOST, 7 + Resource::GetIndexSprite2(type));
	display.Blit(sprite, posx, posy);
	text.Blit(posx + (sprite.w() - text.w()) / 2, posy + sprite.h() - 12);
    }

    void Redraw(const Funds* res = NULL) const
    {
	if(! res) res = &resource;

	for(std::vector<Rect>::const_iterator
	    it = positions.begin(); it != positions.end(); ++it)
	{
	    u8 rs = Resource::FromIndexSprite2(std::distance(positions.begin(), it));
	    RedrawResource(rs, res->Get(rs), (*it).x, (*it).y);
	}
    }

    s8 GetIndexClick(void) const
    {
	return GetIndexClickRects(positions);
    }

    bool QueueEventProcessing(Funds & funds, u8 mul)
    {
	const s8 index = GetIndexClick();

	if(index >= 0)
	{
	    u8 rs = Resource::FromIndexSprite2(index);
	    u8 step = rs == Resource::GOLD ? 100 : 1;

	    u32 cur = resource.Get(rs);
	    u32 sel = cur;
	    u32 max = mul > 1 ? (funds.Get(rs) + resource.Get(rs)) / mul : funds.Get(rs) + resource.Get(rs);
	    
	    if(0 == mul)
	    {
		Dialog::Message("", "First select recipients!", Font::BIG, Dialog::OK);
	    }
	    else
	    if(0 == max)
	    {
		std::string msg = _("You cannot select %{resource}!");
		String::Replace(msg, "%{resource}", Resource::String(rs));
		Dialog::Message("", msg, Font::BIG, Dialog::OK);
	    }
	    else
	    {
		std::string msg = _("Select count %{resource}:");
		String::Replace(msg, "%{resource}", Resource::String(rs));

		if(Dialog::SelectCount(msg, 0, max, sel, step) && cur != sel)
		{
		    s32* from = funds.GetPtr(rs);
		    s32* to = resource.GetPtr(rs);

		    if(from && to)
		    {
			s32 count = sel - cur;

			*from -= mul > 1 ? count * mul : count;
			*to += count;

			return true;
		    }
		}
	    }
	}

	return false;
    }
};

void Dialog::MakeGiftResource(void)
{
    Cursor & cursor = Cursor::Get();
    Display & display = Display::Get();
    LocalEvent & le = LocalEvent::Get();
    const Settings & conf = Settings::Get();

    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);

    const u16 window_w = 320;
    const u16 window_h = 224;

    Dialog::FrameBorder frameborder;
    frameborder.SetPosition((display.w() - window_w) / 2 - BORDERWIDTH, (display.h() - window_h) / 2 - BORDERWIDTH, window_w, window_h);
    frameborder.Redraw();

    const Rect & box = frameborder.GetArea();
    const Sprite & background = AGG::GetICN(ICN::STONEBAK, 0);
    display.Blit(background, Rect(0, 0, window_w, window_h), box);

    Funds funds1(world.GetKingdom(conf.CurrentColor()).GetFunds());
    Funds funds2;
    Text text;

    text.Set("Select Recipients");
    text.Blit(box.x + (box.w - text.w()) / 2, box.y + 5);

    SelectRecipientsColors selector(Point(box.x + 65, box.y + 28));
    selector.Redraw();

    text.Set("Your Funds");
    text.Blit(box.x + (box.w - text.w()) / 2, box.y + 55);

    ResourceBar info1(funds1, box.x + 25, box.y + 80);
    info1.Redraw();

    text.Set("Planned Gift");
    text.Blit(box.x + (box.w - text.w()) / 2, box.y + 125);

    ResourceBar info2(funds2, box.x + 25, box.y + 150);
    info2.Redraw();


    ButtonGroups btnGroups(box, Dialog::OK|Dialog::CANCEL);
    btnGroups.DisableButton1(true);
    btnGroups.Draw();


    cursor.Show();
    display.Flip();

    u8 count = Color::Count(selector.recipients);

    // message loop
    u16 result = Dialog::ZERO;

    while(result == Dialog::ZERO && le.HandleEvents())
    {
	if(selector.QueueEventProcessing())
	{
	    u8 new_count = Color::Count(selector.recipients);
	    cursor.Hide();
	    btnGroups.DisableButton1(0 == new_count || 0 == funds2.GetValidItems());
	    if(count != new_count)
	    {
		funds1 = world.GetKingdom(conf.CurrentColor()).GetFunds();
		funds2.Reset();
		info1.Redraw();
		info2.Redraw();
		count = new_count;
	    }
	    btnGroups.Draw();
	    selector.Redraw();
	    cursor.Show();
	    display.Flip();
	}
	else
	if(info2.QueueEventProcessing(funds1, count))
	{
	    cursor.Hide();
	    btnGroups.DisableButton1(0 == Color::Count(selector.recipients) || 0 == funds2.GetValidItems());
	    info1.Redraw();
	    info2.Redraw();
	    btnGroups.Draw();
	    cursor.Show();
	    display.Flip();
	}

        result = btnGroups.QueueEventProcessing();
    }

    if(Dialog::OK == result)
    {
	EventDate event;

	event.resource = funds2;
	event.computer = true;
	event.first = world.CountDay() + 1;
	event.subsequent = 0;
	event.colors = selector.recipients;
	event.message = "Gift from %{name}";
	const Player* player = Settings::Get().GetPlayers().GetCurrent();
	if(player)
	    String::Replace(event.message, "%{name}", player->name);

	world.AddEventDate(event);

	const Colors colors(selector.recipients);
	for(Colors::const_iterator
	    it = colors.begin(); it != colors.end(); ++it)
	{
	    world.GetKingdom(*it).OddFundsResource(funds2);
	}
    }
}
