// -*- mode: C++; c-basic-offset: 2; -*-
//
// $Id$

#ifndef _domx_XML_
#define _domx_XML_

#include <string>
#include <sstream>

// Includes for Xerces-C
//#include <util/XercesDefs.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/sax/HandlerBase.hpp>

//#include <dom/DOM_NodeList.hpp>
//#include <sax/SAXParseException.hpp>
//#include <sax/ErrorHandler.hpp>

#ifdef notdef
class ostream;
ostream& operator<< (ostream& target, const DOMString& s);
#endif

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
   * Check the DOM node for the named attribute.  If the
   * attribute exists, return true.  If value is nonzero, then set it
   * to the value of the attribute.
   **/
  bool
  getAttribute (DOMNode* node, const XMLCh* name, xstring *value = 0);

  inline bool
  getAttribute (DOMNode* node, const char* name, xstring *value = 0)
  {
    return getAttribute(node, xstring(name), value);
  }

  inline void
  setAttribute (DOMElement* node, const xstring& name, const xstring& value)
  {
    node->setAttribute (name, value);
  }

  template <typename T>
  void
  setAttribute (DOMElement* node, const xstring& name, T value)
  {
    ostringstream os;
    os << value;
    node->setAttribute (name, xstring(os.str()));
  }

  void
  appendTextElement (DOMNode* node, const xstring& tag, const xstring& data);


  /**
   * Convenience function which tests whether node is an element node
   * and returns a DOMElement pointer to that node if so.  Otherwise
   * returns null.
   **/
  DOMElement*
  asElement (DOMNode* node);



  class ErrorFormatter
  {
  public:
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
