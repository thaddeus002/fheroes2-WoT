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

#include <libxml/parser.h>
#include <algorithm>
#include <sstream>
#include "element.h"

XML::Element::Element()
{
}

XML::Element::Element(const std::string & nam)
{
    name = nam;
}

bool XML::Element::operator== (const std::string & nam) const
{
    return name == nam;
}

void XML::Element::setName(const char* nam)
{
    name.assign(nam);
}

void XML::Element::setName(const std::string & nam)
{
    name = nam;
}

void XML::Element::setContent(const char* cont)
{
    content.assign(cont);
}

void XML::Element::setContent(int cont)
{
    std::ostringstream stream;
    stream << cont;
    content = stream.str();
}

void XML::Element::setContent(const std::string & cont)
{
    content = cont;
}

void XML::Element::setAttribute(const std::string & nam, int value)
{
    std::ostringstream stream;
    stream << value;
    setAttribute(nam, stream.str());
}

void XML::Element::setAttribute(const std::string & nam, const std::string & value)
{
    AttributeList::iterator it;

    if(attrs.size() && attrs.end() != (it = std::find(attrs.begin(), attrs.end(), nam)))
	(*it).setValue(value);
    else
	attrs.push_back(Attribute(nam, value));
}

void XML::Element::setAttribute(const Attribute & attr)
{
    AttributeList::iterator it;

    if(attrs.size() && attrs.end() != (it = std::find(attrs.begin(), attrs.end(), attr.getName())))
	(*it).setValue(attr.getValue());
    else
	attrs.push_back(attr);
}

XML::Element & XML::Element::addElement(const Element & elem)
{
    children.push_back(elem);

    return children.back();
}

XML::Element & XML::Element::addElement(const std::string & nam)
{
    children.push_back(Element(nam));

    return children.back();
}

const std::string & XML::Element::getName(void) const
{
    return name;
}

const std::string & XML::Element::getContent(void) const
{
    return content;
}

const XML::ElementList & XML::Element::getElementList(void) const
{
    return children;
}

const XML::AttributeList & XML::Element::getAttributeList(void) const
{
    return attrs;
}

const XML::Element* XML::Element::getElement(const std::string & nam) const
{
    ElementList::const_iterator it;

    if(children.size() && children.end() != (it = std::find(children.begin(), children.end(), nam)))
	return &(*it);

    return NULL;
}

XML::Element* XML::Element::getElement(const std::string & nam)
{
    std::list<Element>::iterator it;

    if(children.size() && children.end() != (it = std::find(children.begin(), children.end(), nam)))
	return &(*it);

    return NULL;
}

const XML::Attribute* XML::Element::getAttribute(const std::string & nam) const
{
    AttributeList::const_iterator it;

    if(attrs.size() && attrs.end() != (it = std::find(attrs.begin(), attrs.end(), nam)))
	return &(*it);

    return NULL;
}

XML::Attribute* XML::Element::getAttribute(const std::string & nam)
{
    AttributeList::iterator it;

    if(attrs.size() && attrs.end() != (it = std::find(attrs.begin(), attrs.end(), nam)))
	return &(*it);

    return NULL;
}

void XML::Element::exportXML(xmlNodePtr parent) const
{
    if(parent)
    {
        xmlNodePtr node = NULL;

	// elements
	if(children.size())
	{
	    node = xmlNewChild(parent, NULL, reinterpret_cast<const xmlChar *>(name.c_str()), NULL);
	    if(node) std::for_each(children.begin(), children.end(), std::bind2nd(std::mem_fun_ref(&Element::exportXML), node));
	}
	else
	// content
	if(content.size())
	{
	    node = xmlNewChild(parent, NULL, reinterpret_cast<const xmlChar *>(name.c_str()), reinterpret_cast<const xmlChar *>(content.c_str()));
	}

	// attributes
	if(node && attrs.size())
	    std::for_each(attrs.begin(), attrs.end(), std::bind2nd(std::mem_fun_ref(&Attribute::exportXML), node));
    }
}

void XML::Element::importXML(xmlNodePtr node)
{
    if(node)
    {
	children.clear();
	attrs.clear();

	// name
	if(node->name) name.assign(reinterpret_cast<const char*>(node->name));

	// content
	if(node->content) content.assign(reinterpret_cast<const char*>(node->content));

	// attrs
	xmlAttrPtr attr = node->properties;
	while(attr)
	{
	    attr = attr->next;
	}

	// children
	xmlNodePtr child = node->children;
	while(child)
	{
	    children.push_back(Element());
	    children.back().importXML(child);

	    child = child->next;
	}
    }
}
