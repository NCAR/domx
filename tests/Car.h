// -*- C++ -*-

#ifndef _Car_h_
#define _Car_h_

#include "Vehicle.h"

class Car : public Vehicle
{
  XmlObjectNode* _xi;

public:

  XmlObjectMember<std::string> Color;
  XmlObjectMember<int> Year;
  XmlObjectMember<float> Price;

  Car() :
    _xi (newNode ("car", &construct)),
    Color (_xi, "color", "white"),
    Year (_xi, "year", 1986),
    Price (_xi, "price", 543.21)
  {
  }

  static void
  construct (XmlObjectInterface* xi)
  {
    Car* c = static_cast<Car*>(xi);
    c->setMake("");
    c->setModel("");
  }

  void
  setMake (const std::string& make)
  {
    _xi->set ("make", make);
  }

  std::string
  getMake ()
  {
    std::string make;
    _xi->get ("make", make);
    return make;
  }

  void
  setModel (const std::string& model)
  {
    _xi->set ("model", model);
  }

  std::string
  getModel ()
  {
    std::string model;
    _xi->get ("model", model);
    return model;
  }

};

#endif
