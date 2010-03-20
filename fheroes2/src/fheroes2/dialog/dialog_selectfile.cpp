/***************************************************************************
 *   Copyright (C) 2009 by Andrey Afletdinov <fheroes2@gmail.com>          *
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

#include <algorithm>
#include <ctime>
#include <sstream>
#include <string>
#include "dir.h"
#include "agg.h"
#include "button.h"
#include "cursor.h"
#include "splitter.h"
#include "settings.h"
#include "maps.h"
#include "dialog.h"
#include "tools.h"


bool SelectFileListSimple(const std::string &, MapsFileInfoList &, std::string &, bool);
void RedrawFileListSimple(const Point &, const std::string &, const std::string & filename, const MapsFileInfoList &, MapsFileInfoList::const_iterator, MapsFileInfoList::const_iterator, const u8);

void ResizeToShortName(const std::string & str, std::string & res)
{
    res.assign(GetBasename(str.c_str()));
    size_t it = res.find('.');
    if(std::string::npos != it) res.resize(it);
}

bool Dialog::SelectFileSave(std::string & file)
{
    Dir dir;
    const std::string store_dir(Settings::Get().LocalPrefix() + SEPARATOR + "files" + SEPARATOR + "save");
    dir.Read(store_dir, ".sav", false);

    MapsFileInfoList lists(dir.size());
    MapsFileInfoList::const_iterator res;
    int ii = 0;
    for(Dir::const_iterator itd = dir.begin(); itd != dir.end(); ++itd, ++ii) if(!lists[ii].ReadSAV(*itd)) --ii;
    if(static_cast<size_t>(ii) != lists.size()) lists.resize(ii);
    std::sort(lists.begin(), lists.end());

    return SelectFileListSimple(_("File to Save:"), lists, file, true);
}

bool Dialog::SelectFileLoad(std::string & file)
{
    Dir dir;
    const std::string store_dir(Settings::Get().LocalPrefix() + SEPARATOR + "files" + SEPARATOR + "save");
    dir.Read(store_dir, ".sav", false);

    MapsFileInfoList lists(dir.size());
    MapsFileInfoList::const_iterator res;
    int ii = 0;
    for(Dir::const_iterator itd = dir.begin(); itd != dir.end(); ++itd, ++ii) if(!lists[ii].ReadSAV(*itd)) --ii;
    if(static_cast<size_t>(ii) != lists.size()) lists.resize(ii);
    std::sort(lists.begin(), lists.end());

    return SelectFileListSimple(_("File to Load:"), lists, file, false);
}

bool SelectFileListSimple(const std::string & header, MapsFileInfoList & lists, std::string & result, bool editor)
{
    // preload
    AGG::PreloadObject(ICN::REQBKG);
    AGG::PreloadObject(ICN::REQUEST);

    Display & display = Display::Get();
    Cursor & cursor = Cursor::Get();

    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);

    const Sprite & sprite = AGG::GetICN(ICN::REQBKG, 0);
    Size panel(sprite.w(), sprite.h());
    bool pocket = Settings::Get().PocketPC();
    if(pocket) panel = Size(sprite.w(), 224);

    Background back((display.w() - panel.w) / 2, (display.h() - panel.h) / 2, panel.w, panel.h);
    back.Save();

    const Rect & rt = back.GetRect();
    const Rect list_rt(rt.x + 40, rt.y + 55, 265, (pocket ? 78 : 215));
    const Rect enter_field(rt.x + 45, rt.y + (pocket ? 148 : 286), 260, 16);

    Button buttonCancel(rt.x + 34, rt.y + (pocket ? 176 : 315), ICN::REQUEST, 3, 4);
    Button buttonOk(rt.x + 244, rt.y + (pocket ? 176 : 315), ICN::REQUEST, 1, 2);
    Button buttonPgUp(rt.x + 327, rt.y + 55, ICN::REQUEST, 5, 6);
    Button buttonPgDn(rt.x + 327, rt.y + (pocket ? 117 : 257), ICN::REQUEST, 7, 8);

    const u8 max_items = pocket ? 5 : 11;
    MapsFileInfoList::iterator top = lists.begin();
    MapsFileInfoList::iterator cur = lists.begin();

    Splitter split(AGG::GetICN(ICN::ESCROLL, 3), Rect(rt.x + 330, rt.y + 73, 12, (pocket ? 40 : 180)), Splitter::VERTICAL);
    split.SetRange(0, (max_items < lists.size() ? lists.size() - max_items : 0));

    std::string filename;

    if(editor)
    {
	if(Settings::Get().PocketPC())
	{
            std::ostringstream ss;
	    ss << std::time(0);
	    filename = ss.str();
	}
	else
	    filename.assign("newgame");
	cur = lists.end();
    }
    else
    {
	if(lists.empty())
	    buttonOk.SetDisable(true);
	else
	    ResizeToShortName((*cur).file, filename);
    }

    RedrawFileListSimple(rt, header, filename, lists, top, cur, max_items);

    buttonOk.Draw();
    buttonCancel.Draw();
    buttonPgUp.Draw();
    buttonPgDn.Draw();
    split.Redraw();

    cursor.Show();
    display.Flip();

    LocalEvent & le = LocalEvent::Get();
    bool redraw = false;
    bool edit_mode = false;

    while(le.HandleEvents())
    {
        le.MousePressLeft(buttonOk) && buttonOk.isEnable() ? buttonOk.PressDraw() : buttonOk.ReleaseDraw();
        le.MousePressLeft(buttonCancel) ? buttonCancel.PressDraw() : buttonCancel.ReleaseDraw();
        le.MousePressLeft(buttonPgUp) ? buttonPgUp.PressDraw() : buttonPgUp.ReleaseDraw();
        le.MousePressLeft(buttonPgDn) ? buttonPgDn.PressDraw() : buttonPgDn.ReleaseDraw();

        if((le.MouseClickLeft(buttonOk) && buttonOk.isEnable()) || le.KeyPress(KEY_RETURN))
        {
    	    if(editor && filename.size())
		result = Settings::Get().LocalPrefix() + SEPARATOR + "files" + SEPARATOR + "save" + SEPARATOR + filename + ".sav";
    	    else
    	    if(cur != lists.end())
    		result = (*cur).file;
    	    break;
    	}
    	else
        if(le.MouseClickLeft(buttonCancel) || le.KeyPress(KEY_ESCAPE))
        {
    	    result.clear();
    	    break;
	}
	else
        if(le.MouseClickLeft(enter_field) && editor)
	{
	    edit_mode = true;
	    redraw = true;
	}
	else
	if(edit_mode && le.KeyPress())
	{
	    String::AppendKey(filename, le.KeyValue(), le.KeyMod());

	    buttonOk.SetDisable(filename.empty());

	    redraw = true;
	}
	else
        if((le.MouseClickLeft(buttonPgUp) || le.KeyPress(KEY_PAGEUP)) && (top > lists.begin()))
	{
	    top = (top - lists.begin() > max_items ? top - max_items : lists.begin());
	    split.Move(top - lists.begin());
	    redraw = true;
	}
	else
        if((le.MouseClickLeft(buttonPgDn) || le.KeyPress(KEY_PAGEDOWN)) && (top + max_items < lists.end()))
	{
	    top += max_items;
	    if(top + max_items > lists.end()) top = lists.end() - max_items;
	    split.Move(top - lists.begin());
	    redraw = true;
	}
	else
        if(le.KeyPress(KEY_UP) && (cur > lists.begin()))
	{
	    --cur;
	    if((cur < top) || (cur > top + max_items - 1))
	    {
		top = cur;
		if(top + max_items > lists.end()) top = lists.end() - max_items;
	    }
	    split.Move(top - lists.begin());
	    redraw = true;
	    edit_mode = false;
	}
	else
        if(le.KeyPress(KEY_DOWN) && (cur < (lists.end() - 1)))
	{
	    ++cur;
	    if((cur < top) || (cur > top + max_items - 1))
	    {
		top = cur - max_items + 1;
		if(top + max_items > lists.end()) top = lists.end() - max_items;
		else
		if(top < lists.begin()) top = lists.begin();
	    }
	    split.Move(top - lists.begin());
	    redraw = true;
	    edit_mode = false;
	}
	else
	if((le.MouseWheelUp(list_rt) || le.MouseWheelUp(split.GetRect())) && (top > lists.begin()))
	{
	    --top;
	    split.Backward();
	    redraw = true;
	}
	else
	if((le.MouseWheelDn(list_rt) || le.MouseWheelDn(split.GetRect())) && (top < (lists.end() - max_items)))
	{
	    ++top;
	    split.Forward();
	    redraw = true;
	}
	else
	if(le.MousePressLeft(split.GetRect()) && (lists.size() > max_items))
	{
	    s16 seek = (le.GetMouseCursor().y - split.GetRect().y) * 100 / split.GetStep();
	    if(seek < split.Min()) seek = split.Min();
	    else
	    if(seek > split.Max()) seek = split.Max();
	    top = lists.begin() + seek;
	    split.Move(seek);
	    redraw = true;
 	}
	else
	if((le.KeyPress(KEY_DELETE) || (Settings::Get().PocketPC() && le.MousePressRight())) && cur != lists.end())
	{
	    std::string msg(_("Are you sure you want to delete file:"));
	    msg.append("\n \n");
	    msg.append(GetBasename((*cur).file.c_str()));
	    if(Dialog::YES == Dialog::Message(_("Warning!"), msg, Font::BIG, Dialog::YES | Dialog::NO))
	    {
		unlink((*cur).file.c_str());
		lists.erase(cur);
		if(lists.empty() || filename.empty()) buttonOk.SetDisable(true);
		top = lists.begin();
		cur = top;
		split.SetRange(0, (max_items < lists.size() ? lists.size() - max_items : 0));
		split.Move(top - lists.begin());
		redraw = true;
	    }
	}

	if(le.MouseClickLeft(list_rt) && lists.size())
	{
	    MapsFileInfoList::iterator cur2 = top + static_cast<size_t>((le.GetMouseReleaseLeft().y - list_rt.y) * max_items / static_cast<float>(list_rt.h));
	    if(cur2 > (lists.end() - 1)) cur2 = lists.end() - 1;
	    // double click
	    if(cur2 == cur)
	    {
    		result = (*cur).file;
    		break;
	    }
	    else
		cur = cur2;
	    redraw = true;
	    edit_mode = false;
	}

	if(redraw)
	{
	    cursor.Hide();

	    if(edit_mode && editor)
	    {
		RedrawFileListSimple(rt, header, filename + "_", lists, top, cur, max_items);
	    }
	    else
	    if(cur != lists.end())
	    {
	    	ResizeToShortName((*cur).file, filename);
		RedrawFileListSimple(rt, header, filename, lists, top, cur, max_items);
	    }
	    else
		RedrawFileListSimple(rt, header, filename, lists, top, cur, max_items);

	    buttonOk.Draw();
	    buttonCancel.Draw();
	    buttonPgUp.Draw();
	    buttonPgDn.Draw();
	    split.Redraw();
	    cursor.Show();
	    display.Flip();
	    redraw = false;
	}
    }

    cursor.Hide();
    back.Restore();

    return result.size();
}

void RedrawFileListSimple(const Point & dst, const std::string & header, const std::string & filename, const MapsFileInfoList & lists, MapsFileInfoList::const_iterator top, MapsFileInfoList::const_iterator cur, const u8 max_items)
{
    Display & display = Display::Get();
    const Sprite & panel = AGG::GetICN(ICN::REQBKG, 0);

    if(Settings::Get().PocketPC())
    {
	display.Blit(panel, Rect(0, 0, panel.w(), 120), dst.x, dst.y);
	display.Blit(panel, Rect(0, panel.h() - 120, panel.w(), 120), dst.x, dst.y + 224 - 120);
    }
    else
	display.Blit(panel, dst);

    Text text(header, Font::BIG);
    text.Blit(dst.x + 175 - text.w() / 2, dst.y + 30);
    
    u16 oy = 60;
    char short_date[15];
         short_date[14] = 0;

    for(MapsFileInfoList::const_iterator ii = top; ii != lists.end() && (ii - top < max_items); ++ii)
    {
	const Maps::FileInfo & info = *ii;

	const std::string name(GetBasename(info.file.c_str()));

	std::strftime(short_date, 14, "%b %d, %H:%M", std::localtime(&info.localtime));
	std::string savname(GetBasename(info.file.c_str()));
	if(savname.empty()) continue;

	const size_t dotpos = savname.size() - 4;
	std::string ext = savname.substr(dotpos);
	String::Lower(ext);
    	if(ext == ".sav") savname.erase(dotpos);

	text.Set(savname, (cur == ii ? Font::YELLOW_BIG : Font::BIG));
	text.Blit(dst.x + 45, dst.y + oy);

	text.Set(short_date, (cur == ii ? Font::YELLOW_BIG : Font::BIG));
	text.Blit(dst.x + 305 - text.w(), dst.y + oy);

	oy += text.h() + 2;
    }

    if(filename.size())
    {
	text.Set(filename, Font::BIG);
	text.Blit(dst.x + 175 - text.w() / 2, Settings::Get().PocketPC() ? dst.y + 148 : dst.y + 289);
    }
}
