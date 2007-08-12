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
#include <fstream>
#include <cstdlib>
#include <iostream>

#include "error.h"
#include "tools.h"
#include "maps.h"
#include "agg.h"

namespace AGG {
    static std::fstream                        *fd = NULL;
    static std::map<std::string, AGG::aggfat_t> fat;
    static std::vector<u32>                     palette(AGGSIZEPALETTE);
    static u32 shadowAlpha = 0;
    static u32 colorKey = 0;
    static SDL_Color *colors = NULL;
};

/* AGG init */
void AGG::Init(const std::string & aggname)
{
    AGG::fd = new std::fstream(aggname.c_str(), std::ios::in | std::ios::binary);

    if(! AGG::fd || ! *AGG::fd || AGG::fd->fail())
	Error::Except(aggname + " file not found.");
    else
	Error::Verbose("AGG data: " + aggname);

    u16 count;
    char buf[AGGSIZENAME];
    aggfat_t fat_element;

    AGG::fd->read(reinterpret_cast<char *>(&count), sizeof(u16));
    SWAP16(count);

    for(u16 seg = 0; seg < count; ++seg){

	// name fat_element
	AGG::fd->seekg(-AGGSIZENAME * (count - seg), std::ios_base::end);
	AGG::fd->read(buf, AGGSIZENAME);
	std::string key_name(buf);

	// seek fat_element
	AGG::fd->seekg(sizeof(u16) + seg * (3 * sizeof(u32)), std::ios_base::beg);
	AGG::fd->seekg(sizeof(u32), std::ios_base::cur);

	// read fat_element
	AGG::fd->read(reinterpret_cast<char *>(&fat_element.blockOffset), sizeof(u32));
	SWAP32(fat_element.blockOffset);
	AGG::fd->read(reinterpret_cast<char *>(&fat_element.blockSize), sizeof(u32));
	SWAP32(fat_element.blockSize);

	// type data
	if(std::string::npos != key_name.find(".ICN")){
	    fat_element.blockType = DATA_ICN;
	    fat_element.vectorICN = NULL;
	}else if(std::string::npos != key_name.find(".82M"))
	    fat_element.blockType = DATA_WAV;
	else if(std::string::npos != key_name.find(".TIL")){
	    fat_element.blockType = DATA_TIL;
	    fat_element.ptrTIL = NULL;
	}else if(std::string::npos != key_name.find(".BMP"))
	    fat_element.blockType = DATA_BMP;
	else if(std::string::npos != key_name.find(".XMI"))
	    fat_element.blockType = DATA_XMI;
	else if(std::string::npos != key_name.find(".BIN"))
	    fat_element.blockType = DATA_BIN;
	else if(std::string::npos != key_name.find(".PAL")){
	    fat_element.blockType = DATA_PAL;
	}else if(std::string::npos != key_name.find(".FNT"))
	    fat_element.blockType = DATA_FNT;
	else
	    fat_element.blockType = DATA_UNK;

	//
	AGG::fat[key_name] = fat_element;
    }

    // palette for 8 bit surface
    colors = new SDL_Color[AGGSIZEPALETTE];

    // quit
    atexit(AGG::Quit);
}

void AGG::Quit(void)
{
    std::map<std::string, aggfat_t>::iterator itMap = AGG::fat.begin();
    std::map<std::string, aggfat_t>::iterator itMapEnd = AGG::fat.end();
	
    while(itMap != itMapEnd){ AGG::FreeObject(itMap->second); ++itMap; }

    AGG::fd->close();
    delete AGG::fd;

    if(colors) delete [] colors;
}

/* preload object to map */
void AGG::PreloadObject(const std::string & name)
{
    switch(AGG::fat[name].blockType){
    
	case DATA_ICN:
	    AGG::LoadICN(name);
	    break;

	case DATA_TIL:
	    AGG::LoadTIL(name);
	    break;

	case DATA_PAL:
	    break;

	default:
	    Error::Warning("AGG::PreloadObject: unknown object: " + name);
	    break;
    }
}

/* free object from map */
void AGG::FreeObject(const std::string & name)
{
    AGG::FreeObject(AGG::fat[name]);
}

void AGG::FreeObject(aggfat_t & fat){

    switch(fat.blockType){
    
	case DATA_ICN:
	    AGG::FreeICN(fat);
	    break;

	case DATA_TIL:
	    AGG::FreeTIL(fat);
	    break;

	case DATA_PAL:
	    break;
	
	default:
	    break;
    }
}

/* load TIL data */
void AGG::LoadTIL(const std::string & name)
{
    // check TIL
    if(AGG::fat[name].blockType != DATA_TIL) Error::Except("unknown til: " + name);

    // object exists
    if(AGG::fat[name].ptrTIL) return;

    Error::Verbose("AGG::LoadTIL: " + name);

    // offset
    AGG::fd->seekg(AGG::fat[name].blockOffset, std::ios_base::beg);
    
    u16 count, width, height;
    AGG::fd->read(reinterpret_cast<char *>(&count), sizeof(u16));
    SWAP16(count);
    AGG::fd->read(reinterpret_cast<char *>(&width), sizeof(u16));
    SWAP16(width);
    AGG::fd->read(reinterpret_cast<char *>(&height), sizeof(u16));
    SWAP16(height);

    // size
    u32 sizeData = count * width * height;

    // store
    AGG::fat[name].ptrTIL = new char[sizeData];
    AGG::fd->read(AGG::fat[name].ptrTIL, sizeData);
}

/* get TIL Sprite */
void AGG::GetTIL(const std::string & name, u16 index, u8 shape, Surface &surface)
{
    // check object
    if(NULL == AGG::fat[name].ptrTIL) PreloadObject(name);
    if(NULL == AGG::fat[name].ptrTIL) Error::Except("AGG::GetTIL: " + name +", empty ptr");

    // skip
    const u8 *vdata = reinterpret_cast<const u8 *>(AGG::fat[name].ptrTIL + TILEWIDTH * TILEWIDTH * index);

    // valid surface
    if(!surface.valid() || surface.w() != TILEWIDTH || surface.h() != TILEWIDTH){
	surface = Surface(TILEWIDTH, 8, SDL_SWSURFACE);
	surface.LoadPalette(AGG::GetPalette());
    }

    s16 x, y;
    u8 *pdst;

    // draw tiles
    surface.Lock();
    u8 *pixels = static_cast<u8 *>(const_cast<void *>(surface.pixels()));
    switch(shape % 4){

        // normal
	case 0:
	    surface.Lock();
	    memcpy(const_cast<void *>(surface.pixels()), vdata, TILEWIDTH * TILEWIDTH);
	    surface.Unlock();
            break;

        // vertical reflect
        case 1:
	{
	    pdst = pixels + TILEWIDTH * (TILEWIDTH - 1);
	    for(int i = 0; i < TILEWIDTH; i++){
    		memcpy(pdst, vdata, TILEWIDTH);
    		vdata += TILEWIDTH;
    		pdst -= TILEWIDTH;
	    }
	}
            break;

        // horizontal reflect
        case 2:
            for(y = 0; y < TILEWIDTH; ++y)
                for(x = TILEWIDTH - 1; x >= 0; --x){
		    pdst = pixels + y * TILEWIDTH + x;
		    *pdst = *vdata;
                    ++vdata;
                }
            break;

        // any variant
        case 3:
            for(y = TILEWIDTH - 1; y >= 0; --y)
                for( x = TILEWIDTH - 1; x >= 0; --x){
		    pdst = pixels + y * TILEWIDTH + x;
		    *pdst = *vdata;
                    ++vdata;
                }
            break;
    }
    surface.Unlock();
}

/* load ICN Sprite to map */
void AGG::LoadICN(const std::string & name)
{
    // check ICN
    if(AGG::fat[name].blockType != DATA_ICN) Error::Except("unknown icn: " + name);

    // object exists
    if(AGG::fat[name].vectorICN) return;

    Error::Verbose("AGG::LoadICN: " + name);

    // offset
    AGG::fd->seekg(AGG::fat[name].blockOffset, std::ios_base::beg);

    u32 pos, debug_pos1, debug_pos2;

    debug_pos1 = AGG::fd->tellg();

    u16 countSprite;
    u32 totalSize;

    AGG::fd->read(reinterpret_cast<char *>(&countSprite), sizeof(u16));
    SWAP16(countSprite);
    AGG::fd->read(reinterpret_cast<char *>(&totalSize), sizeof(u32));
    SWAP32(totalSize);

    pos = AGG::fd->tellg();

    std::vector<icnheader_t> vectorHeader;
    icnheader_t header;

    // read icn header
    for(int i = 0; i < countSprite; ++i){

	AGG::fd->read(reinterpret_cast<char *>(&header.offsetX), sizeof(s16));
	SWAP16(header.offsetX);
	AGG::fd->read(reinterpret_cast<char *>(&header.offsetY), sizeof(s16));
	SWAP16(header.offsetY);
	
	AGG::fd->read(reinterpret_cast<char *>(&header.width), sizeof(u16));
	SWAP16(header.width);
	AGG::fd->read(reinterpret_cast<char *>(&header.height), sizeof(u16));
	SWAP16(header.height);
	
	// skip char
	AGG::fd->ignore();
	AGG::fd->read(reinterpret_cast<char *>(&header.offsetData), sizeof(u32));
	SWAP32(header.offsetData);

	vectorHeader.push_back(header);
    }

    std::vector<icnheader_t>::iterator it = vectorHeader.begin();
    std::vector<icnheader_t>::iterator it_end = vectorHeader.end();
    u32 sizeData, newSize;
    AGG::fat[name].vectorICN = new std::vector<Sprite *>;

    // read icn data
    while(it != it_end){

	sizeData = (it + 1 != it_end ? (*(it + 1)).offsetData - (*it).offsetData : totalSize - (*it).offsetData);

	// offset
	AGG::fd->seekg(pos + (*it).offsetData, std::ios_base::beg);

	// read
	newSize = sizeData + 100;	// hmm, RLE ICN 90% decoding, and 0x80 it's RLE end
	char *buf = new char[newSize];
	std::memset(buf, 0x80, newSize);
	AGG::fd->read(buf, sizeData);

	AGG::fat[name].vectorICN->push_back(new Sprite((*it).width, (*it).height, (*it).offsetX, (*it).offsetY, sizeData, reinterpret_cast<const u8*>(buf)));

	delete [] buf;

	++it;
    }

    debug_pos2 = AGG::fd->tellg();

    // warning
    if(debug_pos2 != debug_pos1 + AGG::fat[name].blockSize) Error::Warning("incorrect size.");
}

/* free ICN object */
void AGG::FreeICN(aggfat_t & fat)
{
    // empty vector
    if(!fat.vectorICN) return;

    std::vector<Sprite *>::iterator it = fat.vectorICN->begin();
    std::vector<Sprite *>::iterator it_end = fat.vectorICN->end();

    // read icn data
    while(it != it_end){

	delete *it;

	++it;
    }

    delete fat.vectorICN;

    fat.vectorICN = NULL;
}

/* free TIL object */
void AGG::FreeTIL(aggfat_t & fat)
{
    // empty vector
    if(!fat.ptrTIL) return;

    delete [] fat.ptrTIL;

    fat.ptrTIL = NULL;
}

/* return Sprite */
const Sprite & AGG::GetICN(const std::string & name, u16 index)
{
    // check object
    if(!AGG::fat[name].vectorICN) PreloadObject(name);

    // empty vector
    if(AGG::fat[name].vectorICN->empty()) Error::Except("AGG::GetICN: " + name +", empty vector");

    // out of range
    if(AGG::fat[name].vectorICN->size() <= index){
	std::string sindex, range;
	String::AddInt(range, AGG::fat[name].vectorICN->size());
	String::AddInt(sindex, index);
	Error::Warning("AGG::GetICN: " + name + ", index(" + sindex + ") out of range(" + range + ").");
	index = AGG::fat[name].vectorICN->size() - 1;
    }

    return *(AGG::fat[name].vectorICN->at(index));
}

/* get color palette */
u32 AGG::GetColor(u8 index)
{
    // empty vector
    return AGG::palette[index];
}

/* load palette */
void AGG::LoadPalette(void)
{
    const std::string name("KB.PAL");

    Error::Verbose("Load palette: " + name);

    // check PAL
    if(AGG::fat[name].blockType != DATA_PAL) Error::Except("AGG::LoadPalette: unknown " + name);

    // offset
    AGG::fd->seekg(AGG::fat[name].blockOffset, std::ios_base::beg);

    Surface surface(1, 1, true);

    // read
    char r, g, b;

    for(int i = 0; i < AGGSIZEPALETTE; ++i){

	AGG::fd->read(&r, 1);
	AGG::fd->read(&g, 1);
	AGG::fd->read(&b, 1);

	r <<= 2;
	g <<= 2;
	b <<= 2;

	colors[i].r = r;
	colors[i].g = g;
	colors[i].b = b;
	
	AGG::palette[i] = surface.MapRGB(r, g, b);
    }

    shadowAlpha = surface.MapRGB(0, 0, 0, DEFAULT_SHADOW_ALPHA);
    colorKey = surface.MapRGB(0xFF, 0x00, 0xFF);
}

/* return palette 8bit */
const SDL_Color * AGG::GetPalette(void){ return colors; }

/* return shadow alpha */
u32 AGG::GetShadowAlpha(void){ return shadowAlpha; }

/* return color key */
u32 AGG::GetColorKey(void){ return colorKey; }

/* return letter sprite */
const Sprite & AGG::GetLetter(char ch, Font::type_t ft)
{
    if(ch < 0x21) Error::Warning("AGG::GetLetter: unknown letter");

    return Font::SMALL == ft ? AGG::GetICN("SMALFONT.ICN", ch - 0x20) : AGG::GetICN("FONT.ICN", ch - 0x20);
}
