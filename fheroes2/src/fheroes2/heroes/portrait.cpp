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
#include "gamedefs.h"
#include "race.h"
#include "heroes.h"
#include "portrait.h"

const Surface & Portrait::Captain(const Race::race_t rc, const Portrait::size_t sz)
{
    switch(sz)
    {
	case BIG:
	    switch(rc)
	    {
    		case Race::KNGT:        return AGG::GetICN(ICN::PORT0090, 0);
    		case Race::BARB:        return AGG::GetICN(ICN::PORT0091, 0);
    		case Race::SORC:        return AGG::GetICN(ICN::PORT0092, 0);
    		case Race::WRLK:        return AGG::GetICN(ICN::PORT0093, 0);
    		case Race::WZRD:        return AGG::GetICN(ICN::PORT0094, 0);
    		case Race::NECR:        return AGG::GetICN(ICN::PORT0095, 0);
    		default: break;
	    }
	    break;

	case MEDIUM:
	case SMALL:
	    switch(rc)
	    {
    		case Race::KNGT:        return AGG::GetICN(ICN::MINICAPT, 0);
    		case Race::BARB:        return AGG::GetICN(ICN::MINICAPT, 1);
    		case Race::SORC:        return AGG::GetICN(ICN::MINICAPT, 2);
    		case Race::WRLK:        return AGG::GetICN(ICN::MINICAPT, 3);
    		case Race::WZRD:        return AGG::GetICN(ICN::MINICAPT, 4);
    		case Race::NECR:        return AGG::GetICN(ICN::MINICAPT, 5);
    		default: break;
	    }
	    break;
    }

    Error::Warning("Portrait::Captain: unknown race.");

    return AGG::GetICN(ICN::PORT0090, 0);
}

const Surface & Portrait::Hero(const Heroes & hero, const Portrait::size_t sz)
{
    return Hero(hero.GetID(), sz);
}

const Surface & Portrait::Hero(const Heroes::heroes_t hid, const Portrait::size_t sz)
{
    switch(sz)
    {
	case BIG:
	{
	    ICN::icn_t icn = ICN::UNKNOWN;

	    switch(hid)
	    {
    		case Heroes::LORDKILBURN:	icn = ICN::PORT0000; break;
    		case Heroes::SIRGALLANTH:	icn = ICN::PORT0001; break;
    		case Heroes::ECTOR:		icn = ICN::PORT0002; break;
    		case Heroes::GVENNETH:		icn = ICN::PORT0003; break;
    		case Heroes::TYRO:		icn = ICN::PORT0004; break;
    		case Heroes::AMBROSE:		icn = ICN::PORT0005; break;
    		case Heroes::RUBY:		icn = ICN::PORT0006; break;
    		case Heroes::MAXIMUS:		icn = ICN::PORT0007; break;
    		case Heroes::DIMITRY:		icn = ICN::PORT0008; break;
    		case Heroes::THUNDAX:		icn = ICN::PORT0009; break;
    		case Heroes::FINEOUS:		icn = ICN::PORT0010; break;
    		case Heroes::JOJOSH:		icn = ICN::PORT0011; break;
    		case Heroes::CRAGHACK:		icn = ICN::PORT0012; break;
    		case Heroes::JEZEBEL:		icn = ICN::PORT0013; break;
    		case Heroes::JACLYN:		icn = ICN::PORT0014; break;
    		case Heroes::ERGON:		icn = ICN::PORT0015; break;
    		case Heroes::TSABU:		icn = ICN::PORT0016; break;
    		case Heroes::ATLAS:		icn = ICN::PORT0017; break;
                case Heroes::ASTRA:		icn = ICN::PORT0018; break;
                case Heroes::NATASHA:		icn = ICN::PORT0019; break;
                case Heroes::TROYAN:		icn = ICN::PORT0020; break;
                case Heroes::VATAWNA:		icn = ICN::PORT0021; break;
                case Heroes::REBECCA:		icn = ICN::PORT0022; break;
                case Heroes::GEM:		icn = ICN::PORT0023; break;
                case Heroes::ARIEL:		icn = ICN::PORT0024; break;
                case Heroes::CARLAWN:		icn = ICN::PORT0025; break;
                case Heroes::LUNA:		icn = ICN::PORT0026; break;
                case Heroes::ARIE:		icn = ICN::PORT0027; break;
                case Heroes::ALAMAR:		icn = ICN::PORT0028; break;
                case Heroes::VESPER:		icn = ICN::PORT0029; break;
                case Heroes::CRODO:		icn = ICN::PORT0030; break;
                case Heroes::BAROK:		icn = ICN::PORT0031; break;
                case Heroes::KASTORE:		icn = ICN::PORT0032; break;
                case Heroes::AGAR:		icn = ICN::PORT0033; break;
                case Heroes::FALAGAR:		icn = ICN::PORT0034; break;
                case Heroes::WRATHMONT:		icn = ICN::PORT0035; break;
                case Heroes::MYRA:		icn = ICN::PORT0036; break;
                case Heroes::FLINT:		icn = ICN::PORT0037; break;
                case Heroes::DAWN:		icn = ICN::PORT0038; break;
                case Heroes::HALON:		icn = ICN::PORT0039; break;
                case Heroes::MYRINI:		icn = ICN::PORT0040; break;
                case Heroes::WILFREY:		icn = ICN::PORT0041; break;
                case Heroes::SARAKIN:		icn = ICN::PORT0042; break;
                case Heroes::KALINDRA:		icn = ICN::PORT0043; break;
                case Heroes::MANDIGAL:		icn = ICN::PORT0044; break;
                case Heroes::ZOM:		icn = ICN::PORT0045; break;
                case Heroes::DARLANA:		icn = ICN::PORT0046; break;
                case Heroes::ZAM:		icn = ICN::PORT0047; break;
                case Heroes::RANLOO:		icn = ICN::PORT0048; break;
                case Heroes::CHARITY:		icn = ICN::PORT0049; break;
                case Heroes::RIALDO:		icn = ICN::PORT0050; break;
                case Heroes::ROXANA:		icn = ICN::PORT0051; break;
                case Heroes::SANDRO:		icn = ICN::PORT0052; break;
                case Heroes::CELIA:		icn = ICN::PORT0053; break;
                case Heroes::ROLAND:		icn = ICN::PORT0054; break;
                case Heroes::CORLAGON:		icn = ICN::PORT0055; break;
                case Heroes::ELIZA:		icn = ICN::PORT0056; break;
                case Heroes::ARCHIBALD:		icn = ICN::PORT0057; break;
                case Heroes::HALTON:		icn = ICN::PORT0058; break;
                case Heroes::BAX:		icn = ICN::PORT0059; break;

		case Heroes::SOLMYR:		icn = ICN::PORT0060; break;
		case Heroes::DAINWIN:		icn = ICN::PORT0061; break;
		case Heroes::MOG:		icn = ICN::PORT0062; break;
		case Heroes::UNCLEIVAN:		icn = ICN::PORT0063; break;
		case Heroes::JOSEPH:		icn = ICN::PORT0064; break;
		case Heroes::GALLAVANT:		icn = ICN::PORT0065; break;
		case Heroes::ELDERIAN:		icn = ICN::PORT0066; break;
		case Heroes::CEALLACH:		icn = ICN::PORT0067; break;
		case Heroes::DRAKONIA:		icn = ICN::PORT0068; break;
		case Heroes::MARTINE:		icn = ICN::PORT0069; break;
		case Heroes::JARKONAS:		icn = ICN::PORT0070; break;

                default: break;
	    }

	    // FIXME: VERSION: 2.1 add new portrait
	    return AGG::GetICN(Heroes::SANDYSANDY > hid ? icn : ICN::PORT0059, 0);
	}

	case MEDIUM:
	    if(Heroes::UNKNOWN <= hid) return AGG::GetICN(ICN::PORTMEDI, 0);
	    else
	    if(Heroes::SANDYSANDY > hid) return AGG::GetICN(ICN::PORTMEDI, hid + 1);
	    else
	    return AGG::GetICN(ICN::PORTMEDI, 60);

	case SMALL:
	    return AGG::GetICN(ICN::MINIPORT, Heroes::SANDYSANDY > hid ? hid : 59);
    }

    return AGG::GetICN(ICN::PORTMEDI, 0);
}
