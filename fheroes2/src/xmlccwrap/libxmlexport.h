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

#ifndef _libxmlexport_h
#define _libxmlexport_h

extern "C++"
{

#include "xmlccwrap.h"
#include <libxml/tree.h>

  namespace xmlcc
  {
    xmlDocPtr exportXMLDocPtr(const XMLTree & xmlTree);
    // you have to call
    //    xmlFreeDoc(doc);
    // yourselve!

  }
  ; // namespace xmlcc

}

#endif

