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
#include <map>
#include <vector>
#include <string>
#include "background.h"
#include "rand.h"
#include "dir.h"
#include "agg.h"
#include "sprite.h"
#include "spritecursor.h"
#include "error.h"
#include "localevent.h"
#include "button.h"
#include "cursor.h"
#include "config.h"
#include "maps_fileinfo.h"
#include "difficulty.h"
#include "dialog.h"
#include "text.h"
#include "tools.h"
#include "splitter.h"
#include "game.h"

#define LISTMAXITEM	9
#define LISTHEIGHTROW	19

namespace EditScenario
{
    void DrawList(std::vector<Maps::FileInfo>::const_iterator &it_top, u8 count = LISTMAXITEM);
    void DrawSelectInfo(std::vector<Maps::FileInfo>::const_iterator &it_current);
}

Game::menu_t Game::Editor::LoadMaps(void)
{
    Display & display = Display::Get();

    std::vector<Maps::FileInfo> info_maps;

    // read maps directory
    Dir dir;
    Settings & conf = Settings::Get();

    dir.Read(conf.MapsDirectory(), ".mp2");
    dir.Read(conf.MapsDirectory(), ".Mp2");
    dir.Read(conf.MapsDirectory(), ".MP2");

    // loyality version
    if(conf.Modes(Settings::PRICELOYALTY))
    {
    	dir.Read(conf.MapsDirectory(), ".mx2");
	dir.Read(conf.MapsDirectory(), ".Mx2");
	dir.Read(conf.MapsDirectory(), ".MX2");
    }

    const u16 count_mp2 = dir.size();

    // empty maps dir
    if(0 == count_mp2)
    {
        Dialog::Message("Warning", "No maps available!", Font::BIG, Dialog::OK);
        return MAINMENU;
    }

    info_maps.resize(count_mp2);

    for(u16 ii = 0; ii < count_mp2; ++ii)
        info_maps[ii].Read(dir[ii]);

    // preload
    AGG::PreloadObject(ICN::REQSBKG);


    conf.LoadFileMaps(info_maps.front().FileMaps());

    LocalEvent & le = LocalEvent::GetLocalEvent();

    std::vector<Maps::FileInfo> smallmaps, mediummaps, largemaps, xlargemaps;
    
    std::vector<Maps::FileInfo> & allmaps = info_maps;

    std::vector<Maps::FileInfo>::const_iterator it = allmaps.begin();
    std::vector<Maps::FileInfo>::const_iterator it_end = allmaps.end();

    while(it != it_end)
    {
	switch((*it).SizeMaps())
	{
    	    case Maps::SMALL:
		smallmaps.push_back(*it);
		break;
    	    case Maps::MEDIUM:
		mediummaps.push_back(*it);
		break;
    	    case Maps::LARGE:
		largemaps.push_back(*it);
		break;
    	    case Maps::XLARGE:
		xlargemaps.push_back(*it);
		break;
	    default:
		break;
	}
	++it;
    }

    const std::vector<Maps::FileInfo> *curmaps = &allmaps;

    // cursor
    Cursor & cursor = Cursor::Get();
    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);

    Display::SetVideoMode(Display::SMALL);

    Background background(Rect(110, 0, 510, 480));
    background.Save();

    // image panel
    const Sprite &shadow = AGG::GetICN(ICN::REQSBKG, 1);
    display.Blit(shadow, 114, 21);
    const Sprite &panel = AGG::GetICN(ICN::REQSBKG, 0);
    display.Blit(panel, 130, 5);

    Background backgroundList(Rect(170, 60, 270, 175));
    backgroundList.Save();
    Background backgroundInfo(Rect(165, 265, 305, 145));
    backgroundInfo.Save();

    // area list
    Rect rectAreaList(Rect(170, 60, 270, 175));
    
    // Splitter
    Splitter split(AGG::GetICN(ICN::ESCROLL, 3), Rect(460, 78, 8, 141), Splitter::VERTICAL);
    split.SetRange(0, (LISTMAXITEM < curmaps->size() ? curmaps->size() - LISTMAXITEM : 0));

    // button
    Button buttonOk(270, 415, ICN::REQUESTS, 1, 2);
    Button buttonPgUp(457, 60, ICN::REQUESTS, 5, 6);
    Button buttonPgDn(457, 222, ICN::REQUESTS, 7, 8);

    Button buttonSelectSmall(167, 28, ICN::REQUESTS, 9, 10);
    Button buttonSelectMedium(229, 28, ICN::REQUESTS, 11, 12);
    Button buttonSelectLarge(291, 28, ICN::REQUESTS, 13, 14);
    Button buttonSelectXLarge(353, 28, ICN::REQUESTS, 15, 16);
    Button buttonSelectAll(415, 28, ICN::REQUESTS, 17, 18);

    std::vector<Maps::FileInfo>::const_iterator it_list_head = curmaps->begin();
    std::vector<Maps::FileInfo>::const_iterator it_current = it_list_head;

    EditScenario::DrawList(it_list_head, (LISTMAXITEM > curmaps->size() ? curmaps->size() : LISTMAXITEM));
    EditScenario::DrawSelectInfo(it_current);

    buttonOk.Draw();
    buttonPgUp.Draw();
    buttonPgDn.Draw();
    buttonSelectSmall.Draw();
    buttonSelectMedium.Draw();
    buttonSelectLarge.Draw();
    buttonSelectXLarge.Draw();
    buttonSelectAll.Draw();

    cursor.Show();
    display.Flip();

    u16 num_select = 0xFFFF;

    // newstandard loop
    while(le.HandleEvents())
    {
	// press button
	le.MousePressLeft(buttonOk) ? buttonOk.PressDraw() : buttonOk.ReleaseDraw();
	le.MousePressLeft(buttonPgUp) ? buttonPgUp.PressDraw() : buttonPgUp.ReleaseDraw();
	le.MousePressLeft(buttonPgDn) ? buttonPgDn.PressDraw() : buttonPgDn.ReleaseDraw();

	le.MousePressLeft(buttonSelectSmall) ? buttonSelectSmall.PressDraw() : buttonSelectSmall.ReleaseDraw();
	le.MousePressLeft(buttonSelectMedium) ? buttonSelectMedium.PressDraw() : buttonSelectMedium.ReleaseDraw();
	le.MousePressLeft(buttonSelectLarge) ? buttonSelectLarge.PressDraw() : buttonSelectLarge.ReleaseDraw();
	le.MousePressLeft(buttonSelectXLarge) ? buttonSelectXLarge.PressDraw() : buttonSelectXLarge.ReleaseDraw();
	le.MousePressLeft(buttonSelectAll) ? buttonSelectAll.PressDraw() : buttonSelectAll.ReleaseDraw();

	// click small
	if(le.MouseClickLeft(buttonSelectSmall) && smallmaps.size()){
	    curmaps = &smallmaps;
	    it_list_head = curmaps->begin();
	    it_current = it_list_head;
	    cursor.Hide();
	    backgroundList.Restore();
	    EditScenario::DrawList(it_list_head, (LISTMAXITEM > curmaps->size() ? curmaps->size() : LISTMAXITEM));
	    backgroundInfo.Restore();
	    EditScenario::DrawSelectInfo(it_current);
	    split.SetRange(0, (LISTMAXITEM < curmaps->size() ? curmaps->size() - LISTMAXITEM : 0));
	    cursor.Show();
	    display.Flip();
	}

	// click medium
	if(le.MouseClickLeft(buttonSelectMedium) && mediummaps.size()){
	    curmaps = &mediummaps;
	    it_list_head = curmaps->begin();
	    it_current = it_list_head;
	    cursor.Hide();
	    backgroundList.Restore();
	    EditScenario::DrawList(it_list_head, (LISTMAXITEM > curmaps->size() ? curmaps->size() : LISTMAXITEM));
	    backgroundInfo.Restore();
	    EditScenario::DrawSelectInfo(it_current);
	    split.SetRange(0, (LISTMAXITEM < curmaps->size() ? curmaps->size() - LISTMAXITEM : 0));
	    cursor.Show();
	    display.Flip();
	}

	// click large
	if(le.MouseClickLeft(buttonSelectLarge) && largemaps.size()){
	    curmaps = &largemaps;
	    it_list_head = curmaps->begin();
	    it_current = it_list_head;
	    cursor.Hide();
	    backgroundList.Restore();
	    EditScenario::DrawList(it_list_head, (LISTMAXITEM > curmaps->size() ? curmaps->size() : LISTMAXITEM));
	    backgroundInfo.Restore();
	    EditScenario::DrawSelectInfo(it_current);
	    split.SetRange(0, (LISTMAXITEM < curmaps->size() ? curmaps->size() - LISTMAXITEM : 0));
	    cursor.Show();
	    display.Flip();
	}

	// click xlarge
	if(le.MouseClickLeft(buttonSelectXLarge) && xlargemaps.size()){
	    curmaps = &xlargemaps;
	    it_list_head = curmaps->begin();
	    it_current = it_list_head;
	    cursor.Hide();
	    backgroundList.Restore();
	    EditScenario::DrawList(it_list_head, (LISTMAXITEM > curmaps->size() ? curmaps->size() : LISTMAXITEM));
	    backgroundInfo.Restore();
	    EditScenario::DrawSelectInfo(it_current);
	    split.SetRange(0, (LISTMAXITEM < curmaps->size() ? curmaps->size() - LISTMAXITEM : 0));
	    cursor.Show();
	    display.Flip();
	}

	// click all
	if(le.MouseClickLeft(buttonSelectAll)){
	    curmaps = &allmaps;
	    it_list_head = curmaps->begin();
	    it_current = it_list_head;
	    cursor.Hide();
	    backgroundList.Restore();
	    EditScenario::DrawList(it_list_head, (LISTMAXITEM > curmaps->size() ? curmaps->size() : LISTMAXITEM));
	    backgroundInfo.Restore();
	    EditScenario::DrawSelectInfo(it_current);
	    split.SetRange(0, (LISTMAXITEM < curmaps->size() ? curmaps->size() - LISTMAXITEM : 0));
	    cursor.Show();
	    display.Flip();
	}

	// click list
	if(le.MouseClickLeft(rectAreaList)){
	    u16 num = (le.MouseReleaseLeft().y - rectAreaList.y) / LISTHEIGHTROW;
	    if(num >= curmaps->size()) num = curmaps->size() - 1;
	    if(num_select == num) break;
	    num_select = num;
	    cursor.Hide();
	    backgroundInfo.Restore();
	    it_current = it_list_head + num;
	    EditScenario::DrawSelectInfo(it_current);
	    split.Move(it_list_head - curmaps->begin());
	    cursor.Show();
	    display.Flip();
	    num_select = num;
	}

	// click up
	if((le.MouseWheelUp(rectAreaList) || le.MouseWheelUp(split.GetRect()) || le.MouseClickLeft(buttonPgUp)) &&
	    curmaps->size() && it_list_head > curmaps->begin()){
	    cursor.Hide();
	    backgroundList.Restore();
	    EditScenario::DrawList(--it_list_head);
	    split.Backward();
	    cursor.Show();
	    display.Flip();
	}

	// click down
	if((le.MouseWheelDn(rectAreaList) || le.MouseWheelDn(split.GetRect()) || le.MouseClickLeft(buttonPgDn)) &&
	    LISTMAXITEM < curmaps->size() && it_list_head + LISTMAXITEM < curmaps->end()){
	    cursor.Hide();
	    backgroundList.Restore();
	    EditScenario::DrawList(++it_list_head);
	    split.Forward();
	    cursor.Show();
	    display.Flip();
	}

	// move cursor splitter
	if(le.MousePressLeft(split.GetRect()) && curmaps->size() > LISTMAXITEM && le.MouseCursor(split.GetRect()))
	{
	    u16 seek = (le.MouseCursor().y - split.GetRect().y) * 100 / split.GetStep();
	    if(seek > curmaps->size() - LISTMAXITEM) seek = curmaps->size() - LISTMAXITEM;

	    it_list_head = curmaps->begin() + seek;
	    cursor.Hide();
	    backgroundList.Restore();
	    EditScenario::DrawList(it_list_head, LISTMAXITEM);
	    backgroundInfo.Restore();
	    EditScenario::DrawSelectInfo(it_current);
	    split.Move(seek);
	    cursor.Show();
	    display.Flip();
 	}

	// right info
	if(le.MousePressRight(buttonSelectSmall)) Dialog::Message("Small Maps", "View only maps of size small (36x36).", Font::BIG);
	if(le.MousePressRight(buttonSelectMedium)) Dialog::Message("Medium Maps", "View only maps of size medium (72x72).", Font::BIG);
	if(le.MousePressRight(buttonSelectLarge)) Dialog::Message("Large Maps", "View only maps of size large (108x108).", Font::BIG);
	if(le.MousePressRight(buttonSelectXLarge)) Dialog::Message("Extra Large Maps", "View only maps of size extra large (144x144).", Font::BIG);
	if(le.MousePressRight(buttonSelectAll)) Dialog::Message("All Maps", "View all maps, regardless of size.", Font::BIG);
	//if(le.MousePressRight(?)) Dialog::Message("Players Icon", "Indicates how many players total are in the EditScenario. Any positions not occupied by humans will be occupied by computer players.", Font::BIG);
	//if(le.MousePressRight(?)) Dialog::Message("Size Icon", "Indicates whether the maps is small (36x36), medium (72x72), large (108x108), or extra large (144x144).", Font::BIG);
	//if(le.MousePressRight(?)) Dialog::Message("Selected Name", "The name of the currently selected map.", Font::BIG);
	//if(le.MousePressRight(?)) Dialog::Message("Victory Condition Icon", "There are 6 possiblities:.", Font::BIG);
	//if(le.MousePressRight(?)) Dialog::Message("Loss Condition Icon", "There are 4 possible loss conditions, as indicated by the following icons:.", Font::BIG);
	//if(le.MousePressRight(?)) Dialog::Message("Selected Map Difficulty", "The map difficulty of the currently selected map.  The map difficulty is determined by the EditScenario designer. More difficult maps might include more or stronger enemies, fewer resources, or other special conditions making things tougher for the human player.", Font::BIG);
	//if(le.MousePressRight(?)) Dialog::Message("Selected Description", "The description of the currently selected map.", Font::BIG);
	if(le.MousePressRight(buttonOk)) Dialog::Message("OK", "Accept the choice made.", Font::BIG);

	// click ok
	if(le.MouseClickLeft(buttonOk) || le.KeyPress(KEY_RETURN) || le.KeyPress(KEY_ESCAPE)) break;
    }

    // set current settings
    conf.LoadFileMaps((*it_current).FileMaps());

    cursor.Hide();
    background.Restore();
    
    return Game::MAINMENU;
}

void EditScenario::DrawList(std::vector<Maps::FileInfo>::const_iterator &it_top, u8 count)
{
    Display & display = Display::Get();

    u16 y = 64;
    u16 x = 176;
    u8 index = 0;
    std::vector<Maps::FileInfo>::const_iterator it_head = it_top;

    for(int ii = 0; ii < count; ++ii)
    {
	// sprite count
	const std::bitset<8> colors((*it_head).KingdomColors());
	index = 19 + colors.count();
	const Sprite &spriteCount = AGG::GetICN(ICN::REQUESTS, index);
	display.Blit(spriteCount, x, y);

        // sprite size
	switch((*it_head).SizeMaps())
	{
            case Maps::SMALL:
    		index = 26;
                break;
            case Maps::MEDIUM:
    		index = 27;
                break;
            case Maps::LARGE:
    		index = 28;
                break;
            case Maps::XLARGE:
    		index = 29;
                break;
            default:
    		index = 30;
                break;
        }
	const Sprite &spriteSize = AGG::GetICN(ICN::REQUESTS, index);
	display.Blit(spriteSize, x + spriteCount.w() + 2, y);

	// text longname
	Text((*it_head).Name(), Font::BIG, x + spriteCount.w() + spriteSize.w() + 18, y);

	// sprite wins
	index = 30 + (*it_head).ConditionsWins();
	const Sprite &spriteWins = AGG::GetICN(ICN::REQUESTS, index);
	display.Blit(spriteWins, x + 224, y);

	// sprite loss
	index = 36 + (*it_head).ConditionsLoss();
	const Sprite &spriteLoss = AGG::GetICN(ICN::REQUESTS, index);
	display.Blit(spriteLoss, x + 226 + spriteWins.w(), y);

	y += LISTHEIGHTROW;
	++it_head;
    }
}

void EditScenario::DrawSelectInfo(std::vector<Maps::FileInfo>::const_iterator &it_current)
{
    Display & display = Display::Get();

    u16 x = 196;
    u16 y = 269;
    u8 index = 0;

    // sprite count
    const std::bitset<8> colors((*it_current).KingdomColors());
    index = 19 + colors.count();
    const Sprite &spriteCount = AGG::GetICN(ICN::REQUESTS, index);
    display.Blit(spriteCount, x, y);

    // sprite size
    switch((*it_current).SizeMaps())
    {
        case Maps::SMALL:
    	    index = 26;
            break;
        case Maps::MEDIUM:
    	    index = 27;
            break;
        case Maps::LARGE:
    	    index = 28;
            break;
        case Maps::XLARGE:
    	    index = 29;
            break;
        default:
    	    index = 30;
            break;
    }
    const Sprite &spriteSize = AGG::GetICN(ICN::REQUESTS, index);
    display.Blit(spriteSize, x + spriteCount.w() + 2, y);

    // text longname
    Text((*it_current).Name(), Font::BIG, x + spriteCount.w() + spriteSize.w() + 22, y);

    // sprite wins
    index = 30 + (*it_current).ConditionsWins();
    const Sprite &spriteWins = AGG::GetICN(ICN::REQUESTS, index);
    display.Blit(spriteWins, x + 209, y);

    // sprite loss
    index = 36 + (*it_current).ConditionsLoss();
    const Sprite &spriteLoss = AGG::GetICN(ICN::REQUESTS, index);
    display.Blit(spriteLoss, x + 211 + spriteWins.w(), y);

    Text("Maps difficulty:", Font::BIG, 200, 295);
    Text(Difficulty::String((*it_current).Difficulty()), Font::BIG, 360, 295);
    
    TextBox((*it_current).Description(), Font::BIG, Rect(175, 322, 282, 90));
}
