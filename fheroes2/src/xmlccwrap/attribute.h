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

#ifndef XMLATTRIBUTE_H
#define XMLATTRIBUTE_H

#include <utility>
#include <list>
#include <string>
#include <libxml/parser.h>

namespace XML
{
    class Attribute;
    typedef std::list<Attribute> AttributeList;

    class Attribute : protected std::pair<std::string, std::string>
    {
    public:
	Attribute();
	Attribute(const std::string &, const std::string & = std::string());

	bool operator== (const std::string &) const;

	void setName(const std::string &);
	void setValue(const std::string &);

	void exportXML(xmlNodePtr) const;

	const std::string & getName(void) const;
	const std::string & getValue(void) const;
    };
};

#endif
