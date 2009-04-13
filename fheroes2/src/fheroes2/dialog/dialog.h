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
#ifndef H2DIALOG_H
#define H2DIALOG_H

#include <string>
#include <list>
#include "gamedefs.h"
#include "monster.h"
#include "spell.h"
#include "skill.h"
#include "text.h"
#include "resource.h"
#include "game.h"

#define	SHADOWWIDTH	16

#define BOXAREA_WIDTH   245
#define BUTTON_HEIGHT   50

class Castle;
class Heroes;
class Surface;

namespace Army
{
    class Troop;
    class BattleTroop;
};

namespace Maps
{
    class FileInfo;
};

namespace Dialog
{
    typedef enum
    {
	ZERO	= 0x0000,
	YES     = 0x0001,
        OK      = 0x0002,
        NO      = 0x0004,
        CANCEL  = 0x0008,
        DISMISS = 0x0010,
        UPGRADE = 0x0020,
        MAX     = 0x0040,
	PREV	= 0x0080,
	NEXT	= 0x0100,

	WORLD	= 0x0200,
	PUZZLE	= 0x0400,
	INFO	= 0x0800,
	DIG	= 0x1000,

	READONLY= 0x2000,
	BATTLE	= 0x4000,
	BUTTONS = (YES|OK|NO|CANCEL),
    } answer_t;

    answer_t AdventureOptions(const bool enabledig);
    Game::menu_t FileOptions(void);
    u8    SystemOptions(void);
    const Maps::FileInfo * SelectFileInfo(const std::list<Maps::FileInfo *> & list);

    // show info cell maps
    void QuickInfo(const Maps::Tiles & tile);
    void QuickInfo(const Castle & castle);
    void QuickInfo(const Heroes & heroes);
    
    // buttons: OK : CANCEL : OK|CANCEL : YES|NO
    u16 Message(const std::string &header, const std::string &message, Font::type_t ft, u16 buttons = 0);

    // other info
    Skill::Secondary::skill_t LevelUpSelectSkill(const std::string &header, const Skill::Secondary & sec1, const Skill::Secondary & sec2);
    bool SelectGoldOrExp(const std::string &header, const std::string &message, const u16 gold, const u16 expr);

    void SpellInfo(const std::string &header, const std::string &message, const Spell::spell_t spell, const bool ok_button = true);
    void SkillInfo(const Skill::Secondary::skill_t skill, const Skill::Level::type_t level, const bool ok_button = true);
    void SkillInfo(const std::string &header, const std::string &message, const Skill::Secondary::skill_t skill, const Skill::Level::type_t level, const bool ok_button = true);
    void SkillInfo(const std::string &header, const std::string &message, const Skill::Primary::skill_t skill);
    u16  SpriteInfo(const std::string &header, const std::string &message, const Surface & sprite, u16 buttons = Dialog::OK);
    u16 ResourceInfo(const std::string &header, const std::string &message, const Resource::funds_t &rs, bool yesNo = false);

    Skill::Primary::skill_t SelectSkillFromArena(void);

    // redistribute count
    bool SelectCount(const std::string &header, u16 min, u16 max, u16 & res);
    bool InputString(const std::string &, std::string &);

    // recruit monster
    u16 RecruitMonster(const Monster & monster, u16 available);
    void DwellingInfo(const Monster & monster, u16 available);

    // army info
    answer_t ArmyInfo(const Army::Troop & troop, u16 flags);
    answer_t ArmyInfo(const Army::BattleTroop & troop, u16 flags);

    // dialog marketplace
    void Marketplace(bool fromTradingPost = false);

    // dialog buy boat
    answer_t BuyBoat(bool enable);
    
    // puzzle
    void PuzzleMaps(void);

    // thieves guild
    void ThievesGuild(u8);

    void GameInfo(void);

class Box
{
public:
    Box(u16 height, bool buttons = false);
    ~Box();

    const Rect & GetArea(void){ return area; };

private:
    Background back;
    Rect area;
};

class FrameBorder
{
public:
    FrameBorder(bool fade=true, u16 width = 640, u16 height = 480);
    ~FrameBorder();

    const Rect & GetArea(void) const{ return area; };

private:
    Background back;
    Rect area;
    bool doFade;
};

class NetworkChat
{
public:
    ~NetworkChat();

    static NetworkChat & Get();

    bool MessageReady(void);
    const std::string & GetMessage(void) const;
    void ResetMessage(void);
    void PushMessage(const std::string &);

    void Hide(void) const;
    void Show(void) const;

private:
    NetworkChat();

    std::string msg;
    std::list<std::string> spool;
    bool modify;
};

};

#endif
