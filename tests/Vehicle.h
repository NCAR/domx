// -*- C++ -*-

#ifndef _Vehicle_h_
#define _Vehicle_h_

#include <domx/XmlObjectNode.h>
#include <domx/XML.h>

using namespace domx;

class Vehicle : public XmlObjectInterface
{
  XmlObjectNode* _xi;

  XmlObjectMember<float> Speed;
  XmlObjectMember<int> Axles;

public:

  Vehicle () :
    _xi (newNode("vehicle")),
    Speed (_xi, "speed"),
    Axles (_xi, "axles")
  {
  }

  void
  setSpeed (float f)
  {
    Speed = f;
  }

  float
  getSpeed ()
  {
    return Speed();
  }

  void
  setAxles (int i)
  {
    Axles = i;
  }

  int
  getAxles ()
  {
    return Axles();
  }

  Vehicle&
  operator= (const Vehicle& v)
  {
    XmlObjectInterface::operator= (v);
    return *this;
  }

};

#endif
