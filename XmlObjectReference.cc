

#include "XmlObjectReference.h"
#include "XmlObjectCatalog.h"

#include <time.h>

using namespace domx;


XmlObjectReference::    
XmlObjectReference() :
  _xi(newNode("xmlobjectreference")),
  Timestamp (_xi, "timestamp"),
  Catalog (_xi, "catalog"),
  Name (_xi, "name")
{}



bool
XmlObjectReference::    
target (XmlObjectCatalog& source, const std::string& name)
{
  Catalog = source.name();
  Name = name;
  Timestamp = time(0);
  return true;
}



bool
XmlObjectReference::    
load (XmlObjectInterface* object)
{
  // First open the object's catalog.
  XmlObjectCatalog catalog;
  if (!catalog.open (Catalog()))
  {
    return false;
  }
  return catalog.load (Name(), object);
}



std::string
XmlObjectReference::    
timekey ()
{
  return Timestamp().key() + "-" + Name();
}


