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

#include <vector>
#include "agg.h"
#include "config.h"
#include "error.h"
#include "sprite.h"

/* ICN Sprite constructor */
Sprite::Sprite(u16 w, u16 h, s16 ox, s16 oy, u32 size, const u8 *data) 
    : Surface(w, h, true), offsetX(ox), offsetY(oy)
{
    Fill(AGG::GetColorKey());
    DrawICN(size, data);
    SetColorKey(AGG::GetColorKey());
    SetDisplayFormat();
};

/* draw RLE ICN to surface */
void Sprite::DrawICN(u32 size, const u8 *vdata)
{
    u8 i, count;
    u16 x = 0;
    u16 y = 0;
    u32 index = 0;

    //if(H2Config::RLEDebug()) std::cerr << "START RLE DEBUG" << std::endl;

    // lock surface
    Lock();

    while(index < size){

	// 0x00 - end line
	if(0 == vdata[index]){
	    ++y;
	    x = 0;
	    //if(H2Config::RLEDebug()) fprintf(stderr, " M:%hhX\n", vdata[index]);
	    ++index;
	    continue;

	// range 0x01..0x7F XX
	}else if(0x80 > vdata[index]){
	    //if(H2Config::RLEDebug()) fprintf(stderr, " M:%hhX C:%d:D", vdata[index], vdata[index]);
	    count = vdata[index];
	    ++index;
	    i = 0;
	    while(i++ < count && index < size){
		SetPixel2(x++, y, AGG::GetColor(vdata[index++]));
		//if(H2Config::RLEDebug()) fprintf(stderr, ":%hhX", vdata[index]);
	    }
	    continue;

	// end data
	}else if(0x80 == vdata[index]){
	    //if(H2Config::RLEDebug()) fprintf(stderr, "\nM:%hhX\n", vdata[index]);
	    break;

	// range 0x81..0xBF 00 
	}else if(0x80 < vdata[index] && 0xC0 > vdata[index]){
	    //if(H2Config::RLEDebug()) fprintf(stderr, " M:%hhX Z:%d", vdata[index], vdata[index] - 0x80);
	    x += (vdata[index] - 0x80);
	    ++index;
	    continue;

	// 0xC0 - seek
	}else if(0xC0 == vdata[index]){
	    //if(H2Config::RLEDebug()) fprintf(stderr, " M:%hhX", vdata[index]);
	    ++index;

	    if( 0 == vdata[index] % 4){
		//if(H2Config::RLEDebug()) fprintf(stderr, " M4:%hhX:%d ALPHA", vdata[index], vdata[index] % 4);
		count = vdata[index];
		++index;
		for(i = 0; i < vdata[index]; ++i){
		    SetPixel2(x++, y, AGG::GetShadowAlpha());
		    //if(H2Config::RLEDebug()) fprintf(stderr, ":%hhX", count);
		}
		++index;
		continue;

	    }else{
		//if(H2Config::RLEDebug()) fprintf(stderr, " M4:%hhX:%d ALPHA", vdata[index], vdata[index] % 4);
		count = vdata[index];
		for(i = 0; i < vdata[index] % 4; ++i){
		    SetPixel2(x++, y, AGG::GetShadowAlpha());
		    //if(H2Config::RLEDebug()) fprintf(stderr, ":%hhX", count);
		}
		++index;
		continue;
	    }

	// 0xC1 N D count - data
	}else if(0xC1 == vdata[index]){
	    //if(H2Config::RLEDebug()) fprintf(stderr, " M:%hhX", vdata[index]);
	    ++index;
	    count = vdata[index];
	    //if(H2Config::RLEDebug()) fprintf(stderr, " C:%d:D", count);
	    ++index;
	    for(i = 0; i < count; ++i){
	    	SetPixel2(x++, y, AGG::GetColor(vdata[index]));
	    	//if(H2Config::RLEDebug()) fprintf(stderr, ":%hhX", vdata[index]);
	    }
	    ++index;
	    continue;

	// 0xC2 more
	}else if(0xC1 < vdata[index]){
	//}else{
	    //if(H2Config::RLEDebug()) fprintf(stderr, " M:%hhX", vdata[index]);
	    count = vdata[index] - 0xC0;
	    //if(H2Config::RLEDebug()) fprintf(stderr, " C:%d:D", count);
	    ++index;
	    for(i = 0; i < count; ++i){
		SetPixel2(x++, y, AGG::GetColor(vdata[index]));
		//if(H2Config::RLEDebug()) fprintf(stderr, ":%hhX", vdata[index]);
	    }
	    ++index;
	    continue;
	}
    }

    // unlock surface
    Unlock();

    //if(H2Config::RLEDebug()) std::cerr << "END RLE DEBUG" << std::endl;
}

