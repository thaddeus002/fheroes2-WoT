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

#include "reader.h"

XML::Reader::Reader() : reader(NULL)
{
}

XML::Reader::~Reader()
{
    if(reader) close();
}

bool XML::Reader::open(const char* filename)
{
    if(reader) close();
    if(filename) reader = xmlNewTextReaderFilename(filename);
    return reader;
}

void XML::Reader::close(void)
{
    if(reader)
    {
	xmlTextReaderClose(reader);
	xmlFreeTextReader(reader);
	reader = NULL;
    }
}

bool XML::Reader::isValid(void) const
{
    return reader;
}
