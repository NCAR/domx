// -*- C++ -*-
//
// $Id$
//

#ifndef _domx_XmlObject_h_
#define _domx_XmlObject_h_

#include <string>
#include <map>
#include <vector>
#include <iosfwd>

namespace domx
{

  class XmlObject;
  class XmlObjectNode;
  class XmlObjectNodeImpl;

  class XmlObjectInterface
  {
  public:

    /**
     * Return the interface name.
     **/
    std::string
    interfaceName ();

    /**
     * Dump this object to the given stream as xml-formatted text.
     **/
    bool
    toXML (std::ostream& out);

    /**
     * Translate this object to a string in XML format.  If the translation
     * fails, the returned string is empty.
     **/
    std::string
    toString ();

    /**
     * Load this object from the XML document contained in the given string
     * @p in.  If the document cannot be parsed, then this method returns
     * false and the current document is not changed.  Otherwise returns
     * true.
     **/
    bool
    fromXML (const std::string& in);

    /**
     * Load an object from the given input stream @p in.  Returns true
     * on success and false otherwise.
     **/
    bool
    fromXML (std::istream& in);

    /**
     * Load this object from the XML file located at @p filepath.  
     * If the file cannot be read or the XML document cannot be parsed,
     * then this method returns false and the current document is not
     * changed.
     **/
    bool
    load (const std::string& filepath);

    /**
     * Write this object to the file at @p filepath as an XML document.
     * Return false if the write fails, otherwise return true.
     **/
    bool
    store (const std::string& filepath);

    /**
     * A derived interface can be assigned to a base interface, but all of
     * the state will be copied, including the state attached to the
     * interfaces being 'sliced off'.  This means it is possible to recover
     * the information in more-derived interfaces by assuming the base
     * interface back into the more derived interface using assume().
     **/
    XmlObjectInterface& operator= (const XmlObjectInterface& rhs);

    /**
     * Assigning one interface to another copies the XML document from @p
     * rhs and extends the copied document to implement all of the
     * interfaces attached to this implementation.  This is the method
     * needed when assigning a base instance to a derived class.
     * Assignment from a derived interface to a base interface can use the
     * usual assignment operator.
     **/
    XmlObjectInterface& assume (const XmlObjectInterface& rhs);

    /**
     * Create an empty XML object.  The creation of the implementation (the
     * XML document) is deferred until actually needed, so that it's cheap
     * to create a default XmlObjectInterface and then assign to it from
     * another interface instance or from an XML document.
     **/
    XmlObjectInterface ();

    virtual
    ~XmlObjectInterface();

    /**
     * Return a subclass interface on the given existing xml object @p xo.
     * If no such interface exists yet and @p create is true, then create
     * the interface, meaning this interface's fields will be added to the
     * given object's document.
     *
     * If the interface cannot be found and create is false, then return
     * null.
     **/
    template <typename T>
    T*
    getInterface (bool create);

    /**
     * Return true if this interface document has nodes matching the given
     * interface @p xi.  In other words, returns true if this object is
     * known to implement the given inteface.
     **/
    bool
    implements (XmlObjectInterface* xi);

    /**
     * Look for an existing interface with the given @p name.  Returns null
     * if the interface is not found.  The returned interface could be
     * either this object itself or a facade interface already attached by
     * this name.
     **/
    XmlObjectInterface*
    getInterface (const std::string& name);


    /**
     * Re-initialize this object by reconstructing its document from
     * scratch.  Any current interfaces onto this object remain intact and
     * valid.
     **/
    void
    reset();

  protected:

    /**
     * Replace the implementation of the interface @p xi with this one,
     * meaning the current object document will be shared with this new
     * interface.  The implementation takes ownership of the pointer and
     * will delete it when the implementing interface is deleted.
     **/
    void
    addInterface (XmlObjectInterface* xi);

    /**
     * Each subclass constructor calls this method to setup its node in the
     * class hierarchy.  This method should only be called during
     * construction.  Once the nodes are setup, this base class uses them
     * to make sure all the nodes for each subclass are added to the
     * document implementation.  When a document node needs to be created,
     * the given construction function is called to populate the node with
     * its members.
     *
     * If a node has members besides XmlObjectMember, then the node must
     * have a construct() method to populate the new element with those
     * members and their initial values.  XmlObjectMember members will be
     * constructed directly through their construct() method.  If a
     * subclass node has only XmlObjectMember members, then the construct
     * pointer can be passed as null.  The XmlObjectMember members will
     * constructed first, in the order they were added to the node.
     **/
    XmlObjectNode*
    newNode (const std::string& name, 
	     void (*construct)(XmlObjectInterface*) = 0);

  private:

    friend class XmlObjectNodeImpl;

    // Copy construction is not allowed.  Instead, create the default
    // interface and assign another interface to it with the assignment
    // operator.
    //
    XmlObjectInterface (const XmlObjectInterface& rhs);

    bool
    createDocument();

    void
    updateInterfaces();

    void
    setupNodes ();

    /**
     * The set of subclass nodes which will be deleted automatically for
     * the subclasses.  Each interface keeps its own set of nodes, unlike
     * interfaces which are held by the sole implementation object.  This
     * must be defined before _xi since _xi is initialized with newNode()
     * and newNode() modifies _nodes.
     **/
    typedef std::vector<XmlObjectNodeImpl*> node_list_t;
    node_list_t _nodes;

    /// The implementation pointer.
    XmlObject* _xo;

    /**
     * The toplevel node pointer, corresponding to the XML document element
     * 'object'.
     **/
    XmlObjectNode* _xi;

  };

  template <typename T>
  T*
  XmlObjectInterface::
  getInterface (bool create)
  {
    T* ip = new T();
    // First look for an existing interface by this interface's name.
    XmlObjectInterface* existing = this->getInterface(ip->interfaceName());
    if (existing)
    {
      delete ip;
      return static_cast<T*> (existing);
    }

    // It's possible the implementation for this interface exists but
    // the facade interface for it has not been created yet.  So check for
    // the interface in the document itself

    if (! this->implements (ip) && !create)
    {
      delete ip;
      return 0;
    }
    this->addInterface (ip);
    return ip;
  }


}

#endif // _domx_XmlObject_h_

