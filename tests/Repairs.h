// -*- C++ -*-

#ifndef _Repairs_h_
#define _Repairs_h_

#include "Vehicle.h"
#include "domx/XmlTime.h"

class Repairs : public Vehicle
{
  XmlObjectNode* _xi;

public:

  XmlObjectMember<XmlTime> NextVisit;

  Repairs() :
    _xi(newNode ("repairs", &construct)),
    NextVisit (_xi, "nextvisit")
  {
  }

  static void
  construct (XmlObjectInterface* xi)
  {
    Repairs* c = static_cast<Repairs*>(xi);
    c->setMechanicName("");
    c->setSparkGap(0);
  }

  void
  setMechanicName (const std::string& mechanicname)
  {
    _xi->set ("mechanicname", mechanicname);
  }

  std::string
  getMechanicName ()
  {
    std::string mechanicname;
    _xi->get ("mechanicname", mechanicname);
    return mechanicname;
  }

  void
  setSparkGap (double d)
  {
    _xi->set ("sparkgap", d);
  }

  double
  getSparkGap ()
  {
    double d;
    _xi->get ("sparkgap", d);
    return d;
  }

};

#endif
