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

#ifndef H2BATTLE_INTERFACE_H
#define H2BATTLE_INTERFACE_H

#include <string>
#include "icn.h"
#include "button.h"
#include "text.h"
#include "statusbar.h"
#include "gamedefs.h"
#include "dialog.h"
#include "battle_board.h"

namespace Battle
{
    class Arena;
    class Unit;
    class Units;
    class Tower;
    class StatusListBox;
    class Cell;
    class Actions;
    struct TargetInfo;
    struct TargetsInfo;
    struct Result;

    void DialogBattleSettings(void);
    bool DialogBattleSurrender(const HeroBase &, u32);

    enum { OP_IDLE, OP_SRRW, OP_CAST };

    class OpponentSprite
    {
    public:
	OpponentSprite(const Rect &, const HeroBase*, bool);

        const Rect &	GetArea(void) const;
        void		Redraw(void) const;
	void		ResetAnimFrame(u8);
	void		IncreaseAnimFrame(bool loop = false);
	bool		isFinishFrame(void) const;
	bool		isStartFrame(void) const;
	u8		GetColor(void) const;
        const HeroBase* GetHero(void) const;

    private:
        const HeroBase	*base;
	ICN::icn_t	icn;
	u8		animframe;
	u8		animframe_start;
	u8		animframe_count;
	bool		reflect;
        Rect		pos;
    };

    class Status : public Rect
    {
    public:
	Status();

        void		SetPosition(s16, s16);
	void		SetLogs(StatusListBox* logs) {listlog = logs; };
        void		SetMessage(const std::string &, bool = false);
        void		Redraw(void);
	const std::string &
			GetMessage(void) const;

    private:
	Text		bar1;
        Text		bar2;
        const Sprite &	back1;
        const Sprite &	back2;
        std::string	message;
	StatusListBox*	listlog;
    };

    class ArmiesOrder : public Rect
    {
    public:
	ArmiesOrder();

	void	Set(const Rect &, const Units*, u8);
	void	Redraw(const Unit*);
	void	QueueEventProcessing(std::string &);

    private:
	typedef std::pair<const Unit*, Rect> UnitPos;

	void RedrawUnit(const Rect &, const Unit &, bool, bool) const;

	const Units*	orders;
	u8		army_color2;
	Rect		area;
	Surface		sf_color[3];
	std::vector<UnitPos> rects;
    };

    class PopupDamageInfo : public Dialog::FrameBorder
    {
    public:
        PopupDamageInfo();

        void		SetInfo(const Cell*, const Unit*, const Unit*);
        void		Reset(void);
        void		Redraw(u16, u16);

    private:
        const Cell*	cell;
        const Unit*	attacker;
        const Unit*	defender;
        bool		redraw;
    };

    class Interface
    {
    public:
	Interface(Arena &, s32);
	~Interface();

	void	Redraw(void);
	void	HumanTurn(const Unit &, Actions &);
	void	NetworkRedraw();

	const Rect & GetArea(void) const;

	void	SetStatus(const std::string &, bool = false);
	void	SetArmiesOrder(const Units*);
	void	FadeArena(void);

        void	RedrawActionAttackPart1(Unit &, Unit &, const TargetsInfo &);
        void	RedrawActionAttackPart2(Unit &, TargetsInfo &);
        void	RedrawActionSpellCastPart1(const Spell &, s16, const HeroBase*, const std::string &, const TargetsInfo &);
        void	RedrawActionSpellCastPart2(const Spell &, TargetsInfo &);
        void	RedrawActionResistSpell(const Unit &);
        void	RedrawActionMonsterSpellCastStatus(const Unit &, const TargetInfo &);
        void	RedrawActionMove(Unit &, const Indexes &);
        void	RedrawActionFly(Unit &, const Position &);
        void	RedrawActionMorale(Unit &, bool);
	void	RedrawActionLuck(Unit &);
	void	RedrawActionTowerPart1(Tower &, Unit &);
	void	RedrawActionTowerPart2(Tower &, TargetInfo &);
	void	RedrawActionCatapult(u8);
	void	RedrawActionTeleportSpell(Unit &, s16);
	void	RedrawActionEarthQuakeSpell(const std::vector<u8> &);
	void	RedrawActionSummonElementalSpell(const Unit &);
	void	RedrawActionMirrorImageSpell(const Unit &, const Position &);
	void	RedrawActionSkipStatus(const Unit &);
	void	RedrawActionRemoveMirrorImage(const Unit &);
	void	RedrawBridgeAnimation(bool down);

    private:
	void	HumanBattleTurn(const Unit &, Actions &, std::string &);
	void	HumanCastSpellTurn(const Unit &, Actions &, std::string &);

	void	RedrawBorder(void);
	void	RedrawCover(void);
	void	RedrawLowObjects(s16) const;
	void	RedrawHighObjects(s16) const;
	void	RedrawCastle1(const Castle &) const;
	void	RedrawCastle2(const Castle &, s16) const;
	void	RedrawCastle3(const Castle &) const;
	void	RedrawKilled(void);
	void	RedrawInterface(void);
	void	RedrawOpponents(void) const;
	void	RedrawOpponentsFlags(void) const;
	void	RedrawArmies(void) const;
	void	RedrawTroopSprite(const Unit &) const;
	void	RedrawTroopCount(const Unit &) const;
	void	RedrawPocketControls(void) const;

	void	RedrawActionWincesKills(TargetsInfo &);
	void	RedrawActionArrowSpell(const Unit &);
	void	RedrawActionColdRaySpell(Unit &);
	void	RedrawActionDisruptingRaySpell(Unit &);
	void	RedrawActionBloodLustSpell(Unit &);
	void	RedrawActionColdRingSpell(s16, const TargetsInfo &);
	void	RedrawActionElementalStormSpell(const TargetsInfo &);
	void	RedrawActionArmageddonSpell(const TargetsInfo &);
	void	RedrawActionResurrectSpell(Unit &, const Spell &);
	void	RedrawActionLightningBoltSpell(Unit &);
        void	RedrawActionChainLightningSpell(const TargetsInfo &);

	void	RedrawTroopFrameAnimation(Unit &);
	void	RedrawTroopWithFrameAnimation(Unit &, ICN::icn_t, M82::m82_t, bool);
	void	RedrawTargetsWithFrameAnimation(s16, const TargetsInfo &, ICN::icn_t, M82::m82_t);
	void	RedrawTargetsWithFrameAnimation(const TargetsInfo &, ICN::icn_t, M82::m82_t, bool);

	bool	IdleTroopsAnimation(void);
	void	CheckGlobalEvents(LocalEvent &);

	void	ProcessingHeroDialogResult(u8, Actions &);

	void	EventAutoSwitch(const Unit &, Actions &);
	void	EventShowOptions(void);
	void	ButtonAutoAction(const Unit &, Actions &);
	void	ButtonSettingsAction(void);
	void	ButtonSkipAction(Actions &);
	void	ButtonWaitAction(Actions &);
	void	MouseLeftClickBoardAction(u16, const Cell &, Actions &);
	void	MousePressRightBoardAction(u16, const Cell &, Actions &);

	u16	GetBattleCursor(std::string &);
	u16	GetBattleSpellCursor(std::string &);
	u8	GetAllowSwordDirection(u16);

	void	CreateDamageInfoPopup(s16, s16, const Unit &, const Unit &);

	Arena & 	arena;
	Dialog::FrameBorder border;
	Surface		sf_hexagon;
	Surface		sf_shadow;
	Surface		sf_cursor;
	ICN::icn_t	icn_cbkg;
	ICN::icn_t	icn_frng;

	Button		btn_auto;
	Button		btn_settings;
	Button		btn_skip;
	Button		btn_wait;
	Status		status;

	OpponentSprite* opponent1;
	OpponentSprite* opponent2;

	Rect		rectBoard;
	Spell		humanturn_spell;
	bool		humanturn_exit;
	bool		humanturn_redraw;
	u32		animation_frame;
	u8		catapult_frame;

	const Unit*	b_current;
	const Unit*	b_move;
	const Unit*	b_fly;
	const Sprite*	b_current_sprite;
	u8		b_current_alpha;
	Point		p_move;
	Point		p_fly;

	s16		index_pos;
	s16		teleport_src;
	Rect		pocket_book;
	Rect		main_tower;

	StatusListBox*	listlog;
	u16		turn;

	PopupDamageInfo popup;
	ArmiesOrder	armies_order;
    };
}

#endif
