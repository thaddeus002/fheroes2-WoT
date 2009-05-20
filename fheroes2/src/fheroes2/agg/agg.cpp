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
#include "engine.h"
#include "midi_xmi.h"
#include "midi_mid.h"
#include "dir.h"
#include "agg.h"
#include "xmlccwrap.h"

#define FATSIZENAME	15

char *basename(const char *path);

/*AGG::File constructor */
AGG::File::File(const std::string & fname) : filename(fname), count_items(0), stream(NULL)
{
    stream = new std::fstream(filename.c_str(), std::ios::in | std::ios::binary);

    if(!stream || stream->fail())
    {
	Error::Warning("AGG::File: error read file: " + filename + ", skipping...");

	return;
    }

    if(Settings::Get().Debug()) Error::Verbose("AGG::File: load: " + filename);

    {
        stream->read(reinterpret_cast<char *>(&count_items), sizeof(u16));
        SwapLE16(count_items);

        if(Settings::Get().Debug()) Error::Verbose("AGG::File: count items: ", count_items);

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
            SwapLE32(f.crc);

            stream->read(reinterpret_cast<char *>(&f.offset), sizeof(u32));
            SwapLE32(f.offset);

            stream->read(reinterpret_cast<char *>(&f.size), sizeof(u32));
            SwapLE32(f.size);
        }
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

    if(Settings::Get().Debug() > 3) f.Dump(key);
    
    if(f.size)
    {
	if(2 < Settings::Get().Debug()) Error::Verbose("AGG::File::Read: " + key);

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
AGG::Cache::Cache() : sprites_memory_size(0)
{
#ifdef WITH_TTF
    Settings & conf = Settings::Get();
    const std::string font1(conf.LocalDataPrefix() + SEPARATOR + "fonts" + SEPARATOR + conf.FontsNormal());
    const std::string font2(conf.LocalDataPrefix() + SEPARATOR + "fonts" + SEPARATOR + conf.FontsSmall());

    if(conf.Unicode())
    {
	if(!font_medium.Open(font1, conf.FontsNormalSize()) ||
	   !font_small.Open(font2, conf.FontsSmallSize())) conf.ResetModes(Settings::UNICODE);
    }
#endif
}

AGG::Cache::~Cache()
{
    // free agg cache
    std::list<File *>::const_iterator agg_it1 = agg_cache.begin();
    std::list<File *>::const_iterator agg_it2 = agg_cache.end();

    for(; agg_it1 != agg_it2; ++agg_it1) delete *agg_it1;
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
    std::string lower;
    bool heroes2_agg = false;
    std::list<File *>::const_iterator it1 = agg_cache.begin();
    std::list<File *>::const_iterator it2 = agg_cache.end();

    for(; it1 != it2; ++it1)
    {
	const File & agg_file = **it1;
	if(agg_file.Name() == fname)
	{
	    Error::Warning("AGG::Cache::AttachFile: already present: " + fname);
	    return false;
	}

	lower = agg_file.Name();
	String::Lower(lower);
	if(std::string::npos != lower.find("heroes2.agg")) heroes2_agg = true;
    }

    lower = fname;
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
	    agg_cache.push_back(file);
	else
	if(std::string::npos != lower.find("heroes2x.agg"))
	{
	    if(heroes2_agg)
		agg_cache.insert(--(agg_cache.end()), file);
	    else
		agg_cache.push_back(file);

	    Settings::Get().SetModes(Settings::PRICELOYALTY);
	}
	else
    	    agg_cache.push_front(file);
    }

    return true;
}

/* load manual ICN object */
void AGG::Cache::LoadExtraICN(const ICN::icn_t icn, bool reflect)
{
    std::vector<Sprite> & v = reflect ? reflect_icn_cache[icn] : icn_cache[icn];

    if(v.size()) return;

    if(Settings::Get().Debug()) Error::Verbose("AGG::Cache::LoadICN: " + ICN::GetString(icn));

    u8 count = 0;		// for animation sprite need update count for ICN::AnimationFrame

    // Preload ICN
    switch(icn)
    {
	case ICN::TELEPORT1:
	case ICN::TELEPORT2:
	case ICN::TELEPORT3: LoadICN(ICN::OBJNMUL2); count = 8; break;

	case ICN::FOUNTAIN:  LoadICN(ICN::OBJNMUL2); count = 2; break;

	case ICN::TREASURE:  LoadICN(ICN::OBJNRSRC); count = 2; break;

	case ICN::ROUTERED:  LoadICN(ICN::ROUTE); count = 145; break;

	case ICN::YELLOW_FONT:  LoadICN(ICN::FONT); count = 96; break;
	case ICN::YELLOW_SMALFONT: LoadICN(ICN::SMALFONT); count = 96; break;

	default: break;
    }

    v.resize(count);

    for(u8 ii = 0; ii < count; ++ii)
    {
	Sprite & sprite = v[ii];

	switch(icn)
	{
	    case ICN::TELEPORT1:
		sprite = GetICN(ICN::OBJNMUL2, 116);
		sprite.ChangeColor(sprite.GetColor(0xEE), sprite.GetColor(0xEE + ii / 2));
		break;

	    case ICN::TELEPORT2:
		sprite = GetICN(ICN::OBJNMUL2, 119);
		sprite.ChangeColor(sprite.GetColor(0xEE), sprite.GetColor(0xEE + ii));
		break;

	    case ICN::TELEPORT3:
		sprite = GetICN(ICN::OBJNMUL2, 122);
		sprite.ChangeColor(sprite.GetColor(0xEE), sprite.GetColor(0xEE + ii));
		break;

	    case ICN::FOUNTAIN:
		sprite = GetICN(ICN::OBJNMUL2, 15);
		sprite.ChangeColor(sprite.GetColor(0xE8), sprite.GetColor(0xE8 - ii));
		sprite.ChangeColor(sprite.GetColor(0xE9), sprite.GetColor(0xE9 - ii));
		sprite.ChangeColor(sprite.GetColor(0xEA), sprite.GetColor(0xEA - ii));
		sprite.ChangeColor(sprite.GetColor(0xEB), sprite.GetColor(0xEB - ii));
		sprite.ChangeColor(sprite.GetColor(0xEC), sprite.GetColor(0xEC - ii));
		sprite.ChangeColor(sprite.GetColor(0xED), sprite.GetColor(0xED - ii));
		sprite.ChangeColor(sprite.GetColor(0xEE), sprite.GetColor(0xEE - ii));
		sprite.ChangeColor(sprite.GetColor(0xEF), sprite.GetColor(0xEF - ii));
		break;

	    case ICN::TREASURE:
		sprite = GetICN(ICN::OBJNRSRC, 19);
		sprite.ChangeColor(sprite.GetColor(0x0A), sprite.GetColor(ii ? 0x00 : 0x0A));
		sprite.ChangeColor(sprite.GetColor(0xC2), sprite.GetColor(ii ? 0xD6 : 0xC2));
		sprite.ChangeColor(sprite.GetColor(0x64), sprite.GetColor(ii ? 0xDA : 0x64));
		break;

	    case ICN::ROUTERED:
		sprite = GetICN(ICN::ROUTE, ii);
		sprite.ChangeColor(sprite.GetColor(0x55), sprite.GetColor(0xB0));
		sprite.ChangeColor(sprite.GetColor(0x5C), sprite.GetColor(0xB7));
		sprite.ChangeColor(sprite.GetColor(0x60), sprite.GetColor(0xBB));
		break;

	    case ICN::YELLOW_FONT:
		sprite = GetICN(ICN::FONT, ii);
		sprite.ChangeColor(sprite.GetColor(0x0A), sprite.GetColor(0xDA));
		sprite.ChangeColor(sprite.GetColor(0x0B), sprite.GetColor(0xDA));
		sprite.ChangeColor(sprite.GetColor(0x0C), sprite.GetColor(0xDA));
		sprite.ChangeColor(sprite.GetColor(0x0D), sprite.GetColor(0xDA));
		sprite.ChangeColor(sprite.GetColor(0x0E), sprite.GetColor(0xDB));
		sprite.ChangeColor(sprite.GetColor(0x0F), sprite.GetColor(0xDB));
		sprite.ChangeColor(sprite.GetColor(0x10), sprite.GetColor(0xDB));
		sprite.ChangeColor(sprite.GetColor(0x11), sprite.GetColor(0xDB));
		sprite.ChangeColor(sprite.GetColor(0x12), sprite.GetColor(0xDB));
		sprite.ChangeColor(sprite.GetColor(0x13), sprite.GetColor(0xDB));
		sprite.ChangeColor(sprite.GetColor(0x14), sprite.GetColor(0xDB));
		break;

	    case ICN::YELLOW_SMALFONT:
		sprite = GetICN(ICN::SMALFONT, ii);
		sprite.ChangeColor(sprite.GetColor(0x0A), sprite.GetColor(0xDA));
		sprite.ChangeColor(sprite.GetColor(0x0B), sprite.GetColor(0xDA));
		sprite.ChangeColor(sprite.GetColor(0x0C), sprite.GetColor(0xDA));
		sprite.ChangeColor(sprite.GetColor(0x0D), sprite.GetColor(0xDA));
		sprite.ChangeColor(sprite.GetColor(0x0E), sprite.GetColor(0xDB));
		sprite.ChangeColor(sprite.GetColor(0x0F), sprite.GetColor(0xDB));
		sprite.ChangeColor(sprite.GetColor(0x10), sprite.GetColor(0xDB));
		sprite.ChangeColor(sprite.GetColor(0x11), sprite.GetColor(0xDB));
		sprite.ChangeColor(sprite.GetColor(0x12), sprite.GetColor(0xDB));
		sprite.ChangeColor(sprite.GetColor(0x13), sprite.GetColor(0xDB));
		sprite.ChangeColor(sprite.GetColor(0x14), sprite.GetColor(0xDB));
		break;

	    default: break;
	}
    }
}

/* load ICN object to AGG::Cache */
void AGG::Cache::LoadICN(const ICN::icn_t icn, bool reflect)
{
    std::vector<Sprite> & v = reflect ? reflect_icn_cache[icn] : icn_cache[icn];

    if(v.size()) return;
    const Settings & conf = Settings::Get();

    // load from image cache dir
    if(conf.Modes(Settings::USECACHE))
    {
	Dir dir;
	const std::string icn_folder(conf.LocalDataPrefix() + SEPARATOR + "cache" + SEPARATOR + ICN::GetString(icn));
	const std::string xml_spec(icn_folder + SEPARATOR + "spec.xml");

	if(conf.Debug()) Error::Verbose("AGG::Cache::LoadICN: " + icn_folder);

	// parse spec.xml
	TiXmlDocument doc;
	TiXmlElement* xml_icn = NULL;

	if(doc.LoadFile(xml_spec.c_str()) &&
	    NULL != (xml_icn = doc.FirstChildElement()) &&
	    0 == std::strcmp("icn", xml_icn->Value()))
	{
	    int count, ox, oy;
	    xml_icn->Attribute("count", &count);
	    v.reserve(count);

	    TiXmlElement *xml_sprite = xml_icn->FirstChildElement();
	    for(; xml_sprite; xml_sprite = xml_sprite->NextSiblingElement())
	    {
		xml_sprite->Attribute("ox", &ox);
		xml_sprite->Attribute("oy", &oy);
		const std::string sprite_file(icn_folder + SEPARATOR + xml_sprite->Attribute("name"));
    		v.push_back(Sprite());
		Sprite & sp = v.back();
		sp.Load(sprite_file.c_str());
		if(! sp.valid()) Error::Warning("AGG::Cache::LoadICN: broken image file: " + sprite_file);
		sp.SetOffset(ox, oy);
		sprites_memory_size += sp.GetSize();
	    }
	    if(2 < conf.Debug()) std::cout << "AGG::Cache::LoadICN: sprites count: " << count << ", total size: " << sprites_memory_size << std::endl;
	}
	else
	if(conf.Debug()) 
    	    Error::Verbose("AGG::Cache::LoadICN: broken xml file: " + xml_spec);
    }

    if(conf.Debug()) Error::Verbose("AGG::Cache::LoadICN: " + ICN::GetString(icn));

    // load from agg file
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

		count_sprite = ReadLE16(reinterpret_cast<const u8*>(&body[0]));
		total_size = ReadLE32(reinterpret_cast<const u8*>(&body[2]));

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

		    Sprite & sp = v[ii];
		    sp.Set(header.Width(), header.Height(), ICN::RequiresAlpha(icn));
		    sp.SetOffset(header.OffsetX(), header.OffsetY());
		    sp.LoadICN(&body[6 + header.OffsetData()], size_data, reflect);
		    sprites_memory_size += sp.GetSize();
		}
		if(2 < conf.Debug()) std::cout << "AGG::Cache::LoadICN: sprites count: " << count_sprite << ", total size: " << sprites_memory_size << std::endl;

		return;
	    }

	Error::Warning("AGG::Cache::LoadICN: not found: " + ICN::GetString(icn));
    }
}

/* load TIL object to AGG::Cache */
void AGG::Cache::LoadTIL(const TIL::til_t til)
{
    std::vector<Surface> & v = til_cache[til];

    if(v.size()) return;

    if(Settings::Get().Debug()) Error::Verbose("AGG::Cache::LoadTIL: " + TIL::GetString(til));

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

		count = ReadLE16(reinterpret_cast<const u8*>(&body.at(0)));
		width = ReadLE16(reinterpret_cast<const u8*>(&body.at(2)));
		height= ReadLE16(reinterpret_cast<const u8*>(&body.at(4)));

		const u32 tile_size = width * height;
		const u32 body_size = 6 + count * tile_size;

		// check size
		if(body.size() != body_size)
		{
		    Error::Warning("AGG::Cache::LoadTIL: size mismach, skipping...");

		    break;
		}

		v.resize(4 * count, NULL);
		for(u16 ii = 0; ii < count; ++ii)
		    v[ii] = Surface(&body[6 + ii * tile_size], width, height, 1, false);

		return;
	    }

	Error::Warning("AGG::Cache::LoadTIL: not found: " + TIL::GetString(til));
    }
}

/* load 82M object to AGG::Cache in Audio::CVT */
void AGG::Cache::LoadWAV(const M82::m82_t m82)
{
    std::vector<u8> & v = wav_cache[m82];

    if(v.size()) return;

    if(Settings::Get().Debug()) Error::Verbose("AGG::Cache::LoadWAV: " + M82::GetString(m82));

    if(! Mixer::isValid()) return;

    Audio::Spec wav_spec;
    wav_spec.format = 8;
    wav_spec.channels = 1;
    wav_spec.freq = 22050;

    const Audio::Spec & hardware = Audio::GetHardwareSpec();

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

    if(Settings::Get().Debug()) Error::Verbose("AGG::Cache::LoadMID: " + XMI::GetString(xmi));

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

void AGG::Cache::LoadPAL(void)
{
    if(Settings::Get().Debug()) Error::Verbose("AGG::Cache::LoadPAL: ", Palette::Get().Size());
}

void AGG::Cache::LoadMUS(const MUS::mus_t mus)
{
    std::vector<u8> & v = mus_cache[mus];
    const Settings & conf = Settings::Get();
    const std::string musname(conf.LocalDataPrefix() + SEPARATOR + "music" + SEPARATOR + MUS::GetString(mus));
    if(v.size()) return;

    if(conf.Debug()) Error::Verbose("AGG::Cache::LoadMUS: " + musname);

    if(! Mixer::isValid()) return;

    std::fstream stream(musname.c_str(), std::ios::in | std::ios::binary);

    if(stream.fail())
    {
	Error::Warning("AGG::Cache::LoadMUS: error read file: " + musname + ", skipping...");
	return;
    }

    stream.seekg(0, std::ios_base::end);
    const u32 size = stream.tellg();
    stream.seekg(0, std::ios_base::beg);
    v.resize(size);
    
    stream.read(reinterpret_cast<char *>(&v[0]), size);
    stream.close();
}

void AGG::Cache::LoadFNT(void)
{
    const Settings & conf = Settings::Get();

#ifdef WITH_TTF
    if(conf.Unicode())
    {
	if(fnt_cache.size()) return;

	const char *letters = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
	const u8 letters_size = std::strlen(letters);
        const Colors white = { 0xFF, 0xFF, 0xFF, 0x00 };

	u16 *unicode = new u16[letters_size + 1];

	String::UTF8_to_UNICODE(unicode, letters, letters_size);

	// small
	if(font_small.isValid())
	{
	    for(u8 ii = 0; ii < letters_size; ++ii)
		font_small.RenderUnicodeChar(fnt_cache[unicode[ii]].second, unicode[ii], white, conf.FontsRenderBlended() ? SDL::Font::BLENDED : SDL::Font::SOLID);
	}
	else
	    PreloadObject(ICN::SMALFONT);

	// medium
	if(font_medium.isValid())
	{
	    for(u8 ii = 0; ii < letters_size; ++ii)
		font_medium.RenderUnicodeChar(fnt_cache[unicode[ii]].first, unicode[ii], white, conf.FontsRenderBlended() ? SDL::Font::BLENDED : SDL::Font::SOLID);
	}
	else
	    PreloadObject(ICN::FONT);

	delete [] unicode;

	if(conf.Debug())
	{
	    if(fnt_cache.size())
	    {
    		Error::Verbose("AGG::LoadFonts: normal fonts " + conf.FontsNormal());
    	        Error::Verbose("AGG::LoadFonts:  small fonts " + conf.FontsSmall());
    		Error::Verbose("AGG::LoadFonts: preload english charsets");
	    }
	    else
		Error::Verbose("AGG::LoadFonts: internal.");
	}
    }
    else
#endif
    {
	PreloadObject(ICN::SMALFONT);
	PreloadObject(ICN::FONT);

	if(conf.Debug()) Error::Verbose("AGG::LoadFonts: internal.");
    }
}

void AGG::Cache::LoadFNT(u16 ch)
{
#ifdef WITH_TTF
    const Settings & conf = Settings::Get();

    if(conf.Unicode())
    {
	const Colors white = { 0xFF, 0xFF, 0xFF, 0x00 };

	// small
	font_small.RenderUnicodeChar(fnt_cache[ch].second, ch, white, conf.FontsRenderBlended() ? SDL::Font::BLENDED : SDL::Font::SOLID);
	// medium
	font_medium.RenderUnicodeChar(fnt_cache[ch].first, ch, white, conf.FontsRenderBlended() ? SDL::Font::BLENDED : SDL::Font::SOLID);

	if(conf.Debug()) Error::Verbose("AGG::LoadChar: ", static_cast<int>(ch));
    }
#endif
}

/* free ICN object in AGG::Cache */
void AGG::Cache::FreeICN(const ICN::icn_t icn, bool reflect)
{
    if(Settings::Get().Debug()) Error::Verbose("AGG::Cache::FreeICN: " + ICN::GetString(icn));

    std::vector<Sprite> & v = reflect ? reflect_icn_cache[icn] : icn_cache[icn];

    std::vector<Sprite>::const_iterator it1 = v.begin();
    std::vector<Sprite>::const_iterator it2 = v.end();
    for(; it1 != it2; ++it1) sprites_memory_size -= (*it1).GetSize();

    if(2 < Settings::Get().Debug())
	std::cout << "AGG::Cache::FreeICN: sprites count: " << v.size() << ", total size: " << sprites_memory_size << std::endl;

    if(v.size()) v.clear();
}

/* free TIL object in AGG::Cache */
void AGG::Cache::FreeTIL(const TIL::til_t til)
{
    if(Settings::Get().Debug()) Error::Verbose("AGG::Cache::FreeTIL: " + TIL::GetString(til));

    std::vector<Surface> & v = til_cache[til];

    if(v.size()) v.clear();
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
    const std::vector<Sprite> & v = reflect ? reflect_icn_cache[icn] : icn_cache[icn];

    if(0 == v.size())
    switch(icn)
    {
	case ICN::YELLOW_FONT:
	case ICN::YELLOW_SMALFONT:
	case ICN::ROUTERED:
	case ICN::TELEPORT1:
	case ICN::TELEPORT2:
	case ICN::TELEPORT3:
	case ICN::FOUNTAIN:
	case ICN::TREASURE:	LoadExtraICN(icn, reflect);	break;

	default:		LoadICN(icn, reflect);		break;
    }

    if(index >= v.size())
    {
	Error::Warning("AGG::GetICN: " + ICN::GetString(icn) + ", index out of range: ", index);

	index = 0;
    }

    const Sprite & sprite = v[index];
	
    if(! sprite.valid())
    {
	Error::Warning("AGG::GetICN: icn: ", icn);
	Error::Warning("AGG::GetICN: icn name: " + ICN::GetString(icn));
	Error::Warning("AGG::GetICN: index: ", index);
	Error::Except("AGG::GetICN: return is NULL");
    }

    return sprite;
}

/* return count of sprites from specific ICN */
int AGG::Cache::GetICNCount(const ICN::icn_t icn)
{
    const std::vector<Sprite> & v = icn_cache[icn];

    if(0 == v.size()) LoadICN(icn, false);

    return v.size();
}

/* return TIL surface from AGG::Cache */
const Surface & AGG::Cache::GetTIL(const TIL::til_t til, u16 index, u8 shape)
{
    std::vector<Surface> & v = til_cache[til];

    if(0 == v.size()) LoadTIL(til);

    u16 index2 = index;

    if(shape)
    {
	switch(til)
	{
	    case TIL::STON: index2 += 36 * (shape % 4); break;
	    case TIL::CLOF32: index2 += 4 * (shape % 4); break;
	    case TIL::GROUND32: index2 += 432 * (shape % 4); break;
	    default: break;
	}
    }

    if(index2 >= v.size())
    {
	Error::Warning("AGG::GetTIL: " + TIL::GetString(til) + ", index out of range: ", index);
	index2 = 0;
    }

    Surface & surface = v[index2];

    if(shape && ! surface.valid())
    {
	const Surface & src = v[index];

	if(src.valid())
	{
	    surface.Set(src.w(), src.h(), 8, SDL_SWSURFACE);
	    TIL::Reflect(surface, src, shape);
	}
    }

    if(! surface.valid())
    {
	Error::Warning("AGG::GetTIL: icn: ", til);
	Error::Warning("AGG::GetTIL: icn name: " + TIL::GetString(til));
	Error::Warning("AGG::GetTIL: index: ", index);
	Error::Except("AGG::GetTIL: return is NULL");
    }

    return surface;
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

#ifdef WITH_TTF
const SDL::Font & AGG::Cache::GetMediumFont(void) const
{
    return font_medium;
}

const SDL::Font & AGG::Cache::GetSmallFont(void) const
{
    return font_small;
}
#endif

bool AGG::Cache::isValidFonts(void) const
{
#ifdef WITH_TTF
    return Settings::Get().Unicode() ? font_small.isValid() && font_medium.isValid() : false;
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

const Surface & AGG::GetTIL(const TIL::til_t til, const u16 index, const u8 shape)
{
    return AGG::Cache::Get().GetTIL(til, index, shape);
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
    if(AGG::Cache::Get().isValidFonts())
    {
	const std::pair<Surface, Surface> & fonts = AGG::Cache::Get().GetFNT(ch);
	return (Font::SMALL | Font::YELLOW_SMALL) & ft ? fonts.second : fonts.first;
    }
    else
    return AGG::GetLetter(ch, ft);
}

const Surface & AGG::GetLetter(char ch, u8 ft)
{
    if(ch < 0x21) Error::Warning("AGG::GetLetter: unknown letter");

    switch(ft)
    {
	case Font::YELLOW_BIG:	return AGG::GetICN(ICN::YELLOW_FONT, ch - 0x20);
	case Font::YELLOW_SMALL:return AGG::GetICN(ICN::YELLOW_SMALFONT, ch - 0x20);
	case Font::BIG:		return AGG::GetICN(ICN::FONT, ch - 0x20);
	case Font::SMALL:	return AGG::GetICN(ICN::SMALFONT, ch - 0x20);

	default: break;
    }

    return AGG::GetICN(ICN::SMALFONT, ch - 0x20);
}
