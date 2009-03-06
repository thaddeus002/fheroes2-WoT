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

#ifndef XMLELEMENT_H
#define XMLELEMENT_H

#include <list>
#include <string>
#include "attribute.h"

namespace XML
{
    class Tree;
    class Element;
    typedef std::list<Element> ElementList;

    class Element
    {
    public:
	Element();
	Element(const std::string &);

	bool operator== (const std::string &) const;

	void setName(const char*);
	void setName(const std::string &);
	void setContent(const char*);
	void setContent(int);
	void setContent(const std::string &);
	void setAttribute(const std::string &, int);
	void setAttribute(const std::string &, const std::string &);
	void setAttribute(const Attribute &);
	Element & addElement(const Element &);
	Element & addElement(const std::string &);

	void exportXML(xmlNodePtr) const;
	void importXML(xmlNodePtr);

	const std::string & getName(void) const;
	const std::string & getContent(void) const;
	const ElementList & getElementList(void) const;
	const AttributeList & getAttributeList(void) const;
	const Element* getElement(const std::string &) const;
	Element* getElement(const std::string &);
	const Attribute* getAttribute(const std::string &) const;
	Attribute* getAttribute(const std::string &);

    private:
	friend class Tree;

	std::string name;
	std::string content;
	AttributeList attrs;
	ElementList children;
    };
};

#endif
