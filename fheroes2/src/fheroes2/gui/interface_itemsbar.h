/***************************************************************************
 *   Copyright (C) 2012 by Andrey Afletdinov <fheroes2@gmail.com>          *
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

#ifndef H2INTERFACE_ITEMSBAR_H
#define H2INTERFACE_ITEMSBAR_H

#include <utility>
#include <algorithm>
#include "gamedefs.h"
#include "cursor.h"

namespace Interface
{
    template<class Item>
    struct ItemsBar
    {
    public:
	typedef std::list<Item*> Items;
	typedef typename std::list<Item*>::iterator ItemsIterator;

    protected:
	Rect		barsz;
	Size		colrows;
	Point		curof;
	s8		hspace;
	s8		vspace;
	Surface		backsf;
	SpriteCursor	spcursor;
	Items		items;
	ItemsIterator	topItem;
	ItemsIterator	curItem;

    public:
	ItemsBar() : colrows(0, 0), hspace(0), vspace(0), topItem(items.begin()), curItem(items.end()) {}
	virtual ~ItemsBar(){}

/*
	void		SetColRows(u8, u8);
	void        	SetPos(s16, s16);
	void		SetHSpace(s8);
	void		SetVSpace(s8);
	void		SetContent(const std::list<Item> &);
	void		SetContent(const std::vector<Item> &);
	void		SetItemBackground(const Surface &, const Rect &, u8 colorindex = 0);
	void		SetSpriteCursor(const Surface &);
	
	const Point &	GetPos(void) const;
	const Rect &	GetArea(void) const;
	const Size &	GetColRows(void) const;
	Item*		GetItem(const Point &) const;
	Item*		GetSelectedItem(void) const;
	s8		GetSelectedIndex(void) const;

	bool		isSelected(void) const;

	void		Redraw(void);
	bool		QueueEventProcessing(void);
*/

	virtual void	RedrawItem(Item &, const Rect &, bool) = 0;

        virtual bool	ActionBarSingleClick(Item & item){ return false; }
        virtual bool	ActionBarDoubleClick(Item & item){ return ActionBarSingleClick(item); }
        virtual bool	ActionBarPressRight(Item & item){ return false; }

        virtual bool	ActionBarSingleClick(Item & item, const Point & cursor, const Rect & itemPos){ return ActionBarSingleClick(item); }
        virtual bool	ActionBarDoubleClick(Item & item, const Point & cursor, const Rect & itemPos){ return ActionBarDoubleClick(item); }
        virtual bool	ActionBarPressRight( Item & item, const Point & cursor, const Rect & itemPos){ return ActionBarPressRight(item); }

        virtual bool	ActionBarCursor(Item &, const Point & cursor, const Rect & itemPos){ return false; }

	//body
	void SetColRows(u8 col, u8 row)
	{
	    colrows.w = col;
	    colrows.h = row;
	    RescanSize();
	}

	void SetContent(std::list<Item> & content)
	{
	    items.clear();
	    for(typename std::list<Item>::iterator
		it = content.begin(); it != content.end(); ++it)
		items.push_back(&(*it));
	    topItem = items.begin();
	    curItem = items.end();
	}

	void SetContent(std::vector<Item> & content)
	{
	    items.clear();
	    for(typename std::vector<Item>::iterator
		it = content.begin(); it != content.end(); ++it)
		items.push_back(&(*it));
	    topItem = items.begin();
	    curItem = items.end();
	}

	void SetPos(s16 px, s16 py)
	{
	    barsz.x = px;
	    barsz.y = py;
	}

	void SetHSpace(s8 val)
	{
	    hspace = val;
	    RescanSize();
	}

	void SetVSpace(s8 val)
	{
	    vspace = val;
	    RescanSize();
	}

	void SetItemBackground(const Surface & srcsf)
	{
	    SetItemBackground(srcsf, Rect(0, 0, srcsf.w(), srcsf.h()));
	}

	void SetItemBackground(const Surface & srcsf, const Rect & srcrt, u8 colorindex = 0)
	{
	    backsf.Set(srcrt.w, srcrt.h);
	    srcsf.Blit(srcrt, 0, 0, backsf);
	    if(colorindex) Cursor::DrawCursor(backsf, colorindex, true);
	    RescanSize();
	    RescanCursorOffset();
	}

	void SetSpriteCursor(const Surface & sf)
	{
	    spcursor.SetSprite(sf);
	    RescanSize();
	    RescanCursorOffset();
	}

	Item* GetSelectedItem(void) const
	{
	    return curItem != items.end() ? *curItem : NULL;
	}

	s8 GetSelectedIndex(void) const
	{
	    return curItem != items.end() ? std::distance(items.begin(), curItem) : -1;
	}

	Item* GetItem(const Point & pt)
	{
	    ItemsIterator posItem = GetItemIter(pt);
	    return posItem != items.end() ? *posItem : NULL;
	}

	const Point & GetPos(void) const
	{
	    return barsz;
	}

	const Rect & GetArea(void) const
	{
	    return barsz;
	}

	const Size & GetColRows(void) const
	{
	    return colrows;
	}

	bool isSelected(void) const
	{
	    return NULL != GetSelectedItem();
	}

	void Redraw(Surface & dstsf = Display::Get())
	{
	    spcursor.Hide();
	    Point dstpt(barsz);
	    ItemsIterator posItem = topItem;

	    for(u16 yy = 0; yy < colrows.h; ++yy)
	    {
		for(u16 xx = 0; xx < colrows.w; ++xx)
		{
		    if(backsf.isValid())
			backsf.Blit(dstpt, dstsf);

		    if(posItem != items.end())
		    {
			RedrawItem(**posItem, Rect(dstpt, backsf.w(), backsf.h()), curItem == posItem);

			if(curItem == posItem)
			    spcursor.Show(dstpt + curof);

			++posItem;
		    }

		    dstpt.x += hspace + backsf.w();
		}

		dstpt.x = barsz.x;
		dstpt.y += vspace + backsf.h();
	    }
	}

        bool QueueEventProcessing(void)
        {
            LocalEvent & le = LocalEvent::Get();
            //Cursor & cursor = Cursor::Get();

            //le.MousePressLeft(buttonPgUp) ? buttonPgUp.PressDraw() : buttonPgUp.ReleaseDraw();
            //le.MousePressLeft(buttonPgDn) ? buttonPgDn.PressDraw() : buttonPgDn.ReleaseDraw();

            if(items.size())
            {
/*
            if((le.MouseClickLeft(buttonPgUp) || (useHotkeys && le.KeyPress(KEY_PAGEUP))) &&
                    (top > content->begin()))
            {
                cursor.Hide();
                top = (top - content->begin() > maxItems ? top - maxItems : content->begin());
                UpdateSplitterRange();
                splitter.Move(top - content->begin());
                return true;
            }
            else
            if((le.MouseClickLeft(buttonPgDn) || (useHotkeys && le.KeyPress(KEY_PAGEDOWN))) &&
                    (top + maxItems < content->end()))
            {
                cursor.Hide();
                top += maxItems;
                if(top + maxItems > content->end()) top = content->end() - maxItems;
                UpdateSplitterRange();
                splitter.Move(top - content->begin());
                return true;
            }
            else
            if(useHotkeys && le.KeyPress(KEY_UP) && (cur > content->begin()))
            {
                cursor.Hide();
                --cur;
                SetCurrentVisible();
                ActionCurrentUp();
                return true;
            }
            else
            if(useHotkeys && le.KeyPress(KEY_DOWN) && (cur < (content->end() - 1)))
            {
                cursor.Hide();
                ++cur;
                SetCurrentVisible();
                ActionCurrentDn();
                return true;
            }
            else
            if((le.MouseWheelUp(rtAreaItems) || le.MouseWheelUp(splitter.GetRect())) && (top > content->begin()))
            {
                cursor.Hide();
                --top;
                splitter.Backward();
                return true;
            }
            else
            if((le.MouseWheelDn(rtAreaItems) || le.MouseWheelDn(splitter.GetRect())) && (top < (content->end() - maxItems)))
            {
                cursor.Hide();
                ++top;
                splitter.Forward();
                return true;
            }
            else
            if(le.MousePressLeft(splitter.GetRect()) && (content->size() > maxItems))
            {
                cursor.Hide();
                UpdateSplitterRange();
                s16 seek = (le.GetMouseCursor().y - splitter.GetRect().y) * 100 / splitter.GetStep();
                if(seek < splitter.Min()) seek = splitter.Min();
                else
                if(seek > splitter.Max()) seek = splitter.Max();
                top = content->begin() + seek;
                splitter.Move(seek);
                return true;
            }
*/
		std::pair<ItemsIterator, Rect> iterPos = GetItemIterPos(le.GetMouseCursor());

		if(iterPos.first != items.end())
		{
            	    if(ActionBarCursor(**iterPos.first, le.GetMouseCursor(), iterPos.second))
                	return true;
            	    else
		    if(le.MouseClickLeft(iterPos.second))
            	    {
            		if(iterPos.first == curItem)
                    	    return ActionBarDoubleClick(**curItem, le.GetMouseCursor(), iterPos.second);
            		else
                	{
                    	    curItem = iterPos.first;
                    	    return ActionBarSingleClick(**curItem, le.GetMouseCursor(), iterPos.second);
            		}
            	    }
            	    else
            	    if(le.MousePressRight(iterPos.second))
            		return ActionBarPressRight(**iterPos.first, le.GetMouseCursor(), iterPos.second);
		}
	    }

            return false;
        }

    private:
	void RescanSize(void)
	{
	    barsz.w = colrows.w ? colrows.w * backsf.w() + (colrows.w - 1) * hspace : 0;
	    barsz.h = colrows.h ? colrows.h * backsf.h() + (colrows.h - 1) * vspace : 0;
	}

	void RescanCursorOffset(void)
	{
	    curof.x = spcursor.Sprite() ? (backsf.w() - spcursor.Sprite()->w()) / 2 : 0;
	    curof.y = spcursor.Sprite() ? (backsf.h() - spcursor.Sprite()->h()) / 2 : 0;
	}

	ItemsIterator GetItemIter(const Point & pt)
	{
	    return GetItemIterPos(pt).first;
	}

	std::pair<ItemsIterator, Rect> GetItemIterPos(const Point & pt)
	{
	    Rect dstrt(barsz, backsf.w(), backsf.h());
	    ItemsIterator posItem = topItem;

	    for(u16 yy = 0; yy < colrows.h; ++yy)
	    {
		for(u16 xx = 0; xx < colrows.w; ++xx)
		{
		    if(posItem != items.end())
		    {
			if(dstrt & pt)
			    return std::pair<ItemsIterator, Rect>(posItem, dstrt);
			++posItem;
		    }

		    dstrt.x += hspace + backsf.w();
		}

		dstrt.x = barsz.x;
		dstrt.y += vspace + backsf.h();
	    }

	    return std::pair<ItemsIterator, Rect>(items.end(), Rect());
	}
    };
}

#endif
