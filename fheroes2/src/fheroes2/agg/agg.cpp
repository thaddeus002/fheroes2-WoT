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

#include "settings.h"
#include "text.h"
#include "audio.h"
#include "midi_xmi.h"
#include "midi_mid.h"
#include "agg.h"

#define FATSIZENAME	15

char *basename(const char *path);

/*AGG::File constructor */
AGG::File::File(const std::string & fname, bool isGameFile) : filename(fname), count_items(0), stream(NULL)
{
    stream = new std::fstream(filename.c_str(), std::ios::in | std::ios::binary);

    if(!stream || stream->fail())
    {
	Error::Warning("AGG::File: error read file: " + filename + ", skipping...");

	return;
    }

    if(H2Config::Debug()) Error::Verbose("AGG::File: load: " + filename);

    if(isGameFile)
    {
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
    else
    {
        FAT &f = fat[basename(fname.c_str())];
        count_items = 1;
        stream->seekg(0, std::ios_base::end);
        f.size = stream->tellg();
        f.offset = 0;
        f.crc = 0;
        stream->seekg(0, std::ios_base::beg);
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
AGG::Cache::Cache() : heroes2_agg(false)
{
#ifdef WITH_TTF
    const Settings & conf = Settings::Get();

    font_medium.Open(conf.FontName(), 15);
    font_small.Open(conf.FontName(), 10);
#endif
}

AGG::Cache::~Cache()
{
    // free agg cache
    std::list<File *>::const_iterator agg_it1 = agg_cache.begin();
    std::list<File *>::const_iterator agg_it2 = agg_cache.end();

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

    // free icn reflect cache
    std::map<ICN::icn_t, std::vector<Sprite *> >::const_iterator reflect_icn_it1 = reflect_icn_cache.begin();
    std::map<ICN::icn_t, std::vector<Sprite *> >::const_iterator reflect_icn_it2 = reflect_icn_cache.end();

    for(; reflect_icn_it1 != reflect_icn_it2; ++reflect_icn_it1)
    {
	const std::vector<Sprite *> & v = (*reflect_icn_it1).second;

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
    std::list<File *>::const_iterator it1 = agg_cache.begin();
    std::list<File *>::const_iterator it2 = agg_cache.end();

    //if(it2 != std::find_if(it1, it2, ))
    for(; it1 != it2; ++it1)
    {
	const File & agg_file = **it1;

	if(agg_file.Name() == fname)
	{
	    Error::Warning("AGG::Cache::AttachFile: already present: " + fname);

	    break;
	}
    }

    std::string lower(fname);
    String::Lower(lower);

    if(std::string::npos != lower.find(".agg"))
    {
	AGG::File *file = new File(fname);
    
	if(! file->CountItems())
	{
	    delete file;
	    return false;
	}

	if(std::string::npos != lower.find("heroes2.agg"))
	{
	    agg_cache.push_back(file);
	    heroes2_agg = true;
	}
	else
	if(std::string::npos != lower.find("heroes2x.agg"))
	{
	    if(heroes2_agg)
	    {
		agg_cache.insert(--(agg_cache.end()), file);
	    }
	    else
		agg_cache.push_back(file);

	    Settings::Get().SetModes(Settings::PRICELOYALTY);
	}
	else
    	    agg_cache.push_front(file);
    }
    else
    {
	AGG::File *file = new File(fname, false);
	agg_cache.push_front(file);
    }

    return true;
}

/* load manual ICN object */
void AGG::Cache::LoadExtraICN(const ICN::icn_t icn, bool reflect)
{
    std::vector<Sprite *> & v = reflect ? reflect_icn_cache[icn] : icn_cache[icn];

    if(v.size()) return;

    if(H2Config::Debug()) Error::Verbose("AGG::Cache::LoadICN: " + ICN::GetString(icn));

    Sprite *sprite = NULL;
    u8 count = 0;		// for animation sprite need update count for ICN::AnimationFrame

    // Preload ICN
    switch(icn)
    {
	case ICN::TELEPORT1:
	case ICN::TELEPORT2:
	case ICN::TELEPORT3: LoadICN(ICN::OBJNMUL2); count = 2; break;

	case ICN::FOUNTAIN:  LoadICN(ICN::OBJNMUL2); count = 2; break;

	case ICN::ROUTERED:  LoadICN(ICN::ROUTE); count = 144; break;

	default: break;
    }

    v.resize(count);

    for(u8 ii = 0; ii < count; ++ii)
    {
	switch(icn)
	{
	    case ICN::TELEPORT1:
		sprite = new Sprite(GetICN(ICN::OBJNMUL2, 116));
		sprite->Lock();
		sprite->ChangeColor(palette.Color(0xEE), palette.Color(0xEE + ii));
		sprite->Unlock();
		break;

	    case ICN::TELEPORT2:
		sprite = new Sprite(GetICN(ICN::OBJNMUL2, 119));
		sprite->Lock();
		sprite->ChangeColor(palette.Color(0xEE), palette.Color(0xEE + ii));
		sprite->Unlock();
		break;

	    case ICN::TELEPORT3:
		sprite = new Sprite(GetICN(ICN::OBJNMUL2, 122));
		sprite->Lock();
		sprite->ChangeColor(palette.Color(0xEE), palette.Color(0xEE + ii));
		sprite->Unlock();
		break;

	    case ICN::FOUNTAIN:
		sprite = new Sprite(GetICN(ICN::OBJNMUL2, 15));
		sprite->Lock();
		sprite->ChangeColor(palette.Color(0xE8), palette.Color(0xE8 - ii));
		sprite->ChangeColor(palette.Color(0xE9), palette.Color(0xE9 - ii));
		sprite->ChangeColor(palette.Color(0xEA), palette.Color(0xEA - ii));
		sprite->ChangeColor(palette.Color(0xEB), palette.Color(0xEB - ii));
		sprite->ChangeColor(palette.Color(0xEC), palette.Color(0xEC - ii));
		sprite->ChangeColor(palette.Color(0xED), palette.Color(0xED - ii));
		sprite->ChangeColor(palette.Color(0xEE), palette.Color(0xEE - ii));
		sprite->ChangeColor(palette.Color(0xEF), palette.Color(0xEF - ii));
		sprite->Unlock();
		break;

	    case ICN::ROUTERED:
		sprite = new Sprite(GetICN(ICN::ROUTE, ii));
		sprite->Lock();
		sprite->ChangeColor(palette.Color(0x55), palette.Color(0xB0));
		sprite->ChangeColor(palette.Color(0x5C), palette.Color(0xB7));
		sprite->ChangeColor(palette.Color(0x60), palette.Color(0xBB));
		sprite->Unlock();
		break;

	    default: break;
	}

	if(sprite) v[ii] = sprite;
    }
}

/* load ICN object to AGG::Cache */
void AGG::Cache::LoadICN(const ICN::icn_t icn, bool reflect)
{
    std::vector<Sprite *> & v = reflect ? reflect_icn_cache[icn] : icn_cache[icn];

    if(v.size()) return;

    if(H2Config::Debug()) Error::Verbose("AGG::Cache::LoadICN: " + ICN::GetString(icn));

    if(agg_cache.size())
    {
	std::vector<char> body;

	std::list<File *>::const_iterator it1 = agg_cache.begin();
	std::list<File *>::const_iterator it2 = agg_cache.end();

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

		    v[ii] = new Sprite(header, &body[6 + header.OffsetData()], size_data, reflect);
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

	std::list<File *>::const_iterator it1 = agg_cache.begin();
	std::list<File *>::const_iterator it2 = agg_cache.end();

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

	std::list<File *>::const_iterator it1 = agg_cache.begin();
	std::list<File *>::const_iterator it2 = agg_cache.end();

	for(; it1 != it2; ++it1)

	    // read only first found
	    if((**it1).Read(PAL::GetString(pal), body))
	    {
		palette.Load(body);

		return;
	    }
    }
}

/* load 82M object to AGG::Cache in Audio::CVT */
void AGG::Cache::LoadWAV(const M82::m82_t m82)
{
    std::vector<u8> & v = wav_cache[m82];

    if(v.size()) return;

    if(H2Config::Debug()) Error::Verbose("AGG::Cache::LoadWAV: " + M82::GetString(m82));

    if(! Mixer::isValid()) return;

    Audio::Spec wav_spec;
    wav_spec.format = 8;
    wav_spec.channels = 1;
    wav_spec.freq = 22050;

    const Audio::Spec & hardware = Mixer::HardwareSpec();

    Audio::CVT cvt;

    if(agg_cache.size() && cvt.Build(wav_spec, hardware))
    {
	std::vector<char> body;

	std::list<File *>::const_iterator it1 = agg_cache.begin();
	std::list<File *>::const_iterator it2 = agg_cache.end();

	for(; it1 != it2; ++it1)
	    if((**it1).Read(M82::GetString(m82), body))
	    {
		    const u32 size = cvt.len_mult * body.size();

		    cvt.buf = new u8[size];
		    cvt.len = body.size();

		    memcpy(cvt.buf, &body[0], body.size());

		    cvt.Convert();

		    v.assign(cvt.buf, cvt.buf + size - 1);

		    delete [] cvt.buf;
		    cvt.buf = NULL;

		    break;
	    }
    }
}

/* load XMI object to AGG::Cache */
void AGG::Cache::LoadMID(const XMI::xmi_t xmi)
{
    std::vector<u8> & v = mid_cache[xmi];

    if(v.size()) return;

    if(H2Config::Debug()) Error::Verbose("AGG::Cache::LoadMID: " + XMI::GetString(xmi));

    if(! Mixer::isValid()) return;

    if(agg_cache.size())
    {
	std::vector<char> body;

	std::list<File *>::const_iterator it1 = agg_cache.begin();
	std::list<File *>::const_iterator it2 = agg_cache.end();

	for(; it1 != it2; ++it1)

	    // read only first found
	    if((**it1).Read(XMI::GetString(xmi), body))
	    {
		MIDI::Xmi x;
		MIDI::Mid m;
		MIDI::MTrk track;

		x.Read(body);
		track.ImportXmiEVNT(x.EVNT());

		m.AddTrack(track);
		m.SetPPQN(64);

		m.Write(reinterpret_cast<std::vector<char> &>(v));

		break;
	    }
    }
}

void AGG::Cache::LoadMUS(const MUS::mus_t mus)
{
    std::vector<u8> & v = mus_cache[mus];
    
    if(v.size()) return;

    if(H2Config::Debug()) Error::Verbose("AGG::Cache::LoadMUS: " + MUS::GetString(mus));

    if(! Mixer::isValid()) return;

    if(agg_cache.size())
    {
	std::vector<char> body;

	std::list<File *>::const_iterator it1 = agg_cache.begin();
	std::list<File *>::const_iterator it2 = agg_cache.end();

	for(; it1 != it2; ++it1)
	    // read only first found
	    if((**it1).Read(MUS::GetString(mus), reinterpret_cast<std::vector<char> &>(v))) break;
    }
}

void AGG::Cache::LoadFNT(void)
{
    const Settings & conf = Settings::Get();

#ifdef WITH_TTF
    if(fnt_cache.size()) return;

    const char *letters = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"; // only for test development
    const u8 letters_size = std::strlen(letters);
    const Colors white = { 0xFF, 0xFF, 0xFF, 0x00 };

    u16 *unicode = new u16[letters_size + 1];

    String::UTF8_to_UNICODE(unicode, letters, letters_size);

    // small
    if(font_small.isValid())
    {
	for(u8 ii = 0; ii < letters_size; ++ii)
	    font_small.RenderUnicodeChar(fnt_cache[unicode[ii]].second, unicode[ii], white);
    }
    else
	PreloadObject(ICN::SMALFONT);

    // medium
    if(font_medium.isValid())
    {
	for(u8 ii = 0; ii < letters_size; ++ii)
	    font_medium.RenderUnicodeChar(fnt_cache[unicode[ii]].first, unicode[ii], white);
    }
    else
	PreloadObject(ICN::FONT);

    delete [] unicode;

    if(conf.Debug()) fnt_cache.size() ? Error::Verbose("AGG::LoadFonts: preload charsets from " + conf.FontName()) : Error::Verbose("AGG::LoadFonts: internal.");
#else
    PreloadObject(ICN::SMALFONT);
    PreloadObject(ICN::FONT);

    if(conf.Debug()) Error::Verbose("AGG::LoadFonts: internal.");
#endif
}

void AGG::Cache::LoadFNT(u16 ch)
{
#ifdef WITH_TTF
    const Settings & conf = Settings::Get();
    const Colors white = { 0xFF, 0xFF, 0xFF, 0x00 };

    // small
    if(font_small.isValid()) font_small.RenderUnicodeChar(fnt_cache[ch].second, ch, white);
    // medium
    if(font_medium.isValid()) font_medium.RenderUnicodeChar(fnt_cache[ch].first, ch, white);

    if(conf.Debug()) Error::Verbose("AGG::LoadChar: ", static_cast<int>(ch));
#endif
}

/* free ICN object in AGG::Cache */
void AGG::Cache::FreeICN(const ICN::icn_t icn, bool reflect)
{
    if(H2Config::Debug()) Error::Verbose("AGG::Cache::FreeICN: " + ICN::GetString(icn));

    std::vector<Sprite *> & v = reflect ? reflect_icn_cache[icn] : icn_cache[icn];

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

/* free 82M object in AGG::Cache */
void AGG::Cache::FreeWAV(const M82::m82_t m82)
{
    std::vector<u8> & v = wav_cache[m82];

    if(v.size()) v.clear();
}

/* free music object in AGG::Cache */
void AGG::Cache::FreeMUS(const MUS::mus_t mus)
{
    std::vector<u8> & v = mus_cache[mus];

    if(v.size()) v.clear();
}

/* free XMI object in AGG::Cache */
void AGG::Cache::FreeMID(const XMI::xmi_t xmi)
{
    std::vector<u8> & v = mid_cache[xmi];

    if(v.size()) v.clear();
}

/* return ICN sprite from AGG::Cache */
const Sprite & AGG::Cache::GetICN(const ICN::icn_t icn, u16 index, bool reflect)
{
    const std::vector<Sprite *> & v = reflect ? reflect_icn_cache[icn] : icn_cache[icn];

    if(0 == v.size())
    switch(icn)
    {
	case ICN::ROUTERED:
	case ICN::TELEPORT1:
	case ICN::TELEPORT2:
	case ICN::TELEPORT3:
	case ICN::FOUNTAIN:	LoadExtraICN(icn, reflect);	break;

	default:		LoadICN(icn, reflect);		break;
    }

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

/* return count of sprites from specific ICN */
int AGG::Cache::GetICNCount(const ICN::icn_t icn)
{
    const std::vector<Sprite *> & v = icn_cache[icn];

    if(0 == v.size()) LoadICN(icn, false);

    return v.size();
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

/* return CVT from AGG::Cache */
const std::vector<u8> & AGG::Cache::GetWAV(const M82::m82_t m82)
{
    const std::vector<u8> & v = wav_cache[m82];

    if(v.empty()) LoadWAV(m82);

    return v;
}

/* return MID from AGG::Cache */
const std::vector<u8> & AGG::Cache::GetMID(const XMI::xmi_t xmi)
{
    const std::vector<u8> & v = mid_cache[xmi];

    if(v.empty()) LoadMID(xmi);

    return v;
}

/* return CVT from AGG::Cache */
const std::vector<u8> & AGG::Cache::GetMUS(const MUS::mus_t mus)
{
    const std::vector<u8> & v = mus_cache[mus];

    if(v.empty()) LoadMUS(mus);

    return v;
}

/* return FNT cache */
const std::pair<Surface, Surface> & AGG::Cache::GetFNT(u16 c)
{
    if(!fnt_cache[c].first.valid()) LoadFNT(c);

    return fnt_cache[c];
}

bool AGG::Cache::isValidFonts(void) const
{
#ifdef WITH_TTF
    return font_small.isValid() && font_medium.isValid();
#endif
    return false;
}

// wrapper AGG::PreloadObject
void AGG::PreloadObject(const ICN::icn_t icn, bool reflect)
{
    return AGG::Cache::Get().LoadICN(icn, reflect);
}

void AGG::PreloadObject(const TIL::til_t til)
{
    return AGG::Cache::Get().LoadTIL(til);
}

// wrapper AGG::FreeObject
void AGG::FreeObject(const ICN::icn_t icn, bool reflect)
{
    return AGG::Cache::Get().FreeICN(icn, reflect);
}

void AGG::FreeObject(const TIL::til_t til)
{
    return AGG::Cache::Get().FreeTIL(til);
}

// wrapper AGG::GetXXX
int AGG::GetICNCount(const ICN::icn_t icn)
{
    return AGG::Cache::Get().GetICNCount(icn);
}

const Sprite & AGG::GetICN(const ICN::icn_t icn, const u16 index, bool reflect)
{
    return AGG::Cache::Get().GetICN(icn, index, reflect);
}

void AGG::GetTIL(const TIL::til_t til, const u16 index, const u8 shape, Surface & dst)
{
    const Surface & src = AGG::Cache::Get().GetTIL(til, index);

    TIL::Reflect(dst, src, shape);
}

const std::vector<u8> & AGG::GetWAV(const M82::m82_t m82)
{
    return AGG::Cache::Get().GetWAV(m82);
}

const std::vector<u8> & AGG::GetMUS(const MUS::mus_t mus)
{
    return AGG::Cache::Get().GetMUS(mus);
}

const std::vector<u8> & AGG::GetMID(const XMI::xmi_t xmi)
{
    return AGG::Cache::Get().GetMID(xmi);
}

// wrapper AGG::GetColor
u32 AGG::GetColor(const u16 index)
{
//    if(Sprite::GREEN2RED & flag) return AGG::Cache::Get().GetPAL().Color(index + 0x5C);
//    else
//    if(Sprite::WHITE2YELLOW & flag) return AGG::Cache::Get().GetPAL().Color(index + 0xC0);
//    else
    return AGG::Cache::Get().GetPAL().Color(index);
}

/* wrapper Audio::Play */
void AGG::PlaySound(const M82::m82_t m82)
{
    const Settings & conf = Settings::Get();

    if(conf.Sound())
    {
	if(conf.Debug()) Error::Verbose("AGG::PlaySound: " + M82::GetString(m82));

	Mixer::PlayRAW(AGG::Cache::Get().GetWAV(m82));
    }
}

/* wrapper Audio::Play */
void AGG::PlayMusic(const MUS::mus_t mus, bool loop)
{
    const Settings & conf = Settings::Get();

    if(!conf.Music()) return;


    if(conf.Modes(Settings::MUSIC_EXT))
    {
	if(MUS::UNUSED != mus && MUS::UNKNOWN != mus) Music::Play(AGG::Cache::Get().GetMUS(mus), loop);
	if(conf.Debug()) Error::Verbose("AGG::PlayMusic: " + MUS::GetString(mus));
    }
    else
    if(conf.Modes(Settings::MUSIC_CD) && Cdrom::isValid())
    {
	if(MUS::UNUSED != mus && MUS::UNKNOWN != mus) Cdrom::Play(mus, loop);
	if(conf.Debug()) Error::Verbose("AGG::PlayMusic: cd track ", mus);
    }
    else
    {
	XMI::xmi_t xmi = XMI::FromMUS(mus);
	if(XMI::UNKNOWN != xmi) Music::Play(AGG::Cache::Get().GetMID(xmi), loop);
        else Music::Reset();
	if(conf.Debug()) Error::Verbose("AGG::PlayMusic: " + XMI::GetString(xmi));
    }
}

/* return letter sprite */
const Surface & AGG::GetUnicodeLetter(u16 ch, u8 ft)
{
#ifdef WITH_TTF
    if(AGG::Cache::Get().isValidFonts())
    {
	const std::pair<Surface, Surface> & fonts = AGG::Cache::Get().GetFNT(ch);
	return Font::SMALL == ft ? fonts.second : fonts.first;
    }
    else
#endif
    return Font::SMALL == ft ? AGG::GetICN(ICN::SMALFONT, ch - 0x20) : AGG::GetICN(ICN::FONT, ch - 0x20);

}

const Surface & AGG::GetLetter(char ch, u8 ft)
{
    if(ch < 0x21) Error::Warning("AGG::GetLetter: unknown letter");
    
    return Font::SMALL == ft ? AGG::GetICN(ICN::SMALFONT, ch - 0x20) : AGG::GetICN(ICN::FONT, ch - 0x20);
}
