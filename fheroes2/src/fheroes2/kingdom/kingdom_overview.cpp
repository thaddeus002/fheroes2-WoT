/***************************************************************************
 *   Copyright (C) 2012 by Andrey Afletdinov <fheroes2@gmail.com>          *
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
#include "text.h"
#include "cursor.h"
#include "button.h"
#include "settings.h"
#include "castle.h"
#include "heroes.h"
#include "world.h"
#include "kingdom.h"
#include "skill.h"
#include "army.h"
#include "selectartifactbar.h"
#include "interface_icons.h"
#include "interface_list.h"

typedef Heroes*	HEROESPTR;
typedef Castle* CASTLEPTR;

class StatsHeroesList : public Interface::ListBox<HEROESPTR>
{
public:
    StatsHeroesList(const Point & pt, KingdomHeroes &);

    void RedrawItem(const HEROESPTR &, s16, s16, bool);
    void RedrawBackground(const Point &);

    void ActionCurrentUp(void){};
    void ActionCurrentDn(void){};
    void ActionListDoubleClick(HEROESPTR &){};
    void ActionListSingleClick(HEROESPTR &){};
    void ActionListPressRight(HEROESPTR &){};
};

StatsHeroesList::StatsHeroesList(const Point & pt, KingdomHeroes & heroes) : Interface::ListBox<HEROESPTR>(pt)
{
    const Sprite & back = AGG::GetICN(ICN::OVERVIEW, 13);

    SetTopLeft(pt);
    SetScrollSplitter(AGG::GetICN(ICN::SCROLL, 4),
                        Rect(pt.x + 630, pt.y + 18, back.w(), back.h()));
    SetScrollButtonUp(ICN::SCROLL, 0, 1,
                        Point(pt.x + 626, pt.y));
    SetScrollButtonDn(ICN::SCROLL, 2, 3,
                        Point(pt.x + 626, pt.y + 20 + back.h()));
    SetAreaMaxItems(4);
    SetAreaItems(Rect(pt.x + 30, pt.y + 17, 594, 344));
    SetListContent(heroes);
}


void StatsHeroesList::RedrawItem(const HEROESPTR & hero, s16 dstx, s16 dsty, bool current)
{
    if(hero)
    {
	Text text("", Font::SMALL);
	const Sprite & back = AGG::GetICN(ICN::OVERVIEW, 10);
	back.Blit(dstx, dsty);

	// base info
	Interface::RedrawHeroesIcon(*hero, dstx + 5, dsty + 4);

	text.Set(GetString(hero->GetAttack()));
	text.Blit(dstx + 90 - text.w(), dsty + 20);

	text.Set(GetString(hero->GetDefense()));
	text.Blit(dstx + 125 - text.w(), dsty + 20);

	text.Set(GetString(hero->GetPower()));
	text.Blit(dstx + 160 - text.w(), dsty + 20);

	text.Set(GetString(hero->GetKnowledge()));
	text.Blit(dstx + 195 - text.w(), dsty + 20);

	// secondary skills info
/*
	SecondarySkillBar secskillsInfo;
	secskillsInfo.SetPos(dstx + 203, dsty + 3);
	secskillsInfo.SetInterval(1);
	secskillsInfo.SetUseMiniSprite();
	secskillsInfo.SetSkills(hero->GetSecondarySkills());
	secskillsInfo.Redraw();
*/
	// artifacts info
	SelectArtifactsBar artifactsInfo(*hero);

	artifactsInfo.SetPos(Point(dstx + 348, dsty + 3));
	artifactsInfo.SetInterval(1);
	artifactsInfo.SetVerticalSpace(8);
	artifactsInfo.SetReadOnly();

	artifactsInfo.SetBackgroundSprite(AGG::GetICN(ICN::OVERVIEW, 12));
	artifactsInfo.SetUseArts32Sprite();
	artifactsInfo.Redraw();

	// army info
	Army::DrawMons32Line(hero->GetArmy(), dstx + 5, dsty + 42, 192);
    }
}

void StatsHeroesList::RedrawBackground(const Point & dst)
{
    Text text("", Font::SMALL);

    // header
    AGG::GetICN(ICN::OVERVIEW, 6).Blit(dst.x + 30, dst.y);

    text.Set(_("Hero/Stats"));
    text.Blit(dst.x + 130 - text.w() / 2, dst.y + 1);

    text.Set(_("Skills"));
    text.Blit(dst.x + 300 - text.w() / 2, dst.y + 1);

    text.Set(_("Artifacts"));
    text.Blit(dst.x + 500 - text.w() / 2, dst.y + 1);

    // splitter background
    AGG::GetICN(ICN::OVERVIEW, 13).Blit(dst.x + 628, dst.y + 17);

    // items background
    for(u8 ii = 0; ii < maxItems; ++ii)
    {
	const Sprite & back = AGG::GetICN(ICN::OVERVIEW, 8);
	back.Blit(dst.x + 30, dst.y + 17 + ii * (back.h() + 4));
    }
}

class StatsCastlesList : public Interface::ListBox<CASTLEPTR>
{
public:
    StatsCastlesList(const Point & pt, KingdomCastles &);

    void RedrawItem(const CASTLEPTR &, s16, s16, bool);
    void RedrawBackground(const Point &);

    void ActionCurrentUp(void){};
    void ActionCurrentDn(void){};
    void ActionListDoubleClick(CASTLEPTR &){};
    void ActionListSingleClick(CASTLEPTR &){};
    void ActionListPressRight(CASTLEPTR &){};
};

StatsCastlesList::StatsCastlesList(const Point & pt, KingdomCastles & castles) : Interface::ListBox<CASTLEPTR>(pt)
{
    const Sprite & back = AGG::GetICN(ICN::OVERVIEW, 13);

    SetTopLeft(pt);
    SetScrollSplitter(AGG::GetICN(ICN::SCROLL, 4),
                        Rect(pt.x + 630, pt.y + 18, back.w(), back.h()));
    SetScrollButtonUp(ICN::SCROLL, 0, 1,
                        Point(pt.x + 626, pt.y));
    SetScrollButtonDn(ICN::SCROLL, 2, 3,
                        Point(pt.x + 626, pt.y + 20 + back.h()));
    SetAreaMaxItems(4);
    SetAreaItems(Rect(pt.x + 30, pt.y + 17, 594, 344));
    SetListContent(castles);
}


void StatsCastlesList::RedrawItem(const CASTLEPTR & cstl, s16 dstx, s16 dsty, bool current)
{
    if(cstl)
    {
	Text text("", Font::SMALL);
	const Sprite & back = AGG::GetICN(ICN::OVERVIEW, 11);
	back.Blit(dstx, dsty);

	// base info
	Interface::RedrawCastleIcon(*cstl, dstx + 17, dsty + 19);

	const Heroes* hero = cstl->GetHeroes().GuardFirst();

	if(hero)
	    Interface::RedrawHeroesIcon(*hero, dstx + 82, dsty + 19);

	text.Set(cstl->GetName());
	text.Blit(dstx + 72 - text.w() / 2, dsty + 62);

	// army info
	Army::DrawMons32Line(cstl->GetArmy(), dstx + 155, dsty + 30, 180);

	// available
	Army army;
	const u32 dwellings[] = { DWELLING_MONSTER1, DWELLING_MONSTER2, DWELLING_MONSTER3, DWELLING_MONSTER4, DWELLING_MONSTER5, DWELLING_MONSTER6 };

	for(u8 ii = 0; ii < ARRAY_COUNT(dwellings); ++ii)
	{
    	    u16 count = cstl->GetDwellingLivedCount(dwellings[ii]);
    	    if(count) army.JoinTroop(Monster(cstl->GetRace(), cstl->GetActualDwelling(dwellings[ii])), count);
	}

	Army::DrawMons32Line(army, dstx + 360, dsty + 30, 220);
    }
}

void StatsCastlesList::RedrawBackground(const Point & dst)
{
    Text text("", Font::SMALL);

    // header
    AGG::GetICN(ICN::OVERVIEW, 7).Blit(dst.x + 30, dst.y);

    text.Set(_("Town/Castle"));
    text.Blit(dst.x + 105 - text.w() / 2, dst.y + 1);

    text.Set(_("Garrison"));
    text.Blit(dst.x + 275 - text.w() / 2, dst.y + 1);

    text.Set(_("Available"));
    text.Blit(dst.x + 500 - text.w() / 2, dst.y + 1);

    // splitter background
    AGG::GetICN(ICN::OVERVIEW, 13).Blit(dst.x + 628, dst.y + 17);

    // items background
    for(u8 ii = 0; ii < maxItems; ++ii)
    {
	const Sprite & back = AGG::GetICN(ICN::OVERVIEW, 8);
	back.Blit(dst.x + 30, dst.y + 17 + ii * (back.h() + 4));
    }
}

void RedrawIncomeInfo(const Point & pt, const Kingdom & myKingdom)
{
    const Funds & funds = myKingdom.GetFunds();
    Text text("", Font::SMALL);

    text.Set(GetString(world.CountCapturedMines(Resource::WOOD, myKingdom.GetColor())));
    text.Blit(pt.x + 54 - text.w() / 2, pt.y + 408);

    text.Set(GetString(world.CountCapturedMines(Resource::MERCURY, myKingdom.GetColor())));
    text.Blit(pt.x + 146 - text.w() / 2, pt.y + 408);

    text.Set(GetString(world.CountCapturedMines(Resource::ORE, myKingdom.GetColor())));
    text.Blit(pt.x + 228 - text.w() / 2, pt.y + 408);

    text.Set(GetString(world.CountCapturedMines(Resource::SULFUR, myKingdom.GetColor())));
    text.Blit(pt.x + 294 - text.w() / 2, pt.y + 408);

    text.Set(GetString(world.CountCapturedMines(Resource::CRYSTAL, myKingdom.GetColor())));
    text.Blit(pt.x + 360 - text.w() / 2, pt.y + 408);

    text.Set(GetString(world.CountCapturedMines(Resource::GEMS, myKingdom.GetColor())));
    text.Blit(pt.x + 428 - text.w() / 2, pt.y + 408);

    text.Set(GetString(world.CountCapturedMines(Resource::GOLD, myKingdom.GetColor())));
    text.Blit(pt.x + 494 - text.w() / 2, pt.y + 408);

    text.Set(GetString(funds.wood));
    text.Blit(pt.x + 56 - text.w() / 2, pt.y + 448);

    text.Set(GetString(funds.mercury));
    text.Blit(pt.x + 146 - text.w() / 2, pt.y + 448);

    text.Set(GetString(funds.ore));
    text.Blit(pt.x + 226 - text.w() / 2, pt.y + 448);

    text.Set(GetString(funds.sulfur));
    text.Blit(pt.x + 294 - text.w() / 2, pt.y + 448);

    text.Set(GetString(funds.crystal));
    text.Blit(pt.x + 362 - text.w() / 2, pt.y + 448);

    text.Set(GetString(funds.gems));
    text.Blit(pt.x + 428 - text.w() / 2, pt.y + 448);

    text.Set(GetString(funds.gold));
    text.Blit(pt.x + 496 - text.w() / 2, pt.y + 448);

    text.Set(_("Gold Per Day:"));
    text.Blit(pt.x + 328 - text.w(), pt.y + 462);

    text.Set(GetString(myKingdom.GetIncome().Get(Resource::GOLD)));
    text.Blit(pt.x + 340, pt.y + 462);
}

void Kingdom::OverviewDialog(void)
{
    Display & display = Display::Get();
    Cursor & cursor = Cursor::Get();
    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);

    Dialog::FrameBorder background;
    background.SetPosition((display.w() - 640 - BORDERWIDTH * 2) / 2, (display.h() - 480 - BORDERWIDTH * 2) / 2, 640, 480);
    background.Redraw();

    const Point cur_pt(background.GetArea().x, background.GetArea().y);
    Point dst_pt(cur_pt);

    AGG::GetICN(ICN::STONEBAK, 0).Blit(dst_pt);
    AGG::GetICN(ICN::OVERBACK, 0).Blit(dst_pt);
    RedrawIncomeInfo(cur_pt, *this);

    StatsHeroesList listHeroes(dst_pt, heroes);
    StatsCastlesList listCastles(dst_pt, castles);

    // buttons
    dst_pt.x = cur_pt.x + 540;
    dst_pt.y = cur_pt.y + 360;
    Button buttonHeroes(dst_pt, ICN::OVERVIEW, 0, 1);

    dst_pt.x = cur_pt.x + 540;
    dst_pt.y = cur_pt.y + 405;
    Button buttonCastle(dst_pt, ICN::OVERVIEW, 2, 3);

    dst_pt.x = cur_pt.x + 540;
    dst_pt.y = cur_pt.y + 453;
    Button buttonExit(dst_pt, ICN::OVERVIEW, 4, 5);

    // set state view: heroes
    buttonHeroes.Press();
    buttonCastle.Release();
    Interface::ListBasic* listStats = &listHeroes;
    listStats->Redraw();

    buttonHeroes.Draw();
    buttonCastle.Draw();
    buttonExit.Draw();

    cursor.Show();
    display.Flip();

    LocalEvent & le = LocalEvent::Get();

    // dialog menu loop
    while(le.HandleEvents())
    {
	le.MousePressLeft(buttonExit) ? buttonExit.PressDraw() : buttonExit.ReleaseDraw();

	// switch view: heroes/castle
	if(buttonHeroes.isReleased() &&
	    le.MouseClickLeft(buttonHeroes))
	{
	    cursor.Hide();
	    buttonHeroes.Press();
	    buttonCastle.Release();
	    buttonHeroes.Draw();
	    buttonCastle.Draw();
	    listStats = &listHeroes;
	}
	else
	if(buttonCastle.isReleased() &&
	    le.MouseClickLeft(buttonCastle))
	{
	    cursor.Hide();
	    buttonCastle.Press();
	    buttonHeroes.Release();
	    buttonHeroes.Draw();
	    buttonCastle.Draw();
	    listStats = &listCastles;
	}

	// exit event
	if(le.MouseClickLeft(buttonExit) ||
	    Game::HotKeyPress(Game::EVENT_DEFAULT_EXIT)) break;

	listStats->QueueEventProcessing();

	// redraw
	if(! cursor.isVisible())
	{
	    listStats->Redraw();
	    cursor.Show();
	    display.Flip();
	}
    }
}
