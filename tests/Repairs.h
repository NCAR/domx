// -*- C++ -*-

#ifndef _Repairs_h_
#define _Repairs_h_

#include "Vehicle.h"
#include "domx/XmlTime.h"

class Repairs : public Vehicle
{
  XmlObjectNode* _xi;

public:

  XmlObjectMember<double> SparkGap;
  XmlObjectMember<XmlTime> NextVisit;

  Repairs() :
    _xi(newNode ("repairs", &construct)),
    SparkGap (_xi, "sparkgap", 0),
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
    _xi->setText ("mechanicname", mechanicname);
  }

  std::string
  getMechanicName ()
  {
    domx::xstring mechanicname;
    _xi->getText ("mechanicname", mechanicname);
    return mechanicname;
  }

  void
  setSparkGap (double d)
  {
    SparkGap = d;
  }

  double
  getSparkGap ()
  {
    return SparkGap();
  }

};

#endif
