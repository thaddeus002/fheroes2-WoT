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

#include <bitset>
#include "agg.h"
#include "config.h"
#include "sprite.h"
#include "localevent.h"
#include "button.h"
#include "dialog.h"
#include "world.h"
#include "gamearea.h"
#include "cursor.h"
#include "radar.h"
#include "splitter.h"
#include "spritecursor.h"
#include "sizecursor.h"
#include "direction.h"
#include "maps_tiles.h"
#include "ground.h"
#include "error.h"
#include "tools.h"
#include "rand.h"
#include "game.h"

namespace Game
{
    namespace Editor
    {
	void RedrawTopNumberCell(const Rect & area);
	void RedrawLeftNumberCell(const Rect & area);
	void ModifyAllTilesAbroad(void);
	void SetGroundToTile(Maps::Tiles & tile, const Maps::Ground::ground_t ground);
    };
};

Game::menu_t Game::Editor::StartGame()
{
    Display & display = Display::Get();

    // cursor
    Cursor & cursor = Cursor::Get();
    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);
    const Size & videomode = Settings::Get().VideoMode();
    Display::SetVideoMode(videomode);
    display.Fill(0x00, 0x00, 0x00);

    GameArea & areaMaps = GameArea::Get();
    areaMaps.Build();

    const Rect area_pos(BORDERWIDTH, BORDERWIDTH, GameArea::GetRect().w * TILEWIDTH, GameArea::GetRect().h * TILEWIDTH);

    const Rect areaScrollLeft(0, BORDERWIDTH, BORDERWIDTH / 2, display.h() - 2 * BORDERWIDTH);
    const Rect areaScrollRight(display.w() - BORDERWIDTH / 2, BORDERWIDTH, BORDERWIDTH / 2, display.h() - 2 * BORDERWIDTH);
    const Rect areaScrollTop(BORDERWIDTH, 0, areaMaps.GetRect().w * TILEWIDTH, BORDERWIDTH / 2);
    const Rect areaScrollBottom(2 * BORDERWIDTH, display.h() - BORDERWIDTH / 2, (areaMaps.GetRect().w - 1) * TILEWIDTH, BORDERWIDTH / 2);
    const Rect areaLeftPanel(display.w() - 2 * BORDERWIDTH - RADARWIDTH, 0, BORDERWIDTH + RADARWIDTH, display.h());

    Rect src_rt;
    Point dst_pt;

    // draw interface
    const Sprite & spriteAdv = AGG::GetICN(ICN::ADVBORD, 0);
    const Sprite & spriteBac = AGG::GetICN(ICN::STONBACK, 0);
    // bottom scroll bar indicator
    const Sprite & spriteBottomBar(AGG::GetICN(ICN::ESCROLL, 0));
    dst_pt.x = BORDERWIDTH * 2;
    dst_pt.y = BORDERWIDTH + areaMaps.GetRect().h * TILEWIDTH;
    src_rt.x = 0;
    src_rt.y = 0;
    src_rt.w = TILEWIDTH;
    src_rt.h = BORDERWIDTH;
    display.Blit(spriteBottomBar, src_rt, dst_pt);
    src_rt.x = TILEWIDTH;
    for(u16 ii = 0; ii < (areaMaps.GetRect().w - 3); ++ii)
    {
	dst_pt.x += TILEWIDTH;
	display.Blit(spriteBottomBar, src_rt, dst_pt);
    }
    dst_pt.x += TILEWIDTH;
    src_rt.x = spriteBottomBar.w() - TILEWIDTH;
    display.Blit(spriteBottomBar, src_rt, dst_pt);
    // left scroll bar indicator
    const Sprite & spriteLeftBar(AGG::GetICN(ICN::ESCROLL, 1));
    dst_pt.x = BORDERWIDTH + areaMaps.GetRect().w * TILEWIDTH;
    dst_pt.y = BORDERWIDTH * 2;
    src_rt.x = 0;
    src_rt.y = 0;
    src_rt.w = BORDERWIDTH;
    src_rt.h = TILEWIDTH;
    display.Blit(spriteLeftBar, src_rt, dst_pt);
    src_rt.y = TILEWIDTH;
    for(u16 ii = 0; ii < (areaMaps.GetRect().h - 3); ++ii)
    {
	dst_pt.y += TILEWIDTH;
	display.Blit(spriteLeftBar, src_rt, dst_pt);
    }
    dst_pt.y += TILEWIDTH;
    src_rt.y = spriteLeftBar.h() - TILEWIDTH;
    display.Blit(spriteLeftBar,src_rt, dst_pt);
    // left top static border
    src_rt.x = spriteAdv.w() - BORDERWIDTH - RADARWIDTH;
    src_rt.y = 0;
    src_rt.w = BORDERWIDTH + RADARWIDTH;
    src_rt.h = BORDERWIDTH;
    dst_pt.x = 2 * BORDERWIDTH + areaMaps.GetRect().w * TILEWIDTH;
    dst_pt.y = 0;
    display.Blit(spriteAdv, src_rt, dst_pt);
    // left bottom static border
    src_rt.x = spriteAdv.w() - BORDERWIDTH - RADARWIDTH;
    src_rt.y = spriteAdv.h() - BORDERWIDTH;
    src_rt.w = BORDERWIDTH + RADARWIDTH;
    src_rt.h = BORDERWIDTH;
    dst_pt.x = 2 * BORDERWIDTH + areaMaps.GetRect().w * TILEWIDTH;
    dst_pt.y = BORDERWIDTH + areaMaps.GetRect().h * TILEWIDTH;
    display.Blit(spriteAdv, src_rt, dst_pt);
    // left static border
    src_rt.x = spriteAdv.w() - BORDERWIDTH;
    src_rt.y = 0;
    src_rt.w = BORDERWIDTH;
    src_rt.h = 250;
    dst_pt.x = display.w() - BORDERWIDTH;
    dst_pt.y = 0;
    display.Blit(spriteAdv, src_rt, dst_pt);
    //
    src_rt.y = 250;
    src_rt.h = TILEWIDTH;
    dst_pt.y = 250;
    //
    u8 var1 = 4 + (videomode.h - 480) / TILEWIDTH;

    for(u8 ii = 0; ii < var1; ++ii)
    {
	display.Blit(spriteAdv, src_rt, dst_pt);
	dst_pt.y += TILEWIDTH;
    }
    //
    src_rt.y = spriteAdv.h() - 102;
    src_rt.h = 102;
    dst_pt.y = display.h() - 102;
    display.Blit(spriteAdv, src_rt, dst_pt);

    // buttons
    dst_pt.x = 0;
    dst_pt.y = 0;
    Button btnLeftTopScroll(dst_pt, ICN::ESCROLL, 12, 13);

    dst_pt.x = BORDERWIDTH + areaMaps.GetRect().w * TILEWIDTH;
    Button btnRightTopScroll(dst_pt, ICN::ESCROLL, 14, 15);
	
    dst_pt.y = BORDERWIDTH;
    Button btnTopScroll(dst_pt, ICN::ESCROLL, 4, 5);

    dst_pt.x = 0;
    dst_pt.y = BORDERWIDTH + areaMaps.GetRect().h * TILEWIDTH;
    Button btnLeftBottomScroll(dst_pt, ICN::ESCROLL, 18, 19);

    dst_pt.x = BORDERWIDTH;
    Button btnLeftScroll(dst_pt, ICN::ESCROLL, 8, 9);

    dst_pt.x = 2 * BORDERWIDTH + (areaMaps.GetRect().w - 1) * TILEWIDTH;
    Button btnRightScroll(dst_pt, ICN::ESCROLL, 10, 11);

    dst_pt.x = BORDERWIDTH + areaMaps.GetRect().w * TILEWIDTH;
    Button btnRightBottomScroll(dst_pt, ICN::ESCROLL, 16, 17);

    dst_pt.y = areaMaps.GetRect().h * TILEWIDTH;
    Button btnBottomScroll(dst_pt, ICN::ESCROLL, 6, 7);
    
    dst_pt.x = display.w() - BORDERWIDTH - RADARWIDTH;
    dst_pt.y = BORDERWIDTH + RADARWIDTH;
    Button btnSelectGround(dst_pt, ICN::EDITBTNS, 0, 1);

    dst_pt.x = btnSelectGround.x + btnSelectGround.w;
    dst_pt.y = btnSelectGround.y;
    Button btnSelectObject(dst_pt, ICN::EDITBTNS, 2, 3);

    dst_pt.x = btnSelectObject.x + btnSelectObject.w;
    dst_pt.y = btnSelectObject.y;
    Button btnSelectInfo(dst_pt, ICN::EDITBTNS, 4, 5);

    dst_pt.x = btnSelectGround.x;
    dst_pt.y = btnSelectGround.y + btnSelectGround.h;
    Button btnSelectRiver(dst_pt, ICN::EDITBTNS, 6, 7);

    dst_pt.x = btnSelectRiver.x + btnSelectRiver.w;
    dst_pt.y = btnSelectRiver.y;
    Button btnSelectRoad(dst_pt, ICN::EDITBTNS, 8, 9);

    dst_pt.x = btnSelectRoad.x + btnSelectRoad.w;
    dst_pt.y = btnSelectRoad.y;
    Button btnSelectClear(dst_pt, ICN::EDITBTNS, 10, 11);

    const Point dstPanel(btnSelectRiver.x, btnSelectRiver.y + btnSelectRiver.h);
    const Sprite & spritePanelGround = AGG::GetICN(ICN::EDITPANL, 0);
    const Sprite & spritePanelObject = AGG::GetICN(ICN::EDITPANL, 1);
    const Sprite & spritePanelInfo = AGG::GetICN(ICN::EDITPANL, 2);
    const Sprite & spritePanelRiver = AGG::GetICN(ICN::EDITPANL, 3);
    const Sprite & spritePanelRoad = AGG::GetICN(ICN::EDITPANL, 4);
    const Sprite & spritePanelClear = AGG::GetICN(ICN::EDITPANL, 5);

    dst_pt.x = dstPanel.x + 14;
    dst_pt.y = dstPanel.y + 127;
    Button btnSizeSmall(dst_pt, ICN::EDITBTNS, 24, 25);
    dst_pt.x = dstPanel.x + 44;
    Button btnSizeMedium(dst_pt, ICN::EDITBTNS, 26, 27);
    dst_pt.x = dstPanel.x + 74;
    Button btnSizeLarge(dst_pt, ICN::EDITBTNS, 28, 29);
    dst_pt.x = dstPanel.x + 104;
    Button btnSizeManual(dst_pt, ICN::EDITBTNS, 30, 31);

    dst_pt.x = dstPanel.x;
    dst_pt.y = dstPanel.y + spritePanelGround.h();
    Button btnZoom(dst_pt, ICN::EDITBTNS, 12, 13);

    dst_pt.x = btnZoom.x + btnZoom.w;
    dst_pt.y = btnZoom.y;
    Button btnUndo(dst_pt, ICN::EDITBTNS, 14, 15);

    dst_pt.x = btnUndo.x + btnUndo.w;
    dst_pt.y = btnUndo.y;
    Button btnNew(dst_pt, ICN::EDITBTNS, 16, 17);

    dst_pt.x = btnZoom.x;
    dst_pt.y = btnZoom.y + btnZoom.h;
    Button btnSpec(dst_pt, ICN::EDITBTNS, 18, 19);

    dst_pt.x = btnSpec.x + btnSpec.w;
    dst_pt.y = btnSpec.y;
    Button btnFile(dst_pt, ICN::EDITBTNS, 20, 21);

    dst_pt.x = btnFile.x + btnFile.w;
    dst_pt.y = btnFile.y;
    Button btnSystem(dst_pt, ICN::EDITBTNS, 22, 23);

    // bottom static
    var1 = (videomode.h - 480) / TILEWIDTH - 2;

    src_rt.x = 0;
    src_rt.y = 0;
    src_rt.w = spriteBac.w();
    src_rt.h = TILEWIDTH;
    dst_pt.x = btnSpec.x;
    dst_pt.y = btnSpec.y + btnSpec.h;
    if(var1) display.Blit(spriteBac, src_rt, dst_pt);
    src_rt.y = TILEWIDTH;
    dst_pt.y += TILEWIDTH;
    for(u8 ii = 0; ii < var1; ++ii)
    {
	display.Blit(spriteBac, src_rt, dst_pt);
	dst_pt.y += TILEWIDTH;
    }
    src_rt.y = spriteBac.h() - TILEWIDTH;
    if(var1) display.Blit(spriteBac, src_rt, dst_pt);

    // splitter
    Splitter split_h(AGG::GetICN(ICN::ESCROLL, 2), Rect(2 * BORDERWIDTH + 3, display.h() - BORDERWIDTH + 4, (areaMaps.GetRect().w - 1) * TILEWIDTH - 6, BORDERWIDTH - 8), Splitter::HORIZONTAL);
    Splitter split_v(AGG::GetICN(ICN::ESCROLL, 3), Rect(BORDERWIDTH + areaMaps.GetRect().w * TILEWIDTH + 4, 2 * BORDERWIDTH + 3, BORDERWIDTH - 8, (areaMaps.GetRect().h - 1) * TILEWIDTH - 6), Splitter::VERTICAL);

    split_h.SetRange(0, world.w() - areaMaps.GetRect().w);
    split_v.SetRange(0, world.h() - areaMaps.GetRect().h);

    split_h.Move(areaMaps.GetRect().x);
    split_v.Move(areaMaps.GetRect().y);

    // Create radar
    Radar & radar = Radar::Get();
    radar.Build();

    areaMaps.Redraw();
    radar.RedrawArea();

    // Create radar cursor
    radar.RedrawCursor();

    RedrawTopNumberCell(areaMaps.GetRect());
    RedrawLeftNumberCell(areaMaps.GetRect());

    LocalEvent & le = LocalEvent::GetLocalEvent();

    btnLeftTopScroll.Draw();
    btnRightTopScroll.Draw();
    btnTopScroll.Draw();
    btnLeftBottomScroll.Draw();
    btnLeftScroll.Draw();
    btnRightScroll.Draw();
    btnRightBottomScroll.Draw();
    btnBottomScroll.Draw();

    btnZoom.Draw();
    btnUndo.Draw();
    btnNew.Draw();
    btnSpec.Draw();
    btnFile.Draw();
    btnSystem.Draw();

	
    btnSelectObject.Draw();
    btnSelectInfo.Draw();
    btnSelectRiver.Draw();
    btnSelectRoad.Draw();
    btnSelectClear.Draw();

    btnSelectGround.Press();
    btnSelectGround.Draw();
    display.Blit(spritePanelGround, dstPanel);

    btnSizeSmall.Draw();
    btnSizeLarge.Draw();
    btnSizeManual.Draw();

    btnSizeMedium.Press();
    btnSizeMedium.Draw();
    
    SizeCursor sizeCursor;
    
    sizeCursor.ModifySize(2, 2);
    sizeCursor.Hide();

    const Rect rectTerrainWater(dstPanel.x + 29, dstPanel.y + 10, 28, 28);
    const Rect rectTerrainGrass(dstPanel.x + 58, dstPanel.y + 10, 28, 28);
    const Rect rectTerrainSnow(dstPanel.x + 87, dstPanel.y + 10, 28, 28);
    const Rect rectTerrainSwamp(dstPanel.x + 29, dstPanel.y + 39, 28, 28);
    const Rect rectTerrainLava(dstPanel.x + 58, dstPanel.y + 39, 28, 28);
    const Rect rectTerrainBeach(dstPanel.x + 87, dstPanel.y + 39, 28, 28);
    const Rect rectTerrainDirt(dstPanel.x + 29, dstPanel.y + 68, 28, 28);
    const Rect rectTerrainWasteland(dstPanel.x + 58, dstPanel.y + 68, 28, 28);
    const Rect rectTerrainDesert(dstPanel.x + 87, dstPanel.y + 68, 28, 28);

    const Rect rectObjectWater(dstPanel.x + 14, dstPanel.y + 11, 28, 28);
    const Rect rectObjectGrass(dstPanel.x + 43, dstPanel.y + 11, 28, 28);
    const Rect rectObjectSnow(dstPanel.x + 72, dstPanel.y + 11, 28, 28);
    const Rect rectObjectSwamp(dstPanel.x + 101, dstPanel.y + 11, 28, 28);
    const Rect rectObjectLava(dstPanel.x + 14, dstPanel.y + 39, 28, 28);
    const Rect rectObjectDesert(dstPanel.x + 43, dstPanel.y + 39, 28, 28);
    const Rect rectObjectDirt(dstPanel.x + 72, dstPanel.y + 39, 28, 28);
    const Rect rectObjectWasteland(dstPanel.x + 101, dstPanel.y + 39, 28, 28);
    const Rect rectObjectBeach(dstPanel.x + 14, dstPanel.y + 67, 28, 28);
    const Rect rectObjectTown(dstPanel.x + 43, dstPanel.y + 67, 28, 28);
    const Rect rectObjectMonster(dstPanel.x + 72, dstPanel.y + 67, 28, 28);
    const Rect rectObjectHero(dstPanel.x + 101, dstPanel.y + 67, 28, 28);
    const Rect rectObjectArtifact(dstPanel.x + 14, dstPanel.y + 95, 28, 28);
    const Rect rectObjectResource(dstPanel.x + 101, dstPanel.y + 95, 28, 28);

    SpriteCursor selectTerrainCursor(AGG::GetICN(ICN::TERRAINS, 9), rectTerrainWater.x - 1, rectTerrainWater.y - 1);
    selectTerrainCursor.Show();

    SpriteCursor selectObjectCursor(AGG::GetICN(ICN::TERRAINS, 9), rectObjectWater.x - 1, rectObjectWater.y - 1);

    u8 selectTerrain = 0;
    u8 selectObject = 0;

    bool selectTerrainEnable = true;
    bool selectObjectEnable = false;

    cursor.Show();
    display.Flip();

    //u32 ticket = 0;

    // startgame loop
    while(le.HandleEvents())
    {
	// ESC
	if(le.KeyPress(KEY_ESCAPE) && (Dialog::YES & Dialog::Message("", "Are you sure you want to quit?", Font::BIG, Dialog::YES|Dialog::NO))) return QUITGAME;

	// scroll area maps left
	if(le.MouseCursor(areaScrollLeft))
	{
	    cursor.Hide();
	    sizeCursor.Hide();
	    cursor.SetThemes(cursor.SCROLL_LEFT);
	    areaMaps.Scroll(GameArea::LEFT);
	    split_h.Backward();
	    radar.RedrawCursor();
	    RedrawTopNumberCell(areaMaps.GetRect());
	    cursor.Show();
	    display.Flip();
	    continue;
	}
	else
	// scroll area maps right
	if(le.MouseCursor(areaScrollRight))
	{
	    cursor.Hide();
	    sizeCursor.Hide();
	    cursor.SetThemes(cursor.SCROLL_RIGHT);
	    areaMaps.Scroll(GameArea::RIGHT);
	    split_h.Forward();
	    radar.RedrawCursor();
	    RedrawTopNumberCell(areaMaps.GetRect());
	    cursor.Show();
	    display.Flip();
	    continue;
	}
	else
	// scroll area maps top
	if(le.MouseCursor(areaScrollTop))
	{
	    cursor.Hide();
	    sizeCursor.Hide();
	    cursor.SetThemes(cursor.SCROLL_TOP);
	    areaMaps.Scroll(GameArea::TOP);
	    split_v.Backward();
	    radar.RedrawCursor();
	    RedrawLeftNumberCell(areaMaps.GetRect());
	    cursor.Show();
	    display.Flip();
	    continue;
	}
	else
	// scroll area maps bottom
	if(le.MouseCursor(areaScrollBottom))
	{
	    cursor.Hide();
	    sizeCursor.Hide();
	    cursor.SetThemes(cursor.SCROLL_BOTTOM);
	    areaMaps.Scroll(GameArea::BOTTOM);
	    split_v.Forward();
	    radar.RedrawCursor();
	    RedrawLeftNumberCell(areaMaps.GetRect());
	    cursor.Show();
	    display.Flip();
	    continue;
	}
	else
	// point radar
	if(le.MouseCursor(radar.GetRect()) &&
	    (le.MouseClickLeft(radar.GetRect()) ||
		le.MousePressLeft(radar.GetRect())))
	{
	    Rect prev(areaMaps.GetRect());
	    areaMaps.CenterFromRadar(le.MouseCursor());
	    if(prev != areaMaps.GetRect())
	    {
		cursor.Hide();
		cursor.SetThemes(cursor.POINTER);
		sizeCursor.Hide();
		split_h.Move(areaMaps.GetRect().x);
		split_v.Move(areaMaps.GetRect().y);
		radar.RedrawCursor();
		RedrawTopNumberCell(areaMaps.GetRect());
		RedrawLeftNumberCell(areaMaps.GetRect());
		cursor.Show();
		display.Flip();
	    }
	}
	else
	// pointer cursor on left panel
	if(le.MouseCursor(areaLeftPanel))
	{
	    cursor.Hide();
	    cursor.SetThemes(cursor.POINTER);
	    sizeCursor.Hide();
	    cursor.Show();
	    display.Flip();
	}
	else
	// cursor over game area
	if(le.MouseCursor(area_pos))
	{
            const Point & mouse_coord = le.MouseCursor();
            const u16 index_maps = Maps::GetIndexFromAreaPoint(mouse_coord);
            Maps::Tiles & tile = world.GetTiles(index_maps);
            const Rect tile_pos(BORDERWIDTH + ((u16) (mouse_coord.x - BORDERWIDTH) / TILEWIDTH) * TILEWIDTH, BORDERWIDTH + ((u16) (mouse_coord.y - BORDERWIDTH) / TILEWIDTH) * TILEWIDTH, TILEWIDTH, TILEWIDTH);
            //u8 object = tile.GetObject();

    	    cursor.SetThemes(cursor.POINTER);

	    const u16 div_x = mouse_coord.x < BORDERWIDTH + TILEWIDTH * (GameArea::GetRect().w - sizeCursor.w()) ?
			    (u16) ((mouse_coord.x - BORDERWIDTH) / TILEWIDTH) * TILEWIDTH + BORDERWIDTH :
			    BORDERWIDTH + (GameArea::GetRect().w - sizeCursor.w()) * TILEWIDTH;
	    const u16 div_y = mouse_coord.y < BORDERWIDTH + TILEWIDTH * (GameArea::GetRect().h - sizeCursor.h()) ?
			    (u16) ((mouse_coord.y - BORDERWIDTH) / TILEWIDTH) * TILEWIDTH + BORDERWIDTH :
			    BORDERWIDTH + (GameArea::GetRect().h - sizeCursor.h()) * TILEWIDTH;

	    if(! sizeCursor.isVisible() || sizeCursor.GetPos().x != div_x || sizeCursor.GetPos().y != div_y)
	    {
		cursor.Hide();

		sizeCursor.Hide();

		sizeCursor.Show(div_x, div_y);

		cursor.Show();
		display.Flip();
	    }

	    if(le.MouseLeft())
	    {
		cursor.Hide();
		sizeCursor.Hide();


		const Point topleft(GameArea::GetRect().x + (div_x - BORDERWIDTH) / 32,
				    GameArea::GetRect().y + (div_y - BORDERWIDTH) / 32);

		
		for(u8 iy = 0; iy < sizeCursor.h(); ++iy)
		{
		    for(u8 ix = 0; ix < sizeCursor.w(); ++ix)
		    {
                	Maps::Tiles & newtile = world.GetTiles(topleft.x + ix, topleft.y + iy);

			switch(selectTerrain)
			{
			    case 0: SetGroundToTile(newtile, Maps::Ground::WATER);	break;
			    case 1: SetGroundToTile(newtile, Maps::Ground::GRASS);	break;
			    case 2: SetGroundToTile(newtile, Maps::Ground::SNOW);	break;
			    case 3: SetGroundToTile(newtile, Maps::Ground::SWAMP);	break;
			    case 4: SetGroundToTile(newtile, Maps::Ground::LAVA);	break;
			    case 5: SetGroundToTile(newtile, Maps::Ground::DESERT);	break;
			    case 6: SetGroundToTile(newtile, Maps::Ground::DIRT);	break;
			    case 7: SetGroundToTile(newtile, Maps::Ground::WASTELAND);	break;
			    case 8: SetGroundToTile(newtile, Maps::Ground::BEACH);	break;

			    default: break;
			}

			newtile.RedrawAll();
		    }
		}
		
		//GetIndexFromAbsPoint(
		//Error::Verbose("X: ", GameArea::GetRect().x + (div_x - BORDERWIDTH) / 32);
		//Error::Verbose("Y: ", GameArea::GetRect().y + (div_y - BORDERWIDTH) / 32);
		//Maps::Tiles & tile = world.GetTiles(topleft);
		//for(u8 ii = 0; ii < sizeCursor.w(); ++ii)
		//{
		    //from + ii;

		    //if(! isValidDirection(from + ii, Direction::LEFT)) break;
		//}

		//u16 GetDirectionIndex(u16 from, Direction::vector_t vector);
		//bool isValidDirection(u16 from, Direction::vector_t vector);    
		//sizeCursor.GetPos();

		ModifyAllTilesAbroad();

		sizeCursor.Show();
		cursor.Show();

		display.Flip();

		// wait
		while(le.HandleEvents() && le.MouseLeft());

	    }
	    else
	    if(le.MouseRight())
	    {
		if(H2Config::Debug()) tile.DebugInfo(index_maps);

		// wait
		while(le.HandleEvents() && le.MouseRight());

		//const std::string & info = (MP2::OBJ_ZERO == object || MP2::OBJ_EVENT == object ?
		//    Maps::Ground::String(tile.GetGround()) : MP2::StringObject(object));

		//Dialog::QuickInfo(info);
	    }
	// end cursor over game area
	}


	// draw push buttons
	le.MousePressLeft(btnLeftTopScroll) ? btnLeftTopScroll.PressDraw() : btnLeftTopScroll.ReleaseDraw();
	le.MousePressLeft(btnRightTopScroll) ? btnRightTopScroll.PressDraw() : btnRightTopScroll.ReleaseDraw();
	le.MousePressLeft(btnTopScroll) ? btnTopScroll.PressDraw() : btnTopScroll.ReleaseDraw();
	le.MousePressLeft(btnLeftBottomScroll) ? btnLeftBottomScroll.PressDraw() : btnLeftBottomScroll.ReleaseDraw();
	le.MousePressLeft(btnLeftScroll) ? btnLeftScroll.PressDraw() : btnLeftScroll.ReleaseDraw();
	le.MousePressLeft(btnRightScroll) ? btnRightScroll.PressDraw() : btnRightScroll.ReleaseDraw();
	le.MousePressLeft(btnRightBottomScroll) ? btnRightBottomScroll.PressDraw() : btnRightBottomScroll.ReleaseDraw();
	le.MousePressLeft(btnBottomScroll) ? btnBottomScroll.PressDraw() : btnBottomScroll.ReleaseDraw();

	le.MousePressLeft(btnZoom) ? btnZoom.PressDraw() : btnZoom.ReleaseDraw();
	le.MousePressLeft(btnUndo) ? btnUndo.PressDraw() : btnUndo.ReleaseDraw();
	le.MousePressLeft(btnNew) ? btnNew.PressDraw() : btnNew.ReleaseDraw();
	le.MousePressLeft(btnSpec) ? btnSpec.PressDraw() : btnSpec.ReleaseDraw();
	le.MousePressLeft(btnFile) ? btnFile.PressDraw() : btnFile.ReleaseDraw();
	le.MousePressLeft(btnSystem) ? btnSystem.PressDraw() : btnSystem.ReleaseDraw();


	// click control button
	if(le.MouseClickLeft(btnSelectGround) ||
	    le.MouseClickLeft(btnSelectObject) ||
	    le.MouseClickLeft(btnSelectInfo) ||
	    le.MouseClickLeft(btnSelectRiver) ||
	    le.MouseClickLeft(btnSelectRoad) ||
	    le.MouseClickLeft(btnSelectClear))
	{
	    cursor.Hide();

	    if(btnSelectGround.isPressed()){ btnSelectGround.Release(); btnSelectGround.Draw(); }
	    else
	    if(btnSelectObject.isPressed()){ btnSelectObject.Release(); btnSelectObject.Draw(); }
	    else
	    if(btnSelectInfo.isPressed()){ btnSelectInfo.Release(); btnSelectInfo.Draw(); }
	    else
	    if(btnSelectRiver.isPressed()){ btnSelectRiver.Release(); btnSelectRiver.Draw(); }
	    else
	    if(btnSelectRoad.isPressed()){ btnSelectRoad.Release(); btnSelectRoad.Draw(); }
	    else
	    if(btnSelectClear.isPressed()){ btnSelectClear.Release(); btnSelectClear.Draw(); }

	    if(le.MouseCursor(btnSelectGround))
	    {
		selectTerrainEnable = true;
		selectObjectEnable = false;

		selectTerrain = 0;

		btnSizeSmall.SetDisable(false);
		btnSizeMedium.SetDisable(false);
		btnSizeLarge.SetDisable(false);
		btnSizeManual.SetDisable(false);

		btnSelectGround.Press();
		btnSelectGround.Draw();

		display.Blit(spritePanelGround, dstPanel);
		Error::Verbose("Game::Editor::StartGame: select Terrain Mode");
		selectTerrainCursor.Move(rectTerrainWater.x - 1, rectTerrainWater.y - 1);
	    }
	    else
	    if(le.MouseCursor(btnSelectObject))
	    {
		selectTerrainEnable = false;
		selectObjectEnable = true;

		selectObject = 0;

		btnSizeSmall.SetDisable(true);
		btnSizeMedium.SetDisable(true);
		btnSizeLarge.SetDisable(true);
		btnSizeManual.SetDisable(true);

		btnSelectObject.Press();
		btnSelectObject.Draw();

		display.Blit(spritePanelObject, dstPanel);
		Error::Verbose("Game::Editor::StartGame: select Object Mode");
		selectObjectCursor.Move(rectObjectWater.x - 1, rectObjectWater.y - 1);
	    }
	    else
	    if(le.MouseCursor(btnSelectInfo))
	    {
		selectTerrainEnable = false;
		selectObjectEnable = false;

		btnSizeSmall.SetDisable(true);
		btnSizeMedium.SetDisable(true);
		btnSizeLarge.SetDisable(true);
		btnSizeManual.SetDisable(true);

		btnSelectInfo.Press();
		btnSelectInfo.Draw();

		display.Blit(spritePanelInfo, dstPanel);
		Error::Verbose("Game::Editor::StartGame: select Detail Mode");
	    }
	    else
	    if(le.MouseCursor(btnSelectRiver))
	    {
		selectTerrainEnable = false;
		selectObjectEnable = false;

		btnSizeSmall.SetDisable(true);
		btnSizeMedium.SetDisable(true);
		btnSizeLarge.SetDisable(true);
		btnSizeManual.SetDisable(true);

		btnSelectRiver.Press();
		btnSelectRiver.Draw();

		display.Blit(spritePanelRiver, dstPanel);
		Error::Verbose("Game::Editor::StartGame: select Stream Mode");
	    }
	    else
	    if(le.MouseCursor(btnSelectRoad))
	    {
		selectTerrainEnable = false;
		selectObjectEnable = false;

		btnSizeSmall.SetDisable(true);
		btnSizeMedium.SetDisable(true);
		btnSizeLarge.SetDisable(true);
		btnSizeManual.SetDisable(true);

		btnSelectRoad.Press();
		btnSelectRoad.Draw();

		display.Blit(spritePanelRoad, dstPanel);		
		Error::Verbose("Game::Editor::StartGame: select Road Mode");
	    }
	    else
	    if(le.MouseCursor(btnSelectClear))
	    {
		selectTerrainEnable = false;
		selectObjectEnable = false;

		btnSizeSmall.SetDisable(false);
		btnSizeMedium.SetDisable(false);
		btnSizeLarge.SetDisable(false);
		btnSizeManual.SetDisable(false);

		btnSelectClear.Press();
		btnSelectClear.Draw();

		display.Blit(spritePanelClear, dstPanel);
		Error::Verbose("Game::Editor::StartGame: select Erase Mode");
	    }
		

	    if(btnSizeSmall.isEnable()) btnSizeSmall.isPressed() ? btnSizeSmall.Press() : btnSizeSmall.Release();
	    if(btnSizeMedium.isEnable()) btnSizeMedium.isPressed() ? btnSizeMedium.Press() : btnSizeMedium.Release();
	    if(btnSizeLarge.isEnable()) btnSizeLarge.isPressed() ? btnSizeLarge.Press() : btnSizeLarge.Release();
	    if(btnSizeManual.isEnable()) btnSizeManual.isPressed() ? btnSizeManual.Press() : btnSizeManual.Release();

	    if(btnSizeSmall.isEnable()) btnSizeSmall.Draw();
	    if(btnSizeMedium.isEnable()) btnSizeMedium.Draw();
	    if(btnSizeLarge.isEnable()) btnSizeLarge.Draw();
	    if(btnSizeManual.isEnable()) btnSizeManual.Draw();

	    cursor.Show();
	    display.Flip();
	}

	// click select size button
	if((btnSizeSmall.isEnable() && le.MouseClickLeft(btnSizeSmall)) ||
	   (btnSizeMedium.isEnable() && le.MouseClickLeft(btnSizeMedium)) ||
	   (btnSizeLarge.isEnable() && le.MouseClickLeft(btnSizeLarge)) ||
	   (btnSizeManual.isEnable() && le.MouseClickLeft(btnSizeManual)))
	{
	    cursor.Hide();

	    btnSizeSmall.Release();
	    btnSizeMedium.Release();
	    btnSizeLarge.Release();
	    btnSizeManual.Release();

	    if(le.MouseCursor(btnSizeSmall)){ btnSizeSmall.Press(); sizeCursor.ModifySize(1, 1); }
	    else
	    if(le.MouseCursor(btnSizeMedium)){ btnSizeMedium.Press(); sizeCursor.ModifySize(2, 2); }
	    else
	    if(le.MouseCursor(btnSizeLarge)){ btnSizeLarge.Press(); sizeCursor.ModifySize(4, 4); }
	    else
	    if(le.MouseCursor(btnSizeManual)){ btnSizeManual.Press(); sizeCursor.ModifySize(2, 2); }

	    btnSizeSmall.Draw();
	    btnSizeMedium.Draw();
	    btnSizeLarge.Draw();
	    btnSizeManual.Draw();
		
	    cursor.Show();
	    display.Flip();
	}

	// click select terrain
	if(selectTerrainEnable && le.MouseClickLeft(rectTerrainWater))
	{
	    selectTerrain = 0;
	    cursor.Hide();
	    selectTerrainCursor.Move(rectTerrainWater.x - 1, rectTerrainWater.y - 1);
	    cursor.Show();
	    display.Flip();
	    Error::Verbose("Game::Editor::StartGame: select terrain: water");
	}
	else
	if(selectTerrainEnable && le.MouseClickLeft(rectTerrainGrass))
	{
	    selectTerrain = 1;
	    cursor.Hide();
	    selectTerrainCursor.Move(rectTerrainGrass.x - 1, rectTerrainGrass.y - 1);
	    cursor.Show();
	    display.Flip();
	    Error::Verbose("Game::Editor::StartGame: select terrain: grass");
	}
	else
	if(selectTerrainEnable && le.MouseClickLeft(rectTerrainSnow))
	{
	    selectTerrain = 2;
	    cursor.Hide();
	    selectTerrainCursor.Move(rectTerrainSnow.x - 1, rectTerrainSnow.y - 1);
	    cursor.Show();
	    display.Flip();
	    Error::Verbose("Game::Editor::StartGame: select terrain: snow");
	}
	else
	if(selectTerrainEnable && le.MouseClickLeft(rectTerrainSwamp))
	{
	    selectTerrain = 3;
	    cursor.Hide();
	    selectTerrainCursor.Move(rectTerrainSwamp.x - 1, rectTerrainSwamp.y - 1);
	    cursor.Show();
	    display.Flip();
	    Error::Verbose("Game::Editor::StartGame: select terrain: swamp");
	}
	else
	if(selectTerrainEnable && le.MouseClickLeft(rectTerrainLava))
	{
	    selectTerrain = 4;
	    cursor.Hide();
	    selectTerrainCursor.Move(rectTerrainLava.x - 1, rectTerrainLava.y - 1);
	    cursor.Show();
	    display.Flip();
	    Error::Verbose("Game::Editor::StartGame: select terrain: lava");
	}
	else
	if(selectTerrainEnable && le.MouseClickLeft(rectTerrainBeach))
	{
	    selectTerrain = 5;
	    cursor.Hide();
	    selectTerrainCursor.Move(rectTerrainBeach.x - 1, rectTerrainBeach.y - 1);
	    cursor.Show();
	    display.Flip();
	    Error::Verbose("Game::Editor::StartGame: select terrain: beach");
	}
	else
	if(selectTerrainEnable && le.MouseClickLeft(rectTerrainDirt))
	{
	    selectTerrain = 6;
	    cursor.Hide();
	    selectTerrainCursor.Move(rectTerrainDirt.x - 1, rectTerrainDirt.y - 1);
	    cursor.Show();
	    display.Flip();
	    Error::Verbose("Game::Editor::StartGame: select terrain: dirt");
	}
	else
	if(selectTerrainEnable && le.MouseClickLeft(rectTerrainWasteland))
	{
	    selectTerrain = 7;
	    cursor.Hide();
	    selectTerrainCursor.Move(rectTerrainWasteland.x - 1, rectTerrainWasteland.y - 1);
	    cursor.Show();
	    display.Flip(); 
	    Error::Verbose("Game::Editor::StartGame: select terrain: wasteland");
	}
	else
	if(selectTerrainEnable && le.MouseClickLeft(rectTerrainDesert))
	{
	    selectTerrain = 8;
	    cursor.Hide();
	    selectTerrainCursor.Move(rectTerrainDesert.x - 1, rectTerrainDesert.y - 1);
	    cursor.Show();
	    display.Flip();
	    Error::Verbose("Game::Editor::StartGame: select terrain: desert");
	}

	// click select object
	if(selectObjectEnable && le.MouseClickLeft(rectObjectWater))
	{
	    selectObject = 0;
	    cursor.Hide();
	    selectObjectCursor.Move(rectObjectWater.x - 1, rectObjectWater.y - 1);
	    cursor.Show();
	    display.Flip();
	    Error::Verbose("Game::Editor::StartGame: select object: water");
	}
	else
	if(selectObjectEnable && le.MouseClickLeft(rectObjectGrass))
	{
	    selectObject = 1;
	    cursor.Hide();
	    selectObjectCursor.Move(rectObjectGrass.x - 1, rectObjectGrass.y - 1);
	    cursor.Show();
	    display.Flip();
	    Error::Verbose("Game::Editor::StartGame: select object: grass");
	}
	else
	if(selectObjectEnable && le.MouseClickLeft(rectObjectSnow))
	{
	    selectObject = 2;
	    cursor.Hide();
	    selectObjectCursor.Move(rectObjectSnow.x - 1, rectObjectSnow.y - 1);
	    cursor.Show();
	    display.Flip();
	    Error::Verbose("Game::Editor::StartGame: select object: snow");
	}
	else
	if(selectObjectEnable && le.MouseClickLeft(rectObjectSwamp))
	{
	    selectObject = 3;
	    cursor.Hide();
	    selectObjectCursor.Move(rectObjectSwamp.x - 1, rectObjectSwamp.y - 1);
	    cursor.Show();
	    display.Flip();
	    Error::Verbose("Game::Editor::StartGame: select object: swamp");
	}
	else
	if(selectObjectEnable && le.MouseClickLeft(rectObjectLava))
	{
	    selectObject = 4;
	    cursor.Hide();
	    selectObjectCursor.Move(rectObjectLava.x - 1, rectObjectLava.y - 1);
	    cursor.Show();
	    display.Flip();
	    Error::Verbose("Game::Editor::StartGame: select object: lava");
	}
	else
	if(selectObjectEnable && le.MouseClickLeft(rectObjectDesert))
	{
	    selectObject = 5;
	    cursor.Hide();
	    selectObjectCursor.Move(rectObjectDesert.x - 1, rectObjectDesert.y - 1);
	    cursor.Show();
	    display.Flip();
	    Error::Verbose("Game::Editor::StartGame: select object: desert");
	}
	else
	if(selectObjectEnable && le.MouseClickLeft(rectObjectDirt))
	{
	    selectObject = 6;
	    cursor.Hide();
	    selectObjectCursor.Move(rectObjectDirt.x - 1, rectObjectDirt.y - 1);
	    cursor.Show();
	    display.Flip();
	    Error::Verbose("Game::Editor::StartGame: select object: dirt");
	}
	else
	if(selectObjectEnable && le.MouseClickLeft(rectObjectWasteland))
	{
	    selectObject = 7;
	    cursor.Hide();
	    selectObjectCursor.Move(rectObjectWasteland.x - 1, rectObjectWasteland.y - 1);
	    cursor.Show();
	    display.Flip();
	    Error::Verbose("Game::Editor::StartGame: select object: wasteland");
	}
	else
	if(selectObjectEnable && le.MouseClickLeft(rectObjectBeach))
	{
	    selectObject = 8;
	    cursor.Hide();
	    selectObjectCursor.Move(rectObjectBeach.x - 1, rectObjectBeach.y - 1);
	    cursor.Show();
	    display.Flip();
	    Error::Verbose("Game::Editor::StartGame: select object: beach");
	}
	else
	if(selectObjectEnable && le.MouseClickLeft(rectObjectTown))
	{
	    selectObject = 9;
	    cursor.Hide();
	    selectObjectCursor.Move(rectObjectTown.x - 1, rectObjectTown.y - 1);
	    cursor.Show();
	    display.Flip();
	    Error::Verbose("Game::Editor::StartGame: select object: town");
	}
	else
	if(selectObjectEnable && le.MouseClickLeft(rectObjectMonster))
	{
	    selectObject = 10;
	    cursor.Hide();
	    selectObjectCursor.Move(rectObjectMonster.x - 1, rectObjectMonster.y - 1);
	    cursor.Show();
	    display.Flip();
	    Error::Verbose("Game::Editor::StartGame: select object: monster");
	}
	else
	if(selectObjectEnable && le.MouseClickLeft(rectObjectHero))
	{
	    selectObject = 11;
	    cursor.Hide();
	    selectObjectCursor.Move(rectObjectHero.x - 1, rectObjectHero.y - 1);
	    cursor.Show();
	    display.Flip();
	    Error::Verbose("Game::Editor::StartGame: select object: hero");
	}
	else
	if(selectObjectEnable && le.MouseClickLeft(rectObjectArtifact))
	{
	    selectObject = 12;
	    cursor.Hide();
	    selectObjectCursor.Move(rectObjectArtifact.x - 1, rectObjectArtifact.y - 1);
	    cursor.Show();
	    display.Flip();
	    Error::Verbose("Game::Editor::StartGame: select object: artifact");
	}
	else
	if(selectObjectEnable && le.MouseClickLeft(rectObjectResource))
	{
	    selectObject = 13;
	    cursor.Hide();
	    selectObjectCursor.Move(rectObjectResource.x - 1, rectObjectResource.y - 1);
	    cursor.Show();
	    display.Flip();
	    Error::Verbose("Game::Editor::StartGame: select object: resource");
	}

	// button click
	if(le.MouseClickLeft(btnZoom))
	{
	    Error::Verbose("Game::Editor::StartGame: FIXME: click button Zoom");
	}
	if(le.MouseClickLeft(btnUndo))
	{
	    Error::Verbose("Game::Editor::StartGame: FIXME: click button Undo");
	}
	if(le.MouseClickLeft(btnNew))
	{
	    return EDITNEWMAP;
	}
	if(le.MouseClickLeft(btnSpec))
	{
	    Error::Verbose("Game::Editor::StartGame: FIXME: click button Spec");
	}
	if(le.MouseClickLeft(btnFile))
	{
	    switch(Dialog::FileOptions())
	    {
		case Game::NEWGAME:	return EDITNEWMAP;
		case Game::LOADGAME:	return EDITLOADMAP;
		case Game::SAVEGAME:	return EDITSAVEMAP;
		case Game::QUITGAME:	return QUITGAME;
		
		default: break;
	    }
	}
	if(le.MouseClickLeft(btnSystem))
	{
	    Error::Verbose("Game::Editor::StartGame: FIXME: click button Options");
	}

	// press right info
	if(le.MousePressRight(btnZoom))
	    Dialog::Message("Magnify", "Change between zoom and normal view.", Font::BIG);
	else
	if(le.MousePressRight(btnUndo))
	    Dialog::Message("Undo", "Undo your last action. Press againt to redo the action.", Font::BIG);
	else
	if(le.MousePressRight(btnNew))
	    Dialog::Message("New", "Start a new map from scratch.", Font::BIG);
	else
	if(le.MousePressRight(btnSpec))
	    Dialog::Message("Specifications", "Edit maps title, description, and other general information.", Font::BIG);
	else
	if(le.MousePressRight(btnFile))
	    Dialog::Message("File Options", "Open the file options menu, where you can save or load maps, or quit out of the editor.", Font::BIG);
	else
	if(le.MousePressRight(btnSystem))
	    Dialog::Message("System Options", "View the editor system options, which let you customize the editor.", Font::BIG);
	else
	if(le.MousePressRight(btnSelectGround))
	    Dialog::Message("Terrain Mode", "Used to draw the underlying grass, dirt, water, etc. on the map.", Font::BIG);
	else
	if(le.MousePressRight(btnSelectObject))
	    Dialog::Message("Object Mode", "Used to place objects (mountains, trees, treasure, etc.) on the map.", Font::BIG);
	else
	if(le.MousePressRight(btnSelectInfo))
	    Dialog::Message("Detail Mode", "Used for special editing of monsters, heroes and towns.", Font::BIG);
	else
	if(le.MousePressRight(btnSelectRiver))
	    Dialog::Message("Stream Mode", "Allows you to draw streams by clicking and dragging.", Font::BIG);
	else
	if(le.MousePressRight(btnSelectRoad))
	    Dialog::Message("Road Mode", "Allows you to draw roads by clicking and dragging.", Font::BIG);
	else
	if(le.MousePressRight(btnSelectClear))
	    Dialog::Message("Erase Mode", "Used to erase objects of the map.", Font::BIG);
	else
	if(selectTerrainEnable && le.MousePressRight(rectTerrainWater))
	    Dialog::Message("Water", "Traversable only by boat.", Font::BIG);
	else
	if(selectTerrainEnable && le.MousePressRight(rectTerrainGrass))
	    Dialog::Message("Grass", "No special modifiers.", Font::BIG);
	else
	if(selectTerrainEnable && le.MousePressRight(rectTerrainSnow))
	    Dialog::Message("Snow", "Cost 1.5 times normal movement for all heroes. (Pathfinding reduces or eliminates the penalty.)", Font::BIG);
	else
	if(selectTerrainEnable && le.MousePressRight(rectTerrainSwamp))
	    Dialog::Message("Swamp", "Cost 1.75 times normal movement for all heroes. (Pathfinding reduces or eliminates the penalty.)", Font::BIG);
	else
	if(selectTerrainEnable && le.MousePressRight(rectTerrainLava))
	    Dialog::Message("Lava", "No special modifiers.", Font::BIG);
	else
	if(selectTerrainEnable && le.MousePressRight(rectTerrainBeach))
	    Dialog::Message("Beach", "Cost 1.25 times normal movement for all heroes. (Pathfinding reduces or eliminates the penalty.)", Font::BIG);
	else
	if(selectTerrainEnable && le.MousePressRight(rectTerrainDirt))
	    Dialog::Message("Dirt", "No special modifiers.", Font::BIG);
	else
	if(selectTerrainEnable && le.MousePressRight(rectTerrainWasteland))
	    Dialog::Message("Wasteland", "Cost 1.25 times normal movement for all heroes. (Pathfinding reduces or eliminates the penalty.)", Font::BIG);
	else
	if(selectTerrainEnable && le.MousePressRight(rectTerrainDesert))
	    Dialog::Message("Desert", "Cost 2 times normal movement for all heroes. (Pathfinding reduces or eliminates the penalty.)", Font::BIG);
	else
	if(selectObjectEnable && le.MousePressRight(rectObjectWater))
	    Dialog::Message("Water Objects", "Used to select objects most appropriate for use on water.", Font::BIG);
	else
	if(selectObjectEnable && le.MousePressRight(rectObjectGrass))
	    Dialog::Message("Grass Objects", "Used to select objects most appropriate for use on grass.", Font::BIG);
	else
	if(selectObjectEnable && le.MousePressRight(rectObjectSnow))
	    Dialog::Message("Snow Objects", "Used to select objects most appropriate for use on snow.", Font::BIG);
	else
	if(selectObjectEnable && le.MousePressRight(rectObjectSwamp))
	    Dialog::Message("Swamp Objects", "Used to select objects most appropriate for use on swamp.", Font::BIG);
	else
	if(selectObjectEnable && le.MousePressRight(rectObjectLava))
	    Dialog::Message("Lava Objects", "Used to select objects most appropriate for use on lava.", Font::BIG);
	else
	if(selectObjectEnable && le.MousePressRight(rectObjectDesert))
	    Dialog::Message("Desert Objects", "Used to select objects most appropriate for use on desert.", Font::BIG);
	else
	if(selectObjectEnable && le.MousePressRight(rectObjectDirt))
	    Dialog::Message("Dirt Objects", "Used to select objects most appropriate for use on dirt.", Font::BIG);
	else
	if(selectObjectEnable && le.MousePressRight(rectObjectWasteland))
	    Dialog::Message("Wasteland Objects", "Used to select objects most appropriate for use on wasteland.", Font::BIG);
	else
	if(selectObjectEnable && le.MousePressRight(rectObjectBeach))
	    Dialog::Message("Beach Objects", "Used to select objects most appropriate for use on beach.", Font::BIG);
	else
	if(selectObjectEnable && le.MousePressRight(rectObjectTown))
	    Dialog::Message("Towns", "Used to place a town or castle.", Font::BIG);
	else
	if(selectObjectEnable && le.MousePressRight(rectObjectMonster))
	    Dialog::Message("Monsters", "Used to place a monster group.", Font::BIG);
	else
	if(selectObjectEnable && le.MousePressRight(rectObjectHero))
	    Dialog::Message("Heroes", "Used to place a hero.", Font::BIG);
	else
	if(selectObjectEnable && le.MousePressRight(rectObjectArtifact))
	    Dialog::Message("Artifact", "Used to place an artifact.", Font::BIG);
	else
	if(selectObjectEnable && le.MousePressRight(rectObjectResource))
	    Dialog::Message("Treasures", "Used to place a resource or treasure.", Font::BIG);

    }

    return QUITGAME;
}

void Game::Editor::RedrawTopNumberCell(const Rect & area)
{
    Point dst_pt;

    // top number cell
    for(u16 ii = 0; ii < area.w; ++ii)
    {
	dst_pt.x = BORDERWIDTH + ii * TILEWIDTH;
	dst_pt.y = 0;

	Display::Get().Blit(AGG::GetICN(ICN::EDITBTNS, 34), dst_pt);

	std::string number;
	String::AddInt(number, area.x + ii);

	dst_pt.x = 2 * BORDERWIDTH + ii * TILEWIDTH - Text::width(number, Font::SMALL) / 2;
	dst_pt.y = 2;
	Text(number, Font::SMALL, dst_pt);

    }
}

void Game::Editor::RedrawLeftNumberCell(const Rect & area)
{
    Point dst_pt;

    // left number cell
    for(u16 ii = 0; ii < area.h; ++ii)
    {
	dst_pt.x = 0;
	dst_pt.y = BORDERWIDTH + ii * TILEWIDTH;

	Display::Get().Blit(AGG::GetICN(ICN::EDITBTNS, 33), dst_pt);

	std::string number;
	String::AddInt(number, area.y + ii);

 	dst_pt.x = BORDERWIDTH / 2 - Text::width(number, Font::SMALL) / 2 - 1;
	dst_pt.y = BORDERWIDTH + ii * TILEWIDTH + BORDERWIDTH - 5;
	Text(number, Font::SMALL, dst_pt);
    }
}

/* modify all tiles maps for correct abroad direction */
void Game::Editor::ModifyAllTilesAbroad(void)
{
    std::vector<Direction::vector_t> directs_frnt(4);
    std::vector<Direction::vector_t> directs_diag(4);

    std::vector<Direction::vector_t>::const_iterator it1;
    std::vector<Direction::vector_t>::const_iterator it2;
    
    directs_frnt[0] = Direction::TOP;
    directs_frnt[1] = Direction::RIGHT;
    directs_frnt[2] = Direction::BOTTOM;
    directs_frnt[3] = Direction::LEFT;

    directs_diag[0] = Direction::TOP_LEFT;
    directs_diag[1] = Direction::TOP_RIGHT;
    directs_diag[2] = Direction::BOTTOM_RIGHT;
    directs_diag[3] = Direction::BOTTOM_LEFT;

    for(u16 ii = 0; ii < world.w() * world.h(); ++ii)
    {
	Maps::Tiles & center = world.GetTiles(ii);

	u16 around_frnt = 0;
	u16 around_diag = 0;

	//u8 around_ground = 0;

	//u8 count_desert = 0;
	//u8 count_snow = 0;
	//u8 count_swamp = 0;
	//u8 count_wasteland = 0;
	//u8 count_beach = 0;
	//u8 count_lava = 0;
	//u8 count_dirt = 0;
	//u8 count_grass = 0;
	//u8 count_water = 0;

	//Maps::Ground::ground_t max = Maps::Ground::UNKNOWN;

	bool skip = true;

	// frontal
	it1 = directs_frnt.begin();
	it2 = directs_frnt.end();

	for(; it1 != it2; ++it1)
	{
	    const Direction::vector_t & direct = *it1;

	    if(Maps::isValidDirection(ii, direct))
	    {
		const Maps::Tiles & opposition = world.GetTiles(Maps::GetDirectionIndex(ii, direct));

		if(center.GetGround() != opposition.GetGround())
		{
		    skip = false;

		    around_frnt |= direct;
		}
	    }
	}

	// diagonal
	it1 = directs_diag.begin();
	it2 = directs_diag.end();

	for(; it1 != it2; ++it1)
	{
	    const Direction::vector_t & direct = *it1;

	    if(Maps::isValidDirection(ii, direct))
	    {
		const Maps::Tiles & opposition = world.GetTiles(Maps::GetDirectionIndex(ii, direct));

		if(center.GetGround() != opposition.GetGround())
		{
		    skip = false;

		    around_diag |= direct;
		}
	    }
	}

	// skip if all around ground equalent
	if(skip) continue;
	
	// if
	std::bitset<16> around_bits(around_frnt);

	if(2 > around_bits.count())
	{
	}

	// modify opposite tile
	// center.ModifyTileSprite(around, );
    }

/*
Maps::Ground::WATER:
Maps::Ground::GRASS:
Maps::Ground::SNOW:
Maps::Ground::SWAMP:
Maps::Ground::LAVA:
Maps::Ground::DESERT:
Maps::Ground::DIRT:
Maps::Ground::WASTELAND:
Maps::Ground::BEACH:

Direction::TOP
Direction::TOP_RIGHT
Direction::RIGHT
Direction::BOTTOM_RIGHT
Direction::BOTTOM
Direction::BOTTOM_LEFT
Direction::LEFT
Direction::TOP_LEFT
*/
}

/* set ground to tile */
void Game::Editor::SetGroundToTile(Maps::Tiles & tile, const Maps::Ground::ground_t ground)
{
    u16 index_ground = 0;

    switch(ground)
    {
	case Maps::Ground::WATER:	return; //index_ground =  16; break;
	case Maps::Ground::GRASS:	index_ground =  68; break;
	case Maps::Ground::SNOW:	index_ground = 130; break;
	case Maps::Ground::SWAMP:	index_ground = 184; break;
	case Maps::Ground::LAVA:	index_ground = 246; break;
	case Maps::Ground::DESERT:	index_ground = 300; break;
	case Maps::Ground::DIRT:	index_ground = 337; break;
	case Maps::Ground::WASTELAND:	index_ground = 399; break;
	case Maps::Ground::BEACH:	index_ground = 415; break;
	
	default: break;
    }

    switch(Rand::Get(1, 7))
    {
	// 85% simple ground
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
	    tile.SetTile(Rand::Get(index_ground, index_ground + 7), 0);
	    break;

        // 15% extended ground
        default:
	    tile.SetTile(Rand::Get(index_ground + 8, index_ground + 15), 0);
	    break;
    }
}
