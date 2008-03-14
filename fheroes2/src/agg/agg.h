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

#ifndef H2AGG_H
#define H2AGG_H

#include <string>
#include <vector>
#include <map>

#include "gamedefs.h"
#include "icn.h"
#include "til.h"
#include "pal.h"
#include "xmi.h"
#include "m82.h"
#include "palette.h"
#include "settings.h"
#include "text.h"

class Surface;
class Sprite;

namespace AGG
{	
    class FAT
    {
    public:
	FAT() : crc(0), offset(0), size(0) {};

	u32 crc;
	u32 offset;
	u32 size;

	void Dump(const std::string & n) const;
    };

    class File
    {
    public:
	File(const std::string & fname);
	~File();

	const std::string & Name(void) const;
	const FAT & Fat(const std::string & key);
	u16 CountItems(void);

	bool Read(const std::string & key, std::vector<char> & body);

	void Dump(void) const;

    private:
	const std::string filename;
	std::map<std::string, FAT> fat;
	u16 count_items;
	std::fstream * stream;
    };

    class Cache
    {
    public:
	~Cache();

	static Cache & Get(void);

	bool AttachFile(const std::string & fname);

	const Sprite & GetICN(const ICN::icn_t icn, u16 index, bool reflect = false);
	const Surface & GetTIL(const TIL::til_t til, u16 index);
	const Palette & GetPAL(void);
	const std::vector<u8> & GetWAV(const M82::m82_t m82);

	void LoadICN(const ICN::icn_t icn, bool reflect = false);
	void LoadTIL(const TIL::til_t til);
	void LoadWAV(const M82::m82_t m82);
	void LoadPAL(void);

	void FreeICN(const ICN::icn_t icn, bool reflect = false);
	void FreeTIL(const TIL::til_t til);
	void FreeWAV(const M82::m82_t m82);

    private:
	Cache();

	std::vector<File *> agg_cache;

	std::map<ICN::icn_t, std::vector<Sprite *> > icn_cache;
	std::map<ICN::icn_t, std::vector<Sprite *> > reflect_icn_cache;
	std::map<TIL::til_t, std::vector<Surface *> > til_cache;
	std::map<M82::m82_t, std::vector<u8> > wav_cache;

	Palette palette;

    };

    // wrapper AGG::PreloadObject
    void PreloadObject(const ICN::icn_t icn, bool reflect = false);
    void PreloadObject(const TIL::til_t til);

    // wrapper AGG::FreeObject
    void FreeObject(const ICN::icn_t icn, bool reflect = false);
    void FreeObject(const TIL::til_t til);

    // wrapper AGG::GetXXX
    const Sprite & GetICN(const ICN::icn_t icn, const u16 index, bool reflect = false);
    void GetTIL(const TIL::til_t til, const u16 index, const u8 shape, Surface & dst);
    const std::vector<u8> & GetWAV(const M82::m82_t m82);

    // wrapper AGG::GetColor
    u32 GetColor(const u16 index, const u8 flag = 0);

    const Sprite & GetLetter(char ch, Font::type_t ft);
    
    // wrapper Audio::Play
    void PlaySound(const M82::m82_t m82, const u8 volume = 10, bool loop = false);
};

#endif
