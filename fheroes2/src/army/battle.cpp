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

namespace Army {
   void CreateBackground(const Maps::Tiles & tile, const Point & dst_pt);
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

    const Point cur_pt(frameborder.GetArea().x, frameborder.GetArea().y);
    Point dst_pt(cur_pt);

    display.FillRect(0, 0, 0, Rect(dst_pt, 640, 480));

    CreateBackground(tile, dst_pt);
    LocalEvent & le = LocalEvent::GetLocalEvent();
    display.Flip();
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

void Army::CreateBackground(const Maps::Tiles & tile, const Point & dst_pt)
{
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
			icn = ICN::UNKNOWN;
		}
   display.Blit(AGG::GetICN(icn, 0), dst_pt);

}
