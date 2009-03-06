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

#include <sstream>
#include "writer.h"

XML::Writer::Writer() : writer(NULL)
{
}

XML::Writer::~Writer()
{
    if(writer) close();
}

bool XML::Writer::open(const char* filename, int compress)
{
    if(writer) close();
    if(filename) writer = xmlNewTextWriterFilename(filename, compress);
    startDocument();
    return writer;
}

void XML::Writer::close(void)
{
    if(writer)
    {
	endDocument();
	xmlFreeTextWriter(writer);
	writer = NULL;
    }
}

bool XML::Writer::isValid(void) const
{
    return writer;
}

void XML::Writer::endDocument(void)
{
    if(writer) xmlTextWriterEndDocument(writer);
}

void XML::Writer::endElement(void)
{
    if(writer) xmlTextWriterEndElement(writer);
}

bool XML::Writer::startDocument(void)
{
    return writer && 0 == xmlTextWriterStartDocument(writer, NULL, NULL, NULL);
}

bool XML::Writer::startElement(const char* name)
{
    return writer && name && 0 == xmlTextWriterStartElement(writer, reinterpret_cast<const xmlChar*>(name));
}

bool XML::Writer::addElement(const char* name, const std::string & content)
{
    return addElement(name, content.c_str());
}

bool XML::Writer::addElement(const char* name, const char* content)
{
    return writer && name && content && 0 == xmlTextWriterWriteElement(writer, reinterpret_cast<const xmlChar*>(name), reinterpret_cast<const xmlChar*>(content));
}

bool XML::Writer::addElement(const char* name, int content)
{
    std::ostringstream stream;
    stream << content;

    return writer && name && content && 0 == xmlTextWriterWriteElement(writer, reinterpret_cast<const xmlChar*>(name), reinterpret_cast<const xmlChar*>(stream.str().c_str()));
}

bool XML::Writer::addAttribute(const char* name, const std::string & content)
{
    return addAttribute(name, content.c_str());
}

bool XML::Writer::addAttribute(const char* name, const char* content)
{
    return writer && name && content && 0 == xmlTextWriterWriteAttribute(writer, reinterpret_cast<const xmlChar*>(name), reinterpret_cast<const xmlChar*>(content));
}

bool XML::Writer::addAttribute(const char* name, int content)
{
    std::ostringstream stream;
    stream << content;

    return writer && name && content && 0 == xmlTextWriterWriteAttribute(writer, reinterpret_cast<const xmlChar*>(name), reinterpret_cast<const xmlChar*>(stream.str().c_str()));
}
