// $Id$

#include "XML.h"
#include <logx/Logging.h>

#include <iostream>
#include <map>

#include <xercesc/util/PlatformUtils.hpp>

LOGGING("domx");

using std::string;

namespace domx
{

  bool
  xmlInitialize ()
  {
    static bool initialized = false;

    if (initialized)
      return true;

    // Initialize the XML4C2 system
    try
    {
      XMLPlatformUtils::Initialize();
      initialized = true;
    }
    catch (const XMLException& xe)
    {
      ELOG << "Error during Xerces-c Initialization.\n"
	   << "  Exception message:"
	   << xe.getMessage() << endlog;
    }

    return initialized;
  }


  XercesDOMParser *
  createDefaultParser()
  {
    if (!domx::xmlInitialize ())
      return 0;
    //
    //  Create our parser, then attach an error handler to the parser.
    //  The parser will call back to methods of the ErrorHandler if it
    //  discovers errors during the course of parsing the XML document.
    //
    XercesDOMParser *parser;
    parser = new XercesDOMParser;
    parser->setValidationScheme(XercesDOMParser::Val_Auto);
    parser->setDoNamespaces(false);
    ErrorHandler *errReporter = 
      new StreamErrorLogger();
    parser->setErrorHandler(errReporter);
    parser->setCreateEntityReferenceNodes(false);
    return parser;
  }


  void
  appendTextElement (xercesc::DOMNode* node, const xstring& tag, const xstring& data)
  {
    DOMDocument* doc = node->getOwnerDocument();
    DOMElement* tnode = doc->createElement (tag);
    tnode->appendChild (doc->createTextNode (data));
    node->appendChild (tnode);
  }


  std::string
  getTextElement (xercesc::DOMNode* node)
  {
    xstring value;
    xercesc::DOMNode* child = node->getFirstChild();
    while (child)
    {
      if (child->getNodeType() == xercesc::DOMNode::TEXT_NODE)
      {
	value = child->getNodeValue();
      }
      child = child->getNextSibling();
    }
    return value;
  }


  DOMElement*
  asElement (xercesc::DOMNode* node)
  {
    DOMElement* enode = 0;
    if (node->getNodeType() == xercesc::DOMNode::ELEMENT_NODE)
    {
      enode = (DOMElement *)node;
    }
    return enode;
  }


  template <>
  void
  setAttribute (xercesc::DOMNode* node, const xstring& name, const xstring& value)
  {
    DOMElement* enode = asElement (node);
    if (enode != 0)
      enode->setAttribute (name, value);
  }


  template <>
  bool
  getAttribute (xercesc::DOMNode* node, const xstring& name, xstring *value)
  {
    bool found = false;
    if (node->getNodeType() == xercesc::DOMNode::ELEMENT_NODE)
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
    std::ostringstream out;
    out << "Error at file \"" << xstring(toCatch.getSystemId())
	<< "\", line " << toCatch.getLineNumber()
	<< ", column " << toCatch.getColumnNumber()
	<< "\n   Message: " 
	<< xstring(toCatch.getMessage());
    return out.str();
  }

  string ErrorFormatter::error(const SAXParseException& toCatch)
  {
    std::ostringstream out;
    out << "Error at file \"" << xstring(toCatch.getSystemId())
	<< "\", line " << toCatch.getLineNumber()
	<< ", column " << toCatch.getColumnNumber()
	<< "\n   Message: " 
	<< xstring(toCatch.getMessage());
    return out.str();
  }

  string ErrorFormatter::fatalError(const SAXParseException& toCatch)
  {
    std::ostringstream out;
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


