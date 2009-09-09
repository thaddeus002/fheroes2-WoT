/***************************************************************************
 *   Copyright (C) 2009 by Andrey Afletdinov                               *
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

#ifndef H2INTERFACE_ICONS_H
#define H2INTERFACE_ICONS_H

#include "gamedefs.h"
#include "button.h"
#include "dialog.h"
#include "splitter.h"

namespace Interface
{
    class IconsBar
    {
    public:
	IconsBar();

	void            SetPos(s16, s16);
	void            SetCount(u8);
	void            Redraw(void) const;

	const std::vector<Rect> & GetCoords(void) const;
        const Rect &    GetArea(void) const;
        Splitter &      GetSplitter(void);

    protected:
	std::vector<Rect> coords;
        Rect              area;
        Surface           sf_cursor;
        SpriteCursor      sp_cursor;
        Splitter          splitter;
        bool              visible;
    };

    class HeroesIcons : public IconsBar
    {
    public:
        HeroesIcons();

	void		SetPos(s16, s16);
	u32             Size(void) const;

        void            Redraw(void);

        void            Reset(void);
        bool            Prev(void);
        bool            Next(void);
        void            SetTop(const u8 index);
        void            Hide(void);
        void            Unselect(void);
        bool            isSelected(void) const;

        void            Select(const Heroes *hero);
        const Heroes *  Selected(void) const;

        const Heroes *  MouseClickLeft(void) const;
        const Heroes *  MousePressRight(void) const;


    private:
        static void RedrawIcon(const Heroes & hero, const Rect & dst);
        std::vector<Heroes *>::const_iterator it_top;
        std::vector<Heroes *>::const_iterator it_cur;
    };


    class CastleIcons : public IconsBar
    {
    public:
	CastleIcons();

	void		SetPos(s16, s16);
	u32             Size(void) const;

        void            Redraw(void);

        void            Reset(void);
        bool            Prev(void);
        bool            Next(void);
        void            SetTop(const u8 index);
        void            Hide(void);
        void            Unselect(void);
        bool            isSelected(void) const;

	void            Select(const Castle *castle);
        const Castle *  Selected(void) const;

        const Castle *  MouseClickLeft(void) const;
        const Castle *  MousePressRight(void) const;

    private:
        static void RedrawIcon(const Castle & castle, const Rect & dst);
        std::vector<Castle *>::const_iterator it_top;
        std::vector<Castle *>::const_iterator it_cur;
    };

    class IconsPanel : protected Rect
    {
    public:
	static IconsPanel & Get(void);
    
	void SetPos(s16, s16);
	void Redraw(void);
	void QueueEventProcessing(void);

        const Rect & GetArea(void) const;
        u8           CountIcons(void) const;

	HeroesIcons & GetHeroesBar(void);
	CastleIcons & GetCastleBar(void);

    private:
	IconsPanel();

	Button	buttonScrollHeroesUp;
	Button	buttonScrollCastleUp;
	Button	buttonScrollHeroesDown;
	Button	buttonScrollCastleDown;
	
	HeroesIcons heroesIcons;
	CastleIcons castleIcons;

	u8      count_icons;
	Dialog::FrameBorder border;
    };
};

#endif
