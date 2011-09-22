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

#ifndef H2BATTLE2_INTERFACE_H
#define H2BATTLE2_INTERFACE_H

#include <string>
#include "icn.h"
#include "button.h"
#include "text.h"
#include "statusbar.h"
#include "gamedefs.h"
#include "dialog.h"

namespace Battle2
{
    class Arena;
    class Stats;
    class QueueMessage;
    class Actions;
    class Tower;
    struct TargetInfo;
    struct TargetsInfo;
    struct Result;
    class StatusListBox;
    class Cell;

    void DialogBattleSettings(void);
    bool DialogBattleSurrender(const HeroBase &, u32);

    enum { OP_IDLE, OP_SRRW, OP_CAST };

    class OpponentSprite
    {
    public:
	OpponentSprite(const Rect &, const HeroBase *, bool);

        const Rect & GetArea(void) const;
        void Redraw(void) const;
	void ResetAnimFrame(u8);
	void IncreaseAnimFrame(bool loop = false);
	bool isFinishFrame(void) const;
	bool isStartFrame(void) const;
	u8   GetColor(void) const;
        const HeroBase* GetHero(void) const;

    protected:
        const HeroBase *base;
	ICN::icn_t icn;
	u8 animframe;
	u8 animframe_start;
	u8 animframe_count;
	bool reflect;
        Rect pos;
    };

    class Status : public Rect
    {
    public:
	Status();
    
        void SetPosition(s16, s16);
	void SetLogs(StatusListBox* logs) {listlog = logs; };
        void SetMessage(const std::string &, bool = false);
        void Redraw(void);
	const std::string & GetMessage(void) const;

    protected:
	Text bar1;
        Text bar2;
        const Sprite & back1;
        const Sprite & back2;
        std::string message;
	StatusListBox* listlog;
    };

    class PopupDamageInfo : public Dialog::FrameBorder
    {
    public:
        PopupDamageInfo();

        void SetInfo(const Cell*, const Stats*, const Stats*);
        void Reset(void);
        void Redraw(u16, u16);

    protected:
        const Cell*  cell;
        const Stats* attacker;
        const Stats* defender;
        bool   redraw;
    };

    class Interface
    {
    public:
	Interface(Arena &, s32);
	~Interface();

	void Redraw(void);
	void HumanTurn(const Stats &, Actions &);
	bool NetworkTurn(Result &);

	const Rect & GetArea(void) const;

	void SetStatus(const std::string &, bool = false);
	void FadeArena(void);

        void RedrawActionAttackPart1(Stats &, Stats &, const TargetsInfo &);
        void RedrawActionAttackPart2(Stats &, TargetsInfo &);
        void RedrawActionSpellCastPart1(const Spell &, u16, const HeroBase*, const std::string &, const TargetsInfo &);
        void RedrawActionSpellCastPart2(const Spell &, TargetsInfo &);
        void RedrawActionResistSpell(const Stats &);
        void RedrawActionMonsterSpellCastStatus(const Stats &, const TargetInfo &);
        void RedrawActionMove(Stats &, const std::vector<u16> &);
        void RedrawActionFly(Stats &, u16);
        void RedrawActionMorale(Stats &, bool);
	void RedrawActionLuck(Stats &);
	void RedrawActionTowerPart1(Tower &, Stats &);
	void RedrawActionTowerPart2(Tower &, TargetInfo &);
	void RedrawActionCatapult(u8);
	void RedrawActionTeleportSpell(Stats &, const u16);
	void RedrawActionEarthQuakeSpell(const std::vector<u8> &);
	void RedrawActionSummonElementalSpell(const Stats &);
	void RedrawActionMirrorImageSpell(const Stats &, u16);
	void RedrawActionRemoveMirrorImage(const Stats &);
	void RedrawActionSkipStatus(const Stats &);

	void HumanBattleTurn(const Stats &, Actions &, std::string &);
	void HumanCastSpellTurn(const Stats &, Actions &, std::string &);

	void RedrawBorder(void);
	void RedrawCover(void);
	void RedrawLowObjects(const u16) const;
	void RedrawHighObjects(const u16) const;
	void RedrawCastle1(void) const;
	void RedrawCastle2(const u16) const;
	void RedrawCastle3(void) const;
	void RedrawKilled(void);
	void RedrawInterface(void);
	void RedrawOpponents(void) const;
	void RedrawOpponentsFlags(void) const;
	void RedrawArmies(void) const;
	void RedrawTroopSprite(const Stats &, const Rect &) const;
	void RedrawTroopCount(const Stats &, const Rect &) const;
	void RedrawPocketControls(void) const;

	void RedrawActionWincesKills(TargetsInfo &);
	void RedrawActionArrowSpell(const Stats &);
	void RedrawActionColdRaySpell(Stats &);
	void RedrawActionDisruptingRaySpell(Stats &);
	void RedrawActionBloodLustSpell(Stats &);
	void RedrawActionColdRingSpell(const u16, const TargetsInfo &);
	void RedrawActionElementalStormSpell(const TargetsInfo &);
	void RedrawActionArmageddonSpell(const TargetsInfo &);
	void RedrawActionResurrectSpell(Stats &, const Spell &);
	void RedrawActionLightningBoltSpell(Stats &);
        void RedrawActionChainLightningSpell(const TargetsInfo &);

	void RedrawTroopFrameAnimation(Stats &);
	void RedrawTroopWithFrameAnimation(Stats &, ICN::icn_t, M82::m82_t, bool);
	void RedrawTargetsWithFrameAnimation(const u16, const TargetsInfo &, ICN::icn_t, M82::m82_t);
	void RedrawTargetsWithFrameAnimation(const TargetsInfo &, ICN::icn_t, M82::m82_t, bool);
	void RedrawBridgeAnimation(bool down);

	bool IdleTroopsAnimation(void);
	void CheckGlobalEvents(LocalEvent &);

	void ProcessingHeroDialogResult(u8, Actions &);

	void EventAutoSwitch(const Stats &, Actions &);
	void EventShowOptions(void);
	void ButtonAutoAction(const Stats &, Actions &);
	void ButtonSettingsAction(void);
	void ButtonSkipAction(Actions &);
	void ButtonWaitAction(Actions &);
	void MouseLeftClickBoardAction(u16, s16, Actions &);
	void MousePressRightBoardAction(u16, s16, Actions &);

	u16 GetBattleCursor(const Point &, std::string &);
	u16 GetBattleSpellCursor(const Point &, std::string &);
	u8 GetAllowSwordDirection(u16);

    protected:
	void CreateDamageInfoPopup(s16, s16, const Stats &, const Stats &);

	Arena & arena;
	Dialog::FrameBorder border;
	Surface sf_hexagon;
	Surface sf_shadow;
	Surface sf_cursor;
	ICN::icn_t icn_cbkg;
	ICN::icn_t icn_frng;

	Button btn_auto;
	Button btn_settings;
	Button btn_skip;
	Button btn_wait;
	Status status;

	OpponentSprite* opponent1;
	OpponentSprite* opponent2;

	Rect rectBoard;
	Spell humanturn_spell;
	bool humanturn_exit;
	bool humanturn_redraw;
	u32 animation_frame;
	u8 catapult_frame;

	const Stats* b_current;
	const Stats* b_move;
	const Stats* b_fly;
	const Sprite* b_current_sprite;
	u8 b_current_alpha;
	Point p_move;
	Point p_fly;

	u16 teleport_src;
	Rect pocket_book;

	bool openlog;
	StatusListBox* listlog;
	u16 turn;

	PopupDamageInfo popup;
    };
}

#endif
