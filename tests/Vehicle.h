// -*- C++ -*-

#ifndef _Vehicle_h_
#define _Vehicle_h_

#include <domx/XmlObjectNode.h>
#include <domx/XML.h>

using namespace domx;

class Vehicle : public XmlObjectInterface
{
  XmlObjectNode* _xi;

  static void
  construct (XmlObjectInterface* xi)
  {
    Vehicle* v = static_cast<Vehicle*>(xi);
    v->setSpeed(0);
    v->setAxles(0);
  }

public:

  Vehicle ()
  {
    _xi = newNode("vehicle", &construct);
  }

  void
  setSpeed (float f)
  {
    _xi->set ("speed", f);
  }

  float
  getSpeed ()
  {
    float f;
    _xi->get ("speed", f);
    return f;
  }

  void
  setAxles (int i)
  {
    _xi->set("axles", i);
  }

  int
  getAxles ()
  {
    int i;
    _xi->get("axles", i);
    return i;
  }

};

#endif
