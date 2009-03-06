/***************************************************************************
 *   Copyright (C) 2009 by Andrey Afletdinov                               *
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

#include "attribute.h"

XML::Attribute::Attribute()
{
}

XML::Attribute::Attribute(const std::string & name, const std::string & value)
    : std::pair<std::string, std::string>(name, value)
{
}

bool XML::Attribute::operator== (const std::string & name) const
{
    return first == name;
}

void XML::Attribute::setName(const std::string & name)
{
    first = name;
}

void XML::Attribute::setValue(const std::string & value)
{
    second = value;
}

const std::string & XML::Attribute::getName(void) const
{
    return first;
}

const std::string & XML::Attribute::getValue(void) const
{
    return second;
}

void XML::Attribute::exportXML(xmlNodePtr node) const
{
    if(node)
	xmlSetProp(node, (xmlChar *) first.c_str(), (xmlChar *) second.c_str());
}
