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

#include <string>
#include "agg.h"
#include "localevent.h"
#include "button.h"
#include "world.h"
#include "cursor.h"
#include "config.h"
#include "heroes.h"
#include "background.h"
#include "tools.h"
#include "text.h"
#include "error.h"
#include "dialog.h"

Dialog::answer_t Heroes::OpenDialog(void)
{
/*
    // cursor
    Cursor::Hide();
    Cursor::Set(Cursor::POINTER);

    Dialog::FrameBorder background;

    const Point & cur_pt(background.GetArea());
    Point dst_pt(cur_pt);

    std::string str_icn;

    // town background
    switch(race)
    {
	case Race::KNGT: str_icn = "TOWNBKG0.ICN"; break;
	case Race::BARB: str_icn = "TOWNBKG1.ICN"; break;
	case Race::SORC: str_icn = "TOWNBKG2.ICN"; break;
	case Race::WRLK: str_icn = "TOWNBKG3.ICN"; break;
	case Race::WZRD: str_icn = "TOWNBKG4.ICN"; break;
	case Race::NECR: str_icn = "TOWNBKG5.ICN"; break;
	default: Error::Warning("Dialog::OpenCastle: unknown race."); return Dialog::CANCEL;
    }

    const Sprite & townbkg = AGG::GetICN(str_icn, 0);
    display.Blit(townbkg, dst_pt);

    // button prev castle
    dst_pt.y += 480 - 19;
    Button buttonPrevCastle(dst_pt, "SMALLBAR.ICN", 1, 2);

    // bottom small bar
    Dialog::StatusBar statusBar(Point(cur_pt.x + 21, cur_pt.y + 461), AGG::GetICN("SMALLBAR.ICN", 0), Font::BIG);
    statusBar.Clear();

    u8 index_sprite = 0;

    // button next castle
    dst_pt.x += 640 - 21;
    Button buttonNextCastle(dst_pt, "SMALLBAR.ICN", 3, 4);

    // strip grid
    str_icn = "STRIP.ICN";
    
    dst_pt.x = cur_pt.x;
    dst_pt.y = cur_pt.y + 256;

    display.Blit(AGG::GetICN(str_icn, 0), dst_pt);

    // color crest
    switch(color)
    {
	case Color::BLUE:	index_sprite = 0; break;
	case Color::GREEN:	index_sprite = 1; break;
	case Color::RED:	index_sprite = 2; break;
	case Color::YELLOW:	index_sprite = 3; break;
	case Color::ORANGE:	index_sprite = 4; break;
	case Color::PURPLE:	index_sprite = 5; break;
	default: Error::Warning("Dialog::OpenCastle: unknown color."); return Dialog::CANCEL;
    }

    const Sprite & crest = AGG::GetICN("CREST.ICN", index_sprite);

    dst_pt.x = cur_pt.x + 5;
    dst_pt.y = cur_pt.y + 262;

    display.Blit(crest, dst_pt);

    // castle troops selector
    dst_pt.x = cur_pt.x + 112;
    dst_pt.y = cur_pt.y + 262;
    Army::SelectBar selectCastleTroops(dst_pt, army);
    selectCastleTroops.Reset();
    selectCastleTroops.Redraw();
    const std::vector<Rect> & coordsCastleTrops = selectCastleTroops.GetCoords();

    // portrait heroes or captain or sign
    const Heroes * heroes = isHeroesPresent();

    dst_pt.x = cur_pt.x + 5;
    dst_pt.y = cur_pt.y + 361;

    if(heroes)
    {
	Heroes::heroes_t name = (*heroes).GetHeroes();
	std::string hero_portrate(10 > name ? "PORT000" : "PORT00");
	String::AddInt(hero_portrate, name);
	hero_portrate += ".ICN";

	display.Blit(AGG::GetICN(hero_portrate, 0), dst_pt);
    }
    else
    if(isBuild(Castle::BUILD_CAPTAIN))
	display.Blit(AGG::GetICN(Captain::BigPortraitString(race), 0), dst_pt);
    else
    	display.Blit(AGG::GetICN("STRIP.ICN", 3), dst_pt);

    // heroes troops background
    dst_pt.x = cur_pt.x + 112;
    dst_pt.y = cur_pt.y + 361;

    Army::SelectBar selectHeroesTroops(dst_pt, heroes ? (*heroes).GetArmy() : army);
    const std::vector<Rect> & coordsHeroesTrops = heroes ? selectHeroesTroops.GetCoords() : coordsCastleTrops;

    if(heroes)
    {
	selectHeroesTroops.Reset();
	selectHeroesTroops.Redraw();
    }
    else
    	display.Blit(AGG::GetICN("STRIP.ICN", 11), dst_pt);
    
    // resource
    RedrawResourcePanel();

    // button exit
    dst_pt.x = cur_pt.x + 553;
    dst_pt.y = cur_pt.y + 428;
    Button buttonExit(dst_pt, "SWAPBTN.ICN", 0, 1);

    LocalEvent & le = LocalEvent::GetLocalEvent();

    display.Flip();
    Cursor::Show();

    Dialog::answer_t result = Dialog::ZERO;
    bool exit = false;

    // dialog menu loop
    while(! exit){

        le.HandleEvents();

        le.MousePressLeft(buttonPrevCastle) ? buttonPrevCastle.Press() : buttonPrevCastle.Release();
        le.MousePressLeft(buttonNextCastle) ? buttonNextCastle.Press() : buttonNextCastle.Release();
        le.MousePressLeft(buttonExit) ? buttonExit.Press() : buttonExit.Release();
	
	// castle troops
	for(u8 ii = 0; ii < CASTLEMAXARMY; ++ii)
	{
	    if(le.MouseClickLeft(coordsCastleTrops[ii]))
	    {
		Cursor::Hide();
		if(selectCastleTroops.isSelected() || (heroes && selectHeroesTroops.isSelected()))
		{
		    // show dialog army info
		    if(army[ii].Valid() && selectCastleTroops.GetCursorIndex() == ii)
		    {
			//army[selectCastleTroops.GetCursorIndex()].ShowDialogInfo();
			selectCastleTroops.Reset();
			selectCastleTroops.Redraw();
		    }
		    else
		    // change army
		    if(army[ii].Valid())
		    {
			// from castle to castle
			if(selectCastleTroops.isSelected())
			{
			    const u8 index1 = selectCastleTroops.GetCursorIndex();
			    const Monster::monster_t monster = army[index1].GetMonster();
			    const u16 count = army[index1].GetCount();
			    army[index1] = army[ii];
			    army[ii].SetMonster(monster);
			    army[ii].SetCount(count);
			}
			else
			// from heroes to castle
			if(heroes && selectHeroesTroops.isSelected())
			{
			    std::vector<Army::Troops> & army2 = const_cast<std::vector<Army::Troops> &>((*heroes).GetArmy());
			    const u8 index2 = selectHeroesTroops.GetCursorIndex();
			    const Monster::monster_t monster = army2[index2].GetMonster();
			    const u16 count = army2[index2].GetCount();
			    army2[index2] = army[ii];
			    army[ii].SetMonster(monster);
			    army[ii].SetCount(count);

			    selectHeroesTroops.Reset();
			    selectHeroesTroops.Redraw();
			}
			
			selectCastleTroops.Reset();
			selectCastleTroops.Redraw();
		    }
		    // move to empty position
		    else
		    {
			// from castle to castle
			if(selectCastleTroops.isSelected())
			{
			    const u8 index1 = selectCastleTroops.GetCursorIndex();
			    army[ii] = army[index1];
			    army[index1].SetMonster(Monster::UNKNOWN);
			    army[index1].SetCount(0);
			}
			else
			// from heroes to castle
			if(heroes && selectHeroesTroops.isSelected())
			{
			    std::vector<Army::Troops> & army2 = const_cast<std::vector<Army::Troops> &>((*heroes).GetArmy());
			    const u8 index2 = selectHeroesTroops.GetCursorIndex();
			    army[ii] = army2[index2];
			    army2[index2].SetMonster(Monster::UNKNOWN);
			    army2[index2].SetCount(0);

			    selectHeroesTroops.Reset();
			    selectHeroesTroops.Redraw();
			}

			selectCastleTroops.Reset();
			selectCastleTroops.Redraw();
		    }
		}
		else
		// select army
		if(army[ii].Valid())
		{
		    selectCastleTroops.Reset();
		    selectCastleTroops.Select(ii);
		    selectCastleTroops.Redraw();
		}

		display.Flip();
		Cursor::Show();
	    }
	}

	// heroes troops
	if(heroes)
	{
	    std::vector<Army::Troops> & army2 = const_cast<std::vector<Army::Troops> &>((*heroes).GetArmy());

	    for(u8 ii = 0; ii < HEROESMAXARMY; ++ii) if(le.MouseClickLeft(coordsHeroesTrops[ii]))
	    {
		Cursor::Hide();
		if(selectCastleTroops.isSelected() || selectHeroesTroops.isSelected())
		{
		    // show dialog army info
		    if(army2[ii].Valid() && selectCastleTroops.GetCursorIndex() == ii)
		    {
			//army[selectCastleTroops.GetCursorIndex()].ShowDialogInfo();
			selectCastleTroops.Reset();
			selectCastleTroops.Redraw();
		    }
		    else
		    // change army
		    if(army2[ii].Valid())
		    {
			// from heroes to heroes
			if(selectHeroesTroops.isSelected())
			{
			    const u8 index2 = selectHeroesTroops.GetCursorIndex();
			    const Monster::monster_t monster = army2[index2].GetMonster();
			    const u16 count = army2[index2].GetCount();
			    army2[index2] = army2[ii];
			    army2[ii].SetMonster(monster);
			    army2[ii].SetCount(count);
			}
			else
			// from castle to heroes
			if(selectCastleTroops.isSelected())
			{
			    const u8 index1 = selectCastleTroops.GetCursorIndex();
			    const Monster::monster_t monster = army[index1].GetMonster();
			    const u16 count = army[index1].GetCount();
			    army[index1] = army2[ii];
			    army2[ii].SetMonster(monster);
			    army2[ii].SetCount(count);

			    selectCastleTroops.Reset();
			    selectCastleTroops.Redraw();
			}
			
			selectHeroesTroops.Reset();
			selectHeroesTroops.Redraw();
		    }
		    // move to empty position
		    else
		    {
			// from heroes to heroes
			if(selectHeroesTroops.isSelected())
			{
			    const u8 index2 = selectHeroesTroops.GetCursorIndex();
			    army2[ii] = army2[index2];
			    army2[index2].SetMonster(Monster::UNKNOWN);
			    army2[index2].SetCount(0);
			}
			else
			// from castle to heroes
			if(selectCastleTroops.isSelected())
			{
			    const u8 index1 = selectCastleTroops.GetCursorIndex();
			    army2[ii] = army[index1];
			    army[index1].SetMonster(Monster::UNKNOWN);
			    army[index1].SetCount(0);

			    selectCastleTroops.Reset();
			    selectCastleTroops.Redraw();
			}

			selectHeroesTroops.Reset();
			selectHeroesTroops.Redraw();
		    }
		}
		else
		// select army
		if(army[ii].Valid())
		{
		    selectHeroesTroops.Reset();
		    selectHeroesTroops.Select(ii);
		    selectHeroesTroops.Redraw();
		}

		display.Flip();
		Cursor::Show();
	    }
	}

        // prev castle
	if(le.MouseClickLeft(buttonPrevCastle)){ return Dialog::PREV; }

        // next castle
	if(le.MouseClickLeft(buttonNextCastle)){ return Dialog::NEXT; }

        // exit
	if(le.MouseClickLeft(buttonExit) || le.KeyPress(SDLK_ESCAPE)){ result = Dialog::CANCEL; exit = true; }
    }

    le.ResetKey();

    return result;
*/
    return Dialog::CANCEL;
}
