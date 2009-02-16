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

#include <stdio.h>
#include <stdarg.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/HTMLparser.h>
#include <libxml/encoding.h>
/* disable xslt
#include <libxslt/xsltutils.h>
#include <libxslt/transform.h>
*/
#include "xmlccwrap.h"
#include "libxmlexport.h"

namespace xmlcc
{
  void errorFunc(void *ctx, const char *msg, ...)
  {
    std::string * strPtr = (std::string *)ctx;

    char buffer[8000];
    va_list args;
    va_start(args, msg);
    vsnprintf(buffer, 8000, msg, args);
    va_end(args);

    (*strPtr) += buffer;
  }


  Element *readnode(xmlNodePtr);
  void writenode(xmlDocPtr, Element *, xmlNodePtr, int = 0);


/* disable xslt
  XSLTTree::XSLTTree()
  {
    xsltP = 0;
  }


  XSLTTree::XSLTTree(const std::string &fn)
  {
    xsltP = 0;
    XSLTTree::read(fn);
  }


  XSLTTree::~XSLTTree()
  {
    if (xsltP)
    {
      xsltFreeStylesheet((xsltStylesheetPtr)xsltP);
      xsltP = 0;
    }
  }


  bool XSLTTree::read(const std::string &fn)
  {
    if (xsltP = xsltParseStylesheetFile((xmlChar *)fn.c_str()))
      return true;
    return false;
  }


  const void * XSLTTree::exportxsltStylesheetPtr() const
  {
    return xsltP;
  }
*/

  XMLTree::XMLTree() : _filename(), _root(), _compression(0)
  { }


  XMLTree::XMLTree(const XMLTree & xmlTree)
  {
    _filename = xmlTree.getFilename();
    if (xmlTree.getRoot())
      _root = new Element(*xmlTree.getRoot());
    else
      _root = 0;
    _compression = xmlTree.getCompression();
  }


  XMLTree::~XMLTree()
  {
    if (_root)
      delete _root;
  }


  Element *XMLTree::getRoot() const
  {
    return _root;
  }


  Element *XMLTree::setRoot(const Element & element)
  {
    return _root = new Element(element);
  }


  Element *XMLTree::setRoot(Element * elementP)
  {
    return _root = elementP;
  }


  const std::string & XMLTree::getFilename() const
  {
    return _filename;
  }


  const std::string & XMLTree::setFilename(const std::string &fn)
  {
    return _filename = fn;
  }


  const std::string & XMLTree::getEncoding() const
  {
    return _encoding;
  }


  const std::string & XMLTree::setEncoding(const std::string & e)
  {
    if (xmlFindCharEncodingHandler(e.c_str()))
      _encoding = e;
    return _encoding;
  }


  int XMLTree::getCompression() const
  {
    return _compression;
  }


  int XMLTree::setCompression(int c)
  {
    if (c > 9)
      c = 9;

    if (c < 0)
      c = 0;

    _compression = c;

    return _compression;
  }


  bool XMLTree::read()
  {
    errorString = "";
    xmlSetGenericErrorFunc((void *) &errorString, &errorFunc);

    xmlDocPtr doc;
    if (_root)
    {
      delete _root;
      _root = NULL;
    }

    xmlKeepBlanksDefault(0);
    xmlLineNumbersDefault(1);

    doc = xmlParseFile(_filename.c_str());
    if (!doc)
    {
      xmlSetGenericErrorFunc(0, 0);
      return false;
    }

    if (dtdfilename.size() > 0)
    {
      xmlDtdPtr dtd = xmlParseDTD(NULL, (const xmlChar *)dtdfilename.c_str());
      if (!dtd)
      {
        xmlFreeDoc(doc);
        xmlSetGenericErrorFunc(0, 0);
        return false;
      }

      xmlValidCtxt cvp;
      cvp.userData = (void *) & errorString;
      cvp.error = (xmlValidityErrorFunc) & errorFunc;
      cvp.warning = (xmlValidityWarningFunc) & errorFunc;
      if (!xmlValidateDtd(&cvp, doc, dtd))
      {
        xmlFreeDoc(doc);
        xmlFreeDtd(dtd);
        xmlSetGenericErrorFunc(0, 0);
        return false;
      }
      xmlFreeDtd(dtd);
    }

    if (doc->encoding)
      _encoding = (const char *)doc->encoding;
    _compression = doc->compression;
    _root = readnode(xmlDocGetRootElement(doc));
    xmlFreeDoc(doc);

    xmlSetGenericErrorFunc(0, 0);
    return true;
  }


  bool XMLTree::read(const std::string &fn)
  {
    setFilename(fn);
    return read();
  }


  bool XMLTree::readWithDTD(const std::string &fn, const std::string &_dtdfilename)
  {
    dtdfilename = _dtdfilename;
    return read(fn);
  }


  bool XMLTree::readHTML()
  {
    errorString = "";
    xmlSetGenericErrorFunc((void *) &errorString, &errorFunc);

    xmlDocPtr doc;

    if (_root)
    {
      delete _root;
      _root = NULL;
    }


    xmlKeepBlanksDefault(0);
    xmlLineNumbersDefault(1);

    doc = htmlParseFile(_filename.c_str(), 0);
    if (!doc)
    {
      xmlSetGenericErrorFunc(0, 0);
      return false;
    }


    if (doc->encoding)
      _encoding = (const char *)doc->encoding;
    _compression = doc->compression;
    _root = readnode(xmlDocGetRootElement(doc));
    xmlFreeDoc(doc);

    xmlSetGenericErrorFunc(0, 0);
    return true;
  }


  bool XMLTree::readHTML(const std::string &fn)
  {
    setFilename(fn);
    return readHTML();
  }


  bool XMLTree::readBuffer(const std::string &buffer)
  {
    xmlDocPtr doc;

    _filename = "";

    if (_root)
    {
      delete _root;
      _root = NULL;
    }

    xmlLineNumbersDefault(1);

    doc = xmlParseMemory((const char *) buffer.c_str(), buffer.length());
    if (doc->encoding)
      _encoding = (const char *)doc->encoding;
    _compression = doc->compression;
    if (!doc)
    {
      return false;
    }

    _root = readnode(xmlDocGetRootElement(doc));
    xmlFreeDoc(doc);
    return true;
  }


  bool XMLTree::write() const
  {
    xmlDocPtr doc;
    ElementList children;
    int result;

    xmlKeepBlanksDefault(0);
    doc = xmlNewDoc((xmlChar *) "1.0");
    xmlSetDocCompressMode(doc, _compression);
    writenode(doc, _root, doc->children, 1);
    if (_encoding != std::string())
    {
      result = xmlSaveFormatFileEnc(_filename.c_str(), doc, _encoding.c_str(), 1);
    }
    else
    {
      result = xmlSaveFormatFile(_filename.c_str(), doc, 1);
    }
    xmlFreeDoc(doc);

    if (result == -1)
      return false;

    return true;
  }


  bool XMLTree::write(const std::string &fn)
  {
    setFilename(fn);
    return write();
  }


  const std::string & XMLTree::writeBuffer() const
  {
    static std::string retval;
    char *ptr;
    int len;
    xmlDocPtr doc;
    ElementList children;

    xmlKeepBlanksDefault(0);
    doc = xmlNewDoc((xmlChar *) "1.0");
    xmlSetDocCompressMode(doc, _compression);
    writenode(doc, _root, doc->children, 1);

    if (_encoding != std::string())
    {
      // xmlDocDumpMemoryEnc(doc, (xmlChar **) &ptr, &len,_encoding.c_str());
      xmlDocDumpFormatMemoryEnc(doc, (xmlChar **) &ptr, &len, _encoding.c_str(), 1);
    }
    else
    {
      // xmlDocDumpMemory(doc, (xmlChar **) &ptr, &len);
      xmlDocDumpFormatMemory(doc, (xmlChar **) &ptr, &len, 1);
    }
    xmlFreeDoc(doc);

    retval = std::string(ptr, len);

    xmlFree(ptr);

    return retval;
  }


/* disable xslt
  bool XMLTree::xslt(const XSLTTree & xsltTree, const std::string & outputfile, ParameterMap & parameterMap)
  {
    bool ret = false;

    errorString = "";
    xmlSetGenericErrorFunc((void *) &errorString, &errorFunc);

    xmlDocPtr doc = exportXMLDocPtr(*this);
    if (doc)
    {
      if (xsltTree.exportxsltStylesheetPtr())
      {
        const char *params[parameterMap.size()*2 + 1];
        int nbparams = 0;
        for (ParameterMap::const_iterator it = parameterMap.begin();
             it != parameterMap.end();
             it++)
        {
          params[nbparams++] = (*it).first.c_str();
          params[nbparams++] = (*it).second.c_str();
        }
        params[nbparams] = 0;

        xmlDocPtr res = xsltApplyStylesheet((xsltStylesheetPtr)xsltTree.exportxsltStylesheetPtr(), doc, params);
        if (res)
        {
          xsltSaveResultToFilename(outputfile.c_str(), res, (xsltStylesheetPtr)xsltTree.exportxsltStylesheetPtr(), 0);
          xmlFreeDoc(res);
          ret = true;
        }
        xsltCleanupGlobals();
      }
      xmlFreeDoc(doc);
    }
    xmlSetGenericErrorFunc(0, 0);
    return ret;
  }


  bool XMLTree::xslt(const XSLTTree & xsltTree, const std::string & outputfile)
  {
    ParameterMap parameterMap;
    return xslt(xsltTree, outputfile, parameterMap);
  }
*/

  const std::string & XMLTree::getErrorString()
  {
    return errorString;
  }


  Element::Element(const std::string &n)
      : _name(n), _is_content(false), _content(std::string()), _line(0)
  {}


  Element::Element(const std::string &n, int line)
      : _name(n), _is_content(false), _content(std::string()), _line(line)
  {}


  Element::Element(const std::string &n, const std::string &c)
      : _name(std::string()), _is_content(true), _content(c), _line(0)
  {}


  Element::Element(const Element & from)
  {
    _name = from.name();
    setContent(from.getContent());

    AttributeList props = from.getAttributeList();
    for (AttributeIterator curprop = props.begin(); curprop != props.end(); curprop++)
      add
        (**curprop);

    ElementList nodes = from.getElementList();
    for (ElementIterator curnode = nodes.begin(); curnode != nodes.end(); curnode++)
      add
        (**curnode);
  }


  Element::~Element()
  {
    for (ElementIterator curchild = _children.begin(); curchild != _children.end(); curchild++)
      delete *curchild;

    for (AttributeIterator curprop = _proplist.begin(); curprop != _proplist.end(); curprop++)
      delete *curprop;
  }


  Element & Element::operator= (const Element & from)
  {
    for (ElementIterator curchild = _children.begin(); curchild != _children.end(); curchild++)
      delete *curchild;
    _children.clear();

    for (AttributeIterator curprop = _proplist.begin(); curprop != _proplist.end(); curprop++)
      delete *curprop;
    _proplist.clear();
    _prophash.clear();

    _name = from.name();
    setContent(from.getContent());

    AttributeList props = from.getAttributeList();
    for (AttributeIterator curprop = props.begin(); curprop != props.end(); curprop++)
      add
        (**curprop);

    ElementList nodes = from.getElementList();
    for (ElementIterator curnode = nodes.begin(); curnode != nodes.end(); curnode++)
      add
        (**curnode);
  }


  const std::string & Element::name() const
  {
    return _name;
  }


  void Element::setName(const std::string & n)
  {
    _name = n;
  }


  int Element::line()
  {
    return _line;
  }


  bool Element::isContent() const
  {
    return _is_content;
  }


  const std::string & Element::getContent() const
  {
    return _content;
  }


  const std::string & Element::setContent(const std::string &c)
  {
    if (c.empty())
      _is_content = false;
    else
      _is_content = true;

    _content = c;

    return _content;
  }


  const ElementList & Element::getElementList(const std::string &n) const
  {
    static ElementList retval;
    ElementConstIterator cur;

    if (n.length() == 0)
      return _children;

    retval.erase(retval.begin(), retval.end());

    for (cur = _children.begin(); cur != _children.end(); cur++)
      if ((*cur)->name() == n)
        retval.insert(retval.end(), *cur);

    return retval;
  }


  ElementList & Element::getElementList(const std::string &n)
  {
    static ElementList retval;
    ElementConstIterator cur;

    if (n.length() == 0)
      return _children;

    retval.erase(retval.begin(), retval.end());

    for (cur = _children.begin(); cur != _children.end(); cur++)
      if ((*cur)->name() == n)
        retval.insert(retval.end(), *cur);

    return retval;
  }


  const Element * Element::getElement(const std::string &n) const
  {
    for (ElementConstIterator cur = _children.begin();
         cur != _children.end();
         cur++)
      if ((*cur)->name() == n)
        return *cur;

    return 0;
  }


  Element * Element::getElement(const std::string &n)
  {
    for (ElementIterator cur = _children.begin();
         cur != _children.end();
         cur++)
      if ((*cur)->name() == n)
        return *cur;

    return 0;
  }


  Element *Element::add
    (const Element & element)
  {
    Element *tmp = new Element(element);
    if (!tmp)
      return NULL;

    _children.insert(_children.end(), tmp);
    return tmp;
  }


  Element *Element::add
    (Element *n)
  {
    if (!n)
      return NULL;

    _children.insert(_children.end(), n);
    return n;
  }


  Element *Element::addElement(const std::string &n)
  {
    Element *tmp;

    tmp = new Element(n);

    return add
             (tmp);
  }


  Element *Element::addContent(const std::string &c)
  {
    Element *tmp;

    tmp = new Element(std::string(), c);

    return add
             (tmp);
  }


  void Element::removeElement(Element *n)
  {
    if (n)
      _children.remove(n);
  }


  const AttributeList & Element::getAttributeList() const
  {
    return _proplist;
  }


  Attribute *Element::getAttribute(const std::string &n)
  {
    if (_prophash.find(n) == _prophash.end())
      return NULL;

    return _prophash[n];
  }


  const Attribute *Element::getAttribute(const std::string &n) const
  {
    return ((Element *) this)->getAttribute(n);
  }


  Attribute *Element::addAttribute(const std::string &n, const std::string &v)
  {
    Attribute *tmp;

    if (_prophash.find(n) != _prophash.end())
      return NULL;

    tmp = new Attribute(n, v);

    if (!tmp)
      return NULL;

    _prophash[tmp->name()] = tmp;
    _proplist.insert(_proplist.end(), tmp);

    return tmp;
  }


  Attribute *Element::add
    (const Attribute & attribute)
  {
    Attribute *tmp;

    if (_prophash.find(attribute.name()) != _prophash.end())
      return NULL;

    tmp = new Attribute(attribute);

    if (!tmp)
      return NULL;

    _prophash[tmp->name()] = tmp;
    _proplist.insert(_proplist.end(), tmp);

    return tmp;
  }


  Attribute *Element::add
    (Attribute * attributeP)
  {
    if (!attributeP)
      return NULL;

    if (_prophash.find(attributeP->name()) != _prophash.end())
    {
      delete attributeP;
      return NULL;
    }

    _prophash[attributeP->name()] = attributeP;
    _proplist.insert(_proplist.end(), attributeP);

    return attributeP;
  }


  void Element::removeAttribute(const std::string &n)
  {
    if (_prophash.find(n) != _prophash.end())
    {
      _proplist.remove(_prophash[n]);
      _prophash.erase(n);
    }
  }

  Element * readnode(xmlNodePtr node)
  {
    if (0 == node)
      return 0;

    std::string name, content;
    xmlNodePtr child;
    Element *tmp;
    xmlAttrPtr attr;

    if (node->name)
      name = (char *) node->name;
    else
      name = std::string();

    tmp = new Element(name, XML_GET_LINE(node));

    for (attr = node->properties; attr; attr = attr->next)
    {
      name = (char *) attr->name;
      content = "";
      if (attr->children)
        content = (char *) attr->children->content;
      tmp->add
      (Attribute(name, content));
    }

    if (XML_GET_CONTENT(node))
      tmp->setContent((char *) XML_GET_CONTENT(node));
    else
      tmp->setContent(std::string());

    for (child = node->children; child; child = child->next)
      tmp->add
      (readnode(child));

    return tmp;
  }


  void writenode(xmlDocPtr doc, Element *n, xmlNodePtr p, int root)
  {
    AttributeList props;
    AttributeIterator curprop;
    ElementList children;
    ElementIterator curchild;
    xmlNodePtr node;

    if (root)
      //      node = doc->children = xmlNewDocNode(doc, NULL,(xmlChar *) n->name().c_str(),(xmlChar *) n->content().c_str());
      node = doc->children = xmlNewDocNode(doc, NULL, (xmlChar *) n->name().c_str(), 0);
    else
      //      if (n->children().size()>0)
      //        node = xmlNewChild(p, NULL, (xmlChar *) n->name().c_str(),(xmlChar *) n->content().c_str());
      //      else
      node = xmlNewChild(p, NULL, (xmlChar *) n->name().c_str(), 0);

    if (n->isContent())
    {
      node->type = XML_TEXT_NODE;
      xmlNodeSetContentLen(node, (const xmlChar *) n->getContent().c_str(),
                           n->getContent().length());
    }

    props = n->getAttributeList();
    for (curprop = props.begin(); curprop != props.end(); curprop++)
      xmlSetProp(node, (xmlChar *) (*curprop)->name().c_str(),
                 (xmlChar *) (*curprop)->value().c_str());

    children = n->getElementList();
    for (curchild = children.begin(); curchild != children.end(); curchild++)
      writenode(doc, *curchild, node);
  }


  const std::string & Attribute::name() const
  {
    return _name;
  }


  const std::string & Attribute::value() const
  {
    return _value;
  }


  const std::string & Attribute::setValue(const std::string &v)
  {
    return _value = v;
  }


  std::string isolat1ToUTF8 (const std::string & inString)
  {
    int inlen = inString.size();
    int outlen = 4 * inlen;
    char * out = new char[outlen];
    out[0] = 0;

    ::isolat1ToUTF8((unsigned char*)out, &outlen, (unsigned char*)inString.c_str(), &inlen);

    std::string ret(out, outlen);
    delete [] out;

    return ret;
  }

  std::string UTF8Toisolat1(const std::string & inString)
  {
    int inlen = inString.size();
    int outlen = inlen;

    char * out = new char[outlen];
    out[0] = 0;

    ::UTF8Toisolat1((unsigned char*)out, &outlen, (unsigned char*)inString.c_str(), &inlen);

    std::string ret(out, outlen);
    delete [] out;

    return ret;
  }

};

