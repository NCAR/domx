// -*- C++ -*-
//
// $Id$


#ifndef _domx_XmlObjectMember_h_
#define _domx_XmlObjectMember_h_

#include "XmlObjectNode.h"
#include <map>

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


  /**
   * A template for converting generic types to and from strings using the
   * stream operators and a string stream.
   **/
  template <typename T>
  struct StreamStorage
  {
    void
    fromString (const std::string text, T& value)
    {
      std::istringstream os (text);
      os >> value;
    }

    std::string
    toString (const T& value)
    {
      std::ostringstream os;
      os << value;
      return os.str();
    }
  };


  /**
   * A template specialization for strings which shortcuts the conversion
   * and avoids problems with stream input operators separating strings at
   * whitespace.
   **/
  template <>
  struct StreamStorage<std::string>
  {
    void
    fromString (const std::string text, std::string& value)
    {
      value = text;
    }

    std::string
    toString (const std::string& value)
    {
      return value;
    }
  };


  /**
   * A storage class for enumerated types.  Particular types must
   * subclass this template and generate the map from value to string
   * in the constructor, but otherwise the base template takes care of
   * implementing the storage interface required by XmlObjectMember.
   **/
  template <class T>
  class EnumStorage
  {
  public:
    typedef T enum_type;

    void
    fromString (const std::string& value_in, enum_type& val)
    {
      typename enum_map::iterator it;
      for (it = enums.begin(); it != enums.end(); ++it)
      {
	if (it->second == value_in)
	{
	  val = it->first;
	  break;
	}
      }
    }

    std::string
    toString (const enum_type& v)
    {
      return enums[v];
    }

  protected:
    typedef std::map<enum_type,std::string> enum_map;
    enum_map enums;
  };


  struct BoolStorage : public EnumStorage<bool>
  {
    BoolStorage() 
    {
      enums[true] = "true";
      enums[false] = "false";
    }
  };


  /**
   * This is a type wrapper which takes care of setting and getting the
   * text node value when the member is accessed.  The translation to and
   * from text form is parameterized with a storage class.
   **/
  template <typename T, typename Storage = StreamStorage<T> >
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
      _node->setText (_name, _storage.toString(v));
      return *this;
    }

    inline void
    get (T& v)
    {
      _storage.fromString (_node->getString(_name), v);
    }

    inline T
    get ()
    {
      T v;
      _storage.fromString (_node->getString(_name), v);
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

    Storage _storage;
    T _default;

  };


  typedef XmlObjectMember<bool, BoolStorage> XmlBoolean;
  typedef XmlObjectMember<std::string> XmlString;
  typedef XmlObjectMember<int> XmlInteger;

}


#endif // _domx_XmlObjectMember_h_
