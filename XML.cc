// $Id$

#include "XML.h"
#include "Logging.h"

#include <iostream>
#include <map>


namespace
{
  log4cpp::CategoryStream::Separator endlog = log4cpp::CategoryStream::ENDLINE;
}


namespace domx
{
  void
  appendTextElement (DOMNode* node, const xstring& tag, const xstring& data)
  {
    DOMDocument* doc = node->getOwnerDocument();
    DOMElement* tnode = doc->createElement (tag);
    tnode->appendChild (doc->createTextNode (data));
    node->appendChild (tnode);
  }


  DOMElement*
  asElement (DOMNode* node)
  {
    DOMElement* enode = 0;
    if (node->getNodeType() == DOMNode::ELEMENT_NODE)
    {
      enode = (DOMElement *)node;
    }
    return enode;
  }


  template <>
  void
  setAttribute (DOMNode* node, const xstring& name, const xstring& value)
  {
    DOMElement* enode = asElement (node);
    if (enode != 0)
      enode->setAttribute (name, value);
  }


  template <>
  bool
  getAttribute (DOMNode* node, const xstring& name, xstring *value)
  {
    bool found = false;
    if (node->getNodeType() == DOMNode::ELEMENT_NODE)
    {
      DOMElement* enode = (DOMElement *)node;
      found = enode->hasAttribute (name);
      if (found && value)
	*value = enode->getAttribute (name);
    }
    return found;
  }


#ifdef notdef
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
#endif

  string 
  ErrorFormatter::warning(const SAXParseException& toCatch)
  {
    ostringstream out;
    out << "Error at file \"" << xstring(toCatch.getSystemId())
	<< "\", line " << toCatch.getLineNumber()
	<< ", column " << toCatch.getColumnNumber()
	<< "\n   Message: " 
	<< xstring(toCatch.getMessage());
    return out.str();
  }

  string ErrorFormatter::error(const SAXParseException& toCatch)
  {
    ostringstream out;
    out << "Error at file \"" << xstring(toCatch.getSystemId())
	<< "\", line " << toCatch.getLineNumber()
	<< ", column " << toCatch.getColumnNumber()
	<< "\n   Message: " 
	<< xstring(toCatch.getMessage());
    return out.str();
  }

  string ErrorFormatter::fatalError(const SAXParseException& toCatch)
  {
    ostringstream out;
    out << "Error at file \"" << xstring(toCatch.getSystemId())
	<< "\", line " << toCatch.getLineNumber()
	<< ", column " << toCatch.getColumnNumber()
	<< "\n   Message: " 
	<< xstring(toCatch.getMessage());
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


  LogErrorHandler::LogErrorHandler(log4cpp::Category &log) :
    mLog (log)
  {
  }


  bool
  LogErrorHandler::handleError (const DOMError &domError)
  {
    char *msg = XMLString::transcode(domError.getMessage());
    mLog.errorStream() << msg << endlog;
    delete[] msg;

    // Instructs the serializer to continue serialization if possible.
    return true;
  }


} // namespace domx


