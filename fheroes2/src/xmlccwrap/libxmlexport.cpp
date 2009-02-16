/* libxmlccwrap - a C++ wrapper around libxml2
 * Copyright (C) 2002-2003 Juergen Rinas.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "libxmlexport.h"

namespace xmlcc
{
  extern void writenode(xmlDocPtr, Element *, xmlNodePtr, int = 0);

  xmlDocPtr exportXMLDocPtr(const XMLTree & xmlTree)
  {
    xmlDocPtr doc;
    xmlKeepBlanksDefault(0);
    doc = xmlNewDoc((xmlChar *) "1.0");
    writenode(doc, xmlTree.getRoot(), doc->children, 1);

    return doc;
  }

}
; // namespace xmlcc
