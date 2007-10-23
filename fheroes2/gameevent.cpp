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

#define SIZEMESSAGE 400

GameEvent::Sign::Sign(u16 index, const char *str) : index_map(index), message(str)
{ if(H2Config::Debug()) Error::Verbose("add Sign: " + message); }

void GameEvent::Sign::Show(void)
{ Dialog::Message("", message, Font::BIG, Dialog::OK); }

GameEvent::Day::Day(const void *ptr)
{
    const u8  *ptr8  = static_cast<const u8 *>(ptr);
    u16 byte16 = 0;
    u32 byte32 = 0;

    // id
    if(0x00 != *ptr8) Error::Except("GameEvent::Day: unknown magic id");
    ++ptr8;

    // resource
    LOAD32(ptr8, byte32);
    ptr8 += 4;;
    resource.wood = byte32;

    LOAD32(ptr8, byte32);
    ptr8 += 4;;
    resource.mercury = byte32;

    LOAD32(ptr8, byte32);
    ptr8 += 4;;
    resource.ore = byte32;

    LOAD32(ptr8, byte32);
    ptr8 += 4;;
    resource.sulfur = byte32;

    LOAD32(ptr8, byte32);
    ptr8 += 4;;
    resource.crystal = byte32;

    LOAD32(ptr8, byte32);
    ptr8 += 4;;
    resource.gems = byte32;

    LOAD32(ptr8, byte32);
    ptr8 += 4;;
    resource.gold = byte32;

    // skip artifact
    LOAD16(ptr8, byte16);
    ++ptr8;
    ++ptr8;

    // allow computer
    LOAD16(ptr8, byte16);
    ++ptr8;
    ++ptr8;
    computer = byte16;

    // day of first occurent
    LOAD16(ptr8, byte16);
    ++ptr8;
    ++ptr8;
    first = byte16;

    // subsequent occurrences
    LOAD16(ptr8, byte16);
    ++ptr8;
    ++ptr8;
    subsequent = byte16;

    ptr8 += 6;

    colors = 0;
    
    // blue
    if(*ptr8) colors |= Color::BLUE;
    ++ptr8;

    // green
    if(*ptr8) colors |= Color::GREEN;
    ++ptr8;

    // red
    if(*ptr8) colors |= Color::RED;
    ++ptr8;

    // yellow
    if(*ptr8) colors |= Color::YELLOW;
    ++ptr8;

    // orange
    if(*ptr8) colors |= Color::ORANGE;
    ++ptr8;

    // purple
    if(*ptr8) colors |= Color::PURPLE;
    ++ptr8;

    // message
    message = std::string(reinterpret_cast<const char *>(ptr8));
    
    if(SIZEMESSAGE < message.size()) Error::Warning("GameEvent::Day: long message, incorrect block?");

    if(H2Config::Debug()) Error::Verbose("GameEvent::Day: add: " + message);
}

GameEvent::Coord::Coord(u16 index, const void *ptr) : index_map(index)
{
    const u8  *ptr8  = static_cast<const u8 *>(ptr);
    u16 byte16 = 0;
    u32 byte32 = 0;

    // id
    if(0x01 != *ptr8) Error::Except("GameEvent::Coord: unknown magic id");
    ++ptr8;

    // resource
    LOAD32(ptr8, byte32);
    ptr8 += 4;;
    resource.wood = byte32;

    LOAD32(ptr8, byte32);
    ptr8 += 4;;
    resource.mercury = byte32;

    LOAD32(ptr8, byte32);
    ptr8 += 4;;
    resource.ore = byte32;

    LOAD32(ptr8, byte32);
    ptr8 += 4;;
    resource.sulfur = byte32;

    LOAD32(ptr8, byte32);
    ptr8 += 4;;
    resource.crystal = byte32;

    LOAD32(ptr8, byte32);
    ptr8 += 4;;
    resource.gems = byte32;

    LOAD32(ptr8, byte32);
    ptr8 += 4;;
    resource.gold = byte32;

    // artifact
    LOAD16(ptr8, byte16);
    ++ptr8;
    ++ptr8;
    artifact = (0xffff != byte16 && Artifact::MAGIC_BOOK > byte16 ? artifact = Artifact::Artifact(byte16) : Artifact::UNKNOWN);

    // allow computer
    computer = *ptr8;
    ++ptr8;

    // cancel event after first visit
    cancel = *ptr8;
    ptr8 += 11;

    colors = 0;
    
    // blue
    if(*ptr8) colors |= Color::BLUE;
    ++ptr8;

    // green
    if(*ptr8) colors |= Color::GREEN;
    ++ptr8;

    // red
    if(*ptr8) colors |= Color::RED;
    ++ptr8;

    // yellow
    if(*ptr8) colors |= Color::YELLOW;
    ++ptr8;

    // orange
    if(*ptr8) colors |= Color::ORANGE;
    ++ptr8;

    // purple
    if(*ptr8) colors |= Color::PURPLE;
    ++ptr8;

    // message
    message = std::string(reinterpret_cast<const char *>(ptr8));
    
    if(SIZEMESSAGE < message.size()) Error::Warning("GameEvent::Coord: long message, incorrect block?");

    if(H2Config::Debug()) Error::Verbose("GameEvent::Coord: add: " + message);
}

GameEvent::Riddle::Riddle(u16 index, const void *ptr) : index_map(index)
{
    const u8  *ptr8  = static_cast<const u8 *>(ptr);
    u16 byte16 = 0;
    u32 byte32 = 0;

    // id
    if(0x00 != *ptr8) Error::Except("GameEvent::Day: unknown magic id");
    ++ptr8;

    // resource
    LOAD32(ptr8, byte32);
    ptr8 += 4;;
    resource.wood = byte32;

    LOAD32(ptr8, byte32);
    ptr8 += 4;;
    resource.mercury = byte32;

    LOAD32(ptr8, byte32);
    ptr8 += 4;;
    resource.ore = byte32;

    LOAD32(ptr8, byte32);
    ptr8 += 4;;
    resource.sulfur = byte32;

    LOAD32(ptr8, byte32);
    ptr8 += 4;;
    resource.crystal = byte32;

    LOAD32(ptr8, byte32);
    ptr8 += 4;;
    resource.gems = byte32;

    LOAD32(ptr8, byte32);
    ptr8 += 4;;
    resource.gold = byte32;

    // artifact
    LOAD16(ptr8, byte16);
    ++ptr8;
    ++ptr8;
    if(0xffff != byte16 && Artifact::MAGIC_BOOK > byte16) artifact = Artifact::Artifact(byte16);

    // count answers
    u8 count = *ptr8;
    ++ptr8;

    // answers
    for(u8 i = 0; i < 8; ++i){

	std::string str(reinterpret_cast<const char *>(ptr8));
	if(count-- && str.size()) answers.push_back(str);
	
	ptr8 += 13;
    }

    // message
    message = std::string(reinterpret_cast<const char *>(ptr8));
    
    if(SIZEMESSAGE < message.size()) Error::Warning("GameEvent::Riddle: long message, incorrect block?");

    if(H2Config::Debug()) Error::Verbose("GameEvent::Riddle: add: " + message);
}

GameEvent::Rumor::Rumor(const char * ptr) : message(ptr)
{
}
