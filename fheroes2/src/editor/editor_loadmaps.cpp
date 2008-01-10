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
    void SetCurrentSettings(const Maps::FileInfo &maps);
    void DrawList(std::vector<Maps::FileInfo>::const_iterator &it_top, u8 count = LISTMAXITEM);
    void DrawSelectInfo(std::vector<Maps::FileInfo>::const_iterator &it_current);
}

Game::menu_t Game::Editor::LoadMaps(void)
{
    std::vector<Maps::FileInfo> info_maps;

    // read maps directory
    Dir dir1(H2Config::GetMapsDirectory(), "mp2");
    Dir dir2(H2Config::GetMapsDirectory(), "Mp2");
    Dir dir3(H2Config::GetMapsDirectory(), "MP2");

    if(dir1.size())
    {
	Dir::const_iterator itd1 = dir1.begin();
	while(itd1 != dir1.end()){ info_maps.push_back(Maps::FileInfo(*itd1)); ++itd1; }
    }

    if(dir2.size())
    {
	Dir::const_iterator itd2 = dir2.begin();
	while(itd2 != dir2.end()){ info_maps.push_back(Maps::FileInfo(*itd2)); ++itd2; }
    }

    if(dir3.size())
    {
	Dir::const_iterator itd3 = dir3.begin();
	while(itd3 != dir3.end()){ info_maps.push_back(Maps::FileInfo(*itd3)); ++itd3; }
    }

    // empty maps dir
    if(!info_maps.size())
    {
	H2Config::SetInterface(true);
	Dialog::Message("Warning", "None maps available!", Font::BIG, Dialog::OK);
	return MAINMENU;
    }

    // preload
    AGG::PreloadObject("REQSBKG.ICN");


    EditScenario::SetCurrentSettings(info_maps.front());

    LocalEvent & le = LocalEvent::GetLocalEvent();

    std::vector<Maps::FileInfo> smallmaps, mediummaps, largemaps, xlargemaps;
    
    std::vector<Maps::FileInfo> & allmaps = info_maps;

    std::vector<Maps::FileInfo>::const_iterator it = allmaps.begin();
    std::vector<Maps::FileInfo>::const_iterator it_end = allmaps.end();

    while(it != it_end)
    {
	switch((*it).GetSizeMaps())
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
    Cursor::Hide();
    Cursor::Set(Cursor::POINTER);

    Display::SetVideoMode(Display::SMALL);

    Background background(Rect(110, 0, 510, 480));
    background.Save();

    // image panel
    const Sprite &shadow = AGG::GetICN("REQSBKG.ICN", 1);
    display.Blit(shadow, 114, 21);
    const Sprite &panel = AGG::GetICN("REQSBKG.ICN", 0);
    display.Blit(panel, 130, 5);

    Background backgroundList(Rect(170, 60, 270, 175));
    backgroundList.Save();
    Background backgroundInfo(Rect(165, 265, 305, 145));
    backgroundInfo.Save();

    // area list
    Rect rectAreaList(Rect(170, 60, 270, 175));
    
    // Splitter
    Splitter split(AGG::GetICN("ESCROLL.ICN", 3), Rect(460, 78, 8, 141), Splitter::VERTICAL);
    split.SetRange(0, (LISTMAXITEM < curmaps->size() ? curmaps->size() - LISTMAXITEM : 0));

    // button
    Button buttonOk(270, 415, "REQUESTS.ICN", 1, 2);
    Button buttonPgUp(457, 60, "REQUESTS.ICN", 5, 6);
    Button buttonPgDn(457, 222, "REQUESTS.ICN", 7, 8);

    Button buttonSelectSmall(167, 28, "REQUESTS.ICN", 9, 10);
    Button buttonSelectMedium(229, 28, "REQUESTS.ICN", 11, 12);
    Button buttonSelectLarge(291, 28, "REQUESTS.ICN", 13, 14);
    Button buttonSelectXLarge(353, 28, "REQUESTS.ICN", 15, 16);
    Button buttonSelectAll(415, 28, "REQUESTS.ICN", 17, 18);

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

    Cursor::Show();
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
	    Cursor::Hide();
	    backgroundList.Restore();
	    EditScenario::DrawList(it_list_head, (LISTMAXITEM > curmaps->size() ? curmaps->size() : LISTMAXITEM));
	    backgroundInfo.Restore();
	    EditScenario::DrawSelectInfo(it_current);
	    split.SetRange(0, (LISTMAXITEM < curmaps->size() ? curmaps->size() - LISTMAXITEM : 0));
	    Cursor::Show();
	    display.Flip();
	}

	// click medium
	if(le.MouseClickLeft(buttonSelectMedium) && mediummaps.size()){
	    curmaps = &mediummaps;
	    it_list_head = curmaps->begin();
	    it_current = it_list_head;
	    Cursor::Hide();
	    backgroundList.Restore();
	    EditScenario::DrawList(it_list_head, (LISTMAXITEM > curmaps->size() ? curmaps->size() : LISTMAXITEM));
	    backgroundInfo.Restore();
	    EditScenario::DrawSelectInfo(it_current);
	    split.SetRange(0, (LISTMAXITEM < curmaps->size() ? curmaps->size() - LISTMAXITEM : 0));
	    Cursor::Show();
	    display.Flip();
	}

	// click large
	if(le.MouseClickLeft(buttonSelectLarge) && largemaps.size()){
	    curmaps = &largemaps;
	    it_list_head = curmaps->begin();
	    it_current = it_list_head;
	    Cursor::Hide();
	    backgroundList.Restore();
	    EditScenario::DrawList(it_list_head, (LISTMAXITEM > curmaps->size() ? curmaps->size() : LISTMAXITEM));
	    backgroundInfo.Restore();
	    EditScenario::DrawSelectInfo(it_current);
	    split.SetRange(0, (LISTMAXITEM < curmaps->size() ? curmaps->size() - LISTMAXITEM : 0));
	    Cursor::Show();
	    display.Flip();
	}

	// click xlarge
	if(le.MouseClickLeft(buttonSelectXLarge) && xlargemaps.size()){
	    curmaps = &xlargemaps;
	    it_list_head = curmaps->begin();
	    it_current = it_list_head;
	    Cursor::Hide();
	    backgroundList.Restore();
	    EditScenario::DrawList(it_list_head, (LISTMAXITEM > curmaps->size() ? curmaps->size() : LISTMAXITEM));
	    backgroundInfo.Restore();
	    EditScenario::DrawSelectInfo(it_current);
	    split.SetRange(0, (LISTMAXITEM < curmaps->size() ? curmaps->size() - LISTMAXITEM : 0));
	    Cursor::Show();
	    display.Flip();
	}

	// click all
	if(le.MouseClickLeft(buttonSelectAll)){
	    curmaps = &allmaps;
	    it_list_head = curmaps->begin();
	    it_current = it_list_head;
	    Cursor::Hide();
	    backgroundList.Restore();
	    EditScenario::DrawList(it_list_head, (LISTMAXITEM > curmaps->size() ? curmaps->size() : LISTMAXITEM));
	    backgroundInfo.Restore();
	    EditScenario::DrawSelectInfo(it_current);
	    split.SetRange(0, (LISTMAXITEM < curmaps->size() ? curmaps->size() - LISTMAXITEM : 0));
	    Cursor::Show();
	    display.Flip();
	}

	// click list
	if(le.MouseClickLeft(rectAreaList)){
	    u16 num = (le.MouseReleaseLeft().y - rectAreaList.y) / LISTHEIGHTROW;
	    if(num >= curmaps->size()) num = curmaps->size() - 1;
	    if(num_select == num) break;
	    num_select = num;
	    Cursor::Hide();
	    backgroundInfo.Restore();
	    it_current = it_list_head + num;
	    EditScenario::DrawSelectInfo(it_current);
	    split.Move(it_list_head - curmaps->begin());
	    Cursor::Show();
	    display.Flip();
	    num_select = num;
	}

	// click up
	if((le.MouseWheelUp(rectAreaList) || le.MouseWheelUp(split.GetRect()) || le.MouseClickLeft(buttonPgUp)) &&
	    curmaps->size() && it_list_head > curmaps->begin()){
	    Cursor::Hide();
	    backgroundList.Restore();
	    EditScenario::DrawList(--it_list_head);
	    split.Backward();
	    Cursor::Show();
	    display.Flip();
	}

	// click down
	if((le.MouseWheelDn(rectAreaList) || le.MouseWheelDn(split.GetRect()) || le.MouseClickLeft(buttonPgDn)) &&
	    LISTMAXITEM < curmaps->size() && it_list_head + LISTMAXITEM < curmaps->end()){
	    Cursor::Hide();
	    backgroundList.Restore();
	    EditScenario::DrawList(++it_list_head);
	    split.Forward();
	    Cursor::Show();
	    display.Flip();
	}

	// move cursor splitter
	if(le.MousePressLeft(split.GetRect()) && curmaps->size() > LISTMAXITEM && le.MouseCursor(split.GetRect()))
	{
	    u16 seek = (le.MouseCursor().y - split.GetRect().y) * 100 / split.GetStep();
	    if(seek > curmaps->size() - LISTMAXITEM) seek = curmaps->size() - LISTMAXITEM;

	    it_list_head = curmaps->begin() + seek;
	    Cursor::Hide();
	    backgroundList.Restore();
	    EditScenario::DrawList(it_list_head, LISTMAXITEM);
	    backgroundInfo.Restore();
	    EditScenario::DrawSelectInfo(it_current);
	    split.Move(seek);
	    Cursor::Show();
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
	if(le.MouseClickLeft(buttonOk) || le.KeyPress(SDLK_RETURN) || le.KeyPress(SDLK_ESCAPE)) break;
    }

    // set current settings
    EditScenario::SetCurrentSettings(*it_current);

    Cursor::Hide();
    background.Restore();
    
    return Game::MAINMENU;
}

void EditScenario::SetCurrentSettings(const Maps::FileInfo &maps)
{
    H2Config::SetKingdomCount(maps.GetKingdomCount());
    H2Config::SetKingdomColors(maps.GetKingdomColors());
    H2Config::SetAllowChangeColors(maps.GetAllowColors());
    H2Config::SetAllowChangeRaces(0);
    for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color)
        switch(maps.GetKingdomRace(color)){
            case 0x00:
                H2Config::SetKingdomRace(color, Race::KNGT);
                break;
            case 0x01:
                H2Config::SetKingdomRace(color, Race::BARB);
                break;
            case 0x02:
                H2Config::SetKingdomRace(color, Race::SORC);
                break;
            case 0x03:
                H2Config::SetKingdomRace(color, Race::WRLK);
                break;
            case 0x04:
                H2Config::SetKingdomRace(color, Race::WZRD);
                break;
            case 0x05:
                H2Config::SetKingdomRace(color, Race::NECR);
                break;
            case 0x06:
                H2Config::SetKingdomRace(color, Race::MULT);
                break;
            case 0x07:
                H2Config::SetKingdomRace(color, Race::RAND);
                H2Config::SetAllowChangeRaces(color | H2Config::GetAllowChangeRaces());
                break;
            default:
                H2Config::SetKingdomRace(color, Race::BOMG);
                break;
	}
    for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color)
	// set first allow color
	if(maps.GetAllowColors() & color){ H2Config::SetHumanColor(color); break; }
    H2Config::SetSizeMaps(maps.GetSizeMaps());
    H2Config::SetFileMaps(maps.GetFileMaps());
    H2Config::SetNameMaps(maps.GetName());
    H2Config::SetDescriptionMaps(maps.GetDescription());
    //
    H2Config::SetPlayWithHeroes(maps.GetPlayWithHeroes());
}

void EditScenario::DrawList(std::vector<Maps::FileInfo>::const_iterator &it_top, u8 count)
{
    u16 y = 64;
    u16 x = 176;
    u8 index = 0;
    std::vector<Maps::FileInfo>::const_iterator it_head = it_top;

    for(int ii = 0; ii < count; ++ii){

	// sprite count
	index = 19 + (*it_head).GetKingdomCount();
	const Sprite &spriteCount = AGG::GetICN("REQUESTS.ICN", index);
	display.Blit(spriteCount, x, y);

        // sprite size
	switch((*it_head).GetSizeMaps()){
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
	const Sprite &spriteSize = AGG::GetICN("REQUESTS.ICN", index);
	display.Blit(spriteSize, x + spriteCount.w() + 2, y);

	// text longname
	Text((*it_head).GetName(), Font::BIG, x + spriteCount.w() + spriteSize.w() + 18, y);

	// sprite wins
	index = 30 + (*it_head).GetConditionsWins();
	const Sprite &spriteWins = AGG::GetICN("REQUESTS.ICN", index);
	display.Blit(spriteWins, x + 224, y);

	// sprite loss
	index = 36 + (*it_head).GetConditionsLoss();
	const Sprite &spriteLoss = AGG::GetICN("REQUESTS.ICN", index);
	display.Blit(spriteLoss, x + 226 + spriteWins.w(), y);

	y += LISTHEIGHTROW;
	++it_head;
    }
}

void EditScenario::DrawSelectInfo(std::vector<Maps::FileInfo>::const_iterator &it_current)
{
    u16 x = 196;
    u16 y = 269;
    u8 index = 0;

    // sprite count
    index = 19 + (*it_current).GetKingdomCount();
    const Sprite &spriteCount = AGG::GetICN("REQUESTS.ICN", index);
    display.Blit(spriteCount, x, y);

    // sprite size
    switch((*it_current).GetSizeMaps()){
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
    const Sprite &spriteSize = AGG::GetICN("REQUESTS.ICN", index);
    display.Blit(spriteSize, x + spriteCount.w() + 2, y);

    // text longname
    Text((*it_current).GetName(), Font::BIG, x + spriteCount.w() + spriteSize.w() + 22, y);

    // sprite wins
    index = 30 + (*it_current).GetConditionsWins();
    const Sprite &spriteWins = AGG::GetICN("REQUESTS.ICN", index);
    display.Blit(spriteWins, x + 209, y);

    // sprite loss
    index = 36 + (*it_current).GetConditionsLoss();
    const Sprite &spriteLoss = AGG::GetICN("REQUESTS.ICN", index);
    display.Blit(spriteLoss, x + 211 + spriteWins.w(), y);

    Text("Maps difficulty:", Font::BIG, 200, 295);
    Text(Difficulty::String((*it_current).GetDifficulty()), Font::BIG, 360, 295);
    
    TextBox((*it_current).GetDescription(), Font::BIG, Rect(175, 322, 282, 90));
}
