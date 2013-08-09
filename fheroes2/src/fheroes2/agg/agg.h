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

#ifndef H2AGG_H
#define H2AGG_H

#include <string>
#include <list>
#include <vector>
#include <map>

#include "gamedefs.h"
#include "icn.h"
#include "til.h"
#include "xmi.h"
#include "m82.h"
#include "mus.h"
#include "sprite.h"
#include "font.h"

namespace AGG
{	
    class FAT
    {
    public:
	FAT() : crc(0), offset(0), size(0) {}

	u32 crc;
	u32 offset;
	u32 size;

	std::string Info(void) const;
    };

    class File
    {
    public:
	File();
	~File();

	bool Open(const std::string &);
	bool isGood(void) const;
	const std::string & Name(void) const;
	const FAT & Fat(const std::string & key);
	u16 CountItems(void);

	const std::vector<u8> & Read(const std::string & key);

    private:
	std::string filename;
	std::map<std::string, FAT> fat;
	u16 count_items;
	std::ifstream* stream;
	std::string key;
	std::vector<u8> body;
    };

    struct icn_cache_t
    {
	icn_cache_t() : sprites(NULL), reflect(NULL), count(0) {}

	Sprite*	sprites;
	Sprite*	reflect;
	u32	count;
    };

    struct til_cache_t
    {
	til_cache_t() : sprites(NULL),  count(0) {}

	Surface* sprites;
	u32	 count;
    };

    struct fnt_cache_t
    {
	Surface medium_white;
	Surface medium_yellow;
	Surface small_white;
	Surface small_yellow;
    };

    struct loop_sound_t
    {
	loop_sound_t(M82::m82_t w, int c) : sound(w), channel(c) {}
	bool isM82(const M82::m82_t wav) const{ return wav == sound; }

	M82::m82_t sound;
	int        channel;
    };

    class Cache
    {
    public:
	~Cache();

	static Cache & Get(void);

	bool ReadDataDir(void);

	int GetICNCount(const ICN::icn_t icn);
	Sprite GetICN(const ICN::icn_t icn, u32, bool reflect = false);
	Surface GetTIL(const TIL::til_t til, u32, u8 shape);
	const std::vector<u8> & GetWAV(const M82::m82_t m82);
	const std::vector<u8> & GetMID(const XMI::xmi_t xmi);
#ifdef WITH_TTF
	Surface GetFNT(u16, u8);
	const SDL::Font & GetMediumFont(void) const;
	const SDL::Font & GetSmallFont(void) const;
#endif
	void LoadLOOPXXSounds(const u16*);
	void ResetMixer(void);

	static void PreloadPalette(void);
	static void PreloadFonts(void);

    private:
	Cache();

	static bool CheckMemoryLimit(void);
	static u32  ClearFreeObjects(void);

	const std::vector<u8> & ReadICNChunk(const ICN::icn_t, const u32);
	const std::vector<u8> & ReadChunk(const std::string &);

	bool LoadExtICN(const ICN::icn_t, const u32, bool);
	bool LoadAltICN(const ICN::icn_t, const u32, bool);
	bool LoadOrgICN(Sprite &, const ICN::icn_t, const u32, bool);
	bool LoadOrgICN(const ICN::icn_t, const u32, bool);
	void LoadICN(const ICN::icn_t icn, u32, bool reflect = false);
	bool LoadAltTIL(const TIL::til_t, u32 max);
	bool LoadOrgTIL(const TIL::til_t, u32 max);
	void LoadTIL(const TIL::til_t);
	void LoadWAV(const M82::m82_t m82);
	void LoadMID(const XMI::xmi_t xmi);
	void LoadPAL(void);
	void LoadFNT(void);

	void SaveICN(const ICN::icn_t);

	File heroes2_agg;
	File heroes2x_agg;

	icn_cache_t* icn_cache;
	til_cache_t* til_cache;

	std::vector<loop_sound_t> loop_sounds;
	std::map<M82::m82_t, std::vector<u8> > wav_cache;
	std::map<XMI::xmi_t, std::vector<u8> > mid_cache;

#ifdef WITH_TTF
	void LoadFNT(u16);

	std::map<u16, fnt_cache_t> fnt_cache;
	SDL::Font font_medium;
	SDL::Font font_small;
#endif
    };

    int GetICNCount(const ICN::icn_t icn);

    Sprite	GetICN(const ICN::icn_t icn, const u32 index, bool reflect = false);
    Surface	GetTIL(const TIL::til_t til, const u32 index, const u8 shape);
    Surface	GetLetter(char ch, u8 ft);
#ifdef WITH_TTF
    Surface	GetUnicodeLetter(u16 ch, u8 ft);
#endif

    // wrapper Audio
    void PlaySound(const M82::m82_t m82);
    void PlayMusic(const MUS::mus_t mus, bool loop = true);
    void ResetMixer(void);
}

#endif
