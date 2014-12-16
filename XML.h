// -*- mode: C++; c-basic-offset: 2; -*-
//
// $Id$

#ifndef _domx_XML_
#define _domx_XML_

#include "domxfwd.h"

#include <string>
#include <sstream>

// Includes for Xerces-C
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>

namespace log4cpp
{
  class Category;
}

// Our extensions to DOM reside in the DOMX namespace.
//
namespace domx
{

  using xercesc::DOMNode;
  using xercesc::DOMDocument;
  using xercesc::DOMElement;
  using xercesc::DOMError;
  using xercesc::DOMErrorHandler;

  using xercesc::XMLString;
  using xercesc::XMLException;
  using xercesc::XMLPlatformUtils;

  using xercesc::SAXParseException;
  using xercesc::ErrorHandler;

  using xercesc::XercesDOMParser;

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

  /**
   * A class for easily interchanging between std::string and XMLCh*.  It
   * stores the current value as a std::string as transcoded by
   * XMLString::transcode(), and it can transcode std::string back to
   * XMLCh* as needed.  The XMLCh* string is allocated by xerces but owned
   * by the xstring instance, so it is only valid for the lifetime of the
   * instance and until Xerces has been terminated.
   *
   * When the DOM interface returns XMLCh* strings, often those strings are
   * internal to the DOMNode and do not need to be released.  This instance
   * does not hold the pointers to those strings so the lifetime is not
   * tied to the DOMNode.
   *
   * For XMLCh* strings which need to be released, this class provides the
   * take() method.  That method is like assign(), except this instance
   * takes ownership of the XMLCh* string and makes sure it gets released.
   **/
  class xstring : public std::string
  {
  public:
    typedef std::string string;

    const XMLCh *
    xc() const
    {
      xercesc::XMLString::release(&mxc);
      mxc = XMLString::transcode(this->c_str());
      return mxc;
    }

    operator const XMLCh *() const
    {
      return xc();
    }

    xstring (const XMLCh *xc)
    {
      mxc = 0;
      assign(xc);
    }

    explicit
    xstring (XMLCh *xc, bool take_=false)
    {
      mxc = 0;
      if (!take_)
	assign(xc);
      else
	take(xc);
    }

    xstring ()
    {
      mxc = 0;
    }

    xstring (const char s[])
    {
      mxc = 0;
      string::operator= (s);
    }

    xstring (const xstring &xs) : string (xs)
    {
      mxc = 0;
    }

    xstring (const string &xs) : string (xs)
    {
      mxc = 0;
    }

    xstring &operator= (const XMLCh *xc)
    {
      return assign (xc);
    }

    xstring &operator= (const xstring &xs)
    {
      string::operator= (xs);
      return *this;
    }

    xstring &assign (const XMLCh *xc)
    {
      if (xc)
      {
	char *p = XMLString::transcode(xc);
	string::operator= (p);
        xercesc::XMLString::release(&p);
      }
      else
      {
	string::operator= ("");
      }
      return *this;
    }

    xstring& take(XMLCh* xc)
    {
      this->assign(xc);
      if (xc)
      {
	xercesc::XMLString::release(&xc);
      }
      return *this;
    }

    ~xstring ()
    {
      xercesc::XMLString::release(&mxc);
      mxc = 0;
    }

  private:
    mutable XMLCh *mxc;
  };


  /**
   * Convenience function which tests whether node is an element node
   * and returns a DOMElement pointer to that node if so.  Otherwise
   * returns null.
   **/
  DOMElement*
  asElement (xercesc::DOMNode* node);

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

  /**
   * Check the DOM node for the named attribute.  If the
   * attribute exists, return true.  If value is nonzero, then set it
   * to the value of the attribute.
   **/
  template <typename T>
  bool
  getAttribute (xercesc::DOMNode* node, const xstring& name, T* value)
  {
    xstring xvalue;
    if (getAttribute<xstring> (node, name, &xvalue))
    {
      if (value)
      {
	std::istringstream os (xvalue);
	os >> *value;
      }
      return true;
    }
    return false;
  }

  template <>
  bool
  getAttribute (xercesc::DOMNode* node, const xstring& name, xstring *value);

#ifdef notdef
  inline bool
  getAttribute (xercesc::DOMNode* node, const char* name, xstring *value = 0)
  {
    return getAttribute<xstring> (node, xstring(name), value);
  }
#endif

  template <typename T>
  void
  setAttribute (xercesc::DOMNode* node, const xstring& name, const T& value)
  {
    std::ostringstream os;
    os << value;
    setAttribute<xstring> (node, name, xstring(os.str()));
  }

  template <>
  void
  setAttribute (xercesc::DOMNode* node, 
		const xstring& name, const xstring& value);

  void
  appendTextElement (xercesc::DOMNode* node, const xstring& tag, const xstring& data);

  /**
   * Return the value of the child text node of the given node, or an
   * empty string if the child is not a text node or does not exist.
   **/
  std::string
  getTextElement (xercesc::DOMNode* node);

  /**
   * Check the DOM node for the named element with a text element child.
   * If the element exists, check it against the current value before setting 
   * the new value.  Return true if the new value is different.
   **/
  template <typename T>
  bool
  getValue (xercesc::DOMNode* node, const xstring& name, T& value);

  template <>
  bool
  getValue (xercesc::DOMNode* node, const xstring& name, xstring& value);

  template <typename T>
  bool
  getValue (xercesc::DOMNode* node, const xstring& name, T& value)
  {
    xstring xvalue;
    bool result = false;
    if (getValue<xstring> (node, name, xvalue))
    {
      T newvalue;
      std::istringstream os (xvalue);
      os >> newvalue;
      result = (value != newvalue);
      value = newvalue;
    }
    return result;
  }

#ifdef notdef
  inline bool
  getValue (xercesc::DOMNode* node, const char* name, xstring& value)
  {
    return getValue<xstring> (node, xstring(name), value);
  }
#endif

  template <typename T>
  void
  setValue (xercesc::DOMNode* node, const xstring& name, const T& value)
  {
    std::ostringstream os;
    os << value;
    setValue<xstring> (node, name, xstring(os.str()));
  }

  template <>
  void
  setValue (xercesc::DOMNode* node, 
	    const xstring& name, const xstring& value);

  /**
   * Write the given XML document @p doc as text to the ostream @p out,
   * beginning with the document element @p node and using @p indent as the
   * number of spaces to indent the node.
   **/
  std::ostream&
  domToStream (std::ostream& out, DOMDocument* doc, DOMNode* node, 
	       int indent);

  /**
   * Same as the other domToStream(), except the document is retrieved
   * by calling getOwnerDocument() on the @p node.
   **/
  std::ostream&
  domToStream (std::ostream& out, DOMNode* node, int indent);

  /**
   * Prune all text nodes which are empty or have only whitespace, and trim
   * leading and trailing whitespace from all other text nodes.
   **/
  void
  pruneWhitespace (DOMNode* node);

  class ErrorFormatter
  {
  public:
    typedef std::string string;

    // I purposefully decided to leave these non-const to give subclasses
    // more flexibility, such as allowing each formatted string to include
    // the current error count.  The implication is that we can't use a
    // shared default error formatter in ErrorHandler classes like
    // StreamErrorLogger.
    //
    virtual string warning(const SAXParseException& toCatch);
    virtual string error(const SAXParseException& toCatch);
    virtual string fatalError(const SAXParseException& toCatch);
    virtual ~ErrorFormatter();
  };


  class StreamErrorLogger : public ErrorHandler
  {
  public:

    // We take ownership of the ErrorFormatter and will delete it 
    // when destroyed.
    //
    StreamErrorLogger (ErrorFormatter *fmt = new ErrorFormatter());

    // ------------------------------------------------------------------
    //  Implementation of the error handler interface
    // ------------------------------------------------------------------
    void warning(const SAXParseException& toCatch);
    void error(const SAXParseException& toCatch);
    void fatalError(const SAXParseException& toCatch);
    void resetErrors();

    ~StreamErrorLogger () { delete mFormat; }

  private:
    StreamErrorLogger (const StreamErrorLogger &);
    StreamErrorLogger &operator= (const StreamErrorLogger &);

    log4cpp::Category &log;
    int mNumFatal;
    int mNumError;
    int mNumWarning;
    ErrorFormatter *mFormat;

  };


  class LogErrorHandler : public DOMErrorHandler
  {
  public:

    LogErrorHandler(log4cpp::Category &log);

    virtual bool
    handleError (const DOMError &domError);

  private:

    log4cpp::Category &mLog;

  };

}


#endif // _domx_XML_
