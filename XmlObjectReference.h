// -*- C++ -*-
//
// $Id$
//
#ifndef _domx_XmlObjectReference_h_
#define _domx_XmlObjectReference_h_

#include "XmlObjectNode.h"
#include "XmlTime.h"

#include <string>

namespace domx
{

  class XmlObjectCatalog;

  /**
   * Store a reference to an object in a catalog using the catalog name and
   * the object key (name), like a database table relation.  The catalog
   * name and the object name together form a global key for the object.
   **/
  class XmlObjectReference : public XmlObjectInterface
  {
    XmlObjectNode* _xi;

    XmlObjectMember<XmlTime> Timestamp;

  public:

    /**
     * The name of the object's catalog.
     **/
    XmlObjectMember<std::string> Catalog;

    /**
     * The object name.
     **/
    XmlObjectMember<std::string> Name;

    /**
     * Return the timestamp of this reference.  The timestamp records the
     * absolute time when this reference was targeted to an object.  See
     * target().  The value is seconds since the unix epoch.  Note this
     * value is not enough to be absolutely sure whether the target has
     * changed since it was referenced, since the resolution is only
     * seconds.
     **/
    int
    getTimestamp()
    {
      return Timestamp();
    }

    XmlObjectReference();

    /**
     * Setup this reference to point to the given target.
     **/
    bool
    target (XmlObjectCatalog& source, const std::string& name);


    /**
     * Load the object given by this reference.
     **/
    bool
    load (XmlObjectInterface* object);


    /**
     * Return a key for this object reference based on the object
     * name and the timestamp.  The time key can be used in a queue
     * catalog to order the object in the queue by time.  The name is
     * included in the key so that objects with similar times still
     * have unique keys.
     **/
    std::string
    timekey ();


  };


}



#endif // _domx_XmlObjectReference_h_

