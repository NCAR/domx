

#include "XmlFileReference.h"
#include "XmlFileObject.h"

#include "logx/Logging.h"

LOGGING("XmlFileReference")

using namespace domx;



XmlFileReference::
XmlFileReference() :
  _xi(newNode("xmlfilereference")),
  Size (_xi, "size"),
  Modified (_xi, "modified"),
  MD5 (_xi, "md5")
{}




bool
XmlFileReference::
fileHasChanged()
{
  // Use the XmlObjectReference to first restore the referenced file.
  // If the source file does not exist any more, then no sense indicating
  // that it has changed.
  XmlFileObject xfo;
  if (!load (&xfo))
  {
    return false;
  }

  bool changed = (xfo.Modified() != Modified());
  changed = changed || (xfo.Size() != Size());
  if (xfo.MD5() != "" && xfo.MD5() != "")
  {
    changed = (changed || (xfo.MD5() != MD5()));
  }
  return changed;
}



bool 
XmlFileReference::
target (XmlObjectCatalog& source, const std::string& name)
{
  // First make the object reference.
  if (! XmlObjectReference::target (source, name))
  {
    return false;
  }

  // Now load the object and copy over attributes, depending upon
  // whether it's an actual file or another reference.
  XmlObjectInterface object;
  if (! load (&object))
  {
    return false;
  }
  XmlFileObject* xfo = object.getInterface<XmlFileObject> (false);
  XmlFileReference* ref = object.getInterface<XmlFileReference> (false);

  if (xfo && ref)
  {
    ELOG << "referenced object has both FileObject and FileReference"
	 << " interfaces!";
  }
      
  if (xfo)
  {
    MD5 = xfo->MD5();
    Modified = xfo->Modified();
    Size = xfo->Size();
  }
  else if (ref)
  {
    MD5 = ref->MD5();
    Modified = ref->Modified();
    Size = ref->Size();
  }
  else
  {
    ELOG << "referenced object is neither a FileObject nor a Reference";
    return false;
  }
  return true;
}

