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

#ifndef H2GAMEINTERFACE_H
#define H2GAMEINTERFACE_H

#include "gamedefs.h"
#include "game.h"
#include "cursor.h"
#include "direction.h"
#include "players.h"
#include "interface_border.h"
#include "interface_radar.h"
#include "interface_buttons.h"
#include "interface_icons.h"
#include "interface_status.h"
#include "interface_gamearea.h"
#include "interface_cpanel.h"

enum redraw_t
{
    REDRAW_RADAR     = 0x01,
    REDRAW_HEROES    = 0x02,
    REDRAW_CASTLES   = 0x04,
    REDRAW_BUTTONS   = 0x08,
    REDRAW_STATUS    = 0x10,
    REDRAW_BORDER    = 0x20,
    REDRAW_GAMEAREA  = 0x40,
    REDRAW_CURSOR    = 0x80,

    REDRAW_ICONS     = REDRAW_HEROES | REDRAW_CASTLES,
    REDRAW_ALL       = 0xFF
};

class Castle;
class Heroes;

namespace Maps
{
    class Tiles;
}

namespace GameFocus
{
    enum { UNSEL = FOCUS_UNSEL, HEROES = FOCUS_HEROES, CASTLE = FOCUS_CASTLE, FIRSTHERO };
}

namespace Interface
{
    Castle*	GetFocusCastle(void);
    Heroes*	GetFocusHeroes(void);
    u8		GetFocusType(void);
    Point	GetFocusCenter(void);

    class Basic
    {
    public:
    	static Basic & Get(void);


	bool    NeedRedraw(void) const;
	void    SetRedraw(u8);
    	void	Redraw(u8 f = 0);

	const Rect & GetScrollLeft(void) const;
	const Rect & GetScrollRight(void) const;
	const Rect & GetScrollTop(void) const;
	const Rect & GetScrollBottom(void) const;

	s32 GetDimensionDoorDestination(const s32, const u8, bool) const;

	GameArea &	GetGameArea(void);
	Radar &		GetRadar(void);
	IconsPanel &	GetIconsPanel(void);
	ButtonsArea &	GetButtonsArea(void);
	StatusWindow &	GetStatusWindow(void);
	ControlPanel &	GetControlPanel(void);

	void	SetFocus(Heroes*);
	void	SetFocus(Castle*);
	void	ResetFocus(u8);
	void	RedrawFocus(void);

	void	SetHideInterface(bool);

	void	EventSwitchHeroSleeping(void);
	void	EventDefaultAction(void);
	void	EventOpenFocus(void);
	void	EventSaveGame(void);
        void	EventPuzzleMaps(void);
        void	EventGameInfo(void);
	void	EventSystemDialog(void);
        void	EventNextHero(void);
	void	EventNextTown(void);
	void	EventContinueMovement(void);
	void	EventKingdomInfo(void);
	void	EventCastSpell(void);
        void	EventSwitchShowRadar(void);
        void	EventSwitchShowStatus(void);
        void	EventSwitchShowButtons(void);
        void	EventSwitchShowIcons(void);
        void	EventSwitchShowControlPanel(void);
        void	EventDebug1(void);
	void	EventDebug2(void);

	void	EventLoadGame(Game::menu_t &);
	void	EventAdventureDialog(Game::menu_t &);
	void	EventFileDialog(Game::menu_t &);
	void	EventEndTurn(Game::menu_t &);
	void	EventExit(Game::menu_t &);
	void	EventDigArtifact(Game::menu_t &);
	void	EventKeyArrowPress(Direction::vector_t);

	Game::menu_t	StartGame(void);

	void MouseCursorAreaClickLeft(s32);
	void MouseCursorAreaPressRight(s32);

        static Cursor::themes_t GetCursorTileIndex(const s32);
	static Cursor::themes_t GetCursorFocusCastle(const Castle &, const Maps::Tiles &);
	static Cursor::themes_t GetCursorFocusHeroes(const Heroes &, const Maps::Tiles &);
	static Cursor::themes_t GetCursorFocusShipmaster(const Heroes &, const Maps::Tiles &);

    private:
	Basic();
	void RedrawSystemInfo(s16, s16, u32);
	void ShowPathOrStartMoveHero(Heroes*, const s32);
	void MoveHeroFromArrowKeys(Heroes & hero, Direction::vector_t);
	Game::menu_t HumanTurn(bool);

	GameArea	gameArea;
	Radar		radar;
	IconsPanel	iconsPanel;
	ButtonsArea	buttonsArea;
	StatusWindow	statusWindow;
	ControlPanel	controlPanel;

	u8 redraw;

	Rect scrollLeft;
	Rect scrollRight;
	Rect scrollBottom;
	Rect scrollTop;

	Text system_info;
    };
}

#endif
