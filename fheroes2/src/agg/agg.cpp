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

#include <fstream>
#include <iostream>

#include "error.h"
#include "config.h"
#include "surface.h"
#include "sprite.h"
#include "agg.h"

#define FATSIZENAME	15

/*AGG::File constructor */
AGG::File::File(const std::string & fname) : filename(fname), count_items(0), stream(NULL)
{
    stream = new std::fstream(filename.c_str(), std::ios::in | std::ios::binary);

    if(!stream || stream->fail())
    {
	Error::Warning("AGG::File: error read file: " + filename + ", skipping...");

	return;
    }

    if(H2Config::Debug()) Error::Verbose("AGG::File: load: " + filename);

    stream->read(reinterpret_cast<char *>(&count_items), sizeof(u16));
    SWAP16(count_items);

    if(H2Config::Debug()) Error::Verbose("AGG::File: count items: ", count_items);

    char buf[FATSIZENAME + 1];
    buf[FATSIZENAME] = 0;

    for(u16 ii = 0; ii < count_items; ++ii)
    {
	const u32 pos = stream->tellg();

	stream->seekg(-FATSIZENAME * (count_items - ii), std::ios_base::end);
	stream->read(buf, FATSIZENAME);

	const std::string key(buf);

	FAT & f = fat[key];
		
	stream->seekg(pos, std::ios_base::beg);

	stream->read(reinterpret_cast<char *>(&f.crc), sizeof(u32));
	SWAP32(f.crc);

	stream->read(reinterpret_cast<char *>(&f.offset), sizeof(u32));
	SWAP32(f.offset);

	stream->read(reinterpret_cast<char *>(&f.size), sizeof(u32));
	SWAP32(f.size);
    }
}

AGG::File::~File()
{
    if(stream)
    {
	stream->close();
	
	delete stream;
    }
}

/* get AGG file name */
const std::string & AGG::File::Name(void) const
{
    return filename;
}

/* get FAT element */
const AGG::FAT & AGG::File::Fat(const std::string & key)
{
    return fat[key];
}

/* get count elements */
u16 AGG::File::CountItems(void)
{
    return count_items;
}

/* dump FAT */
void AGG::FAT::Dump(const std::string & n) const
{
    printf("%s\t%lX\t%lX\t%lX\n", n.c_str(), static_cast<long unsigned int>(crc), static_cast<long unsigned int>(offset), static_cast<long unsigned int>(size));
}

/* read element to body */
bool AGG::File::Read(const std::string & key, std::vector<char> & body)
{
    const FAT & f = fat[key];

    body.clear();

    if(H2Config::Debug() > 3) f.Dump(key);

    if(f.size)
    {
	if(2 < H2Config::Debug()) Error::Verbose("AGG::File::Read: " + key);

	body.resize(f.size);

	stream->seekg(f.offset, std::ios_base::beg);

	stream->read(& body.at(0), f.size);

	return true;
    }

    return false;
}

/* dump AGG file */
void AGG::File::Dump(void) const
{
    std::map<std::string, FAT>::const_iterator it1 = fat.begin();
    std::map<std::string, FAT>::const_iterator it2 = fat.end();

    for(; it1 != it2; ++it1)
    {
	const std::string & n = (*it1).first;
	const FAT & f = (*it1).second;

	f.Dump(n);
    }
}

/* AGG::Cache constructor */
AGG::Cache::Cache()
{
}

AGG::Cache::~Cache()
{
    // free agg cache
    std::vector<File *>::const_iterator agg_it1 = agg_cache.begin();
    std::vector<File *>::const_iterator agg_it2 = agg_cache.end();

    for(; agg_it1 != agg_it2; ++agg_it1) delete *agg_it1;


    // free icn cache
    std::map<ICN::icn_t, std::vector<Sprite *> >::const_iterator icn_it1 = icn_cache.begin();
    std::map<ICN::icn_t, std::vector<Sprite *> >::const_iterator icn_it2 = icn_cache.end();

    for(; icn_it1 != icn_it2; ++icn_it1)
    {
	const std::vector<Sprite *> & v = (*icn_it1).second;

	if(v.size())
	{
	    std::vector<Sprite *>::const_iterator it1 = v.begin();
	    std::vector<Sprite *>::const_iterator it2 = v.end();

	    for(; it1 != it2; ++it1) delete *it1;
	}
    }

    // free til cache
    std::map<TIL::til_t, std::vector<Surface *> >::const_iterator til_it1 = til_cache.begin();
    std::map<TIL::til_t, std::vector<Surface *> >::const_iterator til_it2 = til_cache.end();

    for(; til_it1 != til_it2; ++til_it1)
    {
	const std::vector<Surface *> & v = (*til_it1).second;
	    
	if(v.size())
	{
	    std::vector<Surface *>::const_iterator it1 = v.begin();
	    std::vector<Surface *>::const_iterator it2 = v.end();

	    for(; it1 != it2; ++it1) delete *it1;
	}
    }
}

/* get AGG::Cache object */
AGG::Cache & AGG::Cache::Get(void)
{
    static Cache agg_cache;

    return agg_cache;
}

/* attach AGG::File to AGG::Cache */
bool AGG::Cache::AttachFile(const std::string & fname)
{
    std::vector<File *>::const_iterator it1 = agg_cache.begin();
    std::vector<File *>::const_iterator it2 = agg_cache.end();

    for(; it1 != it2; ++it1)
    {
	const File & agg_file = **it1;

	if(agg_file.Name() == fname)
	{
	    Error::Warning("AGG::Cache::AttachFile: already present: " + fname);

	    break;
	}
    }

    AGG::File *file = new File(fname);
    
    if(file->CountItems())
    {
	agg_cache.push_back(file);

	return true;
    }

    delete file;

    return false;
}

/* load ICN object to AGG::Cache */
void AGG::Cache::LoadICN(const ICN::icn_t icn)
{
    std::vector<Sprite *> & v = icn_cache[icn];

    if(v.size()) return;

    if(H2Config::Debug()) Error::Verbose("AGG::Cache::LoadICN: " + ICN::GetString(icn));

    if(agg_cache.size())
    {
	std::vector<char> body;

	std::vector<File *>::const_iterator it1 = agg_cache.begin();
	std::vector<File *>::const_iterator it2 = agg_cache.end();

	for(; it1 != it2; ++it1)

	    // read only first found
	    if((**it1).Read(ICN::GetString(icn), body))
	    {
		u16 count_sprite;
		u32 total_size;

		LOAD16(&body[0], count_sprite);

		LOAD32(&body[2], total_size);

		v.resize(count_sprite);

		std::vector<ICN::Header> icn_headers(count_sprite);

		// read icn headers
		for(u16 ii = 0; ii < count_sprite; ++ii)
		    icn_headers[ii].Load(&body[6 + ii * ICN::Header::SizeOf()]);

		std::vector<ICN::Header>::const_iterator it1 = icn_headers.begin();
    		std::vector<ICN::Header>::const_iterator it2 = icn_headers.end();

		// read icn data
		for(u16 ii = 0; ii < count_sprite; ++ii)
		{
		    const ICN::Header & header = icn_headers[ii];

		    const u32 size_data = (ii + 1 != count_sprite ? icn_headers[ii + 1].OffsetData() - header.OffsetData() : total_size - header.OffsetData());

		    v[ii] = new Sprite(header, &body[6 + header.OffsetData()], size_data);
		}

		return;
	    }

	Error::Warning("AGG::Cache::LoadICN: not found: " + ICN::GetString(icn));
    }
}

/* load TIL object to AGG::Cache */
void AGG::Cache::LoadTIL(const TIL::til_t til)
{
    std::vector<Surface *> & v = til_cache[til];

    if(v.size()) return;

    if(H2Config::Debug()) Error::Verbose("AGG::Cache::LoadTIL: " + TIL::GetString(til));

    if(agg_cache.size())
    {
	std::vector<char> body;

	std::vector<File *>::const_iterator it1 = agg_cache.begin();
	std::vector<File *>::const_iterator it2 = agg_cache.end();

	for(; it1 != it2; ++it1)

	    // read only first found
	    if((**it1).Read(TIL::GetString(til), body))
	    {
		u16 count, width, height;

		LOAD16(&body.at(0), count);

		LOAD16(&body.at(2), width);

		LOAD16(&body.at(4), height);

		const u32 tile_size = width * height;
		const u32 body_size = 6 + count * tile_size;

		// check size
		if(body.size() != body_size)
		{
		    Error::Warning("AGG::Cache::LoadTIL: size mismach, skipping...");

		    break;
		}

		v.resize(count);
		
		for(u16 ii = 0; ii < count; ++ii)
		{
		    v[ii] = new Surface(width, height, 8, SDL_SWSURFACE);
		    
		    Surface & sf = *v[ii];

		    sf.LoadPalette(palette);

    		    sf.Lock();

            	    memcpy(const_cast<void *>(sf.pixels()), &body[6 + ii * tile_size], tile_size);

                    sf.Unlock();
		}

		return;
	    }

	Error::Warning("AGG::Cache::LoadTIL: not found: " + TIL::GetString(til));
    }
}

/* load PAL object to AGG::Cache */
void AGG::Cache::LoadPAL(void)
{
    const PAL::pal_t pal = PAL::KB;

    if(palette.Size()) return;

    if(H2Config::Debug()) Error::Verbose("AGG::Cache::LoadPAL: " + PAL::GetString(pal));

    if(agg_cache.size())
    {
	std::vector<char> body;

	std::vector<File *>::const_iterator it1 = agg_cache.begin();
	std::vector<File *>::const_iterator it2 = agg_cache.end();

	for(; it1 != it2; ++it1)

	    // read only first found
	    if((**it1).Read(PAL::GetString(pal), body))
	    {
		palette.Load(body);

		return;
	    }
    }
}

/* free ICN object in AGG::Cache */
void AGG::Cache::FreeICN(const ICN::icn_t icn)
{
    if(H2Config::Debug()) Error::Verbose("AGG::Cache::FreeICN: " + ICN::GetString(icn));

    std::vector<Sprite *> & v = icn_cache[icn];

    if(v.size())
    {
	std::vector<Sprite *>::const_iterator it1 = v.begin();
	std::vector<Sprite *>::const_iterator it2 = v.end();

	for(; it1 != it2; ++it1) delete *it1;

	v.clear();
    }
}

/* free TIL object in AGG::Cache */
void AGG::Cache::FreeTIL(const TIL::til_t til)
{
    if(H2Config::Debug()) Error::Verbose("AGG::Cache::FreeTIL: " + TIL::GetString(til));

    std::vector<Surface *> & v = til_cache[til];

    if(v.size())
    {
	std::vector<Surface *>::const_iterator it1 = v.begin();
	std::vector<Surface *>::const_iterator it2 = v.end();

	for(; it1 != it2; ++it1) delete *it1;

	v.clear();
    }
}

/* return ICN sprite from AGG::Cache */
const Sprite & AGG::Cache::GetICN(const ICN::icn_t icn, u16 index)
{
    const std::vector<Sprite *> & v = icn_cache[icn];

    if(0 == v.size()) LoadICN(icn);

    if(index >= v.size())
    {
	Error::Warning("AGG::GetICN: " + ICN::GetString(icn) + ", index out of range: ", index);

	index = 0;
    }

    const Sprite * sprite = v[index];
	
    if(NULL == sprite)
    {
	Error::Warning("AGG::GetICN: icn: ", icn);
	Error::Warning("AGG::GetICN: icn name: " + ICN::GetString(icn));
	Error::Warning("AGG::GetICN: index: ", index);
	Error::Except("AGG::GetICN: return is NULL");
    }

    return *sprite;
}

/* return TIL surface from AGG::Cache */
const Surface & AGG::Cache::GetTIL(const TIL::til_t til, u16 index)
{
    const std::vector<Surface *> & v = til_cache[til];

    if(0 == v.size()) LoadTIL(til);

    if(index >= v.size())
    {
	Error::Warning("AGG::GetTIL: " + TIL::GetString(til) + ", index out of range: ", index);

	index = 0;
    }

    const Surface * surface = v[index];

    if(NULL == surface)
    {
	Error::Warning("AGG::GetTIL: icn: ", til);
	Error::Warning("AGG::GetTIL: icn name: " + TIL::GetString(til));
	Error::Warning("AGG::GetTIL: index: ", index);
	Error::Except("AGG::GetTIL: return is NULL");
    }

    return *surface;
}

/* return Palette from AGG::Cache */
const Palette & AGG::Cache::GetPAL(void)
{
    return palette;
}

// wrapper AGG::PreloadObject
void AGG::PreloadObject(const ICN::icn_t icn)
{
    return AGG::Cache::Get().LoadICN(icn);
}

void AGG::PreloadObject(const TIL::til_t til)
{
    return AGG::Cache::Get().LoadTIL(til);
}

// wrapper AGG::FreeObject
void AGG::FreeObject(const ICN::icn_t icn)
{
    return AGG::Cache::Get().FreeICN(icn);
}

void AGG::FreeObject(const TIL::til_t til)
{
    return AGG::Cache::Get().FreeTIL(til);
}

// wrapper AGG::GetXXX
const Sprite & AGG::GetICN(const ICN::icn_t icn, const u16 index)
{
    return AGG::Cache::Get().GetICN(icn, index);
}

void AGG::GetTIL(const TIL::til_t til, const u16 index, const u8 shape, Surface & dst)
{
    const Surface & src = AGG::Cache::Get().GetTIL(til, index);

    TIL::Reflect(dst, src, shape);
}

// wrapper AGG::GetColor
u32 AGG::GetColor(const u16 index)
{
    return AGG::Cache::Get().GetPAL().Color(index);
}

/* return letter sprite */
const Sprite & AGG::GetLetter(char ch, Font::type_t ft)
{
    if(ch < 0x21) Error::Warning("AGG::GetLetter: unknown letter");
    
    return Font::SMALL == ft ? AGG::GetICN(ICN::SMALFONT, ch - 0x20) : AGG::GetICN(ICN::FONT, ch - 0x20);
}
