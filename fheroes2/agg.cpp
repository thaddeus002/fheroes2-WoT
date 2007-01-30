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

namespace AGG {
    static std::fstream                        *fd = NULL;
    static std::map<std::string, AGG::aggfat_t> fat;
    static std::vector<SDLmm::Color>            palette(AGGSIZEPALETTE);
};

/* AGG init */
void AGG::Init(const std::string & aggname)
{
    Error::Verbose("AGG data: " + aggname);
    
    AGG::fd = new std::fstream(aggname.c_str(), std::ios::in | std::ios::binary);
    if(! AGG::fd || ! *AGG::fd || AGG::fd->fail()) Error::Except("file not found.");
    
    u16 count;
    char buf[AGGSIZENAME];
    aggfat_t fat_element;

    AGG::fd->read(reinterpret_cast<char *>(&count), sizeof(count));

    for(u16 seg = 0; seg < count; ++seg){

	// name fat_element
	AGG::fd->seekg(-AGGSIZENAME * (count - seg), std::ios_base::end);
	AGG::fd->read(buf, AGGSIZENAME);
	std::string key_name(buf);

	// seek fat_element
	AGG::fd->seekg(sizeof(u16) + seg * (3 * sizeof(u32)), std::ios_base::beg);
	AGG::fd->seekg(sizeof(u32), std::ios_base::cur);

	// read fat_element
	AGG::fd->read(reinterpret_cast<char *>(&fat_element.blockOffset), sizeof(fat_element.blockOffset));
	AGG::fd->read(reinterpret_cast<char *>(&fat_element.blockSize), sizeof(fat_element.blockSize));
    
	// type data
	if(std::string::npos != key_name.find(".ICN")){
	    fat_element.blockType = DATA_ICN;
	    fat_element.vectorICN = NULL;
	}else if(std::string::npos != key_name.find(".82M"))
	    fat_element.blockType = DATA_WAV;
	else if(std::string::npos != key_name.find(".TIL"))
	    fat_element.blockType = DATA_TIL;
	else if(std::string::npos != key_name.find(".BMP"))
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
}

/* preload object to map */
void AGG::PreloadObject(const std::string & name)
{
    switch(AGG::fat[name].blockType){
    
	case DATA_ICN:
	    AGG::LoadICN(name);
	    break;

	case DATA_PAL:
	    break;

	default:
	    Error::Warning("unknown object: " + name);
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

	case DATA_PAL:
	    break;
	
	default:
	    break;
    }
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

    AGG::fd->read(reinterpret_cast<char *>(&countSprite), sizeof(countSprite));
    AGG::fd->read(reinterpret_cast<char *>(&totalSize), sizeof(totalSize));

    pos = AGG::fd->tellg();

    std::vector<icnheader_t> vectorHeader;
    icnheader_t header;

    // read icn header
    for(int i = 0; i < countSprite; ++i){

	AGG::fd->read(reinterpret_cast<char *>(&header.offsetX), sizeof(header.offsetX));
	AGG::fd->read(reinterpret_cast<char *>(&header.offsetY), sizeof(header.offsetY));
	
	AGG::fd->read(reinterpret_cast<char *>(&header.width), sizeof(header.width));
	AGG::fd->read(reinterpret_cast<char *>(&header.height), sizeof(header.height));
	
	// skip char
	AGG::fd->ignore();
	AGG::fd->read(reinterpret_cast<char *>(&header.offsetData), sizeof(header.offsetData));

	vectorHeader.push_back(header);
    }

    std::vector<icnheader_t>::iterator it = vectorHeader.begin();
    std::vector<icnheader_t>::iterator it_end = vectorHeader.end();
    u32 sizeData;
    AGG::fat[name].vectorICN = new std::vector<Sprite *>;

    // read icn data
    while(it != it_end){

	sizeData = (it + 1 != it_end ? (*(it + 1)).offsetData - (*it).offsetData : totalSize - (*it).offsetData);

	// offset
	AGG::fd->seekg(pos + (*it).offsetData, std::ios_base::beg);

	// read
	char *buf = new char[sizeData + 100];
	std::memset(buf, 0x80, sizeData + 100);
	AGG::fd->read(buf, sizeData);
	std::vector<unsigned char> vdata(buf, &buf[sizeData-1]);
	delete [] buf;

	if((*it).offsetX < 0 || (*it).offsetY < 0) Error::Warning("AGG: sprite " + name + ": x,y < 0");
	AGG::fat[name].vectorICN->push_back(new Sprite((*it).width, (*it).height, (*it).offsetX, (*it).offsetY, vdata));

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

    // SDLmm
    SDLmm::Surface surface(SDLmm::Surface::CreateSurface(SDL_SWSURFACE|SDL_SRCALPHA, 1, 1, DEFAULT_DEPTH, RMASK, GMASK, BMASK, AMASK));
    SDLmm::PixelFormat pixelFormat = surface.GetPixelFormat();

    // read
    char r, g, b;

    for(int i = 0; i < AGGSIZEPALETTE; ++i){

	AGG::fd->read(&r, 1);
	AGG::fd->read(&g, 1);
	AGG::fd->read(&b, 1);

	switch(i){
	
	    case INDEX_SHADOW_ALPHA:
		AGG::palette[i] = pixelFormat.MapRGBA(0, 0, 0, DEFAULT_SHADOW_ALPHA);
		break;

	    case INDEX_COLOR_KEY:
		r = 0xFF;
		g = 0x00;
		b = 0xFF;
		AGG::palette[i] = pixelFormat.MapRGB(r, g, b);
		break;
	
	    default:
		r <<= 2;
		g <<= 2;
		b <<= 2;
		AGG::palette[i] = pixelFormat.MapRGB(r, g, b);
		break;
	}
    }
}
