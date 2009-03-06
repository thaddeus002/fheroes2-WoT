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

#ifndef XMLWRITER_H
#define XMLWRITER_H

#include <libxml/xmlwriter.h>

namespace XML
{
    class Writer
    {
    public:
	Writer();
	~Writer();

	bool open(const char*, int = 0);
	bool isValid(void) const;

	void close(void);

	void endElement(void);
	bool startElement(const char*);

	bool addElement(const char*, const std::string &);
	bool addElement(const char*, const char*);
	bool addElement(const char*, int);
	bool addAttribute(const char*, const std::string &);
	bool addAttribute(const char*, const char*);
	bool addAttribute(const char*, int);

    protected:
	void endDocument(void);
	bool startDocument(void);

    private:
	xmlTextWriterPtr writer;
    };
};

#endif
