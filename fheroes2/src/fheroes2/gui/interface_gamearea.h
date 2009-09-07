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

#ifndef H2INTERFACE_GAMEAREA_H
#define H2INTERFACE_GAMEAREA_H

#include "gamedefs.h"
#include "cursor.h"

enum scroll_t
{
    SCROLL_NONE	  = 0x00,
    SCROLL_LEFT	  = 0x01,
    SCROLL_RIGHT  = 0x02,
    SCROLL_TOP	  = 0x04,
    SCROLL_BOTTOM = 0x08,
};

namespace Interface
{
    class GameArea
    {
    public:
	static GameArea & Get(void);
	void Build(void);

	const Rect & GetArea(void) const;
	const Rect & GetRectMaps(void) const;

	Cursor::themes_t GetScrollCursor(void) const;
	bool NeedScroll(void) const;
	void Scroll(void);
	void SetScroll(scroll_t);

	void Center(s16, s16);
	void Center(const Point &pt);
	void Redraw(bool drawFog = true) const;

	void SetUpdateCursor(void);
        void QueueEventProcessing(void);
        
	s16 GetIndexFromMousePoint(const Point & pt) const;

	static void GenerateUltimateArtifactAreaSurface(const u16, Surface &);
	static void SrcRectFixed(Rect & src, Point & dst, const u16 rw, const u16 rh);

    private:
	GameArea();

	Rect	rectArea;
	Rect	rectMaps;
	u16	oldIndexPos;
	u8	scrollDirection;
	bool    updateCursor;
    };
};

#endif
