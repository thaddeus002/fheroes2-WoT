/*
    freeHeroes2 engine
    turn-based game engine (clone of Heroes Of the Might and Magic II)
    Copyright (C) 2006

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    3DO, New World Computing, Heroes of Might and Magic, and the related
    3DO logos are trademarks and or registered trademarks of 3DO Company.
    All other trademarks belong to their respective owners.

    Web: http://sourceforge.net/projects/fheroes2

    Description:
*/

#include "SDL.h"
#include "gamedefs.h"
#include "agg.h"
#include "tools.h"
#include "config.h"
#include "object.h"

E_OBJECT CheckCorrectObject(Uint8 type){

    E_OBJECT a = OBJ_ZERO;

    for(a = OBJ_ZERO; a < OBJ_END; ++a)
	if(a == type)
	    return a;

    if(GetIntValue("debug"))
	fprintf(stderr, "CheckCorrectObject: unknown object: 0x%hhX\n", type);

    return OBJ_ZERO;
}

ICNHEADER *GetICNHEADERCellObject(Uint8 type, Uint8 index){

    AGGSPRITE sprite;

    switch(type){

	// артифакты
	case 0x2C:
	case 0x2D:
	case 0x2E:
	case 0x2F:
	    FillSPRITE(&sprite, "OBJNARTI.ICN", index);
	    break;

	// монстры
	case 0x30:
	case 0x31:
	case 0x32:
	case 0x33:
	    FillSPRITE(&sprite, "MONS32.ICN", index);
	    break;

	// флажок возле замка
	case 0x38:
	    return NULL;
	    break;

	// heroes (skip)
	case 0x54:
	case 0x55:
	case 0x56:
	case 0x57:
	    return NULL;
	    break;

	// рельеф snow
	case 0x58:
	case 0x59:
	case 0x5A:
	case 0x5B:
	    FillSPRITE(&sprite, "MTNSNOW.ICN", index);
	    break;

	// рельеф swamp
	case 0x5C:
	case 0x5D:
	case 0x5E:
	case 0x5F:
	    FillSPRITE(&sprite, "MTNSWMP.ICN", index);
	    break;

	// рельеф lava
	case 0x60:
	case 0x61:
	case 0x62:
	case 0x63:
	    FillSPRITE(&sprite, "MTNLAVA.ICN", index);
	    break;

	// рельеф desert
	case 0x64:
	case 0x65:
	case 0x66:
	case 0x67:
	    FillSPRITE(&sprite, "MTNDSRT.ICN", index);
	    break;

	// рельеф dirt
	case 0x68:
	case 0x69:
	case 0x6A:
	case 0x6B:
	    FillSPRITE(&sprite, "MTNDIRT.ICN", index);
	    break;

	// рельеф общие
	case 0x6C:
	case 0x6D:
	case 0x6E:
	case 0x6F:
	    FillSPRITE(&sprite, "MTNMULT.ICN", index);
	    break;

	// тип шахты
	case 0x74:
	    FillSPRITE(&sprite, "EXTRAOVR.ICN", index);
	    break;

	// дорога
	case 0x78:
	case 0x79:
	case 0x7A:
	case 0x7B:
	    FillSPRITE(&sprite, "ROAD.ICN", index);
	    break;

	// рельеф crck
	case 0x7C:
	case 0x7D:
	case 0x7E:
	case 0x7F:
	    FillSPRITE(&sprite, "MTNCRCK.ICN", index);
	    break;

	// рельеф gras
	case 0x80:
	case 0x81:
	case 0x82:
	case 0x83:
	    FillSPRITE(&sprite, "MTNGRAS.ICN", index);
	    break;

	// деревья jungle
	case 0x84:
	case 0x85:
	case 0x86:
	case 0x87:
	    FillSPRITE(&sprite, "TREJNGL.ICN", index);
	    break;

	// деревья evil
	case 0x88:
	case 0x89:
	case 0x8A:
	case 0x8B:
	    FillSPRITE(&sprite, "TREEVIL.ICN", index);
	    break;

	// замки и деревни
	case 0x8C:
	case 0x8D:
	case 0x8E:
	case 0x8F:
	    FillSPRITE(&sprite, "OBJNTOWN.ICN", index);
	    break;

	// земля под замком
	case 0x90:
	case 0x91:
	case 0x92:
	case 0x93:
	    FillSPRITE(&sprite, "OBJNTWBA.ICN", index);
	    break;

	// тень от замка
	case 0x94:
	case 0x95:
	case 0x96:
	case 0x97:
	    FillSPRITE(&sprite, "OBJNTWSH.ICN", index);
	    break;

	// random castle
	case 0x98:
	case 0x99:
	case 0x9A:
	case 0x9B:
	    FillSPRITE(&sprite, "OBJNTWRD.ICN", index);
	    break;

	// объекты на воде №2
	case 0xA0:
	case 0xA1:
	case 0xA2:
	case 0xA3:
	    FillSPRITE(&sprite, "OBJNWAT2.ICN", index);
	    break;

	// объекты на земле №2
	case 0xA4:
	case 0xA5:
	case 0xA6:
	case 0xA7:
	    FillSPRITE(&sprite, "OBJNMUL2.ICN", index);
	    break;

	// деревья snow
	case 0xA8:
	case 0xA9:
	case 0xAA:
	case 0xAB:
	    FillSPRITE(&sprite, "TRESNOW.ICN", index);
	    break;

	// деревья trefir
	case 0xAC:
	case 0xAD:
	case 0xAE:
	case 0xAF:
	    FillSPRITE(&sprite, "TREFIR.ICN", index);
	    break;

	// деревья
	case 0xB0:
	case 0xB1:
	case 0xB2:
	case 0xB3:
	    FillSPRITE(&sprite, "TREFALL.ICN", index);
	    break;

	// река
	case 0xB4:
	case 0xB5:
	case 0xB6:
	case 0xB7:
	    FillSPRITE(&sprite, "STREAM.ICN", index);
	    break;

	// ресурсы
	case 0xB8:
	case 0xB9:
	case 0xBA:
	case 0xBB:
	    FillSPRITE(&sprite, "OBJNRSRC.ICN", index);
	    break;

	// объекты gras
	case 0xC0:
	case 0xC1:
	case 0xC2:
	case 0xC3:
	    FillSPRITE(&sprite, "OBJNGRA2.ICN", index);
	    break;

	// деревья tredeci
	case 0xC4:
	case 0xC5:
	case 0xC6:
	case 0xC7:
	    FillSPRITE(&sprite, "TREDECI.ICN", index);
	    break;

	// объекты на море
	case 0xC8:
	case 0xC9:
	case 0xCA:
	case 0xCB:
	    FillSPRITE(&sprite, "OBJNWATR.ICN", index);
	    break;

	// растительность gras
	case 0xCC:
	case 0xCD:
	case 0xCE:
	case 0xCF:
	    FillSPRITE(&sprite, "OBJNGRAS.ICN", index);
	    break;

	// объекты на snow
	case 0xD0:
	case 0xD1:
	case 0xD2:
	case 0xD3:
	    FillSPRITE(&sprite, "OBJNSNOW.ICN", index);
	    break;

	// объекты на swamp
	case 0xD4:
	case 0xD5:
	case 0xD6:
	case 0xD7:
	    FillSPRITE(&sprite, "OBJNSWMP.ICN", index);
	    break;

	// объекты на lava
	case 0xD8:
	case 0xD9:
	case 0xDA:
	case 0xDB:
	    FillSPRITE(&sprite, "OBJNLAVA.ICN", index);
	    break;

	// объекты на desert
	case 0xDC:
	case 0xDD:
	case 0xDE:
	case 0xDF:
	    FillSPRITE(&sprite, "OBJNDSRT.ICN", index);
	    break;

	// объекты на dirt
	case 0xE0:
	case 0xE1:
	case 0xE2:
	case 0xE3:
	    FillSPRITE(&sprite, "OBJNDIRT.ICN", index);
	    break;

	// объекты на crck
	case 0xE4:
	case 0xE5:
	case 0xE6:
	case 0xE7:
	    FillSPRITE(&sprite, "OBJNCRCK.ICN", index);
	    break;

	// объекты на lava
	case 0xE8:
	case 0xE9:
	case 0xEA:
	case 0xEB:
	    FillSPRITE(&sprite, "OBJNLAV3.ICN", index);
	    break;

	// объекты на земле
	case 0xEC:
	case 0xED:
	case 0xEE:
	case 0xEF:
	    FillSPRITE(&sprite, "OBJNMULT.ICN", index);
	    break;

	// объекты на lava
	case 0xF0:
	case 0xF1:
	case 0xF2:
	case 0xF3:
	    FillSPRITE(&sprite, "OBJNLAV2.ICN", index);
	    break;

	default:
	    if(GetIntValue("debug")) fprintf(stderr, "GetICNHEADERCellObject: return NULL: object: 0x%hhX, memorize\n", type);
	    return NULL;
	    break;
    }
    
    return GetICNHeader(&sprite);
}
