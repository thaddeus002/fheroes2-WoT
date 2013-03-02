/***************************************************************************
 *   Copyright (C) 2009 by Andrey Afletdinov <fheroes2@gmail.com>          *
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

#ifndef H2INTERFACE_ICONS_H
#define H2INTERFACE_ICONS_H

#include "interface_list.h"
#include "interface_border.h"

enum icons_t { ICON_HEROES = 0x01, ICON_CASTLES = 0x02, ICON_ANY = ICON_HEROES|ICON_CASTLES };

namespace Interface
{
    typedef Heroes* HEROES;
    typedef Castle* CASTLE;

    class IconsBar
    {
    public:
	IconsBar(const u8 & count, const Surface & sf) : icons(count), marker(sf), show(true){};

	void SetShow(bool f) { show = f; };
	bool IsShow(void) const { return show; };
	void RedrawBackground(const Point &);

	static u8 GetItemWidth(void);
	static u8 GetItemHeight(void);
	static bool IsVisible(void);

    protected:
	const u8 & icons;
        const Surface & marker;
	bool show;
    };

    void RedrawHeroesIcon(const Heroes &, s16, s16);
    void RedrawCastleIcon(const Castle &, s16, s16);

    class HeroesIcons : public Interface::ListBox<HEROES>, public IconsBar
    {
    public:
	HeroesIcons(const u8 & count, const Surface & sf) : IconsBar(count, sf) {};

	void SetPos(s16, s16);
	void SetShow(bool);

    protected:
	void ActionCurrentUp(void);
	void ActionCurrentDn(void);
	void ActionListDoubleClick(HEROES &);
	void ActionListSingleClick(HEROES &);
	void ActionListPressRight(HEROES &);
	void RedrawItem(const HEROES &, s16 ox, s16 oy, bool current);
	void RedrawBackground(const Point &);
    };

    class CastleIcons : public Interface::ListBox<CASTLE>, public IconsBar
    {
    public:
	CastleIcons(const u8 & count, const Surface & sf) : IconsBar(count, sf) {};

	void SetPos(s16, s16);
	void SetShow(bool);

    protected:
	void ActionCurrentUp(void);
	void ActionCurrentDn(void);
	void ActionListDoubleClick(CASTLE &);
	void ActionListSingleClick(CASTLE &);
	void ActionListPressRight(CASTLE &);
	void RedrawItem(const CASTLE &, s16 ox, s16 oy, bool current);
	void RedrawBackground(const Point &);
    };

    class Basic;

    class IconsPanel : public BorderWindow
    {
    public:
	IconsPanel(Basic &);
    
	void SetPos(s16, s16);
	void SavePosition(void);
	void SetRedraw(void) const;
	void SetRedraw(icons_t) const;

	void Redraw(void);
	void QueueEventProcessing(void);

        u8   CountIcons(void) const;

	void Select(const Heroes &);
	void Select(const Castle &);

	bool IsSelected(icons_t) const;
	void ResetIcons(icons_t = ICON_ANY);
	void HideIcons(icons_t = ICON_ANY);
	void ShowIcons(icons_t = ICON_ANY);
	void RedrawIcons(icons_t = ICON_ANY);
	void SetCurrentVisible(void);

    private:
	Basic & interface;

	u8 icons;
        Surface sfMarker;

	CastleIcons castleIcons;
	HeroesIcons heroesIcons;
    };
}

#endif
