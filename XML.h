// -*- mode: C++; c-basic-offset: 2; -*-
//
// $Id$

#ifndef _domx_XML_
#define _domx_XML_

#include <string>
#include <sstream>

// Includes for Xerces-C
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/sax/HandlerBase.hpp>

namespace log4cpp
{
  class Category;
}

// Our extensions to DOM reside in the DOMX namespace.
//
namespace domx
{

  /**
   * A class for easily interchanging between std::string and XMLCh*.  It
   * stores the current value as a std::string, then transcodes to XMLCh*
   * as needed.  Likewise the value of the string can be changed by
   * assigning XMLCh* to it.
   **/
  class xstring : public std::string
  {
  public:
    typedef std::string string;

    const XMLCh *
    xc() const
    {
      delete mxc;
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
      assign (xc);
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
      char *p = XMLString::transcode(xc);
      string::operator= (p);
      delete [] p;
      return *this;
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
  asElement (DOMNode* node);

  /**
   * Check the DOM node for the named attribute.  If the
   * attribute exists, return true.  If value is nonzero, then set it
   * to the value of the attribute.
   **/
  template <typename T>
  bool
  getAttribute (DOMNode* node, const xstring& name, T* value)
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
  getAttribute (DOMNode* node, const xstring& name, xstring *value);

  inline bool
  getAttribute (DOMNode* node, const char* name, xstring *value = 0)
  {
    return getAttribute<xstring> (node, xstring(name), value);
  }

  template <typename T>
  void
  setAttribute (DOMNode* node, const xstring& name, const T& value)
  {
    std::ostringstream os;
    os << value;
    setAttribute<xstring> (node, name, xstring(os.str()));
  }

  template <>
  void
  setAttribute (DOMNode* node, 
		const xstring& name, const xstring& value);

  void
  appendTextElement (DOMNode* node, const xstring& tag, const xstring& data);

  /**
   * Return the value of the child text node of the given node, or an
   * empty string if the child is not a text node or does not exist.
   **/
  std::string
  getTextElement (DOMNode* node);

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

};


#endif // _domx_XML_
