// -*- C++ -*-
//
// $Id$
//
#ifndef _domx_XmlFileReference_h_
#define _domx_XmlFileReference_h_

#include "XmlObjectNode.h"
#include "XmlTime.h"
#include "XmlObjectReference.h"

#include <string>

namespace domx
{

  /**
   * Store a reference to a file from a source catalog, and also store a
   * few fields copied from that file when the reference was made so the
   * reference can detect when its outdated.  XmlObjectReferences are the
   * foundation for file queue entries, since they can indicate whether the
   * source file has changed since it was entered into a queue.
   **/
  class XmlFileReference : public XmlObjectReference
  {
    XmlObjectNode* _xi;

  public:

    XmlFileReference();

    XmlObjectMember<unsigned long> Size;
    XmlObjectMember<XmlTime> Modified;
    XmlObjectMember<std::string> MD5;

    /**
     * Return true if the referenced file differs from this file.  The
     * reference is outdated if the size or the last modified fields differ
     * from the original.  If both the reference and the source have the
     * MD5 checksum set, then a difference in the checksum also indicates a
     * change.  Note there is no distinction between a 'forward' change and
     * a 'backward' change.  Any change at all is still a change.
     **/
    bool
    fileHasChanged();


    /**
     * Setup this file reference with a new target.  The target can be
     * a file object or another reference.
     **/
    bool
    target (XmlObjectCatalog& source, const std::string& name);

  };


}



#endif // _domx_XmlFileReference_h_

