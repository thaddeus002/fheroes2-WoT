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
#ifndef H2AGG_H
#define H2AGG_H

#include <map>
#include <vector>
#include <fstream>
#include <string>

#include <cstdlib>
#include <iostream>

#include "error.h"
#include "tools.h"
#include "sprite.h"
#include "gamedefs.h"

#define AGGSIZENAME     15
#define AGGSIZEPALETTE	256

namespace AGG {

    typedef enum { DATA_UNK, DATA_ICN, DATA_WAV, DATA_TIL, DATA_BMP, DATA_XMI, DATA_BIN, DATA_PAL, DATA_FNT } aggtype_t;
                         
    typedef struct {
	s16         offsetX;
	s16         offsetY;
	u16         width;
	u16         height;
	u32         offsetData;
    } icnheader_t;

    typedef struct {
	u32	blockOffset;
	u32	blockSize;
	aggtype_t blockType;
	union {
	    std::vector<Sprite *> *vectorICN;
	    char *ptrTIL;
	};
    } aggfat_t;

    void Init(const std::string & aggname);
    void Quit(void);

    void PreloadObject(const std::string & name);
    void FreeObject(const std::string & name);
    void FreeObject(aggfat_t & fat);

    void LoadPalette(void);
    u32 GetColor(u8 index);
    u32 GetShadowAlpha(void);
    u32 GetColorKey(void);

    void LoadICN(const std::string & name);
    void LoadTIL(const std::string & name);

    Sprite * GetTIL(const std::string & name, u16 index, u8 shape);
    const Sprite & GetICN(const std::string & name, u16 index);

    void FreeICN(aggfat_t & fat);
    void FreeTIL(aggfat_t & fat);
};

#endif
