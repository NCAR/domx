// $Id$

#include "XML.h"
#include <logx/Logging.h>

#include <iostream>
#include <map>

#include <xercesc/util/PlatformUtils.hpp>

LOGGING("domx");

using std::string;
using std::endl;

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
    if (node && (node->getNodeType() == xercesc::DOMNode::ELEMENT_NODE))
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

  std::ostream&
  domToStream (std::ostream& out, DOMDocument* doc, DOMNode* node, int indent)
  {
    xstring node_name = node->getNodeName();
    std::string tab(indent, ' ');
    out << tab << "<" << node_name;

    xercesc::DOMNamedNodeMap* attmap = node->getAttributes();
    for (XMLSize_t i = 0; attmap != 0 && i < attmap->getLength(); ++i)
    {
      DOMNode *attnode = attmap->item (i);
      out << " " << xstring(attnode->getNodeName()) << "='"
	  << xstring(attnode->getNodeValue()) << "'";
    }

    // If node type is text, then check length and surround it by
    // the element tags on separate lines.
    DOMNode* child = node->getFirstChild();
    bool first = true;
    while (child)
    {
      if (first)
	out << ">";
      tab = string(indent, ' ');
      if (child->getNodeType () == DOMNode::TEXT_NODE)
      {
	xstring text = child->getNodeValue();
	if (text.length() + (unsigned)indent > 72)
	{
	  out << std::endl << tab << text << std::endl;
	}
	else
	{
	  out << text;
	  tab = "";
	}
      }
      else
      {
	if (first)
	  out << std::endl;
	domToStream (out, doc, child, indent+2);
      }
      first = false;
      child = child->getNextSibling ();
    }
    if (first)
    {
      // no children
      out << "/>" << std::endl;
    }
    else
    {
      out << tab << "</" << node_name << ">" << endl;
    }
    return out;
  }


  void
  trimTextNode (DOMNode* node, DOMNode* tnode)
  {
    xstring text = tnode->getNodeValue();
    DLOG << "found text node, value:" << text;
    string::size_type length = text.length();

    // trim leading and trailing whitespace
    text.erase(0, text.find_first_not_of(" \t\n"));
    text.erase(text.find_last_not_of(" \t\n")+1);

    // If the text is now empty, then delete the node.  Otherwise
    // replace the text if it changed.
    if (text.length() == 0)
    {
      DLOG << "pruning text node";
      node->removeChild (tnode);
      tnode->release();
    }
    else if (text.length() != length)
    {
      DLOG << "changing text node: " << text;
      tnode->setNodeValue (text);
    }
    else
    {
      DLOG << "text node unchanged.";
    }
  }


  void
  pruneWhitespace (DOMNode* node)
  {
    DLOG << "entering pruneWhitespace";
    if (! node)
      return;
    DOMNode* child = node->getFirstChild();
    while (child)
    {
      xstring node_name = child->getNodeName();
      DLOG << "checking node " << node_name;
      DOMNode* tnode = child;
      child = child->getNextSibling ();
      if (tnode->getNodeType () == DOMNode::TEXT_NODE)
      {
	trimTextNode (node, tnode);
      }
      else
      {
	pruneWhitespace (tnode);
      }
    }
    DLOG << "leaving pruneWhitespace";
  }


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


  ErrorFormatter::
  ~ErrorFormatter()
  {}

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


