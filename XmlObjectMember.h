// -*- C++ -*-
//
// $Id$


#ifndef _domx_XmlObjectMember_h_
#define _domx_XmlObjectMember_h_

#include "XmlObjectNode.h"

namespace domx
{

  class XmlObjectMemberBase 
  {
  public:

    XmlObjectMemberBase (XmlObjectNode* node, const std::string& name);

    virtual void
    construct () = 0;

    virtual
    ~XmlObjectMemberBase();

  protected:

    std::string _name;
    XmlObjectNode* _node;

  private:

    XmlObjectMemberBase&
    operator= (const XmlObjectMemberBase&);

    XmlObjectMemberBase (const XmlObjectMemberBase&);

  };


  template <typename T>
  class XmlObjectMember : public XmlObjectMemberBase
  {
  public:
    XmlObjectMember (XmlObjectNode* node, const std::string& name, 
		     T default_value = T()) :
      XmlObjectMemberBase (node, name),
      _default (default_value)
    {
    }

    inline XmlObjectMember&
    set (const T& v)
    {
      _node->set (_name, v);
      return *this;
    }

    inline void
    get (T& v)
    {
      _node->get (_name, v);
    }

    inline T
    get ()
    {
      T v;
      _node->get (_name, v);
      return v;
    }

    inline T
    operator() ()
    {
      return get();
    }

    inline XmlObjectMember&
    operator= (const T& v)
    {
      return set (v);
    }

    virtual void
    construct ()
    {
      set (_default);
    }

  private:

    T _default;

  };

}


#endif // _domx_XmlObjectMember_h_
