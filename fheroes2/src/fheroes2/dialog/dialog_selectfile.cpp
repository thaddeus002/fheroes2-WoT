/***************************************************************************
 *   Copyright (C) 2008 by Andrey Afletdinov                               *
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
#include <list>
#include <string>
#include "background.h"
#include "dir.h"
#include "agg.h"
#include "sprite.h"
#include "spritecursor.h"
#include "engine.h"
#include "button.h"
#include "cursor.h"
#include "maps_fileinfo.h"
#include "difficulty.h"
#include "dialog.h"
#include "text.h"
#include "tools.h"
#include "splitter.h"
#include "world.h"

#define LISTMAXITEM	9
#define LISTHEIGHTROW	19

void DrawList(std::list<Maps::FileInfo *>::const_iterator &it_top, u8 count = LISTMAXITEM);
void DrawSelectInfo(const Maps::FileInfo & finfo);

const Maps::FileInfo * Dialog::SelectFileInfo(const std::list<Maps::FileInfo *> & info_maps)
{
    // preload
    AGG::PreloadObject(ICN::REQSBKG);

    std::list<Maps::FileInfo *> smallmaps, mediummaps, largemaps, xlargemaps;

    const std::list<Maps::FileInfo *> & allmaps = info_maps;
    const std::list<Maps::FileInfo *> *curmaps = &allmaps;

    std::list<Maps::FileInfo *>::const_iterator it_list_head = allmaps.begin();
    std::list<Maps::FileInfo *>::const_iterator it_current = it_list_head;
    std::list<Maps::FileInfo *>::const_iterator it_end = allmaps.end();

    for(; it_current != it_end; ++it_current)
	switch((**it_current).SizeMaps())
	{
    	    case Maps::SMALL:	smallmaps.push_back(*it_current);break;
    	    case Maps::MEDIUM:	mediummaps.push_back(*it_current); break;
    	    case Maps::LARGE:	largemaps.push_back(*it_current); break;
    	    case Maps::XLARGE:	xlargemaps.push_back(*it_current); break;
	    default: continue;
	}
    it_current = it_list_head;

    Display & display = Display::Get();

    // cursor
    Cursor & cursor = Cursor::Get();
    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);

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

    const Rect countPlayers(175, 65, 17, 170);
    const Rect sizeMaps(195, 65, 17, 170);
    const Rect victoryCond(400, 65, 17, 170);
    const Rect lossCond(420, 65, 17, 170);
    
    // Splitter
    Splitter split(AGG::GetICN(ICN::ESCROLL, 3), Rect(460, 78, 8, 141), Splitter::VERTICAL);
    split.SetRange(0, (LISTMAXITEM < allmaps.size() ? allmaps.size() - LISTMAXITEM : 0));

    // button
    Button buttonOk(270, 415, ICN::REQUESTS, 1, 2);
    Button buttonPgUp(457, 60, ICN::REQUESTS, 5, 6);
    Button buttonPgDn(457, 222, ICN::REQUESTS, 7, 8);

    Button buttonSelectSmall(167, 28, ICN::REQUESTS, 9, 10);
    Button buttonSelectMedium(229, 28, ICN::REQUESTS, 11, 12);
    Button buttonSelectLarge(291, 28, ICN::REQUESTS, 13, 14);
    Button buttonSelectXLarge(353, 28, ICN::REQUESTS, 15, 16);
    Button buttonSelectAll(415, 28, ICN::REQUESTS, 17, 18);

    DrawList(it_list_head, (LISTMAXITEM > allmaps.size() ? allmaps.size() : LISTMAXITEM));
    DrawSelectInfo(**it_current);

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

    LocalEvent & le = LocalEvent::GetLocalEvent();
    size_t num_head = 0;
    size_t num_select = 0;

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
	if(le.MouseClickLeft(buttonSelectSmall) && smallmaps.size())
	{
	    curmaps = &smallmaps;
	    it_list_head = curmaps->begin();
	    it_current = it_list_head;
	    num_head = 0;
	    cursor.Hide();
	    backgroundList.Restore();
	    DrawList(it_list_head, (LISTMAXITEM > curmaps->size() ? curmaps->size() : LISTMAXITEM));
	    backgroundInfo.Restore();
	    DrawSelectInfo(**it_current);
	    split.SetRange(0, (LISTMAXITEM < curmaps->size() ? curmaps->size() - LISTMAXITEM : 0));
	    cursor.Show();
	    display.Flip();
	}

	// click medium
	if(le.MouseClickLeft(buttonSelectMedium) && mediummaps.size())
	{
	    curmaps = &mediummaps;
	    it_list_head = curmaps->begin();
	    it_current = it_list_head;
	    num_head = 0;
	    cursor.Hide();
	    backgroundList.Restore();
	    DrawList(it_list_head, (LISTMAXITEM > curmaps->size() ? curmaps->size() : LISTMAXITEM));
	    backgroundInfo.Restore();
	    DrawSelectInfo(**it_current);
	    split.SetRange(0, (LISTMAXITEM < curmaps->size() ? curmaps->size() - LISTMAXITEM : 0));
	    cursor.Show();
	    display.Flip();
	}

	// click large
	if(le.MouseClickLeft(buttonSelectLarge) && largemaps.size())
	{
	    curmaps = &largemaps;
	    it_list_head = curmaps->begin();
	    it_current = it_list_head;
	    num_head = 0;
	    cursor.Hide();
	    backgroundList.Restore();
	    DrawList(it_list_head, (LISTMAXITEM > curmaps->size() ? curmaps->size() : LISTMAXITEM));
	    backgroundInfo.Restore();
	    DrawSelectInfo(**it_current);
	    split.SetRange(0, (LISTMAXITEM < curmaps->size() ? curmaps->size() - LISTMAXITEM : 0));
	    cursor.Show();
	    display.Flip();
	}

	// click xlarge
	if(le.MouseClickLeft(buttonSelectXLarge) && xlargemaps.size())
	{
	    curmaps = &xlargemaps;
	    it_list_head = curmaps->begin();
	    it_current = it_list_head;
	    num_head = 0;
	    cursor.Hide();
	    backgroundList.Restore();
	    DrawList(it_list_head, (LISTMAXITEM > curmaps->size() ? curmaps->size() : LISTMAXITEM));
	    backgroundInfo.Restore();
	    DrawSelectInfo(**it_current);
	    split.SetRange(0, (LISTMAXITEM < curmaps->size() ? curmaps->size() - LISTMAXITEM : 0));
	    cursor.Show();
	    display.Flip();
	}

	// click all
	if(le.MouseClickLeft(buttonSelectAll) && allmaps.size())
	{
	    curmaps = &allmaps;
	    it_list_head = curmaps->begin();
	    it_current = it_list_head;
	    num_head = 0;
	    cursor.Hide();
	    backgroundList.Restore();
	    DrawList(it_list_head, (LISTMAXITEM > curmaps->size() ? curmaps->size() : LISTMAXITEM));
	    backgroundInfo.Restore();
	    DrawSelectInfo(**it_current);
	    split.SetRange(0, (LISTMAXITEM < curmaps->size() ? curmaps->size() - LISTMAXITEM : 0));
	    cursor.Show();
	    display.Flip();
	}

	// click list
	if(le.MouseClickLeft(rectAreaList) && curmaps->size())
	{
	    u16 num = (le.MouseReleaseLeft().y - rectAreaList.y) / LISTHEIGHTROW;
	    if(num >= curmaps->size()) num = curmaps->size() - 1;
	    if(num_select == num) break;
	    num_select = num;
	    cursor.Hide();
	    backgroundInfo.Restore();
	    it_current = it_list_head;
	    while(num--) ++it_current;
	    DrawSelectInfo(**it_current);
	    cursor.Show();
	    display.Flip();
	}

	// click up
	if((le.MouseWheelUp(rectAreaList) || le.MouseWheelUp(split.GetRect()) || le.MouseClickLeft(buttonPgUp)) &&
	    curmaps->size() && num_head)
	{
	    cursor.Hide();
	    backgroundList.Restore();
	    --it_list_head;
	    --num_head;
	    DrawList(it_list_head);
	    split.Backward();
	    cursor.Show();
	    display.Flip();
	}

	// click down
	if((le.MouseWheelDn(rectAreaList) || le.MouseWheelDn(split.GetRect()) || le.MouseClickLeft(buttonPgDn)) &&
	    LISTMAXITEM < curmaps->size() && num_head + LISTMAXITEM < curmaps->size())
	{
	    cursor.Hide();
	    backgroundList.Restore();
	    ++it_list_head;
	    ++num_head;
	    DrawList(it_list_head);
	    split.Forward();
	    cursor.Show();
	    display.Flip();
	}

	// move cursor splitter
	if(le.MousePressLeft(split.GetRect()) && curmaps->size() > LISTMAXITEM)
	{
	    s16 seek = (le.MouseCursor().y - split.GetRect().y) * 100 / split.GetStep();

	    if(seek < split.Min()) seek = split.Min();
	    else
	    if(seek > split.Max()) seek = split.Max();

	    it_list_head = curmaps->begin();
	    cursor.Hide();
	    backgroundList.Restore();
	    split.Move(seek);
	    while(seek--){ ++it_list_head; ++num_head; }

	    DrawList(it_list_head);
	    cursor.Show();
	    display.Flip();
 	}

	// right info
	if(le.MousePressRight(buttonSelectSmall)) Dialog::Message("Small Maps", "View only maps of size small (36x36).", Font::BIG);
	if(le.MousePressRight(buttonSelectMedium)) Dialog::Message("Medium Maps", "View only maps of size medium (72x72).", Font::BIG);
	if(le.MousePressRight(buttonSelectLarge)) Dialog::Message("Large Maps", "View only maps of size large (108x108).", Font::BIG);
	if(le.MousePressRight(buttonSelectXLarge)) Dialog::Message("Extra Large Maps", "View only maps of size extra large (144x144).", Font::BIG);
	if(le.MousePressRight(buttonSelectAll)) Dialog::Message("All Maps", "View all maps, regardless of size.", Font::BIG);
	if(le.MousePressRight(countPlayers)) Dialog::Message("Players Icon", "Indicates how many players total are in the EditScenario. Any positions not occupied by humans will be occupied by computer players.", Font::BIG);
	if(le.MousePressRight(sizeMaps)) Dialog::Message("Size Icon", "Indicates whether the maps is small (36x36), medium (72x72), large (108x108), or extra large (144x144).", Font::BIG);
	//if(le.MousePressRight(?)) Dialog::Message("Selected Name", "The name of the currently selected map.", Font::BIG);
	if(le.MousePressRight(victoryCond)) Dialog::Message("Victory Condition Icon", "There are 6 possiblities: FIXME.", Font::BIG);
	if(le.MousePressRight(lossCond)) Dialog::Message("Loss Condition Icon", "There are 4 possible loss conditions, as indicated by the following icons: FIXME.", Font::BIG);
	//if(le.MousePressRight(?)) Dialog::Message("Selected Map Difficulty", "The map difficulty of the currently selected map.  The map difficulty is determined by the EditScenario designer. More difficult maps might include more or stronger enemies, fewer resources, or other special conditions making things tougher for the human player.", Font::BIG);
	//if(le.MousePressRight(?)) Dialog::Message("Selected Description", "The description of the currently selected map.", Font::BIG);
	if(le.MousePressRight(buttonOk)) Dialog::Message("OK", "Accept the choice made.", Font::BIG);

	// exit
	if(le.KeyPress(KEY_ESCAPE)){ it_current = allmaps.begin(); break; }
	if(le.MouseClickLeft(buttonOk) || le.KeyPress(KEY_RETURN)) break;
    }

    cursor.Hide();
    background.Restore();
    
    cursor.Show();

    return info_maps.size() ? *it_current : NULL;
}

void DrawList(std::list<Maps::FileInfo *>::const_iterator &it_top, u8 count)
{
    Display & display = Display::Get();

    u16 y = 64;
    u16 x = 176;
    u8 index = 0;
    std::list<Maps::FileInfo *>::const_iterator it_head(it_top);

    for(int ii = 0; ii < count; ++ii)
    {
	// sprite count
	const std::bitset<8> colors((**it_head).KingdomColors());
	index = 19 + colors.count();
	const Sprite &spriteCount = AGG::GetICN(ICN::REQUESTS, index);
	display.Blit(spriteCount, x, y);

        // sprite size
	switch((**it_head).SizeMaps())
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
	Text((**it_head).Name(), Font::BIG, x + spriteCount.w() + spriteSize.w() + 18, y);

	// sprite wins
	index = 30 + (**it_head).ConditionsWins();
	const Sprite &spriteWins = AGG::GetICN(ICN::REQUESTS, index);
	display.Blit(spriteWins, x + 224, y);

	// sprite loss
	index = 36 + (**it_head).ConditionsLoss();
	const Sprite &spriteLoss = AGG::GetICN(ICN::REQUESTS, index);
	display.Blit(spriteLoss, x + 226 + spriteWins.w(), y);

	y += LISTHEIGHTROW;
	++it_head;
    }
}

void DrawSelectInfo(const Maps::FileInfo & finfo)
{
    Display & display = Display::Get();

    u16 x = 196;
    u16 y = 269;
    u8 index = 0;

    // sprite count
    const std::bitset<8> colors(finfo.KingdomColors());
    index = 19 + colors.count();
    const Sprite &spriteCount = AGG::GetICN(ICN::REQUESTS, index);
    display.Blit(spriteCount, x, y);

    // sprite size
    switch(finfo.SizeMaps())
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
    Text(finfo.Name(), Font::BIG, x + spriteCount.w() + spriteSize.w() + 22, y);

    // sprite wins
    index = 30 + finfo.ConditionsWins();
    const Sprite &spriteWins = AGG::GetICN(ICN::REQUESTS, index);
    display.Blit(spriteWins, x + 209, y);

    // sprite loss
    index = 36 + finfo.ConditionsLoss();
    const Sprite &spriteLoss = AGG::GetICN(ICN::REQUESTS, index);
    display.Blit(spriteLoss, x + 211 + spriteWins.w(), y);

    Text("Maps difficulty:", Font::BIG, 200, 295);
    Text(Difficulty::String(finfo.Difficulty()), Font::BIG, 360, 295);
    
    TextBox(finfo.Description(), Font::BIG, Rect(175, 322, 282, 90));
}
