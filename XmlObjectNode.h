// -*- C++ -*-
//
// $Id$
//

/**
 * This header file is only meant for subclasses of XmlObjectInterface.
 * It contains definitions of interfaces only needed for them.
 **/

#ifndef _domx_XmlObjectNode_h_
#define _domx_XmlObjectNode_h_

#include "XmlObjectInterface.h"
#include "XML.h"
#include <sstream>
#include <iostream>

namespace domx
{

  class XmlObjectMemberBase;

  class XmlObjectNode
  {
  public:

    /**
     * Get the typed value of the named node member.
     **/
    template <typename T>
    void
    get (const std::string& name, T& value)
    {
      xstring xvalue;
      getText (name, xvalue);
      std::istringstream os (xvalue);
      os >> value;
    }

    template <typename T>
    void
    set (const std::string& name, const T& value)
    {
      std::ostringstream os;
      os << value;
      setText (name, xstring(os.str()));
    }

    virtual
    void
    getText (const xstring& name, xstring& value) = 0;

    virtual
    void
    setText (const xstring& name, const xstring& value) = 0;

    virtual
    void
    addMember (XmlObjectMemberBase* member) = 0;

    virtual
    ~XmlObjectNode();

  };
  
}

#include "XmlObjectMember.h"

#endif // _domx_XmlObjectNode_h_
