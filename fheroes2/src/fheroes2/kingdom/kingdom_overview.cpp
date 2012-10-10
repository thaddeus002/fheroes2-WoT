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
#include "selectarmybar.h"
#include "interface_icons.h"
#include "interface_list.h"

struct HeroRow
{
    Heroes* 		hero;
    SelectArmyBar	armyBar;
    ArtifactsBar*	artifactsBar;
    SecondarySkillsBar	secskillsBar;
    PrimarySkillsBar*	primskillsBar;
    Surface		sfb;
    Surface		sfc;
    Surface		sfd;

    HeroRow() : hero(NULL), artifactsBar(NULL), primskillsBar(NULL) {}
    ~HeroRow()
    {
	delete artifactsBar;
	delete primskillsBar;
    }

    void Init(Heroes* ptr)
    {
	hero = ptr;

	const Size cell(41, 53);
	sfb.Set(cell.w, cell.h);
	Cursor::DrawCursor(sfb, 0x70, true);

	sfc.Set(cell.w, cell.h);
	Cursor::DrawCursor(sfc, 0xd7, true);

	armyBar.SetArmy(hero->GetArmy());
        armyBar.SetSaveLastTroop();
	armyBar.SetInterval(-1);
	armyBar.SetBackgroundSprite(sfb);
	armyBar.SetCursorSprite(sfc);
	armyBar.SetUseMons32Sprite();
	if(hero->inCastle()) armyBar.SetCastle(*hero->inCastle());

	artifactsBar = new ArtifactsBar(hero, true, false);
	artifactsBar->SetColRows(7, 2);
	artifactsBar->SetHSpace(1);
	artifactsBar->SetVSpace(8);
	artifactsBar->SetContent(hero->GetBagArtifacts());

	secskillsBar.SetColRows(4, 2);
        secskillsBar.SetHSpace(-1);
        secskillsBar.SetVSpace(8);
        secskillsBar.SetContent(hero->GetSecondarySkills());

	if(primskillsBar) delete primskillsBar;
	primskillsBar = new PrimarySkillsBar(ptr, true);
	primskillsBar->SetColRows(4, 1);
	primskillsBar->SetHSpace(2);
	primskillsBar->SetTextOff(20, -13);
    }
};

typedef Castle* CASTLEPTR;

class StatsHeroesList : public Interface::ListBox<HeroRow>
{
public:
    std::vector<HeroRow> content;

    StatsHeroesList(const Point & pt, KingdomHeroes &);

    void RedrawItem(const HeroRow &, s16, s16, bool);
    void RedrawBackground(const Point &);

    void ActionCurrentUp(void){};
    void ActionCurrentDn(void){};
    void ActionListSingleClick(HeroRow &){};
    void ActionListDoubleClick(HeroRow &){};
    void ActionListPressRight(HeroRow &){};

    void ActionListSingleClick(HeroRow &, const Point &, s16, s16);
    void ActionListDoubleClick(HeroRow &, const Point &, s16, s16);
    void ActionListPressRight(HeroRow &, const Point &, s16, s16);
    bool ActionListCursor(HeroRow &, const Point &, s16, s16);
};

StatsHeroesList::StatsHeroesList(const Point & pt, KingdomHeroes & heroes) : Interface::ListBox<HeroRow>(pt)
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

    content.resize(heroes.size());

    for(KingdomHeroes::iterator
	it = heroes.begin(); it != heroes.end(); ++it)
	content[std::distance(heroes.begin(), it)].Init(*it);

    SetListContent(content);
}

void StatsHeroesList::ActionListDoubleClick(HeroRow & row, const Point & cursor, s16 ox, s16 oy)
{
    ActionListSingleClick(row, cursor, ox, oy);
}

void StatsHeroesList::ActionListSingleClick(HeroRow & row, const Point & cursor, s16 ox, s16 oy)
{
    if(row.hero && (Rect(ox + 5, oy + 4, Interface::IconsBar::GetItemWidth(), Interface::IconsBar::GetItemHeight()) & cursor))
	Game::OpenHeroesDialog(row.hero);
}

void StatsHeroesList::ActionListPressRight(HeroRow & row, const Point & cursor, s16 ox, s16 oy)
{
    if(row.hero && (Rect(ox + 5, oy + 4, Interface::IconsBar::GetItemWidth(), Interface::IconsBar::GetItemHeight()) & cursor))
	Dialog::QuickInfo(*row.hero);
}

bool StatsHeroesList::ActionListCursor(HeroRow & row, const Point & cursor, s16 ox, s16 oy)
{
    if((row.armyBar.GetArea() & cursor) &&
	SelectArmyBar::QueueEventProcessing(row.armyBar))
    {
	if(row.artifactsBar->isSelected()) row.artifactsBar->ResetSelected();
	Cursor::Get().Hide();
	return true;
    }
    else
    if((row.artifactsBar->GetArea() & cursor) &&
	row.artifactsBar->QueueEventProcessing())
    {
	if(row.armyBar.isSelected()) row.armyBar.Reset();
	Cursor::Get().Hide();
	return true;
    }
    else
    if((row.primskillsBar->GetArea() & cursor) &&
	row.primskillsBar->QueueEventProcessing())
    {
	Cursor::Get().Hide();
	return true;
    }
    else
    if((row.secskillsBar.GetArea() & cursor) &&
	row.secskillsBar.QueueEventProcessing())
    {
	Cursor::Get().Hide();
	return true;
    }

    return false;
}

void StatsHeroesList::RedrawItem(const HeroRow & row, s16 dstx, s16 dsty, bool current)
{
    if(row.hero)
    {
	Text text("", Font::SMALL);
	const Sprite & back = AGG::GetICN(ICN::OVERVIEW, 10);
	back.Blit(dstx, dsty);

	// base info
	Interface::RedrawHeroesIcon(*row.hero, dstx + 5, dsty + 4);

	text.Set(GetString(row.hero->GetAttack()));
	text.Blit(dstx + 90 - text.w(), dsty + 20);

	text.Set(GetString(row.hero->GetDefense()));
	text.Blit(dstx + 125 - text.w(), dsty + 20);

	text.Set(GetString(row.hero->GetPower()));
	text.Blit(dstx + 160 - text.w(), dsty + 20);

	text.Set(GetString(row.hero->GetKnowledge()));
	text.Blit(dstx + 195 - text.w(), dsty + 20);

	// primary skills info
	const_cast<PrimarySkillsBar*>(row.primskillsBar)->SetPos(dstx + 56, dsty - 3);
	const_cast<PrimarySkillsBar*>(row.primskillsBar)->Redraw();

	// secondary skills info
	const_cast<SecondarySkillsBar &>(row.secskillsBar).SetPos(dstx + 206, dsty + 3);
	const_cast<SecondarySkillsBar &>(row.secskillsBar).Redraw();

	// artifacts info
	const_cast<ArtifactsBar*>(row.artifactsBar)->SetPos(dstx + 348, dsty + 3);
	const_cast<ArtifactsBar*>(row.artifactsBar)->Redraw();

	// army info
	const_cast<SelectArmyBar &>(row.armyBar).SetPos(Point(dstx -1, dsty + 30));
	const_cast<SelectArmyBar &>(row.armyBar).Redraw();
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

    void ActionListSingleClick(CASTLEPTR &, const Point &, s16, s16);
    void ActionListDoubleClick(CASTLEPTR &, const Point &, s16, s16);
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

void StatsCastlesList::ActionListDoubleClick(CASTLEPTR & cstl, const Point & cursor, s16 ox, s16 oy)
{
    ActionListSingleClick(cstl, cursor, ox, oy);
}

void StatsCastlesList::ActionListSingleClick(CASTLEPTR & cstl, const Point & cursor, s16 ox, s16 oy)
{
    if(cstl)
    {
	// click castle icon
	if(Rect(ox + 17, oy + 19, Interface::IconsBar::GetItemWidth(), Interface::IconsBar::GetItemHeight()) & cursor)
	{
	    Game::OpenCastleDialog(cstl);
	}
	else
	// click hero icon
	if(Rect(ox + 82, oy + 19, Interface::IconsBar::GetItemWidth(), Interface::IconsBar::GetItemHeight()) & cursor)
	{
	    Heroes* hero = cstl->GetHeroes().GuardFirst();
	    if(hero) Game::OpenHeroesDialog(hero);
	}
    }
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
	{
	    Interface::RedrawHeroesIcon(*hero, dstx + 82, dsty + 19);
	    text.Set(hero->StringSkills("-"));
	    text.Blit(dstx + 104 - text.w() / 2, dsty + 43);
	}

	text.Set(cstl->GetName());
	text.Blit(dstx + 72 - text.w() / 2, dsty + 62);

	// army info
	Army::DrawMons32Line(cstl->GetArmy(), dstx + 155, dsty + 30, 180);

	// available
	Troops troops;
	const u32 dwellings[] = { DWELLING_MONSTER1, DWELLING_MONSTER2, DWELLING_MONSTER3, DWELLING_MONSTER4, DWELLING_MONSTER5, DWELLING_MONSTER6 };

	for(u8 ii = 0; ii < ARRAY_COUNT(dwellings); ++ii)
	{
    	    u16 count = cstl->GetDwellingLivedCount(dwellings[ii]);
    	    if(count) troops.PushBack(Monster(cstl->GetRace(), cstl->GetActualDwelling(dwellings[ii])), count);
	}

	Army::DrawMons32Line(troops, dstx + 360, dsty + 30, 220);
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

std::string CapturedExtInfoString(u8 res, u8 color, const Funds & funds)
{
    std::ostringstream os;
    os << world.CountCapturedMines(res, color);
    const s32 vals = funds.Get(res);

    if(vals)
    {
	os << " " << "(";
	if(vals > 0) os << "+";
	os << vals << ")";
    }

    return os.str();
}

void RedrawIncomeInfo(const Point & pt, const Kingdom & myKingdom)
{
    const Funds & funds = myKingdom.GetFunds();
    const Funds income = myKingdom.GetIncome(INCOME_ARTIFACTS | INCOME_HEROSKILLS);

    Text text("", Font::SMALL);

    text.Set(CapturedExtInfoString(Resource::WOOD, myKingdom.GetColor(), income));
    text.Blit(pt.x + 54 - text.w() / 2, pt.y + 408);

    text.Set(CapturedExtInfoString(Resource::MERCURY, myKingdom.GetColor(), income));
    text.Blit(pt.x + 146 - text.w() / 2, pt.y + 408);

    text.Set(CapturedExtInfoString(Resource::ORE, myKingdom.GetColor(), income));
    text.Blit(pt.x + 228 - text.w() / 2, pt.y + 408);

    text.Set(CapturedExtInfoString(Resource::SULFUR, myKingdom.GetColor(), income));
    text.Blit(pt.x + 294 - text.w() / 2, pt.y + 408);

    text.Set(CapturedExtInfoString(Resource::CRYSTAL, myKingdom.GetColor(), income));
    text.Blit(pt.x + 360 - text.w() / 2, pt.y + 408);

    text.Set(CapturedExtInfoString(Resource::GEMS, myKingdom.GetColor(), income));
    text.Blit(pt.x + 428 - text.w() / 2, pt.y + 408);

    text.Set(CapturedExtInfoString(Resource::GOLD, myKingdom.GetColor(), income));
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

    text.Set(_("Gold Per Day:") + std::string(" ") + GetString(myKingdom.GetIncome().Get(Resource::GOLD)));
    text.Blit(pt.x + 180, pt.y + 462);

    std::string msg = _("Day: %{day}");
    String::Replace(msg, "%{day}", world.GetDay());
    text.Set(msg);
    text.Blit(pt.x + 360, pt.y + 462);
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
