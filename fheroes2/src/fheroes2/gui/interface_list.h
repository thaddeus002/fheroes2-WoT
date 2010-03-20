/***************************************************************************
 *   Copyright (C) 2010 by Andrey Afletdinov <fheroes2@gmail.com>          *
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

#ifndef H2INTERFACE_LIST_H
#define H2INTERFACE_LIST_H

#include "icn.h"
#include "button.h"
#include "splitter.h"
#include "sprite.h"
#include "cursor.h"
#include "gamedefs.h"

namespace Interface
{
    template<class Item> 
    class ListBox
    {
    public:
	typedef std::vector<Item> Items;
	typedef typename std::vector<Item>::iterator ItemsIterator;

	ListBox(const Point & pt) : ptRedraw(pt), maxItems(0), content(NULL) {};
	virtual ~ListBox(){};

	virtual void RedrawItem(const Item &, u16, u16, bool) = 0;
	virtual void RedrawBackground(const Point &) = 0;

	virtual void ActionCurrentUp(void) = 0;
	virtual void ActionCurrentDn(void) = 0;
	virtual void ActionListDoubleClick(Item &) = 0;
	virtual void ActionListSingleClick(Item &) = 0;

	/*
	void SetScrollButtonUp(const ICN::icn_t, const u16, const u16, const Point &);
	void SetScrollButtonDn(const ICN::icn_t, const u16, const u16, const Point &);
	void SetScrollSplitter(const Sprite &, const Rect &);
	void SetAreaMaxItems(const u8);
	void SetAreaItems(const Rect &);
	void SetListContent(std::vector<Item> &);
	void Redraw(void);
	void QueueEventProcessing(void);
	Item & GetCurrent(void);
	void RemoveSelected(void);
	bool isSelected(void) const;
	void Unselect(void);
	*/

	void SetScrollButtonUp(const ICN::icn_t icn, const u16 index1, const u16 index2, const Point & pos)
	{
	    buttonPgUp.SetSprite(icn, index1, index2);
    	    buttonPgUp.SetPos(pos);
        };

	void SetScrollButtonDn(const ICN::icn_t icn, const u16 index1, const u16 index2, const Point & pos)
	{
	    buttonPgDn.SetSprite(icn, index1, index2);
    	    buttonPgDn.SetPos(pos);
        };

	void SetScrollSplitter(const Sprite & sp, const Rect & area)
	{
	    splitter.SetOrientation(Splitter::VERTICAL);
	    splitter.SetArea(area);
	    splitter.SetSprite(sp);
	};

	void SetAreaMaxItems(const u8 max)
	{
	    maxItems = max;
	};

	void SetAreaItems(const Rect & rt)
	{
	    rtAreaItems = rt;
	};

	void SetListContent(std::vector<Item> & list)
	{
	    content = &list;
	    cur = content->begin();
	    top = content->begin();
	    splitter.SetRange(0, (maxItems < list.size() ? list.size() - maxItems : 0));
	};

	void Redraw(void)
	{
	    Cursor::Get().Hide();

            RedrawBackground(ptRedraw);

            buttonPgUp.Draw();
            buttonPgDn.Draw();
            splitter.Redraw();

            ItemsIterator curt = top;
            ItemsIterator last = top + maxItems < content->end() ? top + maxItems : content->end();
            for(; curt != last; ++curt)
                RedrawItem(*curt, rtAreaItems.x, rtAreaItems.y + (curt - top) * rtAreaItems.h / maxItems, curt == cur);
        };

	Item & GetCurrent(void)
	{
	    return *cur;
	};

	void RemoveSelected(void)
	{
	    if(content && cur != content->end()) content->erase(cur);
	};

	bool isSelected(void) const
	{
	    return content && cur != content->end();
	};

	void Unselect(void)
	{
	    if(content) cur = content->end();
	};

	void QueueEventProcessing(void)
	{
	    LocalEvent & le = LocalEvent::Get();
	    Cursor & cursor = Cursor::Get();

	    le.MousePressLeft(buttonPgUp) ? buttonPgUp.PressDraw() : buttonPgUp.ReleaseDraw();
	    le.MousePressLeft(buttonPgDn) ? buttonPgDn.PressDraw() : buttonPgDn.ReleaseDraw();
    
	    if(!content) return;

	    if((le.MouseClickLeft(buttonPgUp) || le.KeyPress(KEY_PAGEUP)) && (top > content->begin()))
	    {
		cursor.Hide();
		top = (top - content->begin() > maxItems ? top - maxItems : content->begin());
    		splitter.Move(top - content->begin());
	    }
	    else
	    if((le.MouseClickLeft(buttonPgDn) || le.KeyPress(KEY_PAGEDOWN)) && (top + maxItems < content->end()))
	    {
		cursor.Hide();
		top += maxItems;
		if(top + maxItems > content->end()) top = content->end() - maxItems;
		splitter.Move(top - content->begin());
	    }
	    else
	    if(le.KeyPress(KEY_UP) && (cur > content->begin()))
	    {
		cursor.Hide();
		--cur;
		if((cur < top) || (cur > top + maxItems - 1))
		{
		    top = cur;
		    if(top + maxItems > content->end()) top = content->end() - maxItems;
		}
		splitter.Move(top - content->begin());

		ActionCurrentUp();
	    }
	    else
	    if(le.KeyPress(KEY_DOWN) && (cur < (content->end() - 1)))
	    {
		cursor.Hide();
		++cur;
		if((cur < top) || (cur > top + maxItems - 1))
		{
		    top = cur - maxItems + 1;
		    if(top + maxItems > content->end()) top = content->end() - maxItems;
		    else
		    if(top < content->begin()) top = content->begin();
		}
		splitter.Move(top - content->begin());

		ActionCurrentDn();
	    }
	    else
	    if((le.MouseWheelUp(rtAreaItems) || le.MouseWheelUp(splitter.GetRect())) && (top > content->begin()))
	    {
		cursor.Hide();
		--top;
		splitter.Backward();
	    }
	    else
	    if((le.MouseWheelDn(rtAreaItems) || le.MouseWheelDn(splitter.GetRect())) && (top < (content->end() - maxItems)))
	    {
		cursor.Hide();
    		++top;
		splitter.Forward();
	    }
	    else
	    if(le.MousePressLeft(splitter.GetRect()) && (content->size() > maxItems))
	    {
		cursor.Hide();
		s16 seek = (le.GetMouseCursor().y - splitter.GetRect().y) * 100 / splitter.GetStep();
		if(seek < splitter.Min()) seek = splitter.Min();
		else
		if(seek > splitter.Max()) seek = splitter.Max();
		top = content->begin() + seek;
		splitter.Move(seek);
	    }
	    else
	    if(le.MouseClickLeft(rtAreaItems) && content->size())
	    {
		float offset = (le.GetMouseReleaseLeft().y - rtAreaItems.y) * maxItems / rtAreaItems.h;

		cursor.Hide();

		ItemsIterator last  = content->end() - 1;
		ItemsIterator click = top + static_cast<size_t>(offset);

		if(click > last) click = last;

		if(click == cur)
		{
		    ActionListDoubleClick(*cur);
		}
		else
		{
		    cur = click;
		    ActionListSingleClick(*cur);
		}
	    }
	};

    protected:
	Point ptRedraw;
	Rect rtAreaItems;

	Button buttonPgUp;
	Button buttonPgDn;

	Splitter splitter;

	u8 maxItems;

	Items *content;
	ItemsIterator cur;
	ItemsIterator top;
    };
};

#endif
