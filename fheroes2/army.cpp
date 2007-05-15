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

#include "army.h"

using namespace Army;

const std::string & Size::String(Size::size_t size)
{
    static const std::string few("Few");
    static const std::string several("Several");
    static const std::string pack("Pack");
    static const std::string lots("Lots");
    static const std::string horde("Horde");
    static const std::string throng("Throng");
    static const std::string swarm("Swarm");
    static const std::string zounds("Zounds");
    static const std::string legion("Legion");

    switch(size){
	case Size::FEW:		return few;
        case Size::SEVERAL:	return several;
        case Size::PACK:	return pack;
        case Size::LOTS:	return lots;
        case Size::HORDE:	return horde;
        case Size::THRONG:	return throng;
        case Size::SWARM:	return swarm;
        case Size::ZOUNDS:	return zounds;
        default: break;
    }
    
    return legion;
}

