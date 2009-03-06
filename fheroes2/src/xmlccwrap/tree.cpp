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

#include <algorithm>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "tree.h"

XML::Tree::Tree()
{
}

const XML::Element & XML::Tree::getRoot(void) const
{
    return root;
}

XML::Element & XML::Tree::getRoot(void)
{
    return root;
}

bool XML::Tree::write(const char* filename, const unsigned char compression) const
{
    if(!filename) return false;

    xmlDocPtr doc = xmlNewDoc((xmlChar *) "1.0");
    bool result = false;

    if(doc)
    {
	xmlSetDocCompressMode(doc, compression);

	doc->children = NULL;

    	// elements
    	if(root.children.size())
        {
	    doc->children = xmlNewDocNode(doc, NULL, reinterpret_cast<const xmlChar *>(root.getName().c_str()), NULL);
	    if(doc->children) std::for_each(root.children.begin(), root.children.end(), std::bind2nd(std::mem_fun_ref(&Element::exportXML), doc->children));
    	}
	else
    	// content
    	if(root.content.size())
    	{
    	    doc->children = xmlNewDocNode(doc, NULL, reinterpret_cast<const xmlChar *>(root.getName().c_str()), reinterpret_cast<const xmlChar *>(root.content.c_str()));
    	}

    	// attributes
    	if(doc->children && root.attrs.size())
    	    std::for_each(root.attrs.begin(), root.attrs.end(), std::bind2nd(std::mem_fun_ref(&Attribute::exportXML), doc->children));

	result = 0 > xmlSaveFormatFile(filename, doc, 1) ? false : true;

	xmlFreeDoc(doc);
    }

    return result;
}

bool XML::Tree::read(const char* filename)
{
    if(!filename) return false;

    xmlDocPtr doc = xmlParseFile(filename);

    if(doc)
    {
	root.importXML(xmlDocGetRootElement(doc));
	xmlFreeDoc(doc);

	return true;
    }

    return false;
}
