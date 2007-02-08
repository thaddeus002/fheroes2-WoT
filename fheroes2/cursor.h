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
#ifndef H2CURSOR_H
#define H2CURSOR_H

#include "gamedefs.h"

namespace Cursor
{
    typedef enum {
	NONE               = 0x0000,
	// ADVMCO.ICN
	POINTER            = 0x1000,
	WAIT               = 0x1001,
        HEROES             = 0x1002,
	CASTLE             = 0x1003,
        MOVE               = 0x1004,
	FIGHT              = 0x1005,
        BOAT               = 0x1006,
	ANCHOR             = 0x1007,
        CHANGE             = 0x1008,
	ACTION             = 0x1009,
        MOVE2              = 0x100A,
	FIGHT2             = 0x100B,
        BOAT2              = 0x100C,
	ANCHOR2            = 0x100D,
        CHANGE2            = 0x100E,
        ACTION2            = 0x100F,
	MOVE3              = 0x1010,
        FIGHT3             = 0x1011,
	BOAT3              = 0x1012,
        ANCHOR3            = 0x1013,
	CHANGE3            = 0x1014,
        ACTION3            = 0x1015,
	MOVE4              = 0x1016,
        FIGHT4             = 0x1017,
	BOAT4              = 0x1018,
        ANCHOR4            = 0x1019,
	CHANGE4            = 0x101A,
        ACTION4            = 0x101B,
	REDBOAT            = 0x101C,
        REDBOAT2           = 0x101D,
        REDBOAT3           = 0x101E,
        REDBOAT4           = 0x101F,
        SCROLL_TOP         = 0x1020,
        SCROLL_TOPLEFT     = 0x1021,
        SCROLL_RIGHT       = 0x1022,
        SCROLL_BOTTOMLEFT  = 0x1023,
        SCROLL_BOTTOM      = 0x1024,
        SCROLL_BOTTOMRIGHT = 0x1025,
        SCROLL_LEFT        = 0x1026,
        SCROLL_TOPRIGHT    = 0x1027,
	POINTER2           = 0x1028,
	// CMSECO.ICN
        WAR_NONE           = 0x2000,
	WAR_MOVE           = 0x2001,
        WAR_FLIGHT         = 0x2002,
        WAR_ARROW          = 0x2003,
        WAR_HELMET         = 0x2004,
        WAR_INFO           = 0x2005,
        WAR_POINTER        = 0x2006,
        SWORD_TOPLEFT      = 0x2007,
        SWORD_LEFT         = 0x2008,
        SWORD_BOTTOMLEFT   = 0x2009,
        SWORD_BOTTOMRIGHT  = 0x200A,
        SWORD_RIGHT        = 0x200B,
        SWORD_TOPRIGHT     = 0x200C,
        SWORD_TOP          = 0x200D,
        SWORD_BOTTOM       = 0x200E,
        WAR_BROKENARROW    = 0x200F,
        // SPELCO.ICN
        SPELLNONE          = 0x3000,
        SLOW               = 0x3001,
        MASSCURE           = 0x3002,
        CURSE              = 0x3003,
        LIGHTNINGBOLT      = 0x3004,
        CHAINLIGHTNING     = 0x3005,
        CURE               = 0x3006,
        BLESS              = 0x3007,
        FIREBALL           = 0x3008,
        FIREBLAST          = 0x3009,
        TELEPORT           = 0x300A,
        ELEMENTALSTORM     = 0x300B,
        RESURRECT          = 0x300C,
        RESURRECTTRUE      = 0x300D,
        HASTLE             = 0x300E,
        SHIELD             = 0x300F,
        ARMAGEDON          = 0x3010,
        ANTIMAGIC          = 0x3011,
        DISPELMAGIC        = 0x3012,
        BERZERKER          = 0x3013,
        PARALIZE           = 0x3014,
        BLIND              = 0x3015,
        HOLYWORLD          = 0x3016,
        HOLYSHOUT          = 0x3017,
        METEORSHOWER       = 0x3018,
        ANIMATEDEAD        = 0x3019,
        MIRRORIMAGE        = 0x301A,
        BLOODLUST          = 0x301B,
        DEATHRIPPLE        = 0x301C,
        DEATHWAVE          = 0x301D,
        STELSKIN           = 0x301E,
        STONSKIN           = 0x301F,
        DRAGONSLAYER       = 0x3020,
        EARTHQUAKE         = 0x3021,
        DISRUPTINGRAY      = 0x3022,
        COLDRING           = 0x3023,
        COLDRAY            = 0x3024,
        HIPNOTIZE          = 0x3025,
        MAGICARROW         = 0x3026,
    } themes_t;

    void Init(themes_t cursor);
    void Quit(void);

    void Redraw(u16 x, u16 y, bool flag = false);
    void Redraw(bool flag = false);

    void Show(void);
    void Hide(void);
    
    bool Visible(void);

    themes_t Get(void);
    void Set(themes_t cursor);
};

#endif
