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

#include "error.h"
#include "color.h"
#include "dialog.h"
#include "gameevent.h"

GameEvent::Sign::Sign(u16 index, const char *str) : index_map(index), message(str)
{ if(H2Config::Debug()) Error::Verbose("add Sign: " + message); }

void GameEvent::Sign::Show(void)
{ Dialog::Message("", message, Font::BIG, Dialog::OK); }

GameEvent::Day::Day(const void *ptr)
{
    const u8  *byte8  = static_cast<const u8 *>(ptr);
    const u16 *byte16 = NULL;
    const u32 *byte32 = NULL;

    // id
    if(0x00 != byte8[0]) Error::Except("GameEvent::Day: unknown magic id");

    // resource
    byte32 = reinterpret_cast<const u32 *>(& byte8[1]);

    resource.wood = *byte32;
    ++byte32;

    resource.mercury = *byte32;
    ++byte32;

    resource.ore = *byte32;
    ++byte32;

    resource.sulfur = *byte32;
    ++byte32;

    resource.crystal = *byte32;
    ++byte32;

    resource.gems = *byte32;
    ++byte32;

    resource.gold = *byte32;
    ++byte32;

    // skip artifact
    byte16 = reinterpret_cast<const u16 *>(byte32);
    ++byte16;

    // allow computer
    byte8 = reinterpret_cast<const u8 *>(byte16);
    computer = *byte8;
    ++byte8;

    // day of first occurent
    byte16 = reinterpret_cast<const u16 *>(byte8);
    first = *byte16;
    ++byte16;

    // subsequent occurrences
    byte8 = reinterpret_cast<const u8 *>(byte16);
    subsequent = *byte8;
    byte8 += 8;

    colors = 0;
    
    // blue
    if(*byte8) colors |= Color::BLUE;
    ++byte8;

    // green
    if(*byte8) colors |= Color::GREEN;
    ++byte8;

    // red
    if(*byte8) colors |= Color::RED;
    ++byte8;

    // yellow
    if(*byte8) colors |= Color::YELLOW;
    ++byte8;

    // orange
    if(*byte8) colors |= Color::ORANGE;
    ++byte8;

    // purple
    if(*byte8) colors |= Color::PURPLE;
    ++byte8;

    // message
    message = std::string(reinterpret_cast<const char *>(byte8));
    
    if(255 < message.size()) Error::Warning("GameEvent::Day: long message, incorrect block?");

    if(H2Config::Debug()) Error::Verbose("GameEvent::Day: add: " + message);
}

GameEvent::Coord::Coord(u16 index, const void *ptr) : index_map(index)
{
    const u8  *byte8  = static_cast<const u8 *>(ptr);
    const u16 *byte16 = NULL;
    const u32 *byte32 = NULL;

    // id
    if(0x01 != byte8[0]) Error::Except("GameEvent::Coord: unknown magic id");

    // resource
    byte32 = reinterpret_cast<const u32 *>(& byte8[1]);

    resource.wood = *byte32;
    ++byte32;

    resource.mercury = *byte32;
    ++byte32;

    resource.ore = *byte32;
    ++byte32;

    resource.sulfur = *byte32;
    ++byte32;

    resource.crystal = *byte32;
    ++byte32;

    resource.gems = *byte32;
    ++byte32;

    resource.gold = *byte32;
    ++byte32;

    // artifact
    byte16 = reinterpret_cast<const u16 *>(byte32);
    if(0xffff != *byte16 && Artifact::MAGIC_BOOK > *byte16) artifact = Artifact::Artifact(*byte16);
    ++byte16;

    // allow computer
    byte8 = reinterpret_cast<const u8 *>(byte16);
    computer = *byte8;
    ++byte8;

    // cancel event after first visit
    cancel = *byte8;
    byte8 += 11;

    colors = 0;
    
    // blue
    if(*byte8) colors |= Color::BLUE;
    ++byte8;

    // green
    if(*byte8) colors |= Color::GREEN;
    ++byte8;

    // red
    if(*byte8) colors |= Color::RED;
    ++byte8;

    // yellow
    if(*byte8) colors |= Color::YELLOW;
    ++byte8;

    // orange
    if(*byte8) colors |= Color::ORANGE;
    ++byte8;

    // purple
    if(*byte8) colors |= Color::PURPLE;
    ++byte8;

    // message
    message = std::string(reinterpret_cast<const char *>(byte8));
    
    if(255 < message.size()) Error::Warning("GameEvent::Coord: long message, incorrect block?");

    if(H2Config::Debug()) Error::Verbose("GameEvent::Coord: add: " + message);
}

GameEvent::Riddle::Riddle(u16 index, const void *ptr) : index_map(index)
{
    const u8  *byte8  = static_cast<const u8 *>(ptr);
    const u16 *byte16 = NULL;
    const u32 *byte32 = NULL;

    // id
    if(0x00 != byte8[0]) Error::Except("GameEvent::Day: unknown magic id");

    // resource
    byte32 = reinterpret_cast<const u32 *>(& byte8[1]);

    resource.wood = *byte32;
    ++byte32;

    resource.mercury = *byte32;
    ++byte32;

    resource.ore = *byte32;
    ++byte32;

    resource.sulfur = *byte32;
    ++byte32;

    resource.crystal = *byte32;
    ++byte32;

    resource.gems = *byte32;
    ++byte32;

    resource.gold = *byte32;
    ++byte32;

    // artifact
    byte16 = reinterpret_cast<const u16 *>(byte32);
    if(0xffff != *byte16 && Artifact::MAGIC_BOOK > *byte16) artifact = Artifact::Artifact(*byte16);
    ++byte16;

    // count answers
    byte8 = reinterpret_cast<const u8 *>(byte16);
    u8 count = *byte8;
    ++byte8;

    // answers
    for(u8 i = 0; i < 8; ++i){

	std::string str(reinterpret_cast<const char *>(byte8));
	if(count-- && str.size()) answers.push_back(str);
	
	byte8 += 13;
    }

    // message
    message = std::string(reinterpret_cast<const char *>(byte8));
    
    if(255 < message.size()) Error::Warning("GameEvent::Riddle: long message, incorrect block?");

    if(H2Config::Debug()) Error::Verbose("GameEvent::Riddle: add: " + message);
}
