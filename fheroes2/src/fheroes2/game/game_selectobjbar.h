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

#ifndef H2SELECTOBJECTBAR_H
#define H2SELECTOBJECTBAR_H

#include <vector>
#include "gamedefs.h"
#include "game.h"
#include "splitter.h"

namespace Game
{

class SelectObjectBar
{
public:
    SelectObjectBar(const s16 px, const s16 py);

    static void 		DrawCursor(Surface & sf);

    void			Redraw(void) const;

    const std::vector<Rect> &	GetCoords(void) const;
    const Rect & 		GetArea(void) const;

    protected:
	const Point		pos_pt;
	std::vector<Rect>	coords;
	Rect			max_area;
    };

    class SelectBarCastle : public SelectObjectBar
    {
    public:
	static SelectBarCastle & Get(void);

	u32		Size(void) const;

	void		Reset(void);
	bool		Prev(void);
	bool		Next(void);
	void		SetTop(const u8 index);

	void		Redraw(void);
	void		Redraw(const Castle *castle);

	void		Hide(void);

	void		Unselect(void);
	void		Select(const Castle *castle);
	bool		isSelected(void) const;
	const Castle *	Selected(void) const;

	const Castle *	MouseClickLeft(void) const;
	const Castle *	MousePressRight(void) const;

	Splitter &	GetSplitter(void);

    private:
	SelectBarCastle();
	static void RedrawIcon(const Castle & castle, const Rect & dst);

	Surface			sf_cursor;
	SpriteCursor		sp_cursor;
	Splitter 		splitter;

	std::vector<Castle *>::const_iterator it_top;
	std::vector<Castle *>::const_iterator it_cur;

	bool visible;
    };

    class SelectBarHeroes : public SelectObjectBar
    {
    public:
	static SelectBarHeroes & Get(void);

	u32		Size(void) const;

	void		Reset(void);
	bool		Prev(void);
	bool		Next(void);
	void		SetTop(const u8 index);

	void		Redraw(void);
	void		Redraw(const Heroes *hero);

	void		Hide(void);

	void		Unselect(void);
	void		Select(const Heroes *hero);
	bool		isSelected(void) const;
	const Heroes *	Selected(void) const;

	const Heroes *	MouseClickLeft(void) const;
	const Heroes *	MousePressRight(void) const;

	Splitter &	GetSplitter(void);

    private:
	SelectBarHeroes();
	static void RedrawIcon(const Heroes & hero, const Rect & dst);

	Surface			sf_cursor;
	SpriteCursor		sp_cursor;
	Splitter 		splitter;

	std::vector<Heroes *>::const_iterator it_top;
	std::vector<Heroes *>::const_iterator it_cur;

	bool visible;
    };
};

#endif
