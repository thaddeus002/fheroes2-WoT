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
#include "maps_tiles.h"
#include "ground.h"
#include "error.h"
#include "tools.h"
#include "game.h"

namespace Game
{
    namespace Editor
    {
	void RedrawTopNumberCell(const Rect & area);
	void RedrawLeftNumberCell(const Rect & area);
    };
};

Game::menu_t Game::Editor::StartGame(void)
{
    // cursor
    Cursor::Hide();
    Cursor::Set(Cursor::POINTER);

    // new maps
    const Maps::mapsize_t & sizemap = H2Config::GetSizeMaps();
    world.NewMaps(sizemap, sizemap);

    Display::SetVideoMode(H2Config::GetVideoMode());
    display.Fill(0x00, 0x00, 0x00);

    GameArea areaMaps;
    const Rect area_pos(BORDERWIDTH, BORDERWIDTH, GameArea::GetRect().w * TILEWIDTH, GameArea::GetRect().h * TILEWIDTH);
    
    const Rect areaScrollLeft(0, BORDERWIDTH, BORDERWIDTH / 2, display.h() - 2 * BORDERWIDTH);
    const Rect areaScrollRight(display.w() - BORDERWIDTH / 2, BORDERWIDTH, BORDERWIDTH / 2, display.h() - 2 * BORDERWIDTH);
    const Rect areaScrollTop(BORDERWIDTH, 0, areaMaps.GetRect().w * TILEWIDTH, BORDERWIDTH / 2);
    const Rect areaScrollBottom(2 * BORDERWIDTH, display.h() - BORDERWIDTH / 2, (areaMaps.GetRect().w - 1) * TILEWIDTH, BORDERWIDTH / 2);
    const Rect areaLeftPanel(display.w() - 2 * BORDERWIDTH - RADARWIDTH, 0, BORDERWIDTH + RADARWIDTH, display.h());

    const std::string icnscroll("ESCROLL.ICN");
    const std::string icnbtns("EDITBTNS.ICN");
    const std::string icnpanel("EDITPANL.ICN");
    Rect src_rt;
    Point dst_pt;

    // draw interface
    const Sprite & spriteAdv = AGG::GetICN("ADVBORD.ICN", 0);
    const Sprite & spriteBac = AGG::GetICN("STONBACK.ICN", 0);
    // bottom scroll bar indicator
    const Sprite & spriteBottomBar(AGG::GetICN(icnscroll, 0));
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
    const Sprite & spriteLeftBar(AGG::GetICN(icnscroll, 1));
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
    u8 var1 = 0;
    switch(H2Config::GetVideoMode())
    {
        default:
	    var1 = 4;
	    break;
	
	case Display::MEDIUM:
	    var1 = 7;
	    break;

	case Display::LARGE:
	    var1 = 13;
	    break;
		
	case Display::XLARGE:
	    var1 = 21;
	    break;
    }
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
    Button btnLeftTopScroll(dst_pt, icnscroll, 12, 13);

    dst_pt.x = BORDERWIDTH + areaMaps.GetRect().w * TILEWIDTH;
    Button btnRightTopScroll(dst_pt, icnscroll, 14, 15);
	
    dst_pt.y = BORDERWIDTH;
    Button btnTopScroll(dst_pt, icnscroll, 4, 5);

    dst_pt.x = 0;
    dst_pt.y = BORDERWIDTH + areaMaps.GetRect().h * TILEWIDTH;
    Button btnLeftBottomScroll(dst_pt, icnscroll, 18, 19);

    dst_pt.x = BORDERWIDTH;
    Button btnLeftScroll(dst_pt, icnscroll, 8, 9);

    dst_pt.x = 2 * BORDERWIDTH + (areaMaps.GetRect().w - 1) * TILEWIDTH;
    Button btnRightScroll(dst_pt, icnscroll, 10, 11);

    dst_pt.x = BORDERWIDTH + areaMaps.GetRect().w * TILEWIDTH;
    Button btnRightBottomScroll(dst_pt, icnscroll, 16, 17);

    dst_pt.y = areaMaps.GetRect().h * TILEWIDTH;
    Button btnBottomScroll(dst_pt, icnscroll, 6, 7);
    
    dst_pt.x = display.w() - BORDERWIDTH - RADARWIDTH;
    dst_pt.y = BORDERWIDTH + RADARWIDTH;
    Button btnSelectGround(dst_pt, icnbtns, 0, 1);

    dst_pt.x = btnSelectGround.x + btnSelectGround.w;
    dst_pt.y = btnSelectGround.y;
    Button btnSelectObject(dst_pt, icnbtns, 2, 3);

    dst_pt.x = btnSelectObject.x + btnSelectObject.w;
    dst_pt.y = btnSelectObject.y;
    Button btnSelectInfo(dst_pt, icnbtns, 4, 5);

    dst_pt.x = btnSelectGround.x;
    dst_pt.y = btnSelectGround.y + btnSelectGround.h;
    Button btnSelectRiver(dst_pt, icnbtns, 6, 7);

    dst_pt.x = btnSelectRiver.x + btnSelectRiver.w;
    dst_pt.y = btnSelectRiver.y;
    Button btnSelectRoad(dst_pt, icnbtns, 8, 9);

    dst_pt.x = btnSelectRoad.x + btnSelectRoad.w;
    dst_pt.y = btnSelectRoad.y;
    Button btnSelectClear(dst_pt, icnbtns, 10, 11);

    const Point dstPanel(btnSelectRiver.x, btnSelectRiver.y + btnSelectRiver.h);
    const Sprite & spritePanelGround = AGG::GetICN(icnpanel, 0);
    const Sprite & spritePanelObject = AGG::GetICN(icnpanel, 1);
    const Sprite & spritePanelInfo = AGG::GetICN(icnpanel, 2);
    const Sprite & spritePanelRiver = AGG::GetICN(icnpanel, 3);
    const Sprite & spritePanelRoad = AGG::GetICN(icnpanel, 4);
    const Sprite & spritePanelClear = AGG::GetICN(icnpanel, 5);

    dst_pt.x = dstPanel.x + 14;
    dst_pt.y = dstPanel.y + 127;
    Button btnSizeSmall(dst_pt, icnbtns, 24, 25);
    dst_pt.x = dstPanel.x + 44;
    Button btnSizeMedium(dst_pt, icnbtns, 26, 27);
    dst_pt.x = dstPanel.x + 74;
    Button btnSizeLarge(dst_pt, icnbtns, 28, 29);
    dst_pt.x = dstPanel.x + 104;
    Button btnSizeManual(dst_pt, icnbtns, 30, 31);

    dst_pt.x = dstPanel.x;
    dst_pt.y = dstPanel.y + spritePanelGround.h();
    Button btnZoom(dst_pt, icnbtns, 12, 13);

    dst_pt.x = btnZoom.x + btnZoom.w;
    dst_pt.y = btnZoom.y;
    Button btnUndo(dst_pt, icnbtns, 14, 15);

    dst_pt.x = btnUndo.x + btnUndo.w;
    dst_pt.y = btnUndo.y;
    Button btnNew(dst_pt, icnbtns, 16, 17);

    dst_pt.x = btnZoom.x;
    dst_pt.y = btnZoom.y + btnZoom.h;
    Button btnSpec(dst_pt, icnbtns, 18, 19);

    dst_pt.x = btnSpec.x + btnSpec.w;
    dst_pt.y = btnSpec.y;
    Button btnFile(dst_pt, icnbtns, 20, 21);

    dst_pt.x = btnFile.x + btnFile.w;
    dst_pt.y = btnFile.y;
    Button btnSystem(dst_pt, icnbtns, 22, 23);

    // bottom static
    switch(H2Config::GetVideoMode())
    {
        default:
	    var1 = 0;
	    break;
	
	case Display::MEDIUM:
	    var1 = 1;
	    break;

	case Display::LARGE:
	    var1 = 7;
	    break;
		
	case Display::XLARGE:
	    var1 = 15;
	    break;
    }
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
    Splitter split_h(AGG::GetICN(icnscroll, 2), Rect(2 * BORDERWIDTH + 3, display.h() - BORDERWIDTH + 4, (areaMaps.GetRect().w - 1) * TILEWIDTH - 6, BORDERWIDTH - 8), Splitter::HORIZONTAL);
    Splitter split_v(AGG::GetICN(icnscroll, 3), Rect(BORDERWIDTH + areaMaps.GetRect().w * TILEWIDTH + 4, 2 * BORDERWIDTH + 3, BORDERWIDTH - 8, (areaMaps.GetRect().h - 1) * TILEWIDTH - 6), Splitter::VERTICAL);

    split_h.SetRange(0, world.w() - areaMaps.GetRect().w);
    split_v.SetRange(0, world.h() - areaMaps.GetRect().h);

    split_h.Move(areaMaps.GetRect().x);
    split_v.Move(areaMaps.GetRect().y);

    // Create radar
    Radar radar;

    areaMaps.Redraw();
    radar.Redraw();

    // Create radar cursor
    radar.UpdatePosition();

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

    btnSizeMedium.Draw();
    btnSizeLarge.Draw();
    btnSizeManual.Draw();

    btnSizeSmall.Press();
    btnSizeSmall.Draw();
    
    u8 selectSize = 1;

    const Rect rectTerrainWater(dstPanel.x + 29, dstPanel.y + 10, 28, 28);
    const Rect rectTerrainGrass(dstPanel.x + 58, dstPanel.y + 10, 28, 28);
    const Rect rectTerrainSnow(dstPanel.x + 87, dstPanel.y + 10, 28, 28);
    const Rect rectTerrainSwamp(dstPanel.x + 29, dstPanel.y + 39, 28, 28);
    const Rect rectTerrainLava(dstPanel.x + 58, dstPanel.y + 39, 28, 28);
    const Rect rectTerrainBeach(dstPanel.x + 87, dstPanel.y + 39, 28, 28);
    const Rect rectTerrainDirt(dstPanel.x + 29, dstPanel.y + 68, 28, 28);
    const Rect rectTerrainWasteland(dstPanel.x + 58, dstPanel.y + 68, 28, 28);
    const Rect rectTerrainDesert(dstPanel.x + 87, dstPanel.y + 68, 28, 28);

    SpriteCursor selectCursor(AGG::GetICN("TERRAINS.ICN", 9), rectTerrainWater.x - 1, rectTerrainWater.y - 1);
    selectCursor.Show();
    Maps::Ground::ground_t selectTerrain = Maps::Ground::WATER;
    bool selectTerrainEnable = true;

    Cursor::Show();
    display.Flip();

    //u32 ticket = 0;

    // startgame loop
    while(le.HandleEvents())
    {
	// ESC
	if(le.KeyPress(SDLK_ESCAPE) && (Dialog::YES & Dialog::Message("", "Are you sure you want to quit?", Font::BIG, Dialog::YES|Dialog::NO))) return QUITGAME;

	// scroll area maps left
	if(le.MouseCursor(areaScrollLeft))
	{
	    Cursor::Hide();
	    Cursor::Set(Cursor::SCROLL_LEFT);
	    areaMaps.Scroll(GameArea::LEFT);
	    split_h.Backward();
	    radar.UpdatePosition();
	    RedrawTopNumberCell(areaMaps.GetRect());
	    Cursor::Show();
	    display.Flip();
	    continue;
	}
	// scroll area maps right
	if(le.MouseCursor(areaScrollRight))
	{
	    Cursor::Hide();
	    Cursor::Set(Cursor::SCROLL_RIGHT);
	    areaMaps.Scroll(GameArea::RIGHT);
	    split_h.Forward();
	    radar.UpdatePosition();
	    RedrawTopNumberCell(areaMaps.GetRect());
	    Cursor::Show();
	    display.Flip();
	    continue;
	}
	// scroll area maps top
	if(le.MouseCursor(areaScrollTop))
	{
	    Cursor::Hide();
	    Cursor::Set(Cursor::SCROLL_TOP);
	    areaMaps.Scroll(GameArea::TOP);
	    split_v.Backward();
	    radar.UpdatePosition();
	    RedrawLeftNumberCell(areaMaps.GetRect());
	    Cursor::Show();
	    display.Flip();
	    continue;
	}
	// scroll area maps bottom
	if(le.MouseCursor(areaScrollBottom)){
	    Cursor::Hide();
	    Cursor::Set(Cursor::SCROLL_BOTTOM);
	    areaMaps.Scroll(GameArea::BOTTOM);
	    split_v.Forward();
	    radar.UpdatePosition();
	    RedrawLeftNumberCell(areaMaps.GetRect());
	    Cursor::Show();
	    display.Flip();
	    continue;
	}

	// point radar
	if(le.MouseCursor(radar.GetRect()) &&
	    (le.MouseClickLeft(radar.GetRect()) ||
		le.MousePressLeft(radar.GetRect())))
	{
	    Rect prev(areaMaps.GetRect());
	    areaMaps.CenterFromRadar(le.MouseCursor());
	    if(prev != areaMaps.GetRect())
	    {
		Cursor::Hide();
		split_h.Move(areaMaps.GetRect().x);
		split_v.Move(areaMaps.GetRect().y);
		radar.UpdatePosition();
		RedrawTopNumberCell(areaMaps.GetRect());
		RedrawLeftNumberCell(areaMaps.GetRect());
		Cursor::Show();
		display.Flip();
	    }
	}

	// pointer cursor on left panel
	if(le.MouseCursor(areaLeftPanel)){ Cursor::Set(Cursor::POINTER); }
	else
	// cursor over game area
	if(le.MouseCursor(area_pos))
	{
            const Point & mouse_coord = le.MouseCursor();
            const u16 index_maps = Maps::GetIndexFromAreaPoint(mouse_coord);
            const Maps::Tiles & tile = world.GetTiles(index_maps);
            const Rect tile_pos(BORDERWIDTH + ((u16) (mouse_coord.x - BORDERWIDTH) / TILEWIDTH) * TILEWIDTH, BORDERWIDTH + ((u16) (mouse_coord.y - BORDERWIDTH) / TILEWIDTH) * TILEWIDTH, TILEWIDTH, TILEWIDTH);
            //u8 object = tile.GetObject();

    	    Cursor::Set(Cursor::POINTER);

	    if(le.MouseClickRight(tile_pos))
	    {
		if(H2Config::Debug()) tile.DebugInfo(index_maps);

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
	    Cursor::Hide();

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
		selectTerrain = Maps::Ground::WATER;

		btnSizeSmall.SetDisable(false);
		btnSizeMedium.SetDisable(false);
		btnSizeLarge.SetDisable(false);
		btnSizeManual.SetDisable(false);

		btnSelectGround.Press();
		btnSelectGround.Draw();

		display.Blit(spritePanelGround, dstPanel);
		Error::Verbose("Game::Editor::StartGame: select Terrain Mode");
		selectCursor.Move(rectTerrainWater.x - 1, rectTerrainWater.y - 1);
	    }
	    else
	    if(le.MouseCursor(btnSelectObject))
	    {
		selectTerrainEnable = false;

		btnSizeSmall.SetDisable(true);
		btnSizeMedium.SetDisable(true);
		btnSizeLarge.SetDisable(true);
		btnSizeManual.SetDisable(true);

		btnSelectObject.Press();
		btnSelectObject.Draw();

		display.Blit(spritePanelObject, dstPanel);
		Error::Verbose("Game::Editor::StartGame: select Object Mode");
	    }
	    else
	    if(le.MouseCursor(btnSelectInfo))
	    {
		selectTerrainEnable = false;

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

	    Cursor::Show();
	    display.Flip();
	}

	// click select size button
	if((btnSizeSmall.isEnable() && le.MouseClickLeft(btnSizeSmall)) ||
	   (btnSizeMedium.isEnable() && le.MouseClickLeft(btnSizeMedium)) ||
	   (btnSizeLarge.isEnable() && le.MouseClickLeft(btnSizeLarge)) ||
	   (btnSizeManual.isEnable() && le.MouseClickLeft(btnSizeManual)))
	{
	    Cursor::Hide();

	    btnSizeSmall.Release();
	    btnSizeMedium.Release();
	    btnSizeLarge.Release();
	    btnSizeManual.Release();

	    if(le.MouseCursor(btnSizeSmall)){ btnSizeSmall.Press(); selectSize = 1; }
	    else
	    if(le.MouseCursor(btnSizeMedium)){ btnSizeMedium.Press(); selectSize = 2; }
	    else
	    if(le.MouseCursor(btnSizeLarge)){ btnSizeLarge.Press(); selectSize = 3; }
	    else
	    if(le.MouseCursor(btnSizeManual)){ btnSizeManual.Press(); selectSize = 4; }

	    btnSizeSmall.Draw();
	    btnSizeMedium.Draw();
	    btnSizeLarge.Draw();
	    btnSizeManual.Draw();
		
	    Cursor::Show();
	    display.Flip();
	}

	// click select terrain
	if(selectTerrainEnable && le.MouseClickLeft(rectTerrainWater))
	{
	    selectTerrain = Maps::Ground::WATER;
	    Cursor::Hide();
	    selectCursor.Move(rectTerrainWater.x - 1, rectTerrainWater.y - 1);
	    Cursor::Show();
	    display.Flip();
	    Error::Verbose("Game::Editor::StartGame: select terrain: " + Maps::Ground::String(selectTerrain));
	}
	else
	if(selectTerrainEnable && le.MouseClickLeft(rectTerrainGrass))
	{
	    selectTerrain = Maps::Ground::GRASS;
	    Cursor::Hide();
	    selectCursor.Move(rectTerrainGrass.x - 1, rectTerrainGrass.y - 1);
	    Cursor::Show();
	    display.Flip();
	    Error::Verbose("Game::Editor::StartGame: select terrain: " + Maps::Ground::String(selectTerrain));
	}
	else
	if(selectTerrainEnable && le.MouseClickLeft(rectTerrainSnow))
	{
	    selectTerrain = Maps::Ground::SNOW;
	    Cursor::Hide();
	    selectCursor.Move(rectTerrainSnow.x - 1, rectTerrainSnow.y - 1);
	    Cursor::Show();
	    display.Flip();
	    Error::Verbose("Game::Editor::StartGame: select terrain: " + Maps::Ground::String(selectTerrain));
	}
	else
	if(selectTerrainEnable && le.MouseClickLeft(rectTerrainSwamp))
	{
	    selectTerrain = Maps::Ground::SWAMP;
	    Cursor::Hide();
	    selectCursor.Move(rectTerrainSwamp.x - 1, rectTerrainSwamp.y - 1);
	    Cursor::Show();
	    display.Flip();
	    Error::Verbose("Game::Editor::StartGame: select terrain: " + Maps::Ground::String(selectTerrain));
	}
	else
	if(selectTerrainEnable && le.MouseClickLeft(rectTerrainLava))
	{
	    selectTerrain = Maps::Ground::LAVA;
	    Cursor::Hide();
	    selectCursor.Move(rectTerrainLava.x - 1, rectTerrainLava.y - 1);
	    Cursor::Show();
	    display.Flip();
	    Error::Verbose("Game::Editor::StartGame: select terrain: " + Maps::Ground::String(selectTerrain));
	}
	else
	if(selectTerrainEnable && le.MouseClickLeft(rectTerrainBeach))
	{
	    selectTerrain = Maps::Ground::BEACH;
	    Cursor::Hide();
	    selectCursor.Move(rectTerrainBeach.x - 1, rectTerrainBeach.y - 1);
	    Cursor::Show();
	    display.Flip();
	    Error::Verbose("Game::Editor::StartGame: select terrain: " + Maps::Ground::String(selectTerrain));
	}
	else
	if(selectTerrainEnable && le.MouseClickLeft(rectTerrainDirt))
	{
	    selectTerrain = Maps::Ground::DIRT;
	    Cursor::Hide();
	    selectCursor.Move(rectTerrainDirt.x - 1, rectTerrainDirt.y - 1);
	    Cursor::Show();
	    display.Flip();
	    Error::Verbose("Game::Editor::StartGame: select terrain: " + Maps::Ground::String(selectTerrain));
	}
	else
	if(selectTerrainEnable && le.MouseClickLeft(rectTerrainWasteland))
	{
	    selectTerrain = Maps::Ground::WASTELAND;
	    Cursor::Hide();
	    selectCursor.Move(rectTerrainWasteland.x - 1, rectTerrainWasteland.y - 1);
	    Cursor::Show();
	    display.Flip(); 
	    Error::Verbose("Game::Editor::StartGame: select terrain: " + Maps::Ground::String(selectTerrain));
	}
	else
	if(selectTerrainEnable && le.MouseClickLeft(rectTerrainDesert))
	{
	    selectTerrain = Maps::Ground::DESERT;
	    Cursor::Hide();
	    selectCursor.Move(rectTerrainDesert.x - 1, rectTerrainDesert.y - 1);
	    Cursor::Show();
	    display.Flip();
	    Error::Verbose("Game::Editor::StartGame: select terrain: " + Maps::Ground::String(selectTerrain));
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
		case Game::NEWGAME:
		    return EDITNEWMAP;

		case Game::LOADGAME:
		    return EDITLOADMAP;

		case Game::SAVEGAME:
		    Error::Verbose("Game::Editor::StartGame: FIXME: save maps");
			break;

		case Game::QUITGAME:
		    return QUITGAME;

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

/*
        // animation
        if(!(++ticket % 50)) // FIXME: speed animation low
        {
            Cursor::Hide();
            areaMaps.RedrawAnimation();
            Cursor::Show();
            display.Flip();
        }
*/
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

	display.Blit(AGG::GetICN("EDITBTNS.ICN", 34), dst_pt);

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

	display.Blit(AGG::GetICN("EDITBTNS.ICN", 33), dst_pt);

	std::string number;
	String::AddInt(number, area.y + ii);

 	dst_pt.x = BORDERWIDTH / 2 - Text::width(number, Font::SMALL) / 2 - 1;
	dst_pt.y = BORDERWIDTH + ii * TILEWIDTH + BORDERWIDTH - 5;
	Text(number, Font::SMALL, dst_pt);
    }
}
