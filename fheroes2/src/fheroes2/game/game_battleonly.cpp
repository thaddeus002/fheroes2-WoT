/***************************************************************************
 *   Copyright (C) 2011 by Andrey Afletdinov <fheroes2@gmail.com>          *
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

#include "agg.h"
#include "cursor.h"
#include "settings.h"
#include "gamedefs.h"
#include "battle2.h"
#include "world.h"
#include "army.h"
#include "castle.h"
#include "kingdom.h"
#include "heroes.h"
#include "selectarmybar.h"
#include "selectartifactbar.h"
#include "heroes_indicator.h"
#include "heroes_recruits.h"
#include "editor_dialogs.h"
#include "network.h"
#include "button.h"
#include "race.h"
#include "game.h"
#include "players.h"

#ifdef BUILD_BATTLEONLY

#define PRIMARY_MAX_VALUE	20

void RedrawPrimarySkillInfo(const Point &, const Skill::Primary*, const Skill::Primary*);

struct ControlInfo
{
    ControlInfo(const Point & pt, u8 & ctrl) : result(ctrl),
	rtLocal(pt.x, pt.y, 24, 24), rtAI(pt.x + 60, pt.y, 24, 24) {};

    void Redraw(void);
    
    const u8 & result;

    const Rect rtLocal;
    const Rect rtAI;
};

void ControlInfo::Redraw(void)
{
  Display & display = Display::Get();
  
  const Sprite & cell = AGG::GetICN(ICN::CELLWIN, 1);
  const Sprite & mark = AGG::GetICN(ICN::CELLWIN, 2);

  display.Blit(cell, rtLocal.x, rtLocal.y);
  if(result & CONTROL_HUMAN) display.Blit(mark, rtLocal.x + 3, rtLocal.y + 2);
  Text text("Human", Font::SMALL);
  text.Blit(rtLocal.x + cell.w() + 5, rtLocal.y + 5);
  
  display.Blit(cell, rtAI.x, rtAI.y);
  if(result & CONTROL_AI) display.Blit(mark, rtAI.x + 3, rtAI.y + 2);
  text.Set("AI");
  text.Blit(rtAI.x + cell.w() + 5, rtAI.y + 5);
}

struct BattleOnly
{
    Heroes*	hero1;
    Heroes*	hero2;

    Player	player1;
    Player	player2;

    Army::army_t* army1;
    Army::army_t* army2;
    Army::army_t monsters;

    MoraleIndicator* moraleIndicator1;
    MoraleIndicator* moraleIndicator2;
    
    LuckIndicator* luckIndicator1;
    LuckIndicator* luckIndicator2;
    
    SecondarySkillBar* secskill_bar1;
    SecondarySkillBar* secskill_bar2;

    SelectArmyBar selectArmy1;
    SelectArmyBar selectArmy2;
    
    SelectArtifactsBar* selectArtifacts1;
    SelectArtifactsBar* selectArtifacts2;
    
    ControlInfo* cinfo2;

    Rect rtPortrait1;
    Rect rtPortrait2;

    Rect rtAttack1;
    Rect rtAttack2;
    Rect rtDefense1;
    Rect rtDefense2;
    Rect rtPower1;
    Rect rtPower2;
    Rect rtKnowledge1;
    Rect rtKnowledge2;

    const Rect rt1;
    Surface sfb1;
    Surface sfc1;

    const Rect rt2;
    Surface sfb2;
    Surface sfc2;

    bool	ChangeSettings(void);
    void	RedrawBaseInfo(const Point &);
    void	StartBattle(void);
    void	UpdateHero1(const Point &);
    void	UpdateHero2(const Point &);

    static BattleOnly & Get(void);

    BattleOnly() : hero1(NULL), hero2(NULL), player1(Color::BLUE), player2(Color::NONE),
	army1(NULL), army2(NULL),
	moraleIndicator1(NULL), moraleIndicator2(NULL),
	luckIndicator1(NULL), luckIndicator2(NULL), secskill_bar1(NULL), secskill_bar2(NULL),
	selectArtifacts1(NULL), selectArtifacts2(NULL), cinfo2(NULL),
	rt1(36, 267, 43, 53), sfb1(rt1.w, rt1.h), sfc1(rt1.w, rt1.h - 10),
	rt2(23, 347, 34, 34), sfb2(rt2.w, rt2.h), sfc2(rt2.w, rt2.h)
    {
	player1.SetControl(CONTROL_HUMAN);
	player2.SetControl(CONTROL_AI);

#ifdef WITH_NET
	if(! Network::isRemoteClient())
#endif
	{
	    const Sprite &backSprite = AGG::GetICN(ICN::SWAPWIN, 0);

	    sfb1.Blit(backSprite, rt1, 0, 0);
	    sfb2.Blit(backSprite, rt2, 0, 0);

	    Cursor::DrawCursor(sfc1, 0x10, true);
	    Cursor::DrawCursor(sfc2, 0x10, true);
	}
    };
};

bool BattleOnly::ChangeSettings(void)
{
    Settings & conf = Settings::Get();
    Display & display = Display::Get();
    Cursor & cursor = Cursor::Get();
    LocalEvent & le = LocalEvent::Get();

    cursor.Hide();
    cursor.SetThemes(Cursor::POINTER);

    Dialog::FrameBorder frameborder;
    frameborder.SetPosition((display.w() - 640 - BORDERWIDTH * 2) / 2, (display.h() - 480 - BORDERWIDTH * 2) / 2, 640, 480);
    frameborder.Redraw();

    const Point & cur_pt = frameborder.GetArea();

    rtPortrait1 = Rect(cur_pt.x + 93, cur_pt.y + 72, 101, 93);
    rtPortrait2 = Rect(cur_pt.x + 445, cur_pt.y + 72, 101, 93);

    rtAttack1 = Rect(cur_pt.x + 215, cur_pt.y + 50, 33, 33);
    rtAttack2 = Rect(cur_pt.x + 390, cur_pt.y + 50, 33, 33);

    rtDefense1 = Rect(cur_pt.x + 215, cur_pt.y + 83, 33, 33);
    rtDefense2 = Rect(cur_pt.x + 390, cur_pt.y + 83, 33, 33);

    rtPower1 = Rect(cur_pt.x + 215, cur_pt.y + 116, 33, 33);
    rtPower2 = Rect(cur_pt.x + 390, cur_pt.y + 116, 33, 33);

    rtKnowledge1 = Rect(cur_pt.x + 215, cur_pt.y + 149, 33, 33);
    rtKnowledge2 = Rect(cur_pt.x + 390, cur_pt.y + 149, 33, 33);

    if(conf.GameType(Game::TYPE_NETWORK))
    {
	player2.color = Color::RED;

	player1.SetControl(CONTROL_REMOTE);
	player2.SetControl(CONTROL_REMOTE);

	hero2 = world.GetHeroes(Heroes::ZOM);
    }

    hero1 = world.GetHeroes(Heroes::LORDKILBURN);
    army1 = &hero1->GetArmy();

    RedrawBaseInfo(cur_pt);

    UpdateHero1(cur_pt);
    moraleIndicator1->Redraw();
    luckIndicator1->Redraw();
    secskill_bar1->Redraw();
    selectArtifacts1->Redraw();
    selectArmy1.Redraw();
    
    selectArmy1.SetArmy(*army1);
    selectArmy1.SetPos(Point(cur_pt.x + 36, cur_pt.y + 267));
    selectArmy1.SetInterval(2);
    selectArmy1.SetBackgroundSprite(sfb1);
    selectArmy1.SetCursorSprite(sfc1);
    selectArmy1.SetUseMons32Sprite();
    selectArmy1.SetSaveLastTroop();
    selectArmy1.SetChangeMode();
    selectArmy1.Redraw();

    if(hero2)
    {
	UpdateHero2(cur_pt);

	moraleIndicator2->Redraw();
	luckIndicator2->Redraw();
	secskill_bar2->Redraw();
	selectArtifacts2->Redraw();
	selectArmy2.Redraw();
    }

    monsters.At(0) = Army::Troop(Monster::PEASANT, 100);
    army2 = hero2 ? &hero2->GetArmy() : &monsters;

    selectArmy2.SetArmy(*army2);
    selectArmy2.SetPos(Point(cur_pt.x + 381, cur_pt.y + 267));
    selectArmy2.SetInterval(2);
    selectArmy2.SetBackgroundSprite(sfb1);
    selectArmy2.SetCursorSprite(sfc1);
    selectArmy2.SetUseMons32Sprite();
    selectArmy2.SetSaveLastTroop();
    selectArmy2.SetChangeMode();
    selectArmy2.Redraw();

    bool exit = false;
    bool redraw = false;
    bool result = false;
    bool allow1 = true;
    bool allow2 = true;

    Button buttonStart(Point(cur_pt.x + 280, cur_pt.y + 428), ICN::SYSTEM, 1, 2);

#ifdef WITH_NET
    if(Network::isLocalClient())
    {
	FH2LocalClient & local = FH2LocalClient::Get();

	if(local.Modes(ST_ADMIN))
	    FH2LocalClient::SendUpdateBattleOnlySettings(*this);
	else
	{
	    allow1 = false;
	    allow2 = false;
	    buttonStart.SetDisable(true);
	}
    }
#endif

    buttonStart.Draw();

    cursor.Show();
    display.Flip();

    // message loop
    while(!exit && le.HandleEvents())
    {
        buttonStart.isEnable() &&
	    le.MousePressLeft(buttonStart) ? buttonStart.PressDraw() : buttonStart.ReleaseDraw();

	if((buttonStart.isEnable() && le.MouseClickLeft(buttonStart)) ||
	    Game::HotKeyPress(Game::EVENT_DEFAULT_READY))
	{
#ifdef WITH_NET
	    if(Network::isLocalClient())
	    {
		FH2LocalClient & local = FH2LocalClient::Get();
		if(local.Modes(ST_ADMIN)) local.SendStartBattleOnly();
	    }
	    else
#endif
	    {
		result = true;
		exit = true;
	    }
	}
        else
	if(Game::HotKeyPress(Game::EVENT_DEFAULT_EXIT)) exit = true;

	if(allow1 && le.MouseClickLeft(rtPortrait1))
	{
	    Heroes::heroes_t hid = Dialog::SelectHeroes(hero1->GetID());
	    if(hero2 && hid == hero2->GetID())
	    {
		Dialog::Message("Error", "Please, select other hero.", Font::BIG, Dialog::OK);
	    }
	    else
	    if(Heroes::UNKNOWN != hid)
	    {
		hero1 = world.GetHeroes(hid);
		UpdateHero1(cur_pt);
		redraw = true;
	    }
	}
	else
	if(allow2 && le.MouseClickLeft(rtPortrait2))
	{
	    Heroes::heroes_t hid = Dialog::SelectHeroes(hero2 ? hero2->GetID() : Heroes::UNKNOWN);
	    if(hid == hero1->GetID())
	    {
		Dialog::Message("Error", "Please, select other hero.", Font::BIG, Dialog::OK);
	    }
	    else
	    if(Heroes::UNKNOWN != hid)
	    {
		hero2 = world.GetHeroes(hid);
		UpdateHero2(cur_pt);
		if(player2.isLocal() && NULL == cinfo2)
		    cinfo2 = new ControlInfo(Point(cur_pt.x + 510, cur_pt.y + 425), player2.control);
		redraw = true;
	    }
	}

	if(hero1 && allow1)
	{
	    if(le.MouseClickLeft(rtAttack1))
	    {
		u32 value = hero1->attack;
		if(Dialog::SelectCount("Set Attack Skill", 0, PRIMARY_MAX_VALUE, value))
		{
		    hero1->attack = value;
		    redraw = true;
		}
	    }
	    else
	    if(le.MouseClickLeft(rtDefense1))
	    {
		u32 value = hero1->defense;
		if(Dialog::SelectCount("Set Defense Skill", 0, PRIMARY_MAX_VALUE, value))
		{
		    hero1->defense = value;
		    redraw = true;
		}
	    }
	    else
	    if(le.MouseClickLeft(rtPower1))
	    {
		u32 value = hero1->power;
		if(Dialog::SelectCount("Set Power Skill", 0, PRIMARY_MAX_VALUE, value))
		{
		    hero1->power = value;
		    redraw = true;
		}
	    }
	    else
	    if(le.MouseClickLeft(rtKnowledge1))
	    {
		u32 value = hero1->knowledge;
		if(Dialog::SelectCount("Set Knowledge Skill", 0, PRIMARY_MAX_VALUE, value))
		{
		    hero1->knowledge = value;
		    redraw = true;
		}
	    }
	}

	if(hero2 && allow2)
	{
	    if(le.MouseClickLeft(rtAttack2))
	    {
		u32 value = hero2->attack;
		if(Dialog::SelectCount("Set Attack Skill", 0, PRIMARY_MAX_VALUE, value))
		{
		    hero2->attack = value;
		    redraw = true;
		}
	    }
	    else
	    if(le.MouseClickLeft(rtDefense2))
	    {
		u32 value = hero2->defense;
		if(Dialog::SelectCount("Set Defense Skill", 0, PRIMARY_MAX_VALUE, value))
		{
		    hero2->defense = value;
		    redraw = true;
		}
	    }
	    else
	    if(le.MouseClickLeft(rtPower2))
	    {
		u32 value = hero2->power;
		if(Dialog::SelectCount("Set Power Skill", 0, PRIMARY_MAX_VALUE, value))
		{
		    hero2->power = value;
		    redraw = true;
		}
	    }
	    else
	    if(le.MouseClickLeft(rtKnowledge2))
	    {
		u32 value = hero2->knowledge;
		if(Dialog::SelectCount("Set Knowledge Skill", 0, PRIMARY_MAX_VALUE, value))
		{
		    hero2->knowledge = value;
		    redraw = true;
		}
	    }
	}

	if(allow1 && le.MouseCursor(selectArmy1.GetArea()))
	{
	  if(selectArtifacts1->isSelected()) selectArtifacts1->Reset();
	  else
	  if(selectArtifacts2 && selectArtifacts2->isSelected()) selectArtifacts2->Reset();

    	  if(SelectArmyBar::QueueEventProcessing(selectArmy1))
	  {
	    redraw = true;
	  }
	}

	if(allow2 && le.MouseCursor(selectArmy2.GetArea()))
	{
	  if(selectArtifacts1->isSelected()) selectArtifacts1->Reset();
	  else
	  if(selectArtifacts2 && selectArtifacts2->isSelected()) selectArtifacts2->Reset();

	  if(SelectArmyBar::QueueEventProcessing(selectArmy2))
	  {
	    redraw = true;
	  }
	}

	if(allow1 && le.MouseCursor(selectArtifacts1->GetArea()))
	{
	  if(selectArmy1.isSelected()) selectArmy1.Reset();
	  else
	  if(selectArmy2.isSelected()) selectArmy2.Reset();

	  if(SelectArtifactsBar::QueueEventProcessing(*selectArtifacts1))
	      redraw = true;
	}

	if(allow2 && selectArtifacts2 && le.MouseCursor(selectArtifacts2->GetArea()))
	{
	  if(selectArmy1.isSelected()) selectArmy1.Reset();
	  else
	  if(selectArmy2.isSelected()) selectArmy2.Reset();

	  if(SelectArtifactsBar::QueueEventProcessing(*selectArtifacts2))
	      redraw = true;
	}

	if(hero1 && allow1)
	{
          if(le.MouseCursor(moraleIndicator1->GetArea())) MoraleIndicator::QueueEventProcessing(*moraleIndicator1);
          else
	  if(le.MouseCursor(luckIndicator1->GetArea())) LuckIndicator::QueueEventProcessing(*luckIndicator1);
          else
	  if(le.MouseCursor(secskill_bar1->GetArea()) && secskill_bar1->QueueEventProcessing())
	      redraw = true;
	}

	if(hero2 && allow2)
	{
	  if(le.MouseCursor(moraleIndicator2->GetArea())) MoraleIndicator::QueueEventProcessing(*moraleIndicator2);
	  else
	  if(le.MouseCursor(luckIndicator2->GetArea())) LuckIndicator::QueueEventProcessing(*luckIndicator2);
	  else
	  if(le.MouseCursor(secskill_bar2->GetArea()) && secskill_bar2->QueueEventProcessing())
	      redraw = true;
	}

	if(cinfo2 && allow1)
	{
	    if(hero2 && le.MouseClickLeft(cinfo2->rtLocal) && player2.isAI())
	    {
		player2.SetControl(CONTROL_HUMAN);
		redraw = true;
	    }
	    else
	    if(le.MouseClickLeft(cinfo2->rtAI) && player2.isHuman())
	    {
		player2.SetControl(CONTROL_AI);
		redraw = true;
	    }
	}

#ifdef WITH_NET
	if(Network::isLocalClient())
	{
	    FH2LocalClient & local = FH2LocalClient::Get();
	    if(redraw && local.Modes(ST_ADMIN))
	    {
		FH2LocalClient::SendUpdateBattleOnlySettings(*this);
		redraw = false;
	    }

    	    if(local.Ready())
    	    {
		QueueMessage packet;

		if(!local.Recv(packet)) return false;
		DEBUG(DBG_NETWORK, DBG_INFO, "recv: " << Network::GetMsgString(packet.GetID()));
		switch(packet.GetID())
		{
		    case MSG_START_BATTLEONLY:
			result = true;
			exit = true;
			break;

		    case MSG_READY:
			break;

		    case MSG_MESSAGE:
			break;

		    case MSG_SHUTDOWN:
                	exit = true;
                	break;

		    case MSG_UPDATE_BATTLEONLY:
		    {
			const Heroes* h1 = hero1;
			const Heroes* h2 = hero2;
			Network::UnpackBattleOnly(packet, *this);

			if(h1 != hero1)
			    UpdateHero1(cur_pt);

			if(h2 != hero2)
			    UpdateHero2(cur_pt);

			redraw = true;
			break;
		    }

		    default: break;
		}
	    }
	}
#endif

	if(redraw || !cursor.isVisible())
	{
	  cursor.Hide();
	  RedrawBaseInfo(cur_pt);
	  moraleIndicator1->Redraw();
	  luckIndicator1->Redraw();
	  secskill_bar1->Redraw();
	  selectArtifacts1->Redraw();
	  selectArmy1.Redraw();
	  if(hero2)
	  {
	    moraleIndicator2->Redraw();
	    luckIndicator2->Redraw();
	    secskill_bar2->Redraw();
	    selectArtifacts2->Redraw();
	  }
	  selectArmy2.Redraw();
	  if(cinfo2) cinfo2->Redraw();
	  buttonStart.Draw();
	  cursor.Show();
	  display.Flip();
	  redraw = false;
	}
    }

    delete moraleIndicator1;
    delete luckIndicator1;
    delete secskill_bar1;
    delete selectArtifacts1;

    if(hero2)
    {
      delete moraleIndicator2;
      delete luckIndicator2;
      delete secskill_bar2;
      delete selectArtifacts2;
    }
    
    if(cinfo2) delete cinfo2;

    return result;
}

void BattleOnly::UpdateHero1(const Point & cur_pt)
{
    if(moraleIndicator1)
    {
      delete moraleIndicator1;
      moraleIndicator1 = NULL;
    }
    
    if(luckIndicator1)
    {
      delete luckIndicator1;
      luckIndicator1 = NULL;
    }
    
    if(secskill_bar1)
    {
      delete secskill_bar1;
      secskill_bar1 = NULL;
    }

    if(selectArtifacts1)
    {
      delete selectArtifacts1;
      selectArtifacts1 = NULL;
    }
        
    if(hero1)
    {
      player1.color = Color::BLUE;
      player1.race  = hero1->GetRace();

      moraleIndicator1 = new MoraleIndicator(*hero1);
      moraleIndicator1->SetPos(Point(cur_pt.x + 34, cur_pt.y + 75), true);

      luckIndicator1 = new LuckIndicator(*hero1);
      luckIndicator1->SetPos(Point(cur_pt.x + 34, cur_pt.y + 115), true);

      secskill_bar1 = new SecondarySkillBar();
      secskill_bar1->SetPos(cur_pt.x + 23, cur_pt.y + 200);
      secskill_bar1->SetUseMiniSprite();
      secskill_bar1->SetInterval(1);
      secskill_bar1->SetSkills(hero1->GetSecondarySkills());
      secskill_bar1->SetChangeMode();

      selectArtifacts1 = new SelectArtifactsBar(*hero1);
      selectArtifacts1->SetPos(Point(cur_pt.x + 23, cur_pt.y + 347));
      selectArtifacts1->SetInterval(2);
      selectArtifacts1->SetBackgroundSprite(sfb2);
      selectArtifacts1->SetCursorSprite(sfc2);
      selectArtifacts1->SetUseArts32Sprite();
      selectArtifacts1->SetChangeMode();

      army1 = &hero1->GetArmy();
      selectArmy1.SetArmy(*army1);
    }
}

void BattleOnly::UpdateHero2(const Point & cur_pt)
{
    if(moraleIndicator2)
    {
      delete moraleIndicator2;
      moraleIndicator2 = NULL;
    }
    
    if(luckIndicator2)
    {
      delete luckIndicator2;
      luckIndicator2 = NULL;
    }
    
    if(secskill_bar2)
    {
      delete secskill_bar2;
      secskill_bar2 = NULL;
    }

    if(selectArtifacts2)
    {
      delete selectArtifacts2;
      selectArtifacts2 = NULL;
    }

    if(hero2)
    {
      player2.color = Color::RED;
      player2.race  = hero2->GetRace();

      moraleIndicator2 = new MoraleIndicator(*hero2);
      moraleIndicator2->SetPos(Point(cur_pt.x + 566, cur_pt.y + 75), true);

      luckIndicator2 = new LuckIndicator(*hero2);
      luckIndicator2->SetPos(Point(cur_pt.x + 566, cur_pt.y + 115), true);

      secskill_bar2 = new SecondarySkillBar();
      secskill_bar2->SetPos(cur_pt.x + 354, cur_pt.y + 200);
      secskill_bar2->SetUseMiniSprite();
      secskill_bar2->SetInterval(1);
      secskill_bar2->SetSkills(hero2->GetSecondarySkills());
      secskill_bar2->SetChangeMode();

      selectArtifacts2 = new SelectArtifactsBar(*hero2);
      selectArtifacts2->SetPos(Point(cur_pt.x + 367, cur_pt.y + 347));
      selectArtifacts2->SetInterval(2);
      selectArtifacts2->SetBackgroundSprite(sfb2);
      selectArtifacts2->SetCursorSprite(sfc2);
      selectArtifacts2->SetUseArts32Sprite();
      selectArtifacts2->SetChangeMode();

      army2 = &hero2->GetArmy();
      selectArmy2.SetArmy(*army2);
    }
}

void BattleOnly::RedrawBaseInfo(const Point & top)
{
    Display & display = Display::Get();

    display.Blit(AGG::GetICN(ICN::SWAPWIN, 0), top);

    // header
    std::string message = "%{name1} vs %{name2}";

#ifdef WITH_NET
    if(Network::isLocalClient())
    {
	FH2LocalClient & local = FH2LocalClient::Get();
	const Player* player;

	if(NULL != (player = local.FindPlayer(player1.color)))
	    String::Replace(message, "%{name1}", player->name);

	if(NULL != (player = local.FindPlayer(player2.color)))
	    String::Replace(message, "%{name2}", player->name);
	else
	    String::Replace(message, "(wait connect)", player->name);
    }
    else
#endif
    {
	String::Replace(message, "%{name1}", std::string(Race::String(hero1->GetRace())) + " " + hero1->GetName());
	String::Replace(message, "%{name2}", (hero2 ? std::string(Race::String(hero2->GetRace())) + " " + hero2->GetName() : "Monsters"));
    }

    Text text(message, Font::BIG);
    text.Blit(top.x + 320 - text.w() / 2, top.y + 26);
    
    // portrait
    display.Blit(hero1->GetPortrait101x93(), rtPortrait1);

    if(hero2)
      display.Blit(hero2->GetPortrait101x93(), rtPortrait2);
    else
    {
      display.FillRect(0, 0, 0, rtPortrait2);
      text.Set("N/A", Font::BIG);
      text.Blit(rtPortrait2.x + (rtPortrait2.w - text.w()) / 2, rtPortrait2.y + rtPortrait2.h / 2 - 8);
    }
    
    // primary skill
    RedrawPrimarySkillInfo(top, hero1, hero2);
}

void BattleOnly::StartBattle(void)
{
    Settings & conf = Settings::Get();

    Players & players = conf.GetPlayers();
    players.Init(player1.color | player2.color);
    world.GetKingdoms().Init();

    players.SetPlayerRace(player1.color, player1.race);
    players.SetPlayerRace(player2.color, player2.race);

#ifdef WITH_NET
    if(Network::isLocalClient())
    {
        FH2LocalClient & client = FH2LocalClient::Get();
	u8 & mycolor = client.color == player1.color ? player1.color : player2.color;

	conf.SetCurrentColor(mycolor);

	players.SetPlayerControl(mycolor, CONTROL_HUMAN);

	if(mycolor != player1.color)
	    players.SetPlayerControl(player1.color, CONTROL_REMOTE);
	else
	if(mycolor != player2.color)
	    players.SetPlayerControl(player2.color, CONTROL_REMOTE);
    }
    else
#endif
    {
	conf.SetCurrentColor(player1.color);

	players.SetPlayerControl(player1.color, player1.control);
	players.SetPlayerControl(player2.color, player2.control);
    }

    if(hero1)
    {
	hero1->SetSpellPoints(hero1->GetMaxSpellPoints());
	hero1->Recruit(player1.color, Point(5, 5));
    }

    if(hero2)
    {
      hero2->SetSpellPoints(hero2->GetMaxSpellPoints());
      hero2->Recruit(player2.color, Point(5, 6));
    }

    Battle2::Loader(hero1->GetArmy(), (hero2 ? hero2->GetArmy() : monsters), hero1->GetIndex() + 1);
}

Game::menu_t Game::StartBattleOnly(void)
{
    BattleOnly main;

    if(main.ChangeSettings())
	main.StartBattle();

    return Game::MAINMENU;
}

#ifdef WITH_NET
void Network::PackBattleOnly(QueueMessage & msg, const BattleOnly & b)
{
    msg.Push(static_cast<u8>(b.hero1->GetID()));
    Game::IO::PackHeroes(msg, *b.hero1);
    msg.Push(static_cast<u8>(b.hero2->GetID()));
    Game::IO::PackHeroes(msg, *b.hero2);

    msg.Push(b.player1.color);
    msg.Push(b.player2.color);

    msg.Push(b.player1.control);
    msg.Push(b.player2.control);
}

void Network::UnpackBattleOnly(QueueMessage & msg, Recruits & res)
{
    BattleOnly b;
    UnpackBattleOnly(msg, b);
    res.SetHero1(b.hero1);
    res.SetHero2(b.hero2);
}

void Network::UnpackBattleOnly(QueueMessage & msg, BattleOnly & b)
{
    u8 id;

    msg.Pop(id);
    b.hero1 = world.GetHeroes(Heroes::ConvertID(id));
    if(b.hero1)
        Game::IO::UnpackHeroes(msg, *b.hero1);
    else
	DEBUG(DBG_NETWORK, DBG_WARN, "unknown id");

    msg.Pop(id);
    b.hero2 = world.GetHeroes(Heroes::ConvertID(id));
    if(b.hero2)
        Game::IO::UnpackHeroes(msg, *b.hero2);
    else
	DEBUG(DBG_NETWORK, DBG_WARN, "unknown id");

    msg.Pop(b.player1.color);
    msg.Pop(b.player2.color);

    msg.Pop(b.player1.control);
    msg.Pop(b.player2.control);
}
#endif

#else
Game::menu_t Game::StartBattleOnly(void)
{
    return Game::MAINMENU;
}
#endif
