// -*- C++ -*-
//
// $Id$
//
#ifndef _domx_XmlFileObject_h_
#define _domx_XmlFileObject_h_

#include "XmlObjectNode.h"
#include "XmlTime.h"

#include <string>

namespace domx
{

  class XmlFileObject : public XmlObjectInterface
  {
    XmlObjectNode* _xi;

  public:

    /**
     * The name component of the file path.
     **/
    XmlObjectMember<std::string> Name;

    /**
     * The directory component of the file path.
     **/
    XmlObjectMember<std::string> Directory;

    /**
     * The size of the file, which may or may not be set by the
     * application.
     **/
    XmlObjectMember<unsigned long> Size;

    /**
     * The time the file was created.
     **/
    XmlObjectMember<XmlTime> Created;

    XmlFileObject() :
      _xi(newNode("xmlfileobject")),
      Name (_xi, "filename"),
      Directory (_xi, "directory"),
      Size (_xi, "size"),
      Created (_xi, "created")
    {}


    /**
     * Generate a key from this file's timetag and filename so that
     * keys will be sorted in time order by XmlObjectCatalog but
     * will still be unique if different files are inserted with the
     * same time.
     **/
    std::string
    timekey()
    {
      return Created().key() + "-" + Name();
    }

    /**
     * Return the full path to this file.
     **/
    std::string
    fullpath()
    {
      return Directory() + "/" + Name();
    }
      

  };


}



#endif // _domx_XmlFileObject_h_

