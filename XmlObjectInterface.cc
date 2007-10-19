// $Id$
//

#include "XmlObjectNode.h"

#include "logx/Logging.h"
#include "logx/system_error.h"

LOGGING("XmlObject");

#include <xercesc/sax/InputSource.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>

#include <fstream>

using namespace xercesc;
using namespace domx;
using logx::system_error;

namespace
{

  DOMNode*
  findChild (DOMNode* node, const std::string& name)
  {
    DOMNode* child = node->getFirstChild();
    while (child)
    {
      xstring cname = child->getNodeName();
      if (cname == name)
      {
	break;
      }
      child = child->getNextSibling();
    }
    return child;
  }


  DOMDocument*
  parse (const InputSource& source)
  {
    domx::xmlInitialize();
    static XercesDOMParser *parser = 0;
    if (!parser && !(parser = domx::createDefaultParser()))
    {
      return 0;
    }

    bool errorsOccured = false;
    try
    {
      parser->parse (source);
      DOMDocument* doc = parser->getDocument ();
      return doc;
    }
    catch (const XMLException& e)
    {
      ELOG << "An error occured during parsing\n   Message: "
	   << xstring(e.getMessage());
      errorsOccured = true;
    }
    catch (const xercesc::DOMException& e)
    {
      ELOG << "An error occured during parsing\n   Message: "
	   << xstring(e.msg);
      errorsOccured = true;
    }
    catch (...)
    {
      ELOG << "An error occured during parsing\n ";
      errorsOccured = true;
    }
    return 0;
  }
}


namespace domx
{

  /**
   * Every class in the hierarchy of a XmlObject keeps track of its
   * particular element in the XML document.
   **/
  struct XmlObjectNodeImpl : public XmlObjectNode
  {
    /// Pointer to the interface object to which this node belongs.
    XmlObjectInterface* _xi;
    DOMElement* _element;
    std::string _name;

    typedef std::vector<XmlObjectMemberBase*> member_list_t;
    member_list_t _members;

    void (*_construct)(XmlObjectInterface*);
    
    // Implement the virtual set and get interfaces to work on
    // this node's element.
    //
    virtual
    void
    getText (const xstring& name, xstring& value);

    virtual
    void
    setText (const xstring& name, const xstring& value);

    virtual
    void
    addMember (XmlObjectMemberBase* member);

    void
    construct();

    virtual
    ~XmlObjectNodeImpl();

  };

  typedef std::map<std::string,XmlObjectInterface*> interface_map_t;

  /**
   * The XmlObject implementation holds the document pointer and
   * keeps track of all the interfaces created for this object.
   **/
  struct XmlObject
  {
    DOMDocument* _doc;
    interface_map_t _interfaces;

    /**
     * The implementing interface is the owner of the XmlObject and its xml
     * document.  The lifetime of the XmlObject and all the facade
     * interfaces on it are tied to the life of the implementing interface.
     * The implementing interface is implicit in the existence of all
     * XmlObjects.
     **/
    XmlObjectInterface* _ximpl;

    XmlObject(XmlObjectInterface* xi) :
      _doc (0),
      _ximpl (xi)
    {}

    ~XmlObject()
    {
      if (_doc)
	_doc->release();
    }

    bool
    loadDocument (xercesc::InputSource& source)
    {
      DOMDocument* doc = parse (source);
      if (doc)
      {
	// doc->normalizeDocument();
	domx::pruneWhitespace (doc);
	return replaceDocument (doc);
      }
      return false;
    }

    bool
    replaceDocument (DOMDocument* doc)
    {
      if (doc)
      {
	if (_doc)
	  _doc->release();
	_doc = doc;
	return true;
      }
      return false;
    }

    bool
    createDocument ()
    {
      domx::xmlInitialize();

      DOMImplementation *impl = 
	DOMImplementationRegistry::getDOMImplementation(0);
      if (!impl)
      {
	ELOG << "could not get a DOM implementation";
	return false;
      }
      return replaceDocument (impl->createDocument (0, 0, 0));
    }

  };

}


XmlObjectNode::
~XmlObjectNode()
{}


XmlObjectInterface::
XmlObjectInterface () :
  _xo (0),
  _xi (newNode("xmlobject"))
{
}


void
XmlObjectInterface::
reset ()
{
  if (_xo && _xo->_doc)
  {
    if (_xo->createDocument())
    {
      updateInterfaces();
    }
  }
}


XmlObjectInterface& 
XmlObjectInterface::
operator= (const XmlObjectInterface& rhs_const)
{
  XmlObjectInterface& rhs = const_cast<XmlObjectInterface&>(rhs_const);
  if (rhs._xo && (rhs._xo == this->_xo))
  {
    return *this;
  }

  if (1)
  {
    // Copy the source implementation and load it.  One easy way to do that
    // is to translate to text and back.
    std::ostringstream oss;
    rhs.toXML (oss);
    this->fromXML (oss.str());
  }
  else
  {
    if (!rhs._xo) 
    {
      if (! rhs.createDocument())
	return *this;
    }
    // Force a fresh, empty document to import into.
    if (!_xo) _xo = new XmlObject(this);
    if (_xo->createDocument())
    {
      DOMNode* clone = 
	_xo->_doc->importNode (rhs._xo->_doc->getFirstChild(), /*deep*/true);
      _xo->_doc->appendChild (clone);
      updateInterfaces();
    }
  }
  return *this;
}


XmlObjectInterface& 
XmlObjectInterface::
assume (const XmlObjectInterface& rhs)
{
  *this = rhs;
  return *this;
}


bool
XmlObjectInterface::
createDocument()
{
  // Create the implementation object and document if there
  // isn't one already.  This is not meant to re-intialize the
  // document, only make sure it exists for methods which need
  // one to continue.
  if (!_xo) _xo = new XmlObject(this);
  if (!_xo->_doc)
  {
    if (! _xo->createDocument())
    {
      return false;
    }
    updateInterfaces();
  }
  return true;
}


void
XmlObjectInterface::
updateInterfaces()
{
  // For each interface on this object, including the implicit implementing
  // interface, tell the interface to setup its nodes on the current
  // document, presumably because the document was just created or has been
  // replaced.  However, no point to updating interfaces when document
  // doesn't exist yet.
  //
  if (_xo && _xo->_doc)
  {
    _xo->_ximpl->setupNodes();
    interface_map_t::iterator it;
    for (it = _xo->_interfaces.begin(); it != _xo->_interfaces.end(); ++it)
    {
      it->second->setupNodes();
    }
  }
}



void
XmlObjectInterface::
setupNodes ()
{
  // Setup all the subclass nodes on the new document.
  // Descend the node list either assigning or creating that
  // node's element in the new document.
  node_list_t::iterator it;
  node_list_t::iterator previous = _nodes.end();
  for (it = _nodes.begin(); it != _nodes.end(); ++it)
  {
    XmlObjectNodeImpl* node = *it;
    DOMNode* parent = _xo->_doc;
    if (previous != _nodes.end())
    {
      XmlObjectNodeImpl* basenode = *previous;
      parent = basenode->_element;
    }
    node->_element = asElement(findChild (parent, node->_name));
    if (! node->_element)
    {
      node->_element = _xo->_doc->createElement (xstring(node->_name));
      parent->appendChild (node->_element);
      node->construct ();
    }
    previous = it;
  }
}



XmlObjectInterface::
~XmlObjectInterface()
{
  // Delete all the subclass nodes. 
  node_list_t::iterator it;
  for (it = _nodes.begin(); it != _nodes.end(); ++it)
  {
    delete (*it);
  }
  _nodes.erase(_nodes.begin(), _nodes.end());

  // If this object owns the implementation, then we need to delete
  // all the interfaces and then the implementation itself.
  if (_xo && _xo->_ximpl == this)
  {
    interface_map_t::iterator it;
    for (it = _xo->_interfaces.begin(); it != _xo->_interfaces.end(); ++it)
    {
      delete (*it).second;
    }
    delete _xo;
  }
  _xo = 0;
}


// The fully qualified name of this interface is the concatenation
// of all of the node names in the class hierarchy.
std::string
XmlObjectInterface::
interfaceName ()
{
  std::string name;
  node_list_t::iterator it;
  for (it = _nodes.begin(); it != _nodes.end(); ++it)
  {
    if (name.length() > 0)
      name += '.';
    name += (*it)->_name;
  }
  return name;
}


XmlObjectNode*
XmlObjectInterface::
newNode (const std::string& name, void (*construct)(XmlObjectInterface*))
{
  // Create the node and stash the name in it for now.  If we ever create
  // an implementation then we'll actually create the element with the
  // given name.

  XmlObjectNodeImpl* node = new XmlObjectNodeImpl;
  node->_element = 0;
  node->_name = name;
  node->_xi = this;
  node->_construct = construct;
  _nodes.push_back (node);
  return node;
}


void
XmlObjectInterface::
addInterface (XmlObjectInterface* xi)
{
  if (xi->_xo)
  {
    delete xi->_xo;
  }
  // This object at least needs an implementation to register the
  // new interface, even if we don't create the document yet.
  if (!_xo) _xo = new XmlObject (this);
  xi->_xo = this->_xo;
  _xo->_interfaces.insert (std::make_pair(xi->interfaceName(), xi));
  updateInterfaces();
}


bool
XmlObjectInterface::
implements (XmlObjectInterface* xi)
{
  // Search the document for the hierarchy of nodes which
  // make up the given interface.  We can first search
  // our own nodes in case the document does not exist yet.

  node_list_t& matchnodes = xi->_nodes;
  node_list_t::iterator mi = matchnodes.begin();
  node_list_t::iterator ti = _nodes.begin();
  for ( ; mi != matchnodes.end() && ti != _nodes.end(); ++mi, ++ti)
  {
    if ((*mi)->_name != (*ti)->_name)
      break;
  }
  if (mi == matchnodes.end())
    return true;

  // Can't continue from here without a document.
  if (!_xo || !_xo->_doc)
  {
    return false;
  }

  DOMNode* child = _xo->_doc;
  mi = matchnodes.begin();
  while (child && mi != matchnodes.end())
  {
    child = findChild (child, (*mi)->_name);
    ++mi;
  }
  // The match succeeded if there was a child found for
  // all of the matchnodes.
  return (child && mi == matchnodes.end());
}


XmlObjectInterface*
XmlObjectInterface::
getInterface (const std::string& name)
{
  // They might be asking for this very interface by it's own name or one
  // of its base class interfaces.  We can answer this question even if
  // there is no implementation yet.
  //
  if (name == interfaceName())
    return this;

  // Without an implementation, there are no more known interfaces.
  if (!_xo)
    return 0;

  // It's possible they're asking for the name of the implementing
  // interface, which exists implicitly.
  if (_xo->_ximpl->interfaceName() == name)
  {
    return _xo->_ximpl;
  }

  // Check the map for a facade interface.
  XmlObjectInterface* xi = 0;
  interface_map_t::iterator it = _xo->_interfaces.find (name);
  if (it != _xo->_interfaces.end())
  {
    xi = it->second;
  }
  return xi;
}


bool
XmlObjectInterface::
toXML (std::ostream& out)
{
  if (! createDocument())
  {
    return false;
  }
  domToStream (out, _xo->_doc, _nodes[0]->_element, 0);
  return true;
}


std::string
XmlObjectInterface::
toString ()
{
  std::ostringstream out;
  if (! toXML(out))
    return "";
  return out.str();
}


bool
XmlObjectInterface::
fromXML (const std::string& in)
{
  MemBufInputSource source ((const XMLByte*)in.c_str(), in.length(),
			    "XmlObject::fromXML");
  if (!_xo) _xo = new XmlObject (this);
  if (_xo->loadDocument (source))
  {
    updateInterfaces();
    return true;
  }
  return false;
}


bool
XmlObjectInterface::
fromXML (std::istream& in)
{
  std::ostringstream data;
  data << in.rdbuf();
  return fromXML (data.str());
}


bool
XmlObjectInterface::
load (const std::string &filepath)
{
  xstring xpath (filepath);
  LocalFileInputSource source (xpath);
  if (!_xo) _xo = new XmlObject (this);
  if (_xo->loadDocument (source))
  {
    updateInterfaces();
    return true;
  }
  return false;
}


bool
XmlObjectInterface::
store (const std::string& filepath)
{
  std::ofstream out (filepath.c_str());
  if (! out)
  {
    ELOG << system_error("storing", filepath).what();
    return false;
  }
  this->toXML (out);
  return true;
}


void
XmlObjectNodeImpl::
construct()
{
  // Construct explicit members first
  for (member_list_t::iterator mi = _members.begin();
       mi != _members.end(); ++mi)
  {
    (*mi)->construct();
  }
  // Then call the subclass-specific constructor, if any.
  if (_construct)
    (*_construct)(_xi);
}



void
XmlObjectNodeImpl::
getText (const xstring& name, xstring& value)
{
  // We need an implementation to continue.
  if (! _xi->createDocument())
  {
    return;
  }
  DOMNode* child = findChild (_element, name);
  value = getTextElement (child);
}


void
XmlObjectNodeImpl::
setText (const xstring& name, const xstring& value)
{
  // We need an implementation to continue.
  if (! _xi->createDocument())
  {
    return;
  }
  // Now find the member node by this name else create it.
  DOMNode* child = findChild (_element, name);
  if (! child)
  {
    DOMDocument* doc = _element->getOwnerDocument();
    DOMElement* tnode = doc->createElement (name);
    tnode->appendChild (doc->createTextNode (value));
    _element->appendChild (tnode);
  }
  else
  {
    // The text node will always be the only child node.
    child->getFirstChild()->setNodeValue (value);
  }
}


XmlObjectNodeImpl::
~XmlObjectNodeImpl()
{}


void
XmlObjectNodeImpl::
addMember (XmlObjectMemberBase* member)
{
  _members.push_back (member);
}


XmlObjectMemberBase::
XmlObjectMemberBase (XmlObjectNode* node, const std::string& name) :
  _name (name),
  _node (node)
{
  _node->addMember (this);
}


XmlObjectMemberBase::
~XmlObjectMemberBase()
{
}

