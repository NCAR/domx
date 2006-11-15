// -*- mode: C++; c-basic-offset: 2; -*-
//
// $Id: XML.h 5232 2006-04-19 17:17:52Z cjw $

#ifndef _domxfwd_XML_
#define _domxfwd_XML_

#include <iosfwd>
#include <string>

#include <xercesc/util/XercesDefs.hpp>

namespace domx_xercesc = XERCES_CPP_NAMESPACE;

namespace XERCES_CPP_NAMESPACE
{
  class DOMNode;
  class DOMDocument;
  class DOMElement;
  class DOMError;
  class DOMErrorHandler;
  class XMLString;
  class XMLException;
  class XMLPlatformUtils;
  class SAXParseException;
  class ErrorHandler;
  class XercesDOMParser;
}

// Our extensions to DOM reside in the DOMX namespace.
//
namespace domx
{
  using domx_xercesc::DOMNode;
  using domx_xercesc::DOMDocument;
  using domx_xercesc::DOMElement;
  using domx_xercesc::DOMError;
  using domx_xercesc::DOMErrorHandler;

  using domx_xercesc::XMLString;
  using domx_xercesc::XMLException;
  using domx_xercesc::XMLPlatformUtils;

  using domx_xercesc::SAXParseException;
  using domx_xercesc::ErrorHandler;

  using domx_xercesc::XercesDOMParser;

  /**
   * Make sure the Xerces-C library initialization routine has been called.
   * This routine does nothing if initialization has already succeeded in a
   * prior call.
   **/
  bool
  xmlInitialize ();

  /**
   * Return a basic parser, taking care to first call xmlInitialize(),
   * and also setting up a simple error rerporter.
   **/
  XercesDOMParser *
  createDefaultParser();

  class xstring;

  /**
   * Convenience function which tests whether node is an element node
   * and returns a DOMElement pointer to that node if so.  Otherwise
   * returns null.
   **/
  DOMElement*
  asElement (domx_xercesc::DOMNode* node);

  /**
   * Look for a child element of the given @p node along the path @p path.
   * So far only paths for immediate child nodes are supported.
   **/
  DOMElement*
  findElement(DOMNode* node, const std::string& path);

  /**
   * Look for then next child element of the given @p sib along the
   * path @p path.  So far only paths for immediate child nodes are
   * supported.
   **/
  DOMElement*
  findNextElement(DOMNode* sib, const std::string& path);

  void
  appendTextElement (domx_xercesc::DOMNode* node, const xstring& tag, const xstring& data);

  /**
   * Return the value of the child text node of the given node, or an
   * empty string if the child is not a text node or does not exist.
   **/
  std::string
  getTextElement (domx_xercesc::DOMNode* node);

  /**
   * Write the given XML document @p doc as text to the ostream @p out,
   * beginning with the document element @p node and using @p indent as the
   * number of spaces to indent the node.
   **/
  std::ostream&
  domToStream (std::ostream& out, DOMDocument* doc, DOMNode* node, 
	       int indent = 2);

  /**
   * Same as the other domToStream(), except the document is retrieved
   * by calling getOwnerDocument() on the @p node.
   **/
  std::ostream&
  domToStream (std::ostream& out, DOMNode* node, int indent = 2);

  /**
   * Prune all text nodes which are empty or have only whitespace, and trim
   * leading and trailing whitespace from all other text nodes.
   **/
  void
  pruneWhitespace (DOMNode* node);

  class ErrorFormatter;

};


#endif // _domx_XML_
