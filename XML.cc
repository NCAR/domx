// $Id$

#include "XML.h"
#include "Logging.h"

#include <iostream>
#include <strstream>
#include <map>

using std::ostrstream;

ostream& operator<< (ostream& target, const DOMString& s)
{
  char *p = s.transcode();
  target << p;
  delete [] p;
  return target;
}


namespace
{
  log4cpp::CategoryStream::Separator endlog = log4cpp::CategoryStream::ENDLINE;
}

namespace domx
{

  bool
  nodeFindAttribute (Node node, DOMString name, string *value = 0)
  {
    DOM_NamedNodeMap alist = node.getAttributes();
    return mapFindAttribute (alist, name, value);
  }


  bool
  mapFindAttribute (DOM_NamedNodeMap alist, DOMString name, string *value = 0)
  {
    Node att;
    if ((alist != 0) && ((att = alist.getNamedItem (name)) != 0))
    {
      if (value)
      {
	DOMString val = att.getNodeValue ();
	char *cval = val.transcode ();
	*value = cval;
	delete cval;
      }
    }
    return att != 0;
  }


  string 
  ErrorFormatter::warning(const SAXParseException& toCatch)
  {
    ostrstream out;
    out << "Error at file \"" << DOMString(toCatch.getSystemId())
	<< "\", line " << toCatch.getLineNumber()
	<< ", column " << toCatch.getColumnNumber()
	<< "\n   Message: " 
	<< DOMString(toCatch.getMessage());
    return out.str();
  }

  string ErrorFormatter::error(const SAXParseException& toCatch)
  {
    ostrstream out;
    out << "Error at file \"" << DOMString(toCatch.getSystemId())
	<< "\", line " << toCatch.getLineNumber()
	<< ", column " << toCatch.getColumnNumber()
	<< "\n   Message: " 
	<< DOMString(toCatch.getMessage());
    return out.str();
  }

  string ErrorFormatter::fatalError(const SAXParseException& toCatch)
  {
    ostrstream out;
    out << "Error at file \"" << DOMString(toCatch.getSystemId())
	<< "\", line " << toCatch.getLineNumber()
	<< ", column " << toCatch.getColumnNumber()
	<< "\n   Message: " 
	<< DOMString(toCatch.getMessage());
    return out.str();
  }


  StreamErrorLogger::StreamErrorLogger (ErrorFormatter *fmt) :
    log (log4cpp::Category::getInstance ("XMLErrors")),
    mFormat (fmt)
  { }


  // -----------------------------------------------------------------------
  //  Implementation of the error handler interface
  // -----------------------------------------------------------------------
  void
  StreamErrorLogger::warning(const SAXParseException& toCatch)
  {
    ++mNumWarning;
    log.warnStream() << mFormat->warning(toCatch) << endlog;
    throw SAXParseException(toCatch);
  }

  void
  StreamErrorLogger::error(const SAXParseException& toCatch)
  {
    ++mNumError;
    log.errorStream() << mFormat->error(toCatch) << endlog;
    throw SAXParseException(toCatch);
  }

  void
  StreamErrorLogger::fatalError(const SAXParseException& toCatch)
  {
    ++mNumFatal;
    log.fatalStream() << mFormat->fatalError (toCatch) << endlog;
    throw SAXParseException(toCatch);
  }

  void
  StreamErrorLogger::resetErrors()
  {
    mNumWarning = 0;
    mNumError = 0;
    mNumFatal = 0;
  }


} // namespace domx


