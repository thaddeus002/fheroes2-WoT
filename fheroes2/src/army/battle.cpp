/***************************************************************************
 *   Copyright (C) 2008 by Vasily Makarov                                  *
 *   drmoriarty@rambler.ru                                                 *
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


#include "battle.h"
#include "localevent.h"
#include "display.h"
#include "agg.h"
#include "sprite.h"
#include "ground.h"
#include "world.h"
#include "m82.h"
#include "button.h"
#include "cursor.h"

namespace Army {
    void DrawBackground(const Maps::Tiles & tile, const Point & dst_pt);
    void DrawCaptain(const Race::race_t race, const Point & dst_pt, u16 animframe, bool reflect=false);
    void DrawHero(const Heroes & hero, const Point & dst_pt, u16 animframe, bool reflect=false);
}

bool Army::Battle(Heroes& hero1, Heroes& hero2, const Maps::Tiles & tile)
{
   //CreateBackground(tile);
	LocalEvent & le = LocalEvent::GetLocalEvent();
	// dialog menu loop
	while(le.HandleEvents())
	   {
	      // exit
	      if(le.KeyPress(KEY_ESCAPE)) {
		 Display::Get().Fade();
		 return true;
	      }
	   }
	return false;	
}

bool Army::Battle(Heroes& hero, std::vector<Army::Troops>& army, const Maps::Tiles & tile)
{
    Display & display = Display::Get();
    Dialog::FrameBorder frameborder;

    Cursor & cursor = Cursor::Get();
    cursor.SetThemes(cursor.POINTER);

    const Point cur_pt(frameborder.GetArea().x, frameborder.GetArea().y);
    Point dst_pt(cur_pt);

    display.FillRect(0, 0, 0, Rect(dst_pt, 640, 480));

    DrawBackground(tile, dst_pt);
    DrawHero(hero, dst_pt, 1);

    Dialog::StatusBar statusBar(Point(cur_pt.x + 22, cur_pt.y + 440), AGG::GetICN(ICN::HSBTNS, 8), Font::BIG);
    statusBar.Clear("Hero Screen");

    Button button1(200, 200, ICN::CMBTMISC, 2, 3);

    LocalEvent & le = LocalEvent::GetLocalEvent();
    
    button1.Draw();
    cursor.Show();
    display.Flip();

    int i = 0;
    // dialog menu loop
    while(le.HandleEvents()) {
	// exit
	if(le.KeyPress(KEY_ESCAPE)) {
	    Display::Get().Fade();
	    return true;
	}
	le.MousePressLeft(button1) ? button1.PressDraw() : button1.ReleaseDraw();
	if(le.KeyPress(KEY_RETURN)) {
	    display.Blit(AGG::GetICN(ICN::CFLGSMAL, i++), dst_pt);
	}
	display.Flip();
    }
    return false;	
}

bool Army::Battle(std::vector<Army::Troops>& army1, std::vector<Army::Troops> army2, const Maps::Tiles & tile)
{
   //CreateBackground(tile);
	LocalEvent & le = LocalEvent::GetLocalEvent();
	// dialog menu loop
	while(le.HandleEvents())
	   {
	      // exit
	      if(le.KeyPress(KEY_ESCAPE)) {
		 Display::Get().Fade();
		 return true;
	      }
	   }
	return false;	
}

void Army::DrawBackground(const Maps::Tiles & tile, const Point & dst_pt)
{
    AGG::PlaySound(M82::PREBATTL);
    Display & display = Display::Get();
    ICN::icn_t icn;
    bool trees = false;
    u16 index = tile.GetIndex();
    u16 x = index%world.w(), y = index/world.w();
    if((x < world.w()-1 && world.GetTiles(index+1).GetObject() == MP2::OBJ_TREES) ||
       (x > 0 && world.GetTiles(index-1).GetObject() == MP2::OBJ_TREES) ||
       (y < world.h()-1 && world.GetTiles(index+world.w()).GetObject() == MP2::OBJ_TREES) ||
       (y > 0 && world.GetTiles(index-world.w()).GetObject() == MP2::OBJ_TREES) )
	trees = true;
    if(tile.GetObject() == MP2::OBJN_GRAVEYARD)
	icn = ICN::CBKGGRAV;
    else switch(tile.GetGround()) {
	case Maps::Ground::DESERT: icn = ICN::CBKGDSRT; break;
	case Maps::Ground::SNOW: 
	    icn = trees ? ICN::CBKGSNTR : ICN::CBKGSNMT; break;
	case Maps::Ground::SWAMP: icn = ICN::CBKGSWMP; break;
	case Maps::Ground::WASTELAND: icn = ICN::CBKGCRCK; break;
	case Maps::Ground::BEACH: icn = ICN::CBKGBEAC; break;
	case Maps::Ground::LAVA: icn = ICN::CBKGLAVA; break;
	case Maps::Ground::DIRT: 
	    icn = trees ? ICN::CBKGDITR : ICN::CBKGDIMT; break;
	case Maps::Ground::GRASS:
	    icn = trees ? ICN::CBKGGRTR : ICN::CBKGGRMT; break;
	case Maps::Ground::WATER: icn = ICN::CBKGWATR; break;
	default:
	    return;
	}
    display.Blit(AGG::GetICN(icn, 0), dst_pt);

}

void Army::DrawCaptain(const Race::race_t race, const Point & dst_pt, u16 animframe, bool reflect)
{
    Display & display = Display::Get();
    ICN::icn_t cap;
    switch(race) {
    case Race::BARB: cap = ICN::CMBTCAPB; break;
    case Race::KNGT: cap = ICN::CMBTCAPK; break;
    case Race::NECR: cap = ICN::CMBTCAPN; break;
    case Race::SORC: cap = ICN::CMBTCAPS; break;
    case Race::WRLK: cap = ICN::CMBTCAPW; break;
    case Race::WZRD: cap = ICN::CMBTCAPZ; break;
    default: return;
    }
    display.Blit(AGG::GetICN(cap, animframe, reflect), dst_pt.x + (reflect?550:0), dst_pt.y + 75);
}

void Army::DrawHero(const Heroes & hero, const Point & dst_pt, u16 animframe, bool reflect)
{
    Display & display = Display::Get();
    ICN::icn_t cap;
    switch(hero.GetRace()) {
    case Race::BARB: cap = ICN::CMBTHROB; break;
    case Race::KNGT: cap = ICN::CMBTHROK; break;
    case Race::NECR: cap = ICN::CMBTHRON; break;
    case Race::SORC: cap = ICN::CMBTHROS; break;
    case Race::WRLK: cap = ICN::CMBTHROW; break;
    case Race::WZRD: cap = ICN::CMBTHROZ; break;
    default: return;
    }
    display.Blit(AGG::GetICN(cap, animframe, reflect), dst_pt.x + (reflect?550:0), dst_pt.y + 75);
    display.Blit(AGG::GetICN(ICN::CFLGSMAL, Color::GetIndex(hero.GetColor())), dst_pt.x + (reflect?550:18), dst_pt.y + 80);
}

